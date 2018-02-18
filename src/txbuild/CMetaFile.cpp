#include "CMetaFile.h"
#include "CPath.h"
#include "CException.h"
#include <iostream>
#include <algorithm>

using namespace std;

CMetaFile::CMetaFile(const CPath &RootPath):
    m_pOutputInfo(nullptr), m_bServerScript(false), m_bClientScript(false)
{
    m_RootPath = RootPath;
    
    m_pOutputDoc = new TiXmlDocument();
    m_pOutputRoot = m_pOutputDoc->InsertEndChild(TiXmlElement("meta"))->ToElement();
}

CMetaFile::~CMetaFile()
{
    delete m_pOutputDoc;
}

void CMetaFile::Load(const CPath &Path)
{
    CPath MetaPath = m_RootPath.Join(Path + "meta.xml");
    m_MetaList.push_back(Path + "meta.xml");
    TiXmlDocument InputDoc(MetaPath.c_str());
    if(!InputDoc.LoadFile())
    {
        cerr << "Error! Cannot load " << MetaPath << ".\n";
        return;
    }
    
    TiXmlElement *pInputRoot = InputDoc.RootElement();
    if(!pInputRoot)
    {
        cerr << "Error! No meta element found in " << MetaPath << ".\n";
        return;
    }
    
    for(TiXmlNode *pNode = pInputRoot->IterateChildren(NULL); pNode; pNode = pInputRoot->IterateChildren(pNode))
    {
        TiXmlElement *pEl = pNode->ToElement();
        
        const char *pszName = pNode->Value();
        assert(pszName);
        
        string strName = pszName;
        if(!pEl)
        {
            // not element
            m_pOutputRoot->InsertEndChild(*pNode);
        }
        else if(strName == "script")
        {
            const char *pszSrc = pEl->Attribute("src");
            if(!pszSrc)
            {
                cerr << "Expected src attribute for " << strName << "\n";
                continue;
            }
            
            const char *pszType = pEl->Attribute("type");
            EFileType Type;
            
            if(pszType && !strcmp(pszType, "client"))
            {
                Type = FT_CLIENT_SCRIPT;
                m_bClientScript = true;
            }
            else if(pszType && !strcmp(pszType, "shared"))
            {
                Type = FT_SHARED_SCRIPT;
                m_bServerScript = m_bClientScript = true;
            }
            else if(!pszType || !strcmp(pszType, "server"))
            {
                Type = FT_SERVER_SCRIPT;
                m_bServerScript = true;
            } else
                THROW_EXCEPTION("Invalid script type: %s", pszType ? pszType : "unknown");
            
            CPath NewFilePath = Path + pszSrc;
            NewFilePath.Resolve();
            
            AddFile(NewFilePath, Type);
            
            if(!m_Options.bJoin)
            {
                TiXmlElement *pNewEl = m_pOutputRoot->InsertEndChild(*pEl)->ToElement();
                pNewEl->SetAttribute("src", NewFilePath.c_str());
            }
        }
        else if(strName == "file" || strName == "config" || strName == "html" || strName == "serverfile")
        {
            const char *pszSrc = pEl->Attribute("src");
            if(!pszSrc)
            {
                cerr << "Expected src attribute for " << strName << "\n";
                continue;
            }
            
            CPath NewFilePath = Path + pszSrc;
            NewFilePath.Resolve();
            
            AddFile(NewFilePath, FT_STATIC);
            
            if (strName != "serverfile")
            {
                TiXmlElement *pNewEl = m_pOutputRoot->InsertEndChild(*pEl)->ToElement();
                pNewEl->SetAttribute("src", NewFilePath.c_str());
            }
        }
        else if(strName == "info")
        {
            if(Path.empty())
                m_pOutputInfo = m_pOutputRoot->InsertEndChild(*pEl)->ToElement();
            
            const char *pszPages = pEl->Attribute("pages");
            if(pszPages)
            {
                string Pages = pszPages;
                string PagesFixed;
                size_t Pos = 0;
                while(true)
                {
                    size_t EndPos = Pages.find(',', Pos);
                    if(EndPos == string::npos)
                        EndPos = Pages.size();
                    string Page(Pages, Pos, EndPos);
                    string PagePath, PageRest;
                    size_t PathEndPos = PagePath.find_first_of("?|");
                    
                    if(PathEndPos != string::npos)
                    {
                        PageRest.assign(Page, PathEndPos, string::npos);
                        PagePath.assign(Page, 0, PathEndPos);
                    } else
                        PagePath = Page;
                    
                    CPath PageNewPath = Path + PagePath;
                    PageNewPath.Resolve();
                    
                    m_Pages.push_back(PageNewPath + PageRest);
                    
                    if(EndPos == Pages.size())
                        break;
                    else
                        Pos = EndPos + 1;
                }
            }
        }
        else if(strName == "settings")
        {
            for(TiXmlNode *pSubNode = pEl->IterateChildren(NULL); pSubNode; pSubNode = pEl->IterateChildren(pSubNode))
                m_Settings.push_back(pSubNode->Clone());
        }
        else if(strName == "min_mta_version")
        {
            const char *pszClient = pEl->Attribute("client");
            const char *pszServer = pEl->Attribute("server");
            if(pszClient)
                RequestMinVer(pszClient, true);
            if(pszServer)
                RequestMinVer(pszServer, false);
        }
        else if(strName == "export" || strName == "aclrequest" || strName == "include")
        {
            // Copy to destination meta.xml
            m_pOutputRoot->InsertEndChild(*pEl);
        }
        else if(strName == "module")
        {
            const char *pszSrc = pEl->Attribute("src");
            if(!pszSrc)
            {
                cerr << "Expected src attribute for " << strName << "\n";
                continue;
            }
            
            std::string strComment = " Module: ";
            strComment = strComment + Path + pszSrc + " ";
            m_pOutputRoot->InsertEndChild(TiXmlComment(strComment.c_str()));
            Load(Path + pszSrc + "/");
        }
        else
            cerr << "Ignored node: " << strName << "\n";
    }
}

