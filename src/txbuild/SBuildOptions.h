#ifndef SBUILDOPTIONS_H_INCLUDED
#define SBUILDOPTIONS_H_INCLUDED

#include <string>
#include <sstream>

struct SBuildOpts
{
    SBuildOpts():
        bPreprocess(true), bCompile(true), bUtf8Bom(false), bJoin(false), bProtect(false), bStrList(false) {}
    
    std::string ToStr() const
    {
        std::stringstream ss;
        ss << "-p" << (bPreprocess ? 1 : 0);
        ss << " -c" << (bCompile ? 1 : 0);
        ss << " -u" << (bUtf8Bom ? 1 : 0);
        ss << " -j" << (bJoin ? 1 : 0);
        ss << " -e" << (bProtect ? 1 : 0);
        ss << " -s" << (bStrList ? 1 : 0);
        return ss.str();
    }
    
    void fix()
    {
        if(bJoin)
            bCompile = true;
    }
    
    bool bPreprocess;
    bool bCompile;
    bool bUtf8Bom;
    bool bJoin;
    bool bProtect;
    bool bStrList;
};

#endif // SBUILDOPTIONS_H_INCLUDED
