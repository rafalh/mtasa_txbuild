#include "CProject.h"
#include "CMakefile.h"
#include <fstream>
#include <iostream>
#include <tinyxml.h>

using namespace std;

CProject::CProject(const CPath &RootPath, const SBuildOpts &Opts):
    m_RootPath(RootPath), m_BuildDir("build"), m_Options(Opts)
{
    m_pMeta = new CMetaFile(m_RootPath);
    m_pMeta->SetOptions(Opts);
    m_pMeta->Load();
}

CProject::~CProject()
{
    delete m_pMeta;
}

void CProject::BuildMeta()
{
    m_pMeta->Save(m_RootPath.Join(m_BuildDir).Join("meta.xml"));
}

void CProject::GenerateMakefile()
{
    CMakefile *pMakefile = new CMakefile();
    pMakefile->SetBuildDir(m_BuildDir);
    pMakefile->SetOptions(m_Options);
    pMakefile->SetFileList(m_pMeta->GetFileList());
    pMakefile->SetMetaList(m_pMeta->GetMetaList());
    pMakefile->Generate(m_RootPath.Join(m_BuildDir).Join("Makefile.auto"));
    delete pMakefile;
}
