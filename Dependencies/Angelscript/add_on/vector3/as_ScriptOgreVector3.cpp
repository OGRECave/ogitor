#include "as_ScriptOgreVector3.h"

	//-----------------------
	// AngelScript functions
	//-----------------------
    static void Vector2DefaultConstructor(Ogre::Vector2 *self)
	{
		new(self) Ogre::Vector2();
	}

    static void Vector2InitConstructor(float x, float y, Ogre::Vector2 *self)
	{
		new(self) Ogre::Vector2(Ogre::Real(x), Ogre::Real(y));
	}

    static void Vector3DefaultConstructor(Ogre::Vector3 *self)
	{
		new(self) Ogre::Vector3();
	}

	static void Vector3InitConstructor(float x, float y, float z, Ogre::Vector3 *self)
	{
		new(self) Ogre::Vector3(Ogre::Real(x), Ogre::Real(y), Ogre::Real(z));
	}

	static void Vector4DefaultConstructor(Ogre::Vector4 *self)
	{
        new(self) Ogre::Vector4();
	}

	static void Vector4InitConstructor(float x, float y, float z, float w, Ogre::Vector4 *self)
	{
        new(self) Ogre::Vector4(Ogre::Real(x), Ogre::Real(y), Ogre::Real(z), Ogre::Real(w));
	}

    static void CopyConstructVector2(const Ogre::Vector2 &other, Ogre::Vector2 *thisPointer)
    {
        new(thisPointer) Ogre::Vector2(other);
    }

    static void CopyConstructVector3(const Ogre::Vector3 &other, Ogre::Vector3 *thisPointer)
    {
        new(thisPointer) Ogre::Vector3(other);
    }

    static void CopyConstructVector4(const Ogre::Vector4 &other, Ogre::Vector4 *thisPointer)
    {
        new(thisPointer) Ogre::Vector4(other);
    }

    static void DestructVector2(Ogre::Vector2 *thisPointer)
    {
	    thisPointer->~Vector2();
    }

    static void DestructVector3(Ogre::Vector3 *thisPointer)
    {
	    thisPointer->~Vector3();
    }

    static void DestructVector4(Ogre::Vector4 *thisPointer)
    {
	    thisPointer->~Vector4();
    }

    static Ogre::Vector2 &Vector2Assignment(Ogre::Vector2 *other, Ogre::Vector2 *self)
    {
	    return *self = *other;
    }

    static Ogre::Vector3 &Vector3Assignment(Ogre::Vector3 *other, Ogre::Vector3 *self)
    {
	    return *self = *other;
    }

    static Ogre::Vector4 &Vector4Assignment(Ogre::Vector4 *other, Ogre::Vector4 *self)
    {
	    return *self = *other;
    }

    void RegisterScriptOgreVector3(asIScriptEngine *engine)
	{
		int r;

		r = engine->RegisterObjectType("Vector2", sizeof(Ogre::Vector2), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector2", "float x", offsetof(Ogre::Vector2, x)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector2", "float y", offsetof(Ogre::Vector2, y)); assert( r >= 0 );

		r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f()",	asFUNCTION(Vector2DefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(float, float)",	asFUNCTION(Vector2InitConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = engine->RegisterObjectBehaviour("Vector2", asBEHAVE_CONSTRUCT,  "void f(const Vector2 &in)",    asFUNCTION(CopyConstructVector2), asCALL_CDECL_OBJLAST); assert( r >= 0 );

        r = engine->RegisterObjectMethod("Vector2", "Vector2 &opAssign(Vector2&in)", asFUNCTION(Vector2Assignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opAdd(const Vector2 &in) const",			asMETHODPR(Ogre::Vector2, operator+, (const Ogre::Vector2&) const,	Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opSub(const Vector2 &in) const",			asMETHODPR(Ogre::Vector2, operator-, (const Ogre::Vector2&) const,	Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opMul(float) const",						asMETHODPR(Ogre::Vector2, operator*, (float) const,					Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector2", "Vector2 opNeg() const",							asMETHODPR(Ogre::Vector2, operator-, () const,						Ogre::Vector2),	asCALL_THISCALL); assert(r >= 0);


		r = engine->RegisterObjectType("Vector3", sizeof(Ogre::Vector3), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector3", "float x", offsetof(Ogre::Vector3, x)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector3", "float y", offsetof(Ogre::Vector3, y)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector3", "float z", offsetof(Ogre::Vector3, z)); assert( r >= 0 );


	    r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT,  "void f()",                     asFUNCTION(Vector3DefaultConstructor), asCALL_CDECL_OBJLAST); assert( r >= 0 );
        r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT,  "void f(const Vector3 &in)",    asFUNCTION(CopyConstructVector3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectBehaviour("Vector3", asBEHAVE_CONSTRUCT,  "void f(float, float, float)",	asFUNCTION(Vector3InitConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
	    
        
        r = engine->RegisterObjectMethod("Vector3", "Vector3 &opAssign(Vector3&in)", asFUNCTION(Vector3Assignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );
        r = engine->RegisterObjectMethod("Vector3", "Vector3 crossProduct(const Vector3 &in) const",	asMETHOD(Ogre::Vector3, crossProduct),		asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opAdd(const Vector3 &in) const",			asMETHODPR(Ogre::Vector3, operator+, (const Ogre::Vector3&) const,	Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opSub(const Vector3 &in) const",			asMETHODPR(Ogre::Vector3, operator-, (const Ogre::Vector3&) const,	Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opMul(float) const",						asMETHODPR(Ogre::Vector3, operator*, (float) const,					Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector3", "Vector3 opNeg() const",							asMETHODPR(Ogre::Vector3, operator-, () const,						Ogre::Vector3),	asCALL_THISCALL); assert(r >= 0);


		r = engine->RegisterObjectType("Vector4", sizeof(Ogre::Vector4), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CAK); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float x", offsetof(Ogre::Vector4, x)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float y", offsetof(Ogre::Vector4, y)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float z", offsetof(Ogre::Vector4, z)); assert( r >= 0 );
		r = engine->RegisterObjectProperty("Vector4", "float w", offsetof(Ogre::Vector4, w)); assert( r >= 0 );

		r = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(Vector4DefaultConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
		r = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_CONSTRUCT,  "void f(float, float, float, float)", asFUNCTION(Vector4InitConstructor),	asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = engine->RegisterObjectBehaviour("Vector4", asBEHAVE_CONSTRUCT,  "void f(const Vector4 &in)",    asFUNCTION(CopyConstructVector4), asCALL_CDECL_OBJLAST); assert( r >= 0 );

        r = engine->RegisterObjectMethod("Vector4", "Vector4 &opAssign(Vector4&in)", asFUNCTION(Vector4Assignment), asCALL_CDECL_OBJLAST); assert( r >= 0 );
		r = engine->RegisterObjectMethod("Vector4", "Vector4 opAdd(const Vector4 &in) const",			asMETHODPR(Ogre::Vector4, operator+, (const Ogre::Vector4&) const,	Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector4", "Vector4 opSub(const Vector4 &in) const",			asMETHODPR(Ogre::Vector4, operator-, (const Ogre::Vector4&) const,	Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector4", "Vector4 opMul(float) const",						asMETHODPR(Ogre::Vector4, operator*, (float) const,					Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
		r = engine->RegisterObjectMethod("Vector4", "Vector4 opNeg() const",							asMETHODPR(Ogre::Vector4, operator-, () const,						Ogre::Vector4),	asCALL_THISCALL); assert(r >= 0);
    }
