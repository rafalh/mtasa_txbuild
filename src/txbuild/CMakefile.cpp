#include "CMakefile.h"
#include "CException.h"
#include <vector>
#include <iostream>

using namespace std;

CMakefile::CMakefile():
    m_BuildDir("build"),
    m_bServerScript(false), m_bClientScript(false) {}

void CMakefile::Generate(const CPath &Path)
{
    m_Makefile.open(Path.c_str(), ios_base::out);
    if(!m_Makefile.is_open())
        THROW_EXCEPTION("Failed to open %s", Path.c_str());
    
    m_Makefile << "BUILD_DIR := " << m_BuildDir << "\n";
    m_Makefile << "TEMP := " << m_BuildDir << "/$(PROJECT_NAME)\n";
    m_Makefile << "SELF := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))\n";
    m_Makefile << "\n";
    
    for(const CMetaFile::SFileInfo &fi: m_FileList)
        AddFile(fi.Path, fi.Type);
    
    m_Dirs.insert("$(OUTPUT)");
    for(const string &Dir: m_Dirs)
        WriteDirRule(Dir);
    
    if(m_Options.bStrList)
        WriteStrListRule();
    
    if(m_Options.bJoin)
    {
        if(!m_JoinListS.empty())
            WriteJoinRule(m_JoinListS, "server.lua");
        if(!m_JoinListC.empty())
            WriteJoinRule(m_JoinListC, "client.lua");
    }
    
    m_Makefile << "$(OUTPUT)/meta.xml: $(BUILD_DIR)/meta.xml | $(OUTPUT)\n";
    m_Makefile << "\t$(ECHO) Copying meta.xml\n";
    m_Makefile << "\t$(call copyFile,$(BUILD_DIR)/meta.xml,$(OUTPUT)/meta.xml)\n";
    m_Makefile << "\n";
    m_OutputList.push_back("$(OUTPUT)/meta.xml");
    
    m_Makefile << "$(TARGET_PREFIX)all:";
    for(const string &Path: m_OutputList)
        m_Makefile << " " << Path;
    m_Makefile << "\n\n";
    
    m_Makefile << "$(TARGET_PREFIX)clean:\n";
    for(const string &Path: m_CleanList)
        m_Makefile << "\t$(call delFile," << Path << ")\n";
    m_Makefile << "\n";
    
    string strMetaList;
    for(const CPath &Path: m_MetaList)
        strMetaList += Path.ToMakefileFormat() + " ";
    m_Makefile << "$(SELF) $(BUILD_DIR)/meta.xml: $(TXMAIN_PATH) " << strMetaList << "\n";
    m_Makefile << "\t$(ECHO) Building meta.xml\n";
    m_Makefile << "\t$(TXMAIN) " << m_Options.ToStr() << " " << "\n";
    
    m_Makefile.close();
}

void CMakefile::WriteStrListRule()
{
    string Output = "$(OUTPUT)/strings.txt";
    m_OutputList.push_back(Output);
    
    m_Makefile << Output << ":";
    for(const string &Path: m_StrList)
        m_Makefile << " " << Path;
    m_Makefile << "\n";
    m_Makefile << "\t$(TXSTRLIST) -o \"$(call nativePath," << Output << ")\"";
    for(const string &Path: m_StrList)
        m_Makefile << " \"$(call nativePath," << Path << ")\"";
    m_Makefile << "\n\n";
}

