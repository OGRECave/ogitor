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

#include <Ogre.h>
#include "angelscript.h"
#include "Bindings_Ogre.h"

namespace Ogitors
{
    static void ColourValueDefaultConstructor(Ogre::ColourValue *self)
	{
        new(self) Ogre::ColourValue();
	}

    static void ColourValueDefaultConstructor2(float r, float g, float b, float a, Ogre::ColourValue *self)
	{
        new(self) Ogre::ColourValue(r, g, b, a);
	}

    static void CopyConstructColourValue(const Ogre::ColourValue &other, Ogre::ColourValue *thisPointer)
    {
        new(thisPointer) Ogre::ColourValue(other);
    }

    static void DestructColourValue(Ogre::ColourValue *thisPointer)
    {
	    thisPointer->~ColourValue();
    }

    static Ogre::ColourValue &ColourValueAssignment(Ogre::ColourValue *other, Ogre::ColourValue *self)
    {
	    return *self = *other;
    }

    static void SphereDefaultConstructor(const Ogre::Vector3 origin, Ogre::Real radius, Ogre::AxisAlignedBox *self)
	{
        new(self) Ogre::Sphere(origin, radius);
	}

    static void CopyConstructSphere(const Ogre::Sphere &other, Ogre::Sphere *thisPointer)
    {
        new(thisPointer) Ogre::Sphere(other);
    }

    static void DestructSphere(Ogre::Sphere *thisPointer)
    {
	    thisPointer->~Sphere();
    }

    static Ogre::Sphere &SphereAssignment(Ogre::Sphere *other, Ogre::Sphere *self)
    {
	    return *self = *other;
    }

    static void AxisAlignedBoxDefaultConstructor(Ogre::AxisAlignedBox *self)
	{
		new(self) Ogre::AxisAlignedBox();
	}

    static void AxisAlignedBoxDefaultConstructor2(const Ogre::Vector3 min, const Ogre::Vector3 max, Ogre::AxisAlignedBox *self)
	{
		new(self) Ogre::AxisAlignedBox(min, max);
	}

    static void CopyConstructAxisAlignedBox(const Ogre::AxisAlignedBox &other, Ogre::AxisAlignedBox *thisPointer)
    {
        new(thisPointer) Ogre::AxisAlignedBox(other);
    }

    static void DestructAxisAlignedBox(Ogre::AxisAlignedBox *thisPointer)
    {
	    thisPointer->~AxisAlignedBox();
    }

    static Ogre::AxisAlignedBox &AxisAlignedBoxAssignment(Ogre::AxisAlignedBox *other, Ogre::AxisAlignedBox *self)
    {
	    return *self = *other;
    }

    static void RayDefaultConstructor(Ogre::Ray *self)
	{
		new(self) Ogre::Ray();
	}

    static void RayDefaultConstructor2(const Ogre::Vector3& origin, Ogre::Vector3& direction, Ogre::Ray *self)
	{
		new(self) Ogre::Ray(origin, direction);
	}

    static void CopyConstructRay(const Ogre::Ray &other, Ogre::Ray *thisPointer)
    {
        new(thisPointer) Ogre::Ray(other);
    }

    static void DestructRay(Ogre::Ray *thisPointer)
    {
	    thisPointer->~Ray();
    }

    static Ogre::Ray &RayAssignment(Ogre::Ray *other, Ogre::Ray *self)
    {
	    return *self = *other;
    }

