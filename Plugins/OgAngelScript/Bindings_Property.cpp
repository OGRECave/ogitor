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

#include "Ogitors.h"

#include "angelscript.h"

namespace Ogitors
{
// Example REF_CAST behaviour
//-----------------------------------------------------------------------------------------
template<class A, class B>
B* refCast(A* a)
{
    // If the handle already is a null handle, then just return the null handle
    if( !a ) return 0;

    // Now try to dynamically cast the pointer to the wanted type
    B* b = static_cast<B*>(a);
    if( b != 0 )
    {
        // Since the cast was made, we need to increase the ref counter for the returned handle
        b->_addRef();
    }
    return b;
}
//-----------------------------------------------------------------------------------------
#define REGISTER_REFERENCE_OBJECT( name, classname )\
{\
    r = engine->RegisterObjectType(name, 0, asOBJ_REF);assert(r >= 0);\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(classname,_addRef), asCALL_THISCALL); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(classname,_release), asCALL_THISCALL); assert( r >= 0 );\
}\
//-----------------------------------------------------------------------------------------
#define REGISTER_PROPERTY_TYPE( name, ast, typ )\
{\
    r = engine->RegisterObjectType(name, 0, asOBJ_REF);assert(r >= 0);\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asMETHOD(OgitorsProperty<typ>,_addRef), asCALL_THISCALL); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asMETHOD(OgitorsProperty<typ>,_release), asCALL_THISCALL); assert( r >= 0 );\
    r = engine->RegisterObjectMethod(name, "void set(" ast ")", asMETHOD(OgitorsProperty<typ>, set), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectMethod(name, ast " get()", asMETHOD(OgitorsProperty<typ>, get), asCALL_THISCALL);assert(r >= 0);\
    r = engine->RegisterObjectBehaviour("Property", asBEHAVE_REF_CAST, name "@ f()", asFUNCTION((refCast<OgitorsPropertyBase,OgitorsProperty<typ> >)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_IMPLICIT_REF_CAST, "Property@ f()", asFUNCTION((refCast<OgitorsProperty<typ>,OgitorsPropertyBase>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
}\
//-----------------------------------------------------------------------------------------
void RegisterPropertyBindings(asIScriptEngine *engine)
{
    int r;

    REGISTER_REFERENCE_OBJECT("Property", OgitorsPropertyBase);
    REGISTER_PROPERTY_TYPE("PropertyShort", "int16", short);
    REGISTER_PROPERTY_TYPE("PropertyUShort", "uint16", unsigned short);
    REGISTER_PROPERTY_TYPE("PropertyInt", "int", int);
    REGISTER_PROPERTY_TYPE("PropertyUInt", "uint", unsigned int);
    REGISTER_PROPERTY_TYPE("PropertyLong", "int64", long);
    REGISTER_PROPERTY_TYPE("PropertyULong", "uint64", unsigned long);
    REGISTER_PROPERTY_TYPE("PropertyString", "string", Ogre::String);
    REGISTER_PROPERTY_TYPE("PropertyBool", "bool", bool);
    REGISTER_PROPERTY_TYPE("PropertyReal", "float", Ogre::Real);
    //REGISTER_PROPERTY_TYPE("PropertyColour", Ogre::ColourValue);
    REGISTER_PROPERTY_TYPE("PropertyQuaternion", "Quaternion", Ogre::Quaternion);
    //REGISTER_PROPERTY_TYPE("PropertyVector2", Ogre::Vector2);
    REGISTER_PROPERTY_TYPE("PropertyVector3", "Vector3", Ogre::Vector3);
    //REGISTER_PROPERTY_TYPE("PropertyVector4", Ogre::Vector4);

    REGISTER_REFERENCE_OBJECT("PropertySet", OgitorsPropertySet);
    REGISTER_REFERENCE_OBJECT("CustomPropertySet", OgitorsCustomPropertySet);

    r = engine->RegisterObjectMethod("PropertySet", "Property@ getProperty(string &in)", asMETHOD(OgitorsPropertySet, getProperty), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("PropertySet", "bool hasProperty(string &in)", asMETHOD(OgitorsPropertySet, hasProperty), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("CustomPropertySet", "Property@ getProperty(string &in)", asMETHOD(OgitorsCustomPropertySet, getProperty), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("CustomPropertySet", "bool hasProperty(string &in)", asMETHOD(OgitorsCustomPropertySet, hasProperty), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("CustomPropertySet", "int64 addProperty(const string &in, PropertyType)", asMETHODPR(OgitorsCustomPropertySet, addProperty, (const Ogre::String&, OgitorsPropertyType), OgitorsPropertyDef*), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("CustomPropertySet", "void removeProperty(const string &in)", asMETHOD(OgitorsCustomPropertySet, removeProperty), asCALL_THISCALL);assert(r >= 0);

}
//-----------------------------------------------------------------------------------------

}
