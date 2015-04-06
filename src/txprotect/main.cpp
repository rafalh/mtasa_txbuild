#include <cstring>
#include <string>
#include <vector>
#include <cstdio>
#include <cassert>
#include "CException.h"

using namespace std;

class CLuaProtect
{
    public:
        void Load(const char *pszPath);
        void Save(const char *pszPath);
        void SetLoader(const char *pszPath);
        static void Test();
    
    private:
        static string ToLuaStr(const string &str);
        static string StrReplace(const string &str, const string &needle, const string &replacement);
        
        string m_LuaCode;
        string m_Loader;
};

void CLuaProtect::Load(const char *pszPath)
{
    FILE *pFile = fopen(pszPath, "rb");
    if(!pFile)
        throw CException("Failed to open %s", pszPath);
    m_LuaCode = "{";
    char Buf[4096];
    size_t cbBuf;
    while((cbBuf = fread(Buf, 1, sizeof(Buf), pFile)) > 0)
    {
        string RawCode(Buf, cbBuf);
        m_LuaCode += string("'") + ToLuaStr(RawCode) + "',";
    }
    m_LuaCode += "}";
    fclose(pFile);
}

void CLuaProtect::Save(const char *pszPath)
{
    FILE *pFile = fopen(pszPath, "w");
    if(!pFile)
        throw CException("Failed to open %s", pszPath);
    
    string str = StrReplace(m_Loader, "__LUA_CHUNK_TBL__", m_LuaCode);
    fprintf(pFile, "%s", str.c_str());
    
    fclose(pFile);
}

void CLuaProtect::SetLoader(const char *pszPath)
{
    FILE *pFile = fopen(pszPath, "r");
    if(!pFile)
        throw CException("Failed to open %s", pszPath);
    
    char Buf[4096];
    size_t cbBuf;
    while((cbBuf = fread(Buf, 1, sizeof(Buf), pFile)) > 0)
    {
        string RawCode(Buf, cbBuf);
        m_Loader += RawCode;
    }
    
    fclose(pFile);
}

void CLuaProtect::Test()
{
    assert(StrReplace("Ala ma kota", "a", "b") == "Alb mb kotb");
    assert(StrReplace("ABCABBCACCAB", "AB", "X") == "XCXBCACCX");
    
    assert(ToLuaStr("abcdef") == "abcdef");
    assert(ToLuaStr("\2\3\4") == "\\2\\3\\4");
    assert(ToLuaStr("\2\392") == "\\2\\00392");
}

string CLuaProtect::ToLuaStr(const string &str)
{
    string strRet;
    strRet.reserve(str.size()*2);
    for(size_t i = 0; i < str.size(); ++i)
    {
        unsigned char ch = str[i];
        unsigned char nextCh = (i + 1 < str.size()) ? str[i + 1] : 0;
        if(isalnum(ch))
            strRet.append(1, ch);
        else
        {
            char buf[16];
            if(isdigit(nextCh))
                sprintf(buf, "\\%03u", ch);
            else
                sprintf(buf, "\\%u", ch);
            strRet.append(buf);
        }
    }
    return strRet;
}

string CLuaProtect::StrReplace(const string &str, const string &needle, const string &replacement)
{
    string strRet;
    strRet.reserve(str.size());
    
    size_t pos = 0;
    while(true)
    {
        size_t needlePos = str.find(needle, pos);
        if(needlePos == string::npos)
        {
            strRet.append(str, pos, string::npos);
            break;
        }
        
        strRet.append(str, pos, needlePos - pos);
        strRet.append(replacement);
        pos = needlePos + needle.length();
    }
    return strRet;
}

int main(int argc, const char *argv[])
try
{
    const char *pszOutput = "output.lua", *pszInput = "input.lua", *pszLoader = "loader.lua";
    CLuaProtect Protect;
    
    //CLuaProtect::Test();
    
    if(argc == 1)
    {
        printf("Usage: %s [-o output] [-l loader] input\n", argv[0]);
        return 0;
    }
    
    for(int i = 1; i < argc; ++i)
    {
        if(!strcmp(argv[i], "-o") && i + 1 < argc)
            pszOutput = argv[++i];
        if(!strcmp(argv[i], "-l") && i + 1 < argc)
            pszLoader = argv[++i];
        else
            pszInput = argv[i];
    }
    
    Protect.Load(pszInput);
    Protect.SetLoader(pszLoader);
    Protect.Save(pszOutput);
    
    return 0;
}
catch(const CException &e)
{
    printf("Error! %s\n", e.what());
    return -1;
}
