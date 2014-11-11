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

#ifndef OGITORPREFERENCESWIDGET_HXX
#define OGITORPREFERENCESWIDGET_HXX

//----------------------------------------------------------------------------------------

#include <QtWidgets/QWidget>
#include <QtWidgets/QTreeWidget>

#include <QtCore/QMap>
#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"

#include "ui_ogitorpreferencestab.h"

//----------------------------------------------------------------------------------------

class OgitorPreferencesWidget : public QWidget, Ogitors::PreferencesEditor, Ui::ogitorPreferencesWidget
{
    Q_OBJECT
public:
    OgitorPreferencesWidget(Ogre::String prefSectionName, QWidget *parent = 0);
    virtual ~OgitorPreferencesWidget();

    void     getPreferences(Ogre::NameValuePairList& preferences);
    void    *getPreferencesWidget();
    bool     applyPreferences();

public Q_SLOTS:
    void setDirty();
    void keyboardLayoutChanged();
    void languageChanged();
    void treeChanged(QTreeWidgetItem* item, int row);
    void renderSystemChanged();
    void VSyncChanged();
    
Q_SIGNALS:
    void isDirty();

private:
    QTreeWidget *treeWidget;

    void fillOgreTab();
    
    bool                    mKeyboardLayoutChanged;
    bool                    mPluginsChanged;
    bool                    mLanguageChanged;
    bool                    mRenderSystemChanged;
    bool                    mVSyncChanged;
    QMap<QString, QString>  mLanguageMap;
};

//----------------------------------------------------------------------------------------

#endif // OGITORPREFERENCESWIDGET_HXX

//----------------------------------------------------------------------------------------
