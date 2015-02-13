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

#include "angelscript.h"

#include <cassert>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>

#include "inputwidgets.hxx"
#include "dialog.hxx"

//-----------------------------------------------------------------------------------------
// Example REF_CAST behaviour
template<class A, class B>
B* refCast(A* a)
{
    // If the handle already is a null handle, then just return the null handle
    if( !a ) return 0;

    B* b = static_cast<B*>(a);

    return b;
}
//-----------------------------------------------------------------------------------------
void _addRef(void* a)
{
}
//-----------------------------------------------------------------------------------------
void _release(void* a)
{
}
//-----------------------------------------------------------------------------------------
static void ConstructStringQString(const QString& arg, std::string *thisPointer)
{
    new(thisPointer) std::string(arg.toStdString());
}
//-----------------------------------------------------------------------------------------
static void ConstructQString(QString *thisPointer)
{
	new(thisPointer) QString();
}
//-----------------------------------------------------------------------------------------
static void ConstructQString2(const std::string& arg, QString *thisPointer)
{
    new(thisPointer) QString(arg.c_str());
}
//-----------------------------------------------------------------------------------------
static void DestructQString(QString *thisPointer)
{
	thisPointer->~QString();
}
//-----------------------------------------------------------------------------------------
static void ConstructQIcon(const std::string& arg, QIcon *thisPointer)
{
    new(thisPointer) QIcon(QString(arg.c_str()));
}
//-----------------------------------------------------------------------------------------
static void ConstructQIcon2(const QString& arg, QIcon *thisPointer)
{
    new(thisPointer) QIcon(arg);
}
//-----------------------------------------------------------------------------------------
static void DestructQIcon(QIcon *thisPointer)
{
	thisPointer->~QIcon();
}
//-----------------------------------------------------------------------------------------
static std::string &AssignQStringToString(QString& s, std::string &dest)
{
    dest = s.toStdString();
	return dest;
}
//-----------------------------------------------------------------------------------------
static QString &AssignStringToQString(std::string& s, QString &dest)
{
    dest = s.c_str();
	return dest;
}
//-----------------------------------------------------------------------------------------
static void addItemToCombo(const std::string& s, QComboBox &box)
{
    box.addItem(QString(s.c_str()));
}
//-----------------------------------------------------------------------------------------
static void addItemToCombo2(const QIcon &ic, const std::string& s, QComboBox &box)
{
    box.addItem(ic, QString(s.c_str()));
}
//-----------------------------------------------------------------------------------------
static void addItemToCombo3(const QString& s, QComboBox &box)
{
    box.addItem(s);
}
//-----------------------------------------------------------------------------------------
static void addItemToCombo4(const QString& s, QComboBox &box)
{
    box.addItem(s);
}
//-----------------------------------------------------------------------------------------
static void setComboIconSize(int a1, int a2, QComboBox &box)
{
    box.setIconSize(QSize(a1, a2));
}
//-----------------------------------------------------------------------------------------
#define REGISTER_REFERENCE_WIDGETOBJECT( name, classname )\
{\
    r = engine->RegisterObjectType(name, 0, asOBJ_REF);assert(r >= 0);\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asFUNCTION(_addRef), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asFUNCTION(_release), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour("Widget", asBEHAVE_REF_CAST, name "@ f()", asFUNCTION((refCast<QWidget, classname>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_IMPLICIT_REF_CAST, "Widget@ f()", asFUNCTION((refCast<classname, QWidget>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectMethod(name, "void setMinimumSize(int, int)", asMETHODPR(classname, setMinimumSize, (int,int), void), asCALL_THISCALL); assert( r >= 0 );\
    r = engine->RegisterObjectMethod(name, "void setMaximumSize(int, int)", asMETHODPR(classname, setMinimumSize, (int,int), void), asCALL_THISCALL); assert( r >= 0 );\
    r = engine->RegisterObjectMethod(name, "void setFixedSize(int, int)", asMETHODPR(classname, setMinimumSize, (int,int), void), asCALL_THISCALL); assert( r >= 0 );\
}\
//-----------------------------------------------------------------------------------------
#define REGISTER_REFERENCE_LAYOUTOBJECT( name, classname )\
{\
    r = engine->RegisterObjectType(name, 0, asOBJ_REF);assert(r >= 0);\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_ADDREF, "void f()", asFUNCTION(_addRef), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_RELEASE, "void f()", asFUNCTION(_release), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour("Layout", asBEHAVE_REF_CAST, name "@ f()", asFUNCTION((refCast<QLayout, classname>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
    r = engine->RegisterObjectBehaviour(name, asBEHAVE_IMPLICIT_REF_CAST, "Layout@ f()", asFUNCTION((refCast<classname, QLayout>)), asCALL_CDECL_OBJLAST); assert( r >= 0 );\
}\
//-----------------------------------------------------------------------------------------
void RegisterInputWidgetBindings(asIScriptEngine *engine)
{
    int r;

    // Register class definitions since all others need it
    r = engine->RegisterObjectType("AngelGUI", 0, asOBJ_REF | asOBJ_NOHANDLE);assert(r >= 0);

    r = engine->RegisterEnum("GUIAlignment");assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignNone", 0);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignLeft",Qt::AlignLeft);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignLeading",Qt::AlignLeading);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignRight",Qt::AlignRight);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignTrailing",Qt::AlignTrailing);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignHCenter",Qt::AlignHCenter);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignJustify",Qt::AlignJustify);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignAbsolute",Qt::AlignAbsolute);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignTop",Qt::AlignTop);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignBottom",Qt::AlignBottom);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignVCenter",Qt::AlignVCenter);assert(r >= 0);
    r = engine->RegisterEnumValue("GUIAlignment", "AlignCenter",Qt::AlignCenter);assert(r >= 0);

    r = engine->RegisterObjectType("GUIString", sizeof(QString), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDA); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("GUIString", asBEHAVE_CONSTRUCT,  "void f()",	asFUNCTION(ConstructQString),	asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("GUIString", asBEHAVE_CONSTRUCT,  "void f(const string &in)",	asFUNCTION(ConstructQString2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("GUIString", asBEHAVE_DESTRUCT,  "void f()",	asFUNCTION(DestructQString),	asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("GUIString", "GUIString &opAssign(string &in)", asFUNCTION(AssignStringToQString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	//Create an assignment operator for "string" too
	r = engine->RegisterObjectMethod("string", "string &opAssign(GUIString &in)", asFUNCTION(AssignQStringToString), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("string", asBEHAVE_CONSTRUCT,  "void f(const GUIString &in)", asFUNCTION(ConstructStringQString), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectType("Icon", sizeof(QIcon), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_CDA); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Icon", asBEHAVE_CONSTRUCT,  "void f(const string &in)",	asFUNCTION(ConstructQIcon),	asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Icon", asBEHAVE_CONSTRUCT,  "void f(const GUIString &in)",	asFUNCTION(ConstructQIcon2),	asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Icon", asBEHAVE_DESTRUCT,  "void f()",	asFUNCTION(DestructQIcon),	asCALL_CDECL_OBJLAST); assert(r >= 0);

    r = engine->RegisterObjectType("Widget", 0, asOBJ_REF);assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Widget", asBEHAVE_ADDREF, "void f()", asFUNCTION(_addRef), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Widget", asBEHAVE_RELEASE, "void f()", asFUNCTION(_release), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectType("Layout", 0, asOBJ_REF);assert(r >= 0);
    r = engine->RegisterObjectBehaviour("Layout", asBEHAVE_ADDREF, "void f()", asFUNCTION(_addRef), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour("Layout", asBEHAVE_RELEASE, "void f()", asFUNCTION(_release), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    REGISTER_REFERENCE_LAYOUTOBJECT("VBoxLayout", QVBoxLayout);
    r = engine->RegisterObjectMethod("VBoxLayout", "void addLayout(Layout@, int)",asMETHOD(QVBoxLayout, addLayout), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("VBoxLayout", "void addWidget(Widget@, int, GUIAlignment)",asMETHOD(QVBoxLayout, addWidget), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("VBoxLayout", "void setStretch(int, int)",asMETHOD(QVBoxLayout, setStretch), asCALL_THISCALL); assert( r >= 0 );

    REGISTER_REFERENCE_LAYOUTOBJECT("HBoxLayout", QHBoxLayout);
    r = engine->RegisterObjectMethod("HBoxLayout", "void addLayout(Layout@, int)",asMETHOD(QHBoxLayout, addLayout), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HBoxLayout", "void addWidget(Widget@, int, GUIAlignment)",asMETHOD(QHBoxLayout, addWidget), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("HBoxLayout", "void setStretch(int, int)",asMETHOD(QHBoxLayout, setStretch), asCALL_THISCALL); assert( r >= 0 );

    REGISTER_REFERENCE_LAYOUTOBJECT("GridLayout", QGridLayout);
    r = engine->RegisterObjectMethod("GridLayout", "void addLayout(Layout@, int, int, GUIAlignment)",asMETHODPR(QGridLayout, addLayout, ( QLayout *, int, int, Qt::Alignment), void ), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GridLayout", "void addLayout(Layout@, int, int, int, int, GUIAlignment)",asMETHODPR(QGridLayout, addLayout, ( QLayout *, int, int, int, int, Qt::Alignment), void ), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GridLayout", "void addWidget(Widget@, int, int, GUIAlignment)",asMETHODPR(QGridLayout, addWidget, ( QWidget *, int, int, Qt::Alignment), void ), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GridLayout", "void addWidget(Widget@, int, int, int, int, GUIAlignment)",asMETHODPR(QGridLayout, addWidget, ( QWidget *, int, int, int, int, Qt::Alignment), void ), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GridLayout", "void setRowStretch(int, int)",asMETHOD(QGridLayout, setRowStretch), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("GridLayout", "void setColumnStretch(int, int)",asMETHOD(QGridLayout, setColumnStretch), asCALL_THISCALL); assert( r >= 0 );

    REGISTER_REFERENCE_WIDGETOBJECT("Dialog", QDialog);

    REGISTER_REFERENCE_WIDGETOBJECT("Button", QPushButton);
    r = engine->RegisterObjectMethod("Button", "void connect(string &in, string& in)",asMETHOD(ScriptButton, ConnectScript), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("Button", "void setText(GUIString &in)",asMETHOD(QPushButton, setText), asCALL_THISCALL); assert( r >= 0 );

    REGISTER_REFERENCE_WIDGETOBJECT("CheckBox", QCheckBox);
    r = engine->RegisterObjectMethod("CheckBox", "bool isChecked()",asMETHOD(QCheckBox, isChecked), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("CheckBox", "void setChecked(bool)",asMETHOD(QCheckBox, setChecked), asCALL_THISCALL); assert( r >= 0 );

    REGISTER_REFERENCE_WIDGETOBJECT("ComboBox", QComboBox);
    r = engine->RegisterObjectMethod("ComboBox", "void addItem(const string &in)", asFUNCTION(addItemToCombo), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ComboBox", "void addItem(const Icon &in, const string &in)", asFUNCTION(addItemToCombo2), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    //r = engine->RegisterObjectMethod("ComboBox", "void addItem(const GUIString &in)", asFUNCTION(addItemToCombo3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    //r = engine->RegisterObjectMethod("ComboBox", "void addItem(const Icon &in, const GUIString &in)", asFUNCTION(addItemToCombo4), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ComboBox", "void setIconSize(int, int)", asFUNCTION(setComboIconSize), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ComboBox", "int getCurrentIndex()", asMETHOD(QComboBox, currentIndex), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ComboBox", "GUIString getCurrentText()", asMETHOD(QComboBox, currentText), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ComboBox", "void setCurrentIndex(int)", asMETHOD(QComboBox, setCurrentIndex), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ComboBox", "GUIString getItemText(int)", asMETHOD(QComboBox, itemText), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("ComboBox", "void clear()", asMETHOD(QComboBox, clear), asCALL_THISCALL); assert( r >= 0 );

    REGISTER_REFERENCE_WIDGETOBJECT("LineEdit", QLineEdit);
    r = engine->RegisterObjectMethod("LineEdit", "void setText(GUIString &in)",asMETHOD(QLineEdit, setText), asCALL_THISCALL); assert( r >= 0 );
    r = engine->RegisterObjectMethod("LineEdit", "GUIString getText()",asMETHOD(QLineEdit, text), asCALL_THISCALL); assert( r >= 0 );

    REGISTER_REFERENCE_WIDGETOBJECT("Label", QLabel);
}
//-----------------------------------------------------------------------------------------