#include "as_ScriptOgreVector3.h"

	//-----------------------
	// AngelScript functions
	//-----------------------
	static void Vector2DefaultConstructor(float x, float y, Ogre::Vector2 *self)
	{
		new(self) Ogre::Vector2(Ogre::Real(x), Ogre::Real(y));
	}

	static void Vector3DefaultConstructor(float x, float y, float z, Ogre::Vector3 *self)
	{
		new(self) Ogre::Vector3(Ogre::Real(x), Ogre::Real(y), Ogre::Real(z));
	}

	static void Vector4DefaultConstructor(float x, float y, float z, float w, Ogre::Vector4 *self)
	{
        new(self) Ogre::Vector4(Ogre::Real(x), Ogre::Real(y), Ogre::Real(z), Ogre::Real(w));
	}

    void RegisterScriptOgreVector3(asIScriptEngine *engine)
	{
		int r;

		// Register the type
		r = engine->RegisterObjectType("Vector2", sizeof(Ogre::Vector2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );
		r = engine->RegisterObjectType("Vector3", sizeof(Ogre::Vector3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );
		r = engine->RegisterObjectType("Vector4", sizeof(Ogre::Vector4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );

		// Register the object properties
		r = engine->RegisterObjectProperty("Vector2", "float x", offsetof(Ogre::Vector2, x)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector2", "float y", offsetof(Ogre::Vector2, y)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector3", "float x", offsetof(Ogre::Vector3, x)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector3", "float y", offsetof(Ogre::Vector3, y)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector3", "float z", offsetof(Ogre::Vector3, z)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float x", offsetof(Ogre::Vector4, x)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float y", offsetof(Ogre::Vector4, y)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float z", offsetof(Ogre::Vector4, z)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float w", offsetof(Ogre::Vector4, w)); assert( r >= 0 );

		// Register the constructors
		r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(float, float)",	asFUNCTION(Vector2DefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT,  "void f(float, float, float)",	asFUNCTION(Vector3DefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_CONSTRUCT,  "void f(float, float, float, float)", asFUNCTION(Vector4DefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);

		// Register the object methods
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd(const Vector2 &in) const",			asMETHODPR(Ogre::Vector2, operator+, (const Ogre::Vector2&) const,	Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub(const Vector2 &in) const",			asMETHODPR(Ogre::Vector2, operator-, (const Ogre::Vector2&) const,	Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul(float) const",						asMETHODPR(Ogre::Vector2, operator*, (float) const,					Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opNeg() const",							asMETHODPR(Ogre::Vector2, operator-, () const,						Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);

        r = engine->RegisterObjectMethod("Vector3", "Vector3 crossProduct(const Vector3 &in) const",	asMETHOD(Ogre::Vector3, crossProduct),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opAdd(const Vector3 &in) const",			asMETHODPR(Ogre::Vector3, operator+, (const Ogre::Vector3&) const,	Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opSub(const Vector3 &in) const",			asMETHODPR(Ogre::Vector3, operator-, (const Ogre::Vector3&) const,	Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opMul(float) const",						asMETHODPR(Ogre::Vector3, operator*, (float) const,					Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opNeg() const",							asMETHODPR(Ogre::Vector3, operator-, () const,						Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);

		r = engine->RegisterObjectMethod("Vector4", "Vector4 opAdd(const Vector4 &in) const",			asMETHODPR(Ogre::Vector4, operator+, (const Ogre::Vector4&) const,	Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector4", "Vector4 opSub(const Vector4 &in) const",			asMETHODPR(Ogre::Vector4, operator-, (const Ogre::Vector4&) const,	Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector4", "Vector4 opMul(float) const",						asMETHODPR(Ogre::Vector4, operator*, (float) const,					Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector4", "Vector4 opNeg() const",							asMETHODPR(Ogre::Vector4, operator-, () const,						Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
    }
