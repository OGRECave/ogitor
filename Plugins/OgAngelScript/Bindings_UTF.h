/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

/*////////////////////////////////////////////////////////////////////////////////
// The main script binding functions
////////////////////////////////////////////////////////////////////////////////*/

#ifndef BINDINGS_UTF_H
#define BINDINGS_UTF_H

class asIScriptEngine;

#include <string>

namespace Ogitors
{
    static Ogre::UTFString UTFStringFactory(asUINT length, const char *s)
    {
        return Ogre::UTFString(s, length);
    }
    //-----------------------------------------------------------------------------------------
    static void ConstructUTFString(Ogre::UTFString *thisPointer)
    {
	    new(thisPointer) Ogre::UTFString();
    }
    //-----------------------------------------------------------------------------------------
    static void ConstructUTFStringString(const std::string &f, Ogre::UTFString *thisPointer)
    {
	    new(thisPointer) Ogre::UTFString(f);
    }
    //-----------------------------------------------------------------------------------------
    static void DestructUTFString(Ogre::UTFString *thisPointer)
    {
	    thisPointer->~UTFString();
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::UTFString &AssignStringToString(const std::string& f, Ogre::UTFString &dest)
    {
	    dest = f;
	    return dest;
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::UTFString &AddAssignStringToString(const std::string& f, Ogre::UTFString &dest)
    {
	    dest = dest + f;
	    return dest;
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::UTFString AddStringUTFString(Ogre::UTFString &str, const std::string &f)
    {
	    return str + f;
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::UTFString AddUTFString(Ogre::UTFString &str, const Ogre::UTFString &f)
    {
	    return str + f;
    }
    //-----------------------------------------------------------------------------------------
    static Ogre::UTFString AddUTFStringString(std::string &f, Ogre::UTFString &str)
    {
	    return Ogre::UTFString(f + str);
    }
    //-----------------------------------------------------------------------------------------
    static int UTFStringCmp(const Ogre::UTFString &a, const Ogre::UTFString &b)
    {
	    int cmp = 0;
	    if( a < b ) cmp = -1;
	    else if( a > b ) cmp = 1;
	    return cmp;
    }
    //-----------------------------------------------------------------------------------------
    void RegisterUTFBindings(asIScriptEngine *engine)
    {
	    int r;

	    // Register the string type
        r = engine->RegisterObjectType("UTFString", sizeof(Ogre::UTFString), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); assert( r >= 0 );

	    // Register the object operator overloads
	    r = engine->RegisterObjectBehaviour("UTFString", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructUTFString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectBehaviour("UTFString", asBEHAVE_CONSTRUCT,  "void f(const string &in)",    asFUNCTION(ConstructUTFStringString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectBehaviour("UTFString", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructUTFString),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
        r = engine->RegisterObjectMethod("UTFString", "UTFString &opAssign(const UTFString &in)", asMETHODPR(Ogre::UTFString, operator =, (const Ogre::UTFString&), Ogre::UTFString&), asCALL_THISCALL); assert( r >= 0 );

	    r = engine->RegisterObjectMethod("UTFString", "bool opEquals(const UTFString &in) const", asMETHODPR(Ogre::UTFString, operator ==, (const Ogre::UTFString&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("UTFString", "int opCmp(const UTFString &in) const", asFUNCTION(UTFStringCmp), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("UTFString", "UTFString opAdd(const UTFString &in) const", asFUNCTION(AddUTFString), asCALL_CDECL_OBJFIRST); assert( r >= 0 );

	    // Register the object methods
	    if( sizeof(size_t) == 4 )
	    {
            r = engine->RegisterObjectMethod("UTFString", "uint length() const", asMETHOD(Ogre::UTFString,size), asCALL_THISCALL); assert( r >= 0 );
	    }
	    else
	    {
            r = engine->RegisterObjectMethod("UTFString", "uint64 length() const", asMETHOD(Ogre::UTFString,size), asCALL_THISCALL); assert( r >= 0 );
	    }

	    // Automatic conversion from values
	    r = engine->RegisterObjectMethod("UTFString", "UTFString &opAssign(const string &in)", asFUNCTION(AssignStringToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("UTFString", "UTFString &opAddAssign(const string &in)", asFUNCTION(AddAssignStringToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("UTFString", "UTFString opAdd(const string &in) const", asFUNCTION(AddStringUTFString), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("UTFString", "UTFString opAdd_r(const string &in) const", asFUNCTION(AddUTFStringString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    }

};

#endif