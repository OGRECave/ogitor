#include "as_ScriptOgreQuaternion.h"

	//-----------------------
	// AngelScript functions
	//-----------------------
	static void QuaternionDefaultConstructor(Ogre::Quaternion *self)
	{
		new(self) Ogre::Quaternion();
	}

    static void QuaternionInitMainConstructor(float w, float x, float y, float z, Ogre::Quaternion *self)
	{
		new(self) Ogre::Quaternion(Ogre::Real(w), Ogre::Real(x), Ogre::Real(y), Ogre::Real(z));
	}

	static void QuaternionInitConstructor(float angle, Ogre::Vector3 axis, Ogre::Quaternion *self)
	{
		new(self) Ogre::Quaternion(Ogre::Radian(angle), axis);
	}

	static void QuaternionOtherInitConstructor(Ogre::Vector3 x, Ogre::Vector3 y, Ogre::Vector3 z, Ogre::Quaternion *self)
	{
		new(self) Ogre::Quaternion(x,y,z);
	}

    static void CopyConstructQuaternion(const Ogre::Quaternion &other, Ogre::Quaternion *thisPointer)
    {
        new(thisPointer) Ogre::Quaternion(other);
    }

    static void DestructQuaternion(Ogre::Quaternion *thisPointer)
    {
	    thisPointer->~Quaternion();
    }

    static Ogre::Quaternion &QuaternionAssignment(Ogre::Quaternion *other, Ogre::Quaternion *self)
    {
	    return *self = *other;
    }

    void RegisterScriptOgreQuaternion(asIScriptEngine *engine)
	{
		int r;

		// Register the type
        r = engine->RegisterObjectType("Quaternion", sizeof(Ogre::Quaternion), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT,  "void f()",					asFUNCTION(QuaternionDefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT,  "void f(float, float, float, float)",					asFUNCTION(QuaternionInitMainConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT,  "void f(const Quaternion &in)",    asFUNCTION(CopyConstructQuaternion), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT,  "void f(const float, const Vector3)",					asFUNCTION(QuaternionInitConstructor),		asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Quaternion", asBEHAVE_CONSTRUCT,  "void f(const Vector3, const Vector3, const Vector3)",	asFUNCTION(QuaternionOtherInitConstructor), asCALL_CDECL_OBJLAST); assert(r >= 0);
	    r = engine->RegisterObjectMethod("Quaternion", "Quaternion &opAssign(Quaternion&in)", asFUNCTION(QuaternionAssignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );

		// Register the object properties
		r = engine->RegisterObjectProperty("Quaternion", "float w", offsetof(Ogre::Quaternion, w)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Quaternion", "float x", offsetof(Ogre::Quaternion, x)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Quaternion", "float y", offsetof(Ogre::Quaternion, y)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Quaternion", "float z", offsetof(Ogre::Quaternion, z)); assert( r >= 0 );

		// Register the object methods
		r = engine->RegisterObjectMethod("Quaternion", "float Dot(const Quaternion &in) const", asMETHOD(Ogre::Quaternion, Dot),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "float Norm() const",					asMETHOD(Ogre::Quaternion, Norm),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "float normalise()",						asMETHOD(Ogre::Quaternion, normalise),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "Quaternion Inverse() const",			asMETHOD(Ogre::Quaternion, Inverse),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "Quaternion UnitInverse() const",		asMETHOD(Ogre::Quaternion, UnitInverse),asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "Quaternion Exp() const",				asMETHOD(Ogre::Quaternion, Exp),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "Quaternion Log() const",				asMETHOD(Ogre::Quaternion, Log),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "float getRoll(bool) const",				asMETHOD(Ogre::Quaternion, getRoll),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "float getPitch(bool) const",			asMETHOD(Ogre::Quaternion, getPitch),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "float getYaw(bool) const",				asMETHOD(Ogre::Quaternion, getYaw),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "Vector3 xAxis() const",					asMETHOD(Ogre::Quaternion, xAxis),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "Vector3 yAxis() const",					asMETHOD(Ogre::Quaternion, yAxis),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "Vector3 zAxis() const",					asMETHOD(Ogre::Quaternion, zAxis),		asCALL_THISCALL); assert(r >= 0);

		r = engine->RegisterObjectMethod("Quaternion", "void FromAngleAxis(const float &in, const Vector3 &in)",						asMETHOD(Ogre::Quaternion, FromAngleAxis), asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "void ToAngleAxis(float &out, Vector3 &out) const",								asMETHODPR(Ogre::Quaternion, ToAngleAxis,	(Ogre::Radian&, Ogre::Vector3&) const, void),								asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "void FromAxes(const Vector3 &in, const Vector3 &in, const Vector3 &in)",		asMETHODPR(Ogre::Quaternion, FromAxes,		(const Ogre::Vector3&, const Ogre::Vector3&, const Ogre::Vector3&), void),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Quaternion", "void ToAxes(Vector3 &out, Vector3 &out, Vector3 &out) const",					asMETHODPR(Ogre::Quaternion, ToAxes,		(Ogre::Vector3&, Ogre::Vector3&, Ogre::Vector3&) const, void),				asCALL_THISCALL); assert(r >= 0);
	}
