#ifndef CMETAFILE_H
#define CMETAFILE_H

#include <tinyxml.h>
#include <vector>
#include "CPath.h"
#include "SBuildOptions.h"

class CMetaFile: TiXmlDocument
{
    public:
        enum EFileType
        {
            FT_CLIENT_SCRIPT,
            FT_SERVER_SCRIPT,
            FT_SHARED_SCRIPT,
            FT_STATIC
        };
        
        struct SFileInfo
        {
            SFileInfo(const CPath &p, EFileType t):
                Path(p), Type(t) {}
            
            CPath Path;
            EFileType Type;
        };
        
        CMetaFile(const CPath &RootPath);
        ~CMetaFile();
        void Load(const CPath &Path = "");
        void Save(const CPath &Path);
        
        void RequestMinVer(const std::string &Ver, bool bClient = false);
        
        void SetOptions(const SBuildOpts &Opts)
        {
            m_Options = Opts;
        }
        
        const std::vector<CPath> &GetMetaList() const
        {
            return m_MetaList;
        }
        
        const std::vector<SFileInfo> &GetFileList() const
        {
            return m_Files;
        }
        
    private:
        TiXmlDocument *m_pOutputDoc;
        TiXmlElement *m_pOutputRoot;
        TiXmlElement *m_pOutputInfo;
        SBuildOpts m_Options;
        bool m_bServerScript, m_bClientScript;
        CPath m_RootPath;
        std::string m_MinVerS, m_MinVerC;
        
        std::vector<TiXmlNode*> m_Settings;
        std::vector<std::string> m_Pages;
        std::vector<SFileInfo> m_Files;
        std::vector<CPath> m_MetaList;
};

#endif // CMETAFILE_H
