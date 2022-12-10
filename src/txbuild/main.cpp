#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include "CPath.h"
#include "CProject.h"

using namespace std;

int main(int argc, const char *argv[])
try
{
    bool bHelp = false, bQuiet = false;
    SBuildOpts Opts;
    CPath RootPath = "";
    
#ifndef NDEBUG
    CPath::TestImpl();
#endif //NDEBUG
    
    for(int i = 1; i < argc; ++i)
    {
        const char *pArg = argv[i];
        if(*pArg != '/'&& *pArg != '-')
        {
            RootPath = argv[i];
            bHelp = false;
        }
        else if(pArg[1])
        {
            string strArg = pArg + 1;
            
            bool bEnable = true;
            if(isdigit(strArg[strArg.size() - 1]))
            {
                bEnable = strArg[strArg.size() - 1] == '1';
                strArg.resize(strArg.size() - 1);
            }
            
            if(strArg == "h")
                bHelp = bEnable;
            else if(strArg == "q")
                bQuiet = bEnable;
            else if(strArg == "p")
                Opts.bPreprocess = bEnable;
            else if(strArg == "c")
                Opts.bCompile = bEnable;
            else if(strArg == "u")
                Opts.bUtf8Bom = bEnable;
            else if(strArg == "j")
                Opts.bJoin = bEnable;
            else if(strArg == "e")
                Opts.bProtect = bEnable;
            else if(strArg == "s")
                Opts.bStrList = bEnable;
            else
                cerr << "Invalid option: " << argv[i] << "\n";
        }
    }
    
    if(bHelp)
    {
        cout << "Usage: " << argv[0] << " [-o makefile_path] [-h] [-q] [-p] [-c] [-u] [-j] [-e] [-s] root_path";
        return 0;
    }
    
    if(!bQuiet)
        cout << "Loading project...\n";
    Opts.fix();
    CProject *pProj = new CProject(RootPath, Opts);
    if(!bQuiet)
        cout << "Building meta...\n";
    pProj->BuildMeta();
    if(!bQuiet)
        cout << "Generating Makefile...\n";
    pProj->GenerateMakefile();
    
    delete pProj;
    
    if(!bQuiet)
        cout << "Makefile has been updated successfully!\n";
    
    return 0;
}
catch(exception& e)
{
    cerr << "Error! " << e.what() << "\n";
    return -1;
}
