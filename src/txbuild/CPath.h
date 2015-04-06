#ifndef CPATH_H
#define CPATH_H

#include <string>
#include <cassert>

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

inline std::string StrReplace(const std::string &str, const std::string &strSearch, const std::string &strReplace)
{
    std::string strRet;
    unsigned i = 0;
    
    while(i < str.size())
    {
        if(str.compare(i, strSearch.size(), strSearch) == 0)
        {
            strRet += strReplace;
            i += strSearch.size();
        } else
            strRet += str[i++];
    }
    
    return strRet;
}

class CPath: public std::string
{
    public:
#ifdef _WIN32
        static const char SEP = '\\';
#else
        static const char SEP = '/';
#endif
    
        inline CPath(const char *pStr = ""):
            std::string(pStr)
        {
            
        }
        
        inline CPath(const std::string &Str, size_t Pos = 0, size_t Len = std::string::npos):
            std::string(Str, Pos, Len)
        {
            
        }
        
        CPath GetDir() const
        {
            size_t nPos = GetFileNamePos();
            
            if(nPos == 0)
                return "";
            else
                return CPath(*this, 0, nPos - 1);
        }
        
        std::string GetFilename() const
        {
            size_t nPos = GetFileNamePos();
            return std::string(*this, nPos);
        }
        
        CPath Join(const CPath &Path) const
        {
            if(empty())
                return Path;
            CPath Result;
            Result = *this + SEP + Path;
            return Result;
        }
        
        std::string ToMakefileFormat() const
        {
            std::string strTemp = StrReplace(*this, "\\", "/");
            return StrReplace(strTemp, " ", "\\ ");
        }
        
        std::string ToNativeFormat() const
        {
#ifdef _WIN32
            return StrReplace(*this, "/", "\\");
#else
            return StrReplace(*this, "\\", "/");
#endif
        }
        
        operator const char *() const
        {
            return c_str();
        }
        
        void Resolve()
        {
            size_t Pos = 0;
            while(Pos < size())
            {
                size_t SepPos = find_first_of("/\\", Pos);
                
                if(SepPos != npos && at(SepPos) == '\\')
                    at(SepPos) = '/'; // fix separators
                
                size_t Len = (SepPos == npos ? size() : SepPos) - Pos;
                if(compare(Pos, Len, "..") == 0)
                {
                    assert(Pos >= 2); // should throw exception
                    size_t PrevPos = find_last_of("/\\", Pos - 2);
                    if(PrevPos == npos)
                    {
                        erase(0, Pos + Len + 1);
                        Pos = 0;
                    } else {
                        erase(PrevPos, Pos + Len - PrevPos);
                        Pos = PrevPos;
                    }
                }
                else if(compare(Pos, Len, ".") == 0)
                    erase(Pos, Len + 1);
                else // normal component
                    Pos += Len + 1;
            }
        }

#ifndef NDEBUG
#define TEST_PATH(path, result) do { CPath tmp(path); tmp.Resolve(); if(tmp != result) { printf("Expected %s, got %s\n", result, tmp.c_str()); assert(false); } } while(0)
        static void TestImpl()
        {
            TEST_PATH("C:\\lol/folder/../hey\\.\\bitch/die/..", "C:/lol/hey/bitch");
            TEST_PATH("/lol/../wtf", "/wtf");
            TEST_PATH("radio/../conf/radio.xml", "conf/radio.xml");
        }
#endif // NDEBUG
    
    private:
        size_t GetFileNamePos() const
        {
            size_t nPos = find_last_of("/\\");
            
            if(nPos == npos)
                nPos = 0;
            else
                ++nPos;
            
            return nPos;
        }
};

#endif // CPATH_H
