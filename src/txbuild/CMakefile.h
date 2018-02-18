#ifndef CMAKEFILE_H
#define CMAKEFILE_H

#include "CPath.h"
#include "CMetaFile.h"
#include "SBuildOptions.h"
#include <unordered_set>
#include <unordered_map>
#include <fstream>

class CMakefile
{
    public:
        CMakefile();
        void Generate(const CPath &Path);
        
        void SetBuildDir(const CPath &Path)
        {
            m_BuildDir = Path;
        }
        
        void SetOptions(const SBuildOpts &Opts)
        {
            m_Options = Opts;
        }
        
        void SetFileList(const std::vector<CMetaFile::SFileInfo> &FileList)
        {
            m_FileList = FileList;
        }
        
        void SetMetaList(const std::vector<CPath> &MetaList)
        {
            m_MetaList = MetaList;
        }
    
    private:
        bool FileExists(const CPath &Path);
        void AddFile(const CPath &Path, CMetaFile::EFileType type);
        void WriteCompileRule(const CPath &Path, CMetaFile::EFileType Type);
        void WriteCopyRule(const CPath &Path);
        void WriteDirRule(const std::string &Path);
        void WriteStrListRule();
        void WriteJoinRule(const std::vector<std::string> &FileList, const char *pszFilename);
        
        CPath m_RootPath, m_BuildDir;
        SBuildOpts m_Options;
        
        bool m_bServerScript, m_bClientScript;
        std::unordered_map<std::string, CMetaFile::EFileType> m_ResFiles;
        std::unordered_set<std::string> m_Dirs;
        std::ofstream m_Makefile;
        std::vector<CMetaFile::SFileInfo> m_FileList;
        std::vector<CPath> m_MetaList;
        std::vector<std::string> m_CleanList, m_OutputList, m_StrList, m_JoinListS, m_JoinListC;
};

#endif // CMAKEFILE_H
