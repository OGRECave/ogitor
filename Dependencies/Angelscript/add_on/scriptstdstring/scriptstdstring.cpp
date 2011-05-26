#include <assert.h>
#include <sstream>
#include "scriptstdstring.h"
#include <string.h> // strstr

using namespace std;

BEGIN_AS_NAMESPACE

static void StringFactoryGeneric(asIScriptGeneric *gen) {
  asUINT length = gen->GetArgDWord(0);
  const char *s = (const char*)gen->GetArgAddress(1);
  string str(s, length);
  gen->SetReturnObject(&str);
}

static void ConstructStringGeneric(asIScriptGeneric * gen) {
  new (gen->GetObject()) string();
}

static void CopyConstructStringGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgObject(0));
  new (gen->GetObject()) string(*a);
}

static void ConstructStringGenericInt(asIScriptGeneric * gen) {
    int *a = static_cast<int*>(gen->GetAddressOfArg(0));
    std::stringstream sstr;
	sstr << *a;
    new (gen->GetObject()) string(sstr.str());
}

static void ConstructStringGenericUInt(asIScriptGeneric * gen) {
    unsigned int *a = static_cast<unsigned int*>(gen->GetAddressOfArg(0));
    std::stringstream sstr;
	sstr << *a;
    new (gen->GetObject()) string(sstr.str());
}

static void ConstructStringGenericDouble(asIScriptGeneric * gen) {
    double *a = static_cast<double*>(gen->GetAddressOfArg(0));
    std::stringstream sstr;
	sstr << *a;
    new (gen->GetObject()) string(sstr.str());
}


static void DestructStringGeneric(asIScriptGeneric * gen) {
  string * ptr = static_cast<string *>(gen->GetObject());
  ptr->~string();
}

static void AssignStringGeneric(asIScriptGeneric *gen) {
  string * a = static_cast<string *>(gen->GetArgObject(0));
  string * self = static_cast<string *>(gen->GetObject());
  *self = *a;
  gen->SetReturnAddress(self);
}

static void AddAssignStringGeneric(asIScriptGeneric *gen) {
  string * a = static_cast<string *>(gen->GetArgObject(0));
  string * self = static_cast<string *>(gen->GetObject());
  *self += *a;
  gen->SetReturnAddress(self);
}

static void StringEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a == *b);
}

static void StringEqualsGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  string * b = static_cast<string *>(gen->GetArgAddress(0));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a == *b);
}

static void StringCmpGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  string * b = static_cast<string *>(gen->GetArgAddress(0));

  int cmp = 0;
  if( *a < *b ) cmp = -1;
  else if( *a > *b ) cmp = 1;

  *(int*)gen->GetAddressOfReturnLocation() = cmp;
}

static void StringNotEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a != *b);
}

static void StringLEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a <= *b);
}

static void StringGEqualGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a >= *b);
}

static void StringLessThanGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a < *b);
}

static void StringGreaterThanGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetArgAddress(0));
  string * b = static_cast<string *>(gen->GetArgAddress(1));
  *(bool*)gen->GetAddressOfReturnLocation() = (*a > *b);
}

static void StringAddGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  string * b = static_cast<string *>(gen->GetArgAddress(0));
  string ret_val = *a + *b;
  gen->SetReturnObject(&ret_val);
}

static void StringLengthGeneric(asIScriptGeneric * gen) {
  string * self = static_cast<string *>(gen->GetObject());
  *static_cast<size_t *>(gen->GetAddressOfReturnLocation()) = self->length();
}

static void StringResizeGeneric(asIScriptGeneric * gen) {
  string * self = static_cast<string *>(gen->GetObject());
  self->resize(*static_cast<size_t *>(gen->GetAddressOfArg(0)));
}

static void StringCharAtGeneric(asIScriptGeneric * gen) {
  unsigned int index = gen->GetArgDWord(0);
  string * self = static_cast<string *>(gen->GetObject());

  if (index >= self->size()) {
    // Set a script exception
    asIScriptContext *ctx = asGetActiveContext();
    ctx->SetException("Out of range");

    gen->SetReturnAddress(0);
  } else {
    gen->SetReturnAddress(&(self->operator [](index)));
  }
}

