/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2013 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
///
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include <sstream>
#include <cassert>

#include "ScriptStringExtensions.h"
#include "angelscript.h"
#include "scriptstdstring.h"
#include <cstring>

namespace Ogitors
{
    //-----------------------------------------------------------------------------------------
    static void ConstructStringGenericInt(asIScriptGeneric * gen)
    {
        int *a = static_cast<int*>(gen->GetAddressOfArg(0));
        std::stringstream sstr;
	    sstr << *a;
        new (gen->GetObject()) std::string(sstr.str());
    }
    //-----------------------------------------------------------------------------------------
    static void ConstructStringGenericUInt(asIScriptGeneric * gen)
    {
        unsigned int *a = static_cast<unsigned int*>(gen->GetAddressOfArg(0));
        std::stringstream sstr;
	    sstr << *a;
        new (gen->GetObject()) std::string(sstr.str());
    }
    //-----------------------------------------------------------------------------------------
    static void ConstructStringGenericDouble(asIScriptGeneric * gen)
    {
        double *a = static_cast<double*>(gen->GetAddressOfArg(0));
        std::stringstream sstr;
	    sstr << *a;
        new (gen->GetObject()) std::string(sstr.str());
    }
    //-----------------------------------------------------------------------------------------
    static void ConstructStringInt(int a, std::string *thisPointer)
    {
        std::stringstream sstr;
	    sstr << a;
        new (thisPointer) std::string(sstr.str());
    }
    //-----------------------------------------------------------------------------------------
    static void ConstructStringUInt(unsigned int a, std::string *thisPointer)
    {
        std::stringstream sstr;
	    sstr << a;
        new (thisPointer) std::string(sstr.str());
    }
    //-----------------------------------------------------------------------------------------
    static void ConstructStringDouble(double a, std::string *thisPointer)
    {
        std::stringstream sstr;
	    sstr << a;
        new (thisPointer) std::string(sstr.str());
    }
    //-----------------------------------------------------------------------------------------
    inline int getArgumentPos(const std::string &str, int count)
    {
        const char *data = str.c_str();

        int pos = -1;
        int val = 1000;
        for(int i = 0;i < count;i++)
        {
            if(data[i] == '%' && i < (count - 1) && data[i + 1] >= '0' && data[i + 1] <= '9')
            {
                if(val > data[i + 1])
                {
                    pos = i;
                    val = data[i + 1];
                }

                ++i;
            }
        }

        return pos;
    }
    //-----------------------------------------------------------------------------------------
    static std::string Arg(const std::string& f, std::string &str)
    {
        int count = str.length();
        int pos = getArgumentPos(str, count);

        if(pos > -1)
        {
            return str.substr(0, pos) + f + str.substr(pos + 2, count - (pos + 2));
        }

        return str;
    }
    //-----------------------------------------------------------------------------------------
    static std::string Arg(int n, std::string &str)
    {
        int count = str.length();
        int pos = getArgumentPos(str, count);

        if(pos > -1)
        {
            std::ostringstream f;
            f << n;
            return str.substr(0, pos) + f.str() + str.substr(pos + 2, count - (pos + 2));
        }

        return str;
    }
    //-----------------------------------------------------------------------------------------
    static std::string Arg(unsigned int n, std::string &str)
    {
        int count = str.length();
        int pos = getArgumentPos(str, count);

        if(pos > -1)
        {
            std::ostringstream f;
            f << n;
            return str.substr(0, pos) + f.str() + str.substr(pos + 2, count - (pos + 2));
        }

        return str;
    }
    //-----------------------------------------------------------------------------------------
    static std::string Arg(double n, std::string &str)
    {
        int count = str.length();
        int pos = getArgumentPos(str, count);

        if(pos > -1)
        {
            std::ostringstream f;
            f << n;
            return str.substr(0, pos) + f.str() + str.substr(pos + 2, count - (pos + 2));
        }

        return str;
    }
    //-----------------------------------------------------------------------------------------
    static int StringFind(const std::string &arg, std::string &str)
    {
        return str.find(arg);
    }
    //-----------------------------------------------------------------------------------------
    static void StringReplace(const std::string &tofind, const std::string &toreplace, std::string &str)
    {
        int pos = str.find(tofind);
        if(pos != -1)
        {
            str.erase(pos, tofind.length());
            str.insert(pos, toreplace);
        }
    }
    //-----------------------------------------------------------------------------------------
    static int StringToInt(std::string &dest)
    {
        std::stringstream f(dest);
        int val = 0;
        f >> val;

        return val;
    }
    //-----------------------------------------------------------------------------------------
    static unsigned int StringToUInt(std::string &dest)
    {
        std::stringstream f(dest);
        unsigned int val = 0;
        f >> val;

        return val;
    }
    //-----------------------------------------------------------------------------------------
    static double StringToDouble(std::string &dest)
    {
        std::stringstream f(dest);
        double val = 0;
        f >> val;

        return val;
    }
    //-----------------------------------------------------------------------------------------
    static std::string StringSubStr(int st, int len, std::string &str)
    {
        return str.substr(st, len);
    }
    //-----------------------------------------------------------------------------------------
    static void StringErase(int pos, int length, std::string &dest)
    {
        dest.erase(pos, length);
    }
    //-----------------------------------------------------------------------------------------
    bool RegisterScriptStringExtensions(asIScriptEngine *engine)
    {
        int r;

	    if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
        {
            r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(int)",                 asFUNCTION(ConstructStringGenericInt), asCALL_GENERIC); assert( r >= 0 );
            r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(uint)",                 asFUNCTION(ConstructStringGenericUInt), asCALL_GENERIC); assert( r >= 0 );
            r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(double)",                 asFUNCTION(ConstructStringGenericDouble), asCALL_GENERIC); assert( r >= 0 );
        }
	    else
        {
            r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(int)",                    asFUNCTION(ConstructStringInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(uint)",                    asFUNCTION(ConstructStringUInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(double)",                    asFUNCTION(ConstructStringDouble), asCALL_CDECL_OBJLAST); assert( r >= 0 );
            r = engine->RegisterObjectMethod("string", "string arg(const string &in) const", asFUNCTIONPR(Arg, (const std::string&, std::string&), std::string), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "string arg(int) const", asFUNCTIONPR(Arg, (int, std::string&), std::string), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "string arg(uint) const", asFUNCTIONPR(Arg, (unsigned int, std::string&), std::string), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "string arg(double) const", asFUNCTIONPR(Arg, (double, std::string&), std::string), asCALL_CDECL_OBJLAST); assert( r >= 0 );

            r = engine->RegisterObjectMethod("string", "void erase(int , int)", asFUNCTION(StringErase), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "string subString(int, int) const", asFUNCTION(StringSubStr), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "int find(const string &in) const", asFUNCTION(StringFind), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "void replaceString(const string &in, const string&in)", asFUNCTION(StringReplace), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "int toInt()", asFUNCTION(StringToInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "uint toUInt()", asFUNCTION(StringToUInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	        r = engine->RegisterObjectMethod("string", "double toDouble()", asFUNCTION(StringToDouble), asCALL_CDECL_OBJLAST); assert( r >= 0 );
        }

        return true;
    }
}