void CMetaFile::Save(const CPath &Path)
{
    if(m_Options.bJoin)
    {
        if(m_bServerScript)
        {
            TiXmlElement *pEl = m_pOutputRoot->InsertEndChild(TiXmlElement("script"))->ToElement();
            pEl->SetAttribute("type", "server");
            pEl->SetAttribute("src", "server.lua");
        }
        
        if(m_bClientScript)
        {
            TiXmlElement *pEl = m_pOutputRoot->InsertEndChild(TiXmlElement("script"))->ToElement();
            pEl->SetAttribute("type", "client");
            pEl->SetAttribute("src", "client.lua");
        }
    }
    
    if(!m_MinVerS.empty() || !m_MinVerC.empty())
    {
        TiXmlElement *pEl = m_pOutputRoot->InsertEndChild(TiXmlElement("min_mta_version"))->ToElement();
        if(!m_MinVerS.empty())
            pEl->SetAttribute("server", m_MinVerS.c_str());
        if(!m_MinVerC.empty())
            pEl->SetAttribute("client", m_MinVerC.c_str());
    }
    
    if(m_Options.bProtect)
    {
        TiXmlElement *pEl = m_pOutputRoot->InsertEndChild(TiXmlElement("include"))->ToElement();
        pEl->SetAttribute("resource", "txgenuine");
    }
    
    if(!m_Settings.empty())
    {
        TiXmlElement *pSettingsEl = m_pOutputRoot->InsertEndChild(TiXmlElement("settings"))->ToElement();
        for(vector<TiXmlNode*>::const_iterator it = m_Settings.begin(); it != m_Settings.end(); ++it)
            pSettingsEl->LinkEndChild(*it);
        
        m_Settings.clear();
    }
    
    if(!m_Pages.empty())
    {
        string Pages;
        for(string &Page: m_Pages)
        {
            if(!Pages.empty())
                Pages += ",";
            Pages += Page;
        }
        m_pOutputInfo->SetAttribute("pages", Pages.c_str());
        m_Pages.clear();
    }
    
    m_pOutputDoc->SaveFile(Path);
}

void CMetaFile::RequestMinVer(const std::string &Ver, bool bClient)
{
    std::string &Target = bClient ? m_MinVerC : m_MinVerS;
    if(Target.compare(Ver) < 0)
        Target = Ver;
}

void CMetaFile::AddFile(const CPath &p, EFileType t)
{
    if (t == FT_CLIENT_SCRIPT || t == FT_SERVER_SCRIPT)
    {
        EFileType OtherType = (t == FT_CLIENT_SCRIPT ? FT_SERVER_SCRIPT : FT_CLIENT_SCRIPT);
        auto it = std::find(m_Files.begin(), m_Files.end(), SFileInfo(p, OtherType));
        if (it != m_Files.end())
        {
            it->Type = FT_SHARED_SCRIPT;
            return;
        }
    }
    
    SFileInfo FileDesc(p, t);
    if (std::find(m_Files.begin(), m_Files.end(), FileDesc) == m_Files.end())
        m_Files.push_back(FileDesc);
}