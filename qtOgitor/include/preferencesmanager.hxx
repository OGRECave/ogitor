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

#ifndef PREFERENCESMANAGER_H
#define PREFERENCESMANAGER_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QTreeWidget>

#include <QtCore/QSignalMapper>
#include <QtCore/QObject>
#include <QtCore/QSettings>

#include <map>

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "qtogitorsystem.hxx"
#include "ogitorpreferenceswidget.hxx"
#include "shortcutsettings.hxx"

class PreferencesManager : public QObject
{
    Q_OBJECT

    struct PreferencesSection
    {
        QString         identifier;
        QString         sectionName;
        QTreeWidgetItem *treeItem;
        QWidget         *widget;
    };

public:
    PreferencesManager(QWidget *parent = 0);
    virtual ~PreferencesManager();

    void addPluginSection(QString identifier, QString sectionImagePath, QString sectionName, QWidget *tabWidget);
    void showDialog();
    
public Q_SLOTS:
    void showSection(QString identifier);
    void onApply();
    void onAccept();
    void onIsDirty();
    void selectionChanged();
    
private:
    void addCoreSection(QString identifier, QString sectionImagePath,QString sectionName, QWidget *tabWidget);
    void createPreferencesDialog(QWidget *parent);
    void setupSections();
    void savePreferences();
    
    QTreeWidget             *mTreeWidget;
    QTreeWidgetItem         *mTreeWidgetRoot;
    QTreeWidgetItem         *mPluginsRootItem;
    QSettings               *mOgitorSettings;
    QSpacerItem             *mButtonSpacer;
    QWidget                 *mParentWidget;
    QDialog                 *mPrefDlg;
    QHBoxLayout             *mContentsLayout;
    QVBoxLayout             *mButtonsLayout;
    QDialogButtonBox        *mBtnBox;
    int                      mSectionCount;
    PreferencesSection      *mCurrentSection;
    OgitorPreferencesWidget *mOgitorPrefWidget;
    ShortCutSettings        *mShortCutSettings;
    
    std::map<QString, PreferencesSection*>  mPreferencesSections;
    Ogitors::PreferencesEditorDataList      mPreferencesEditors;
};

#endif // PREFERENCESMANAGER_H
