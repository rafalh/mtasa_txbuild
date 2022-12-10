#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdio>

using namespace std;

class CStringList
{
    public:
        enum EType {SERVER, CLIENT, SHARED};
        
    private:
        struct SEntry
        {
            SEntry(const string &str, EType t):
                Str(str), Type(t) {}
            
            string Str;
            EType Type;
        };
        
        typedef vector<SEntry> TVector;
        typedef unordered_map<string, size_t> TMap;
        
        TVector m_Vector;
        TMap m_Map;
        
        void InsertStr(const string &Str, EType Type);
    
    public:
        int LoadFromLua(FILE *pFile, EType Type);
        int LoadFromTxt(FILE *pFile);
        int SaveToTxt(FILE *pFile);
        
        int LoadFromLua(const char *pszPath, EType Type);
        int LoadFromTxt(const char *pszPath);
        int SaveToTxt(const char *pszPath);
};

void CStringList::InsertStr(const string &Str, EType Type)
{
    TMap::iterator it = m_Map.find(Str);
    if(it == m_Map.end())
    {
        m_Map.insert(pair<string, size_t>(Str, m_Vector.size()));
        m_Vector.push_back(SEntry(Str, Type));
    }
    else
    {
        SEntry &e = m_Vector[it->second];
        if(e.Type != Type)
            e.Type = SHARED;
    }
}

int CStringList::LoadFromLua(FILE *pFile, EType Type)
{
    char buf[1024];
    while(fgets(buf, sizeof(buf), pFile))
    {
        bool bQuote = false, bApostrophe = false, bSlash = false;
        string strTemp;
        
        for(char *ptr = buf; *ptr; ++ptr)
        {
            if(*ptr == '-' && ptr[1] == '-' && !bQuote && !bApostrophe)
                break; // comment
            if(*ptr == '\\' && !bSlash)
                bSlash = true;
            else
            {
                if(*ptr == '"' && !bSlash && !bApostrophe)
                {
                    *ptr = 0;
                    
                    if(!bQuote)
                        strTemp.clear();
                    else
                        InsertStr(strTemp, Type);
                    
                    bQuote = !bQuote;
                }
                else if(*ptr == '\'' && !bSlash && !bQuote)
                    bApostrophe = !bApostrophe;
                else
                    strTemp.append(1, *ptr);
                
                bSlash = false;
            }
        }
    }
    return 0;
}

int CStringList::LoadFromTxt(FILE *pFile)
{
    char buf[1024];
    while(fgets(buf, sizeof(buf), pFile))
    {
        char *pszEnd = strchr(buf, '\n');
        if(pszEnd)
            *pszEnd = '\0';
        
        if(!buf[0] || !buf[1])
            continue;
        
        EType Type;
        if(buf[0] == 's')
            Type = SERVER;
        else if(buf[0] == 'c')
            Type = CLIENT;
        else
            Type = SHARED;
        
        InsertStr(buf + 2, Type);
    }
    return -1;
}

int CStringList::SaveToTxt(FILE *pFile)
{
    for(const SEntry &e : m_Vector)
    {
        static const char pszType[] = {'s', 'c', '*'};
        fprintf(pFile, "%c %s\n", pszType[(int)e.Type], e.Str.c_str());
    }
    
    return 0;
}

int CStringList::LoadFromLua(const char *pszPath, EType Type)
{
    FILE *pFile = fopen(pszPath, "r");
    if(!pFile)
        return -1;
    
    int ret = LoadFromLua(pFile, Type);
    fclose(pFile);
    return ret;
}

int CStringList::LoadFromTxt(const char *pszPath)
{
    FILE *pFile = fopen(pszPath, "r");
    if(!pFile)
        return -1;
    
    int ret = LoadFromTxt(pFile);
    fclose(pFile);
    return ret;
}

int CStringList::SaveToTxt(const char *pszPath)
{
    FILE *pFile = fopen(pszPath, "w");
    if(!pFile)
        return -1;
    
    int ret = SaveToTxt(pFile);
    fclose(pFile);
    return ret;
}

int main(int argc, const char *argv[])
{
    CStringList StrList;
    CStringList::EType LuaType = CStringList::SERVER;
    const char *pszOutput = "strings.txt";
    bool bLua = false;
    
    if(argc == 1)
    {
        printf("Usage: %s [-o output] [-l type input1 ...] [-t input1 ...]\n", argv[0]);
        return 0;
    }
    
    for(int i = 1; i < argc; ++i)
    {
        if(!strcmp(argv[i], "-o") && i + 1 < argc)
            pszOutput = argv[++i];
        else if(!strcmp(argv[i], "-l") && i + 1 < argc)
        {
            bLua = true;
            ++i;
            
            if(!strcmp(argv[i], "server"))
                LuaType = CStringList::SERVER;
            else if(!strcmp(argv[i], "client"))
                LuaType = CStringList::CLIENT;
            else
                LuaType = CStringList::SHARED;
        }
        else if(!strcmp(argv[i], "-t"))
            bLua = false;
        else if(bLua)
            StrList.LoadFromLua(argv[i], LuaType);
        else
            StrList.LoadFromTxt(argv[i]);
    }
    
    StrList.SaveToTxt(pszOutput);
    
    return 0;
}