void CMakefile::WriteJoinRule(const std::vector<std::string> &FileList, const char *pszFilename)
{
    assert(m_Options.bCompile);
    
    string Temp = string("$(TEMP)/") + pszFilename;
    string Output = string("$(OUTPUT)/") + pszFilename;
    
    m_OutputList.push_back(Output);
    m_CleanList.push_back(Output);
    
    m_Makefile << Output << ":";
    for(const string &Path: FileList)
        m_Makefile << " $(TEMP)/" << Path;
    if(m_Options.bProtect)
        m_Makefile << " $(LOADER_PATH)";
    m_Makefile << " | $(OUTPUT)\n";
    
    m_Makefile << "\t$(ECHO) Building " << pszFilename << "\n";
    
    m_Makefile << "\t$(CD) $(BUILD_DIR) && $(LUAC) $(LUAC_FLAGS) -o \"$(call nativePath,$(PROJECT_NAME)/" << pszFilename << ")\"";
    for(const string &Path: FileList)
        m_Makefile << " \"$(call nativePath,$(PROJECT_NAME)/" << Path << ")\"";
    m_Makefile << "\n";
    
    if(m_Options.bUtf8Bom)
        m_Makefile << "\t$(call addUtf8BOM," << Temp << ")\n";
    if(m_Options.bProtect)
    {
        m_Makefile << "\t$(call protectFile," << Temp << "," << Temp << ")\n";
        if(m_Options.bPreprocess)
            m_Makefile << "\t$(call preprocessFile," << Temp << "," << Temp << ") -I \"$(dir $(LOADER_PATH))\"\n";
        if(m_Options.bCompile)
            m_Makefile << "\t$(call compileFile,$(BUILD_DIR),$(PROJECT_NAME)/" << pszFilename << ")\n";
    }
    m_Makefile << "\t$(call copyFile," << Temp << "," << Output << ")\n";
    m_Makefile << "\n";
}

bool CMakefile::FileExists(const CPath &Path)
{
    ifstream Stream(Path.c_str(), ios_base::in);
    if(Stream.is_open())
    {
        Stream.close();
        return true;
    }
    
    return false;
}

void CMakefile::WriteCompileRule(const CPath &Path, CMetaFile::EFileType Type)
{
    string Input = Path.ToMakefileFormat();
    string Temp = CPath("$(TEMP)").Join(Path).ToMakefileFormat();
    string Output = CPath("$(OUTPUT)").Join(Path).ToMakefileFormat();
    string TempDir = CPath("$(TEMP)").Join(Path).GetDir().ToMakefileFormat();
    string OutputDir = CPath("$(OUTPUT)").Join(Path).GetDir().ToMakefileFormat();
    
    const char *pszType;
    if(Type == CMetaFile::FT_SERVER_SCRIPT)
        pszType = "server";
    else if(Type == CMetaFile::FT_CLIENT_SCRIPT)
        pszType = "client";
    else
        pszType = "shared";
    
    if(m_Options.bStrList)
    {
        string StrList = Temp + ".txt";
        m_Dirs.insert(TempDir);
        m_CleanList.push_back(StrList);
        m_StrList.push_back(StrList);
        
        m_Makefile << StrList << ": " << Input << " $(TXSTRLIST_PATH) | " << TempDir << "\n";
        m_Makefile << "\t$(call genStrListFromLua," << StrList << "," << Input << "," << pszType << ")\n";
        m_Makefile << "\n";
    }
    
    string Deps = Input;
    if(FileExists("config.mak"))
        Deps += " config.mak";
    
    if(m_Options.bJoin)
    {
        m_Dirs.insert(TempDir);
        m_CleanList.push_back(Temp);
        
        m_Makefile << Temp << ": " << Deps << " | " << TempDir << "\n";
    }
    else
    {
        m_Dirs.insert(TempDir);
        m_Dirs.insert(OutputDir);
        m_CleanList.push_back(Temp);
        m_OutputList.push_back(Output);
        
        m_Makefile << Output << ": " << Deps << " | " << TempDir << " " << OutputDir << "\n";
    }
    
    if(!m_Options.bJoin)
        m_Makefile << "\t$(ECHO) Building " << Path << "\n";
    else if(m_Options.bPreprocess)
        m_Makefile << "\t$(ECHO) Preprocessing " << Path << "\n";
    else
        m_Makefile << "\t$(ECHO) Copying " << Path << "\n";
    
    if(m_Options.bPreprocess)
        m_Makefile << "\t$(call preprocessFile," << Input << "," << Temp << ")\n";
    else
        m_Makefile << "\t$(call copyFile," << Input << "," << Temp << ")\n";
    
    m_Makefile << "\t$(call checkLuaSyntax," << Temp << ")\n";
    
    if(!m_Options.bJoin)
    {
        if(m_Options.bCompile)
            m_Makefile << "\t$(call compileFile,$(BUILD_DIR),$(PROJECT_NAME)/" << Input << ")\n";
        
        if(m_Options.bUtf8Bom)
            m_Makefile << "\t$(call addUtf8BOM," << Temp << ")\n";
        
        if(m_Options.bProtect)
        {
            m_Makefile << "\t$(call protectFile," << Temp << "," << Temp << ")\n";
            if(m_Options.bPreprocess)
                m_Makefile << "\t$(call preprocessFile," << Temp << "," << Temp << ") -I \"$(dir $(LOADER_PATH))\"\n";
            if(m_Options.bCompile)
                m_Makefile << "\t$(call compileFile,$(BUILD_DIR),$(PROJECT_NAME)/" << Input << ")\n";
        }
        
        m_Makefile << "\t$(call copyFile," << Temp << "," << Output << ")\n";
    }
    
    m_Makefile << "\n";
    
    if(m_Options.bJoin)
    {
        if(Type == CMetaFile::FT_SERVER_SCRIPT || Type == CMetaFile::FT_SHARED_SCRIPT)
            m_JoinListS.push_back(Input);
        if(Type == CMetaFile::FT_CLIENT_SCRIPT || Type == CMetaFile::FT_SHARED_SCRIPT)
            m_JoinListC.push_back(Input);
    }
}

