#ifndef CPROJECT_H
#define CPROJECT_H

#include "CPath.h"
#include "CMetaFile.h"
#include "SBuildOptions.h"
#include <set>
#include <vector>
#include <fstream>

class CMetaFile;

class CProject
{
    public:
        CProject(const CPath &RootPath, const SBuildOpts &Opts);
        ~CProject();
        void BuildMeta();
        void GenerateMakefile();
        
        void SetBuildDir(const CPath &Path)
        {
            m_BuildDir = Path;
        }
    
    private:
        CPath m_RootPath, m_BuildDir;
        SBuildOpts m_Options;
        CMetaFile *m_pMeta;
};

#endif // CPROJECT_H