void AssignInt2StringGeneric(asIScriptGeneric *gen) 
{
	int *a = static_cast<int*>(gen->GetAddressOfArg(0));
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignUInt2StringGeneric(asIScriptGeneric *gen) 
{
	unsigned int *a = static_cast<unsigned int*>(gen->GetAddressOfArg(0));
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignDouble2StringGeneric(asIScriptGeneric *gen) 
{
	double *a = static_cast<double*>(gen->GetAddressOfArg(0));
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << *a;
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AssignBool2StringGeneric(asIScriptGeneric *gen) 
{
	bool *a = static_cast<bool*>(gen->GetAddressOfArg(0));
	string *self = static_cast<string*>(gen->GetObject());
	std::stringstream sstr;
	sstr << *a ? "true" : "false";
	*self = sstr.str();
	gen->SetReturnAddress(self);
}

void AddAssignDouble2StringGeneric(asIScriptGeneric * gen) {
  double * a = static_cast<double *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignInt2StringGeneric(asIScriptGeneric * gen) {
  int * a = static_cast<int *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignUInt2StringGeneric(asIScriptGeneric * gen) {
  unsigned int * a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a;
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddAssignBool2StringGeneric(asIScriptGeneric * gen) {
  bool * a = static_cast<bool *>(gen->GetAddressOfArg(0));
  string * self = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a ? "true" : "false";
  *self += sstr.str();
  gen->SetReturnAddress(self);
}

void AddString2DoubleGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  double * b = static_cast<double *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2IntGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  int * b = static_cast<int *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2UIntGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  unsigned int * b = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddString2BoolGeneric(asIScriptGeneric * gen) {
  string * a = static_cast<string *>(gen->GetObject());
  bool * b = static_cast<bool *>(gen->GetAddressOfArg(0));
  std::stringstream sstr;
  sstr << *a << *b ? "true" : "false";
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddDouble2StringGeneric(asIScriptGeneric * gen) {
  double* a = static_cast<double *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddInt2StringGeneric(asIScriptGeneric * gen) {
  int* a = static_cast<int *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddUInt2StringGeneric(asIScriptGeneric * gen) {
  unsigned int* a = static_cast<unsigned int *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << *a << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void AddBool2StringGeneric(asIScriptGeneric * gen) {
  bool* a = static_cast<bool *>(gen->GetAddressOfArg(0));
  string * b = static_cast<string *>(gen->GetObject());
  std::stringstream sstr;
  sstr << (*a ? "true" : "false") << *b;
  std::string ret_val = sstr.str();
  gen->SetReturnObject(&ret_val);
}

void RegisterStdString_Generic(asIScriptEngine *engine) {
  int r;

  // Register the string type
  r = engine->RegisterObjectType("string", sizeof(string), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); assert( r >= 0 );

  // Register the string factory
  r = engine->RegisterStringFactory("string", asFUNCTION(StringFactoryGeneric), asCALL_GENERIC); assert( r >= 0 );

  // Register the object operator overloads
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructStringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(const string &in)",    asFUNCTION(CopyConstructStringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(int)",                 asFUNCTION(ConstructStringGenericInt), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(uint)",                 asFUNCTION(ConstructStringGenericUInt), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(double)",                 asFUNCTION(ConstructStringGenericDouble), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructStringGeneric),  asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asFUNCTION(AssignStringGeneric),    asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(const string &in)", asFUNCTION(AddAssignStringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTION(StringEqualsGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmpGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(const string &in) const", asFUNCTION(StringAddGeneric), asCALL_GENERIC); assert( r >= 0 );

  // Register the object methods
  if (sizeof(size_t) == 4) {
    r = engine->RegisterObjectMethod("string", "uint length() const", asFUNCTION(StringLengthGeneric), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("string", "void resize(uint)",   asFUNCTION(StringResizeGeneric), asCALL_GENERIC); assert( r >= 0 );
  } else {
    r = engine->RegisterObjectMethod("string", "uint64 length() const", asFUNCTION(StringLengthGeneric), asCALL_GENERIC); assert( r >= 0 );
    r = engine->RegisterObjectMethod("string", "void resize(uint64)",   asFUNCTION(StringResizeGeneric), asCALL_GENERIC); assert( r >= 0 );
  }

  // Register the index operator, both as a mutator and as an inspector
  r = engine->RegisterObjectMethod("string", "uint8 &opIndex(uint)", asFUNCTION(StringCharAtGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "const uint8 &opIndex(uint) const", asFUNCTION(StringCharAtGeneric), asCALL_GENERIC); assert( r >= 0 );

  // Automatic conversion from values
  r = engine->RegisterObjectMethod("string", "string &opAssign(double)", asFUNCTION(AssignDouble2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(double)", asFUNCTION(AddAssignDouble2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(double) const", asFUNCTION(AddString2DoubleGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(double) const", asFUNCTION(AddDouble2StringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "string &opAssign(int)", asFUNCTION(AssignInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(int)", asFUNCTION(AddAssignInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(int) const", asFUNCTION(AddString2IntGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(int) const", asFUNCTION(AddInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "string &opAssign(uint)", asFUNCTION(AssignUInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(uint)", asFUNCTION(AddAssignUInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(uint) const", asFUNCTION(AddString2UIntGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(uint) const", asFUNCTION(AddUInt2StringGeneric), asCALL_GENERIC); assert( r >= 0 );

  r = engine->RegisterObjectMethod("string", "string &opAssign(bool)", asFUNCTION(AssignBool2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string &opAddAssign(bool)", asFUNCTION(AddAssignBool2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd(bool) const", asFUNCTION(AddString2BoolGeneric), asCALL_GENERIC); assert( r >= 0 );
  r = engine->RegisterObjectMethod("string", "string opAdd_r(bool) const", asFUNCTION(AddBool2StringGeneric), asCALL_GENERIC); assert( r >= 0 );
}

static string StringFactory(asUINT length, const char *s)
{
	return string(s, length);
}

static void ConstructString(string *thisPointer)
{
	new(thisPointer) string();
}

static void CopyConstructString(const string &other, string *thisPointer)
{
	new(thisPointer) string(other);
}

static void ConstructStringInt(int a, string *thisPointer) {
    std::stringstream sstr;
	sstr << a;
    new (thisPointer) string(sstr.str());
}

static void ConstructStringUInt(unsigned int a, string *thisPointer) {
    std::stringstream sstr;
	sstr << a;
    new (thisPointer) string(sstr.str());
}

static void ConstructStringDouble(double a, string *thisPointer) {
    std::stringstream sstr;
	sstr << a;
    new (thisPointer) string(sstr.str());
}

static void DestructString(string *thisPointer)
{
	thisPointer->~string();
}

static string &AssignUIntToString(unsigned int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest = stream.str();
	return dest;
}

static string &AddAssignUIntToString(unsigned int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest += stream.str();
	return dest;
}

static string AddStringUInt(const string &str, unsigned int i)
{
	ostringstream stream;
	stream << i;
	return str + stream.str();
}

static string AddIntString(int i, const string &str)
{
	ostringstream stream;
	stream << i;
	return stream.str() + str;
}

static string &AssignIntToString(int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest = stream.str();
	return dest;
}

static string &AddAssignIntToString(int i, string &dest)
{
	ostringstream stream;
	stream << i;
	dest += stream.str();
	return dest;
}

static string AddStringInt(const string &str, int i)
{
	ostringstream stream;
	stream << i;
	return str + stream.str();
}

static string AddUIntString(unsigned int i, const string &str)
{
	ostringstream stream;
	stream << i;
	return stream.str() + str;
}

static string &AssignDoubleToString(double f, string &dest)
{
	ostringstream stream;
	stream << f;
	dest = stream.str();
	return dest;
}

static string &AddAssignDoubleToString(double f, string &dest)
{
	ostringstream stream;
	stream << f;
	dest += stream.str();
	return dest;
}

static string &AssignBoolToString(bool b, string &dest)
{
	ostringstream stream;
	stream << b ? "true" : "false";
	dest = stream.str();
	return dest;
}

static string &AddAssignBoolToString(bool b, string &dest)
{
	ostringstream stream;
	stream << b ? "true" : "false";
	dest += stream.str();
	return dest;
}

static string AddStringDouble(const string &str, double f)
{
	ostringstream stream;
	stream << f;
	return str + stream.str();
}

static string AddDoubleString(double f, const string &str)
{
	ostringstream stream;
	stream << f;
	return stream.str() + str;
}

static string AddStringBool(const string &str, bool b)
{
	ostringstream stream;
	stream << b ? "true" : "false";
	return str + stream.str();
}

static string AddBoolString(bool b, const string &str)
{
	ostringstream stream;
	stream << b ? "true" : "false";
	return stream.str() + str;
}

inline int getArgumentPos(const string &str, int count)
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

static string Arg(const string& f, string &str)
{
    int count = str.length();
    int pos = getArgumentPos(str, count);

    if(pos > -1)
    {
        return str.substr(0, pos) + f + str.substr(pos + 2, count - (pos + 2));
    }

    return str;
}

static string Arg(int n, string &str)
{
    int count = str.length();
    int pos = getArgumentPos(str, count);

    if(pos > -1)
    {
        ostringstream f;
        f << n;
        return str.substr(0, pos) + f.str() + str.substr(pos + 2, count - (pos + 2));
    }

    return str;
}

static string Arg(unsigned int n, string &str)
{
    int count = str.length();
    int pos = getArgumentPos(str, count);

    if(pos > -1)
    {
        ostringstream f;
        f << n;
        return str.substr(0, pos) + f.str() + str.substr(pos + 2, count - (pos + 2));
    }

    return str;
}

static string Arg(double n, string &str)
{
    int count = str.length();
    int pos = getArgumentPos(str, count);

    if(pos > -1)
    {
        ostringstream f;
        f << n;
        return str.substr(0, pos) + f.str() + str.substr(pos + 2, count - (pos + 2));
    }

    return str;
}

static int StringFind(const string &arg, string &str)
{
    return str.find(arg);
}

static void StringReplace(const string &tofind, const string &toreplace, string &str)
{
    int pos = str.find(tofind);
    if(pos != -1)
    {
        str.erase(pos, tofind.length());
        str.insert(pos, toreplace);
    }
}

static string StringSubStr(int st, int len, string &str)
{
    return str.substr(st, len);
}

static char *StringCharAt(unsigned int i, string &str)
{
	if( i >= str.size() )
	{
		// Set a script exception
		asIScriptContext *ctx = asGetActiveContext();
		ctx->SetException("Out of range");

		// Return a null pointer
		return 0;
	}

	return &str[i];
}

static int StringCmp(const string &a, const string &b)
{
	int cmp = 0;
	if( a < b ) cmp = -1;
	else if( a > b ) cmp = 1;
	return cmp;
}

static void Erase(int pos, int length, string &dest)
{
    dest.erase(pos, length);
}

static int StringToInt(string &dest)
{
    std::stringstream f(dest);
    int val = 0;
    f >> val;
    
    return val;
}

static unsigned int StringToUInt(string &dest)
{
    std::stringstream f(dest);
    unsigned int val = 0;
    f >> val;
    
    return val;
}

static double StringToDouble(string &dest)
{
    std::stringstream f(dest);
    double val = 0;
    f >> val;
    
    return val;
}

void RegisterStdString_Native(asIScriptEngine *engine)
{
	int r;

	// Register the string type
	r = engine->RegisterObjectType("string", sizeof(string), asOBJ_VALUE | asOBJ_APP_CLASS_CDA); assert( r >= 0 );

	// Register the string factory
	r = engine->RegisterStringFactory("string", asFUNCTION(StringFactory), asCALL_CDECL); assert( r >= 0 );

	// Register the object operator overloads
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f()",                    asFUNCTION(ConstructString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(const string &in)",    asFUNCTION(CopyConstructString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(int)",                    asFUNCTION(ConstructStringInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(uint)",                    asFUNCTION(ConstructStringUInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(double)",                    asFUNCTION(ConstructStringDouble), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_DESTRUCT,   "void f()",                    asFUNCTION(DestructString),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAssign(const string &in)", asMETHODPR(string, operator =, (const string&), string&), asCALL_THISCALL); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(const string &in)", asMETHODPR(string, operator+=, (const string&), string&), asCALL_THISCALL); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "bool opEquals(const string &in) const", asFUNCTIONPR(operator ==, (const string &, const string &), bool), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "int opCmp(const string &in) const", asFUNCTION(StringCmp), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(const string &in) const", asFUNCTIONPR(operator +, (const string &, const string &), string), asCALL_CDECL_OBJFIRST); assert( r >= 0 );

	// Register the object methods
	if( sizeof(size_t) == 4 )
	{
		r = engine->RegisterObjectMethod("string", "uint length() const", asMETHOD(string,size), asCALL_THISCALL); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint)", asMETHODPR(string,resize,(size_t),void), asCALL_THISCALL); assert( r >= 0 );
	}
	else
	{
		r = engine->RegisterObjectMethod("string", "uint64 length() const", asMETHOD(string,size), asCALL_THISCALL); assert( r >= 0 );
		r = engine->RegisterObjectMethod("string", "void resize(uint64)", asMETHODPR(string,resize,(size_t),void), asCALL_THISCALL); assert( r >= 0 );
	}

    r = engine->RegisterObjectMethod("string", "void erase(int , int)", asFUNCTION(Erase), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    // Register the index operator, both as a mutator and as an inspector
	// Note that we don't register the operator[] directory, as it doesn't do bounds checking
	r = engine->RegisterObjectMethod("string", "uint8 &opIndex(uint)", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "const uint8 &opIndex(uint) const", asFUNCTION(StringCharAt), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	// Automatic conversion from values
	r = engine->RegisterObjectMethod("string", "string &opAssign(double)", asFUNCTION(AssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(double)", asFUNCTION(AddAssignDoubleToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(double) const", asFUNCTION(AddStringDouble), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(double) const", asFUNCTION(AddDoubleString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(int)", asFUNCTION(AssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(int)", asFUNCTION(AddAssignIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(int) const", asFUNCTION(AddStringInt), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(int) const", asFUNCTION(AddIntString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "string &opAssign(uint)", asFUNCTION(AssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(uint)", asFUNCTION(AddAssignUIntToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(uint) const", asFUNCTION(AddStringUInt), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(uint) const", asFUNCTION(AddUIntString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectMethod("string", "string &opAssign(bool)", asFUNCTION(AssignBoolToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string &opAddAssign(bool)", asFUNCTION(AddAssignBoolToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd(bool) const", asFUNCTION(AddStringBool), asCALL_CDECL_OBJFIRST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string opAdd_r(bool) const", asFUNCTION(AddBoolString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    
    r = engine->RegisterObjectMethod("string", "string arg(const string &in) const", asFUNCTIONPR(Arg, (const string&, string&), string), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string arg(int) const", asFUNCTIONPR(Arg, (int, string&), string), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string arg(uint) const", asFUNCTIONPR(Arg, (unsigned int, string&), string), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string arg(double) const", asFUNCTIONPR(Arg, (double, string&), string), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	r = engine->RegisterObjectMethod("string", "int find(const string &in) const", asFUNCTION(StringFind), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "string subString(int, int) const", asFUNCTION(StringSubStr), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "void replaceString(const string &in, const string&in)", asFUNCTION(StringReplace), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "int toInt()", asFUNCTION(StringToInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "uint toUInt()", asFUNCTION(StringToUInt), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectMethod("string", "double toDouble()", asFUNCTION(StringToDouble), asCALL_CDECL_OBJLAST); assert( r >= 0 );
}

void RegisterStdString(asIScriptEngine * engine)
{
	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
		RegisterStdString_Generic(engine);
	else
		RegisterStdString_Native(engine);
}

END_AS_NAMESPACE