    void RegisterOgreBindings(asIScriptEngine *engine)
	{
		int r;

        r = engine->RegisterObjectType("Sphere", sizeof(Ogre::Sphere), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("Sphere", asBEHAVE_CONSTRUCT,  "void f(const Vector3 &in, float)",	asFUNCTION(SphereDefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = engine->RegisterObjectBehaviour("Sphere", asBEHAVE_CONSTRUCT,  "void f(const Sphere &in)",    asFUNCTION(CopyConstructSphere), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectBehaviour("Sphere", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructSphere),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("Sphere", "Sphere &opAssign(Sphere&in)", asFUNCTION(SphereAssignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );

        r = engine->RegisterObjectType("ColourValue", sizeof(Ogre::ColourValue), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("ColourValue", asBEHAVE_CONSTRUCT,  "void f()",	asFUNCTION(ColourValueDefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("ColourValue", asBEHAVE_CONSTRUCT,  "void f(float, float, float, float)",	asFUNCTION(ColourValueDefaultConstructor2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = engine->RegisterObjectBehaviour("ColourValue", asBEHAVE_CONSTRUCT,  "void f(const ColourValue &in)",    asFUNCTION(CopyConstructColourValue), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectBehaviour("ColourValue", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructColourValue),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("ColourValue", "ColourValue &opAssign(ColourValue&in)", asFUNCTION(ColourValueAssignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectProperty("ColourValue", "float r", offsetof(Ogre::ColourValue, r)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("ColourValue", "float g", offsetof(Ogre::ColourValue, g)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("ColourValue", "float b", offsetof(Ogre::ColourValue, b)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("ColourValue", "float a", offsetof(Ogre::ColourValue, a)); assert( r >= 0 );

        r = engine->RegisterObjectType("Ray", sizeof(Ogre::Ray), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("Ray", asBEHAVE_CONSTRUCT,  "void f()",	asFUNCTION(RayDefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Ray", asBEHAVE_CONSTRUCT,  "void f(const Vector3 &in, const Vector3 &in)",	asFUNCTION(RayDefaultConstructor2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = engine->RegisterObjectBehaviour("Ray", asBEHAVE_CONSTRUCT,  "void f(const Ray &in)",    asFUNCTION(CopyConstructRay), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectBehaviour("Ray", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructRay),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("Ray", "Ray &opAssign(Ray&in)", asFUNCTION(RayAssignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );

		// Register the object methods
		r = engine->RegisterObjectMethod("Ray", "const Vector3& getOrigin()",	asMETHOD(Ogre::Ray, getOrigin),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Ray", "const Vector3& getDirection()",	asMETHOD(Ogre::Ray, getDirection),	asCALL_THISCALL); assert(r >= 0);

		r = engine->RegisterObjectMethod("Ray", "void setOrigin(const Vector3 &in)",	asMETHOD(Ogre::Ray, setOrigin),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Ray", "void setDirection(const Vector3 &in)",	asMETHOD(Ogre::Ray, setDirection),	asCALL_THISCALL); assert(r >= 0);

        r = engine->RegisterObjectType("AxisAlignedBox", sizeof(Ogre::AxisAlignedBox), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CA | asOBJ_APP_CLASS_ALLFLOATS); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("AxisAlignedBox", asBEHAVE_CONSTRUCT,  "void f()",	asFUNCTION(AxisAlignedBoxDefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("AxisAlignedBox", asBEHAVE_CONSTRUCT,  "void f(const Vector3 &in, const Vector3 &in)",	asFUNCTION(AxisAlignedBoxDefaultConstructor2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = engine->RegisterObjectBehaviour("AxisAlignedBox", asBEHAVE_CONSTRUCT,  "void f(const AxisAlignedBox &in)",    asFUNCTION(CopyConstructAxisAlignedBox), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectBehaviour("AxisAlignedBox", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructAxisAlignedBox),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
	    r = engine->RegisterObjectMethod("AxisAlignedBox", "AxisAlignedBox &opAssign(AxisAlignedBox&in)", asFUNCTION(AxisAlignedBoxAssignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );

		// Register the object methods
        r = engine->RegisterObjectMethod("AxisAlignedBox", "const Vector3& getMinimum()",	asMETHODPR(Ogre::AxisAlignedBox, getMinimum, (void) const, const Ogre::Vector3&),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "const Vector3& getMaximum()",	asMETHODPR(Ogre::AxisAlignedBox, getMaximum, (void) const, const Ogre::Vector3&),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void setMinimum(const Vector3 &in)",	asMETHODPR(Ogre::AxisAlignedBox, setMinimum, (const Ogre::Vector3&), void),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void setMinimum(float, float, float)",	asMETHODPR(Ogre::AxisAlignedBox, setMinimum, (float, float, float), void),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void setMaximum(const Vector3 &in)",	asMETHODPR(Ogre::AxisAlignedBox, setMaximum, (const Ogre::Vector3&), void),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void setMaximum(float, float, float)",	asMETHODPR(Ogre::AxisAlignedBox, setMaximum, (float, float, float), void),	asCALL_THISCALL); assert(r >= 0);
        r = engine->RegisterObjectMethod("AxisAlignedBox", "void merge(const AxisAlignedBox &in)",	asMETHODPR(Ogre::AxisAlignedBox, merge, (const Ogre::AxisAlignedBox&), void),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void merge(const Vector3 &in)",	asMETHODPR(Ogre::AxisAlignedBox, merge, (const Ogre::Vector3&), void),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void setNull()", asMETHOD(Ogre::AxisAlignedBox, setNull),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "bool isNull()", asMETHOD(Ogre::AxisAlignedBox, isNull),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "bool isFinite()", asMETHOD(Ogre::AxisAlignedBox, isFinite),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "bool isInfinite()",	asMETHOD(Ogre::AxisAlignedBox, isInfinite),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void setInfinite()", asMETHOD(Ogre::AxisAlignedBox, setInfinite),	asCALL_THISCALL); assert(r >= 0);
        r = engine->RegisterObjectMethod("AxisAlignedBox", "bool intersects(const AxisAlignedBox &in)",	asMETHODPR(Ogre::AxisAlignedBox, intersects, (const Ogre::AxisAlignedBox&) const, bool),	asCALL_THISCALL); assert(r >= 0);
        r = engine->RegisterObjectMethod("AxisAlignedBox", "bool intersects(const Sphere &in)",	asMETHODPR(Ogre::AxisAlignedBox, intersects, (const Ogre::Sphere&) const, bool),	asCALL_THISCALL); assert(r >= 0);
        r = engine->RegisterObjectMethod("AxisAlignedBox", "bool intersects(const Vector3 &in)",	asMETHODPR(Ogre::AxisAlignedBox, intersects, (const Ogre::Vector3&) const, bool),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "AxisAlignedBox intersection(const AxisAlignedBox &in)",	asMETHOD(Ogre::AxisAlignedBox, intersection),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "float volume()",	asMETHOD(Ogre::AxisAlignedBox, volume),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "void scale(const Vector3 &in)",	asMETHOD(Ogre::AxisAlignedBox, scale),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "Vector3 getCenter()",	asMETHOD(Ogre::AxisAlignedBox, getCenter),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "Vector3 getSize()",	asMETHOD(Ogre::AxisAlignedBox, getSize),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("AxisAlignedBox", "Vector3 getHalfSize()",	asMETHOD(Ogre::AxisAlignedBox, getHalfSize),	asCALL_THISCALL); assert(r >= 0);
        r = engine->RegisterObjectMethod("AxisAlignedBox", "bool contains(const AxisAlignedBox &in)",	asMETHODPR(Ogre::AxisAlignedBox, contains, (const Ogre::AxisAlignedBox&) const, bool),	asCALL_THISCALL); assert(r >= 0);
        r = engine->RegisterObjectMethod("AxisAlignedBox", "bool contains(const Vector3 &in)",	asMETHODPR(Ogre::AxisAlignedBox, contains, (const Ogre::Vector3&) const, bool),	asCALL_THISCALL); assert(r >= 0);
    }

}