void CMakefile::WriteCopyRule(const CPath &Path)
{
    string Input = Path.ToMakefileFormat();
    string Output = CPath("$(OUTPUT)").Join(Path).ToMakefileFormat();
    string OutputDir = CPath("$(OUTPUT)").Join(Path).GetDir().ToMakefileFormat();
    
    m_Dirs.insert(OutputDir);
    m_CleanList.push_back(Output);
    m_OutputList.push_back(Output);
    
    m_Makefile << Output << ": " << Input << " | " << OutputDir << "\n";
    m_Makefile << "\t$(ECHO) Copying " << Path << "\n";
    m_Makefile << "\t$(call copyFile," << Input << "," << Output << ")\n";
    m_Makefile << "\n";
}

void CMakefile::WriteDirRule(const std::string &Path)
{
    m_Makefile << Path << ":\n";
    m_Makefile << "\t$(call createDir," << Path << ")\n";
    m_Makefile << "\n";
}

void CMakefile::AddFile(const CPath &Path, CMetaFile::EFileType Type)
{
    CPath FullPath = m_RootPath.Join(Path);
    
    if(!FileExists(FullPath))
        return;
    
    auto InsRet = m_ResFiles.insert(make_pair(Path, Type));
    if(!InsRet.second)
    {
        CMetaFile::EFileType MergedType = InsRet.first->second;
        if (MergedType == CMetaFile::FT_CLIENT_SCRIPT && Type != CMetaFile::FT_CLIENT_SCRIPT)
            MergedType = CMetaFile::FT_SHARED_SCRIPT;
        if (MergedType == CMetaFile::FT_SERVER_SCRIPT && Type != CMetaFile::FT_SERVER_SCRIPT)
            MergedType = CMetaFile::FT_SHARED_SCRIPT;
        if (MergedType != InsRet.first->second)
        {
            InsRet.first->second = MergedType;
            Type = MergedType;
            cerr << "Suggestion: use one script tag with type=\"shared\" instead of two with types \"client\" and \"server\" for file "
                << Path << ".\n";
            // FIXME: WriteCompileRule doesnt handle repeatition
        }
        else
            cerr << "Warning! File " << Path << " has been defined multiple times.\n";
    }
    
    if(Type == CMetaFile::FT_CLIENT_SCRIPT || Type == CMetaFile::FT_SHARED_SCRIPT)
        m_bClientScript = true;
    if(Type == CMetaFile::FT_SERVER_SCRIPT || Type == CMetaFile::FT_SHARED_SCRIPT)
        m_bServerScript = true;
    
    bool bScript = (Type != CMetaFile::FT_STATIC);
    if(bScript && m_Options.bPreprocess)
        WriteCompileRule(Path, Type);
    else
        WriteCopyRule(Path);
}
