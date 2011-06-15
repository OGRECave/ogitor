#ifndef SCRIPTMATH3D_H
#define SCRIPTMATH3D_H

#include <angelscript.h>

BEGIN_AS_NAMESPACE

// This is not a complete 3D math library. It's only meant as a sample
// for registering value types. The application developers will most
// likely want to register their own math library instead.

struct Vector3
{
	Vector3();
	Vector3(const Vector3 &other);
	Vector3(float x, float y, float z);

	Vector3 &operator=(const Vector3 &other);
	Vector3 &operator+=(const Vector3 &other);
	Vector3 &operator-=(const Vector3 &other);
	Vector3 &operator*=(float scalar);
	Vector3 &operator/=(float scalar);

	float length() const;

	// Swizzle operators
	Vector3 get_xyz() const;
	void    set_xyz(const Vector3 &in);
	Vector3 get_yzx() const;
	void    set_yzx(const Vector3 &in);
	Vector3 get_zxy() const;
	void    set_zxy(const Vector3 &in);
	Vector3 get_zyx() const;
	void    set_zyx(const Vector3 &in);
	Vector3 get_yxz() const;
	void    set_yxz(const Vector3 &in);
	Vector3 get_xzy() const;
	void    set_xzy(const Vector3 &in);

	friend bool operator==(const Vector3 &a, const Vector3 &b);
	friend bool operator!=(const Vector3 &a, const Vector3 &b);
	friend Vector3 operator+(const Vector3 &a, const Vector3 &b);
	friend Vector3 operator-(const Vector3 &a, const Vector3 &b);
	friend Vector3 operator*(float s, const Vector3 &v);
	friend Vector3 operator*(const Vector3 &v, float s);
	friend Vector3 operator/(const Vector3 &v, float s);

	float x;
	float y;
	float z;
};

// This function will determine the configuration of the engine
// and use one of the two functions below to register the string type
void RegisterScriptMath3D(asIScriptEngine *engine);

// Call this function to register the math functions using native calling conventions
void RegisterScriptMath3D_Native(asIScriptEngine *engine);

// Use this one instead if native calling conventions
// are not supported on the target platform
void RegisterScriptMath3D_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
