/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2011 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
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

#include "materialeditorprefseditor.hxx"

#include <QtCore/QSettings>

MaterialEditorPrefsEditor* mMaterialPrefsEditor = 0;

//----------------------------------------------------------------------------------------
MaterialEditorPrefsEditor::MaterialEditorPrefsEditor(QWidget *parent) : QWidget(parent)
{
    setupUi(this);

    connect(fontSizeSpinBox,    SIGNAL(valueChanged(int)),  this,   SLOT(setDirty()));
    connect(lineBreakCheckBox,  SIGNAL(stateChanged(int)),  this,   SLOT(setDirty()));
}
//----------------------------------------------------------------------------------------
MaterialEditorPrefsEditor::~MaterialEditorPrefsEditor()
{
}
//----------------------------------------------------------------------------------------
void MaterialEditorPrefsEditor::getPreferences(Ogre::NameValuePairList& preferences)
{
    preferences.insert(Ogre::NameValuePairList::value_type("fontSize",
        Ogre::StringConverter::toString(fontSizeSpinBox->value())));

    preferences.insert(Ogre::NameValuePairList::value_type("lineWrapping",
        Ogre::StringConverter::toString(lineBreakCheckBox->isChecked())));
}
//----------------------------------------------------------------------------------------
void* MaterialEditorPrefsEditor::getPreferencesWidget()
{
    QSettings settings;
    settings.beginGroup(getPrefsSectionName().c_str());

    fontSizeSpinBox->setValue(settings.value("fontSize", 10).toUInt());
    lineBreakCheckBox->setChecked(settings.value("lineWrapping", false).toBool());

    settings.endGroup();
    
    return this;
}
//----------------------------------------------------------------------------------------
bool MaterialEditorPrefsEditor::applyPreferences()
{
    if(mMaterialTextEditor->findChildren<QPlainTextEdit*>().size() != 0)
    {
        // Change font size
        QFont font = static_cast<QPlainTextEdit*>(mMaterialTextEditor->findChildren<QPlainTextEdit*>().at(0))->font();
        font.setPointSize(fontSizeSpinBox->value());
        foreach(QPlainTextEdit* pEdit, mMaterialTextEditor->findChildren<QPlainTextEdit*>())
            pEdit->setFont(font);

        // Change line wrapping
        if(lineBreakCheckBox->isChecked())
            foreach(QPlainTextEdit* pEdit, mMaterialTextEditor->findChildren<QPlainTextEdit*>())
                pEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        else
            foreach(QPlainTextEdit* pEdit, mMaterialTextEditor->findChildren<QPlainTextEdit*>())
                pEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    }

    return true;
}
//----------------------------------------------------------------------------------------
void MaterialEditorPrefsEditor::setDirty()
{
    emit isDirty();
}
//----------------------------------------------------------------------------------------