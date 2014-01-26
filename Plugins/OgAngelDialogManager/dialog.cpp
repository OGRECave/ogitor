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

#include "angelscript.h"

#include <string>
#include <cassert>
#include <QtGui/QtGui>

#include "dialog.hxx"

extern asIScriptEngine *DialogManagerScriptEngine;
extern asIScriptModule *getDialogManagerActiveModule();
extern asIScriptContext *getDialogManagerActiveContext();
//-----------------------------------------------------------------------------------------
void destroyWidget(QWidget *obj)
{
    delete obj;
}
//-----------------------------------------------------------------------------------------
QDialog *createDialog(int width, int height, void *guiobj)
{
    QDialog *dlg = new QDialog(QApplication::activeWindow());
    dlg->setFixedSize(width, height);
    return dlg;
}
//-----------------------------------------------------------------------------------------
QVBoxLayout *createVBoxLayout(void *guiobj)
{
    return (new QVBoxLayout());
}
//-----------------------------------------------------------------------------------------
QHBoxLayout *createHBoxLayout(void *guiobj)
{
    return (new QHBoxLayout());
}
//-----------------------------------------------------------------------------------------
QGridLayout *createGridLayout(void *guiobj)
{
    return (new QGridLayout());
}
//-----------------------------------------------------------------------------------------
QPushButton *createButton(const std::string& text, void *guiobj)
{
    return (new ScriptButton(QString(text.c_str())));
}
//-----------------------------------------------------------------------------------------
QPushButton *createButtonEx(const QIcon& icon, const std::string& text, void *guiobj)
{
    return (new ScriptButton(icon, QString(text.c_str())));
}
//-----------------------------------------------------------------------------------------
QLineEdit *createLineEdit(const std::string& text, void *guiobj)
{
    return (new QLineEdit(QString(text.c_str())));
}
//-----------------------------------------------------------------------------------------
QLabel *createLabel(const std::string& text, void *guiobj)
{
    return (new QLabel(QString(text.c_str())));
}
//-----------------------------------------------------------------------------------------
QComboBox *createComboBox(void *guiobj)
{
    return (new QComboBox());
}
//-----------------------------------------------------------------------------------------
QCheckBox *createCheckBox(const std::string& text, void *guiobj)
{
    return (new QCheckBox(QString(text.c_str())));
}
//-----------------------------------------------------------------------------------------
void setWindowTitle(const std::string& title, QDialog *obj)
{
    obj->setWindowTitle(QString(title.c_str()));
}
//-----------------------------------------------------------------------------------------
void processEvents(void *guiobj)
{
	QApplication::processEvents();
}
//-----------------------------------------------------------------------------------------
void RegisterDialogBindings(asIScriptEngine *engine)
{
    int r;

    r = engine->RegisterGlobalProperty("AngelGUI gui", (void*)0xDEADBEEF);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "Dialog@ createDialog(int, int)", asFUNCTION(createDialog), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "VBoxLayout@ createVBoxLayout()", asFUNCTION(createVBoxLayout), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "HBoxLayout@ createHBoxLayout()", asFUNCTION(createHBoxLayout), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "GridLayout@ createGridLayout()", asFUNCTION(createGridLayout), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "Button@ createButton(const string &in)", asFUNCTION(createButton), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "Button@ createButton(const Icon &in, const string &in)", asFUNCTION(createButtonEx), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "LineEdit@ createLineEdit(const string &in)", asFUNCTION(createLineEdit), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "Label@ createLabel(const string &in)", asFUNCTION(createLabel), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "ComboBox@ createComboBox()", asFUNCTION(createComboBox), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("AngelGUI", "CheckBox@ createCheckBox(const string &in)", asFUNCTION(createCheckBox), asCALL_CDECL_OBJLAST);assert(r >= 0);
	r = engine->RegisterObjectMethod("AngelGUI", "void processEvents()", asFUNCTION(processEvents), asCALL_CDECL_OBJLAST);assert(r >= 0);

    r = engine->RegisterObjectMethod("Dialog", "void setLayout(Layout@)", asMETHOD(QDialog, setLayout), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("Dialog", "int exec()", asMETHOD(QDialog, exec), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("Dialog", "int show()", asMETHOD(QDialog, show), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("Dialog", "void destroy()", asFUNCTION(destroyWidget), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("Dialog", "void setWindowTitle(const string &in)", asFUNCTION(setWindowTitle), asCALL_CDECL_OBJLAST);assert(r >= 0);
    r = engine->RegisterObjectMethod("Dialog", "void accept()", asMETHOD(QDialog, accept), asCALL_THISCALL);assert(r >= 0);
    r = engine->RegisterObjectMethod("Dialog", "void reject()", asMETHOD(QDialog, reject), asCALL_THISCALL);assert(r >= 0);
}
//-----------------------------------------------------------------------------------------
void ScriptButton::ConnectScript(const std::string& signal_name, const std::string& func_name)
{
    if(signal_name == "click()")
    {
        click_func_name = func_name;
        connect(this, SIGNAL(clicked()), this, SLOT(onClick()));
    }
}
//-----------------------------------------------------------------------------------------
void ScriptButton::onClick()
{
    if(!click_func_name.empty())
    {
        asIScriptModule *module = getDialogManagerActiveModule();
        asIScriptFunction *funcID = module->GetFunctionByDecl(click_func_name.c_str());

        asIScriptContext *ctx = DialogManagerScriptEngine->CreateContext();
        ctx->Prepare( funcID );
        ctx->Execute();
        ctx->Release();
    }
}
//-----------------------------------------------------------------------------------------