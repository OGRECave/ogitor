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

#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>

#include <QtGui/QMessageBox>
#include <QtGui/QPlainTextEdit>

#include "ogitorpreferenceswidget.hxx"
#include "mainwindow.hxx"
#include "generictexteditor.hxx"

#include "OgreRoot.h"
#include "BaseEditor.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"

extern bool    ViewKeyboard[1024];

//----------------------------------------------------------------------------------------
OgitorPreferencesWidget::OgitorPreferencesWidget(Ogre::String prefSectionName, QWidget *parent) 
: QWidget(parent)
{
    mPluginsChanged        = false;
    mLanguageChanged       = false;
    mRenderSystemChanged   = false;    
    mVSyncChanged          = false;
    mKeyboardLayoutChanged = false;

    setPrefsSectionName(prefSectionName);

    setupUi(this);
    styleSheetList->addItem(":/stylesheets/obsidian.qss");
    styleSheetList->addItem(":/stylesheets/KDE4.qss");
    styleSheetList->addItem(":/stylesheets/aqua.qss");
    styleSheetList->addItem(":/stylesheets/dark.qss");

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    styleSheetList->setCurrentIndex(1);
#else
    styleSheetList->setCurrentIndex(0);
#endif

    QDir myDir(QString(Ogitors::Globals::LANGUAGE_PATH.c_str()));
    QStringList list = myDir.entryList(QStringList("ogitor_*.qm")); // filter only translation files

    /** Add System Default Option Manually */
    languageFileList->addItem(tr("Use System Default..."));
    mLanguageMap[tr("Use System Default...")] = "";

    for (int i = 0; i < list.size(); ++i)
    {
        QString str = list.at(i);
        str.remove("ogitor_");
        str.remove(".qm");
        QLocale loc(str);
        languageFileList->addItem(QLocale::languageToString(loc.language()));
        mLanguageMap[QLocale::languageToString(loc.language())] = list.at(i);
    }

    /** Add English Option Manually */
    languageFileList->addItem("English");
    mLanguageMap["English"] = "ogitor_en.qm";

    langGroupBox->setToolTip(tr("System locale: ") + QLocale::system().name());

    fillOgreTab();

    QVBoxLayout *vlayout = new QVBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout();

    treeWidget = new QTreeWidget();
    treeWidget->setHeaderLabel(tr("List of all loaded Plugins"));

    const Ogitors::PluginEntryMap *pluginmap = Ogitors::OgitorsRoot::getSingletonPtr()->GetPluginMap();

    Ogitors::PluginEntryMap::const_iterator it = pluginmap->begin();

    unsigned int t;
    QTreeWidgetItem *topItem;
    QTreeWidgetItem *catItem;
    QTreeWidgetItem *subItem;

    // Make sure plugin list is sorted alphabetically:
    // -> store the values of plugin name and plugin identifier taken from the pluginmap in a vector
    // that gets then sorted and based on the sorted vector process the entries in the pluginmap
    std::vector<std::pair<Ogre::String, void*> > vec;
    while(it != pluginmap->end())
    {
        vec.push_back(std::pair<Ogre::String, void*>(it->second.mName, it->first));
        it++;
    }
    std::sort(vec.begin(), vec.end());

    std::vector<std::pair<Ogre::String, void*> >::const_iterator it_vec = vec.begin();

    // Step through the plugins and collect plugin information
    while(it_vec != vec.end())
    {
        it = pluginmap->find(it_vec->second);

        topItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(it->second.mName.c_str())));
        if(it->second.mLoaded)
            topItem->setCheckState(0, Qt::Checked);
        else
            topItem->setCheckState(0, Qt::Unchecked);

        if(it->second.mLoadingError)
            topItem->setForeground(0, QBrush(QColor(255, 0, 0)));

        topItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        topItem->setData(0, Qt::UserRole, QString(it->second.mPluginPath.c_str()));
        treeWidget->addTopLevelItem(topItem);

        if(it->second.mEditorObjects.size() > 0)
        {
            catItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Editor Objects")));
            catItem->setIcon(0, QIcon(":/icons/objects.svg"));
            topItem->addChild(catItem);

            for(t = 0;t < it->second.mEditorObjects.size();t++)
            {
                subItem =  new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(it->second.mEditorObjects[t].c_str())));
                subItem->setIcon(0, QIcon(":/icons/bullet.svg"));
                catItem->addChild(subItem);
            }
        }

        if(it->second.mSerializers.size() > 0)
        {
            catItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Scene Serializers")));
            catItem->setIcon(0, QIcon(":/icons/import.svg"));
            topItem->addChild(catItem);

            for(t = 0;t < it->second.mSerializers.size();t++)
            {
                subItem =  new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(it->second.mSerializers[t].c_str())));
                subItem->setIcon(0, QIcon(":/icons/bullet.svg"));
                catItem->addChild(subItem);
            }
        }

        if(it->second.mScriptInterpreters.size() > 0)
        {
            catItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Script Interpreters")));
            catItem->setIcon(0, QIcon(":/icons/script.svg"));
            topItem->addChild(catItem);

            for(t = 0;t < it->second.mScriptInterpreters.size();t++)
            {
                subItem =  new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(it->second.mScriptInterpreters[t].c_str())));
                subItem->setIcon(0, QIcon(":/icons/bullet.svg"));
                catItem->addChild(subItem);
            }
        }

        if(it->second.mTabWidgets.size() > 0)
        {
            catItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Tab Widgets")));
            catItem->setIcon(0, QIcon(":/icons/showhide.svg"));
            topItem->addChild(catItem);

            for(t = 0;t < it->second.mTabWidgets.size();t++)
            {
                try
                {
                    subItem =  new QTreeWidgetItem((QTreeWidget*)0, QStringList(static_cast<QWidget*>(it->second.mTabWidgets[t])->objectName()));
                    subItem->setIcon(0, QIcon(":/icons/bullet.svg"));
                    catItem->addChild(subItem);
                }
                catch(...)
                {
                }
            }
        }

        if(it->second.mDockWidgets.size() > 0)
        {
            catItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Dock Widgets")));
            catItem->setIcon(0, QIcon(":/icons/showhide.svg"));
            topItem->addChild(catItem);

            for(t = 0;t < it->second.mDockWidgets.size();t++)
            {
                try
                {
                    subItem =  new QTreeWidgetItem((QTreeWidget*)0, QStringList(static_cast<QWidget*>(it->second.mDockWidgets[t])->objectName()));
                    subItem->setIcon(0, QIcon(":/icons/bullet.svg"));
                    catItem->addChild(subItem);
                }
                catch(...)
                {
                }
            }
        }

        if(it->second.mToolbars.size() > 0)
        {
            catItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Toolbars")));
            catItem->setIcon(0, QIcon(":/icons/toolbar.svg"));
            topItem->addChild(catItem);

            for(t = 0;t < it->second.mToolbars.size();t++)
            {
                try
                {
                    subItem =  new QTreeWidgetItem((QTreeWidget*)0, QStringList(static_cast<QWidget*>(it->second.mToolbars[t])->objectName()));
                    subItem->setIcon(0, QIcon(":/icons/bullet.svg"));
                    catItem->addChild(subItem);
                }
                catch(...)
                {
                }
            }
        }

        if(it->second.mPrefWidgets.size() > 0)
        {
            catItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Preferences Editor")));
            catItem->setIcon(0, QIcon(":/icons/preferences.svg"));
            topItem->addChild(catItem);
        }

        it_vec++;
    }

    hlayout->addWidget(treeWidget);
    vlayout->addLayout(hlayout);
    tabPluginsInfo->setLayout(vlayout);
}
//----------------------------------------------------------------------------------------
OgitorPreferencesWidget::~OgitorPreferencesWidget()
{
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::getPreferences(Ogre::NameValuePairList& preferences)
{
    preferences.insert(Ogre::NameValuePairList::value_type("useAZERTY",
        Ogre::StringConverter::toString(azertyCheckBox->isChecked())));

    preferences.insert(Ogre::NameValuePairList::value_type("skipSplash",
        Ogre::StringConverter::toString(splashscreenCheckBox->isChecked())));

    preferences.insert(Ogre::NameValuePairList::value_type("loadLastLoadedScene",
        Ogre::StringConverter::toString(loadLastCheckBox->isChecked())));

    preferences.insert(Ogre::NameValuePairList::value_type("customStyleSheet",
        Ogre::String(styleSheetList->currentText().toStdString())));

    preferences.insert(Ogre::NameValuePairList::value_type("fontSize",
        Ogre::StringConverter::toString(fontSizeSpinBox->value())));

    preferences.insert(Ogre::NameValuePairList::value_type("lineWrapping",
        Ogre::StringConverter::toString(lineBreakCheckBox->isChecked())));

    QString langFileName = languageFileList->itemText(0);
    if(languageFileList->currentIndex() != 0)
        langFileName = mLanguageMap.value(languageFileList->currentText());

    preferences.insert(Ogre::NameValuePairList::value_type("customLanguage",
        Ogre::String(langFileName.toStdString())));
    preferences.insert(Ogre::NameValuePairList::value_type("useVSync",
        Ogre::StringConverter::toString(useVSyncCheckBox->isChecked())));
    preferences.insert(Ogre::NameValuePairList::value_type("antiAliasing",
        Ogre::String(antiAlliasingComboBox->currentText().toStdString())));
    preferences.insert(Ogre::NameValuePairList::value_type("renderSystem",
        Ogre::String(renderSystemComboBox->currentText().toStdString())));

    // Delete existing plugin usage settings
    QSettings settings;
    settings.beginGroup(QString(getPrefsSectionName().c_str()) + "disabledPlugins");
    settings.clear();

    // Store plugin usage
    QString disabledPluginPrefString;
    for(int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        if(treeWidget->topLevelItem(i)->checkState(0) == Qt::Unchecked)
        {
            disabledPluginPrefString = "disabledPlugins/" + treeWidget->topLevelItem(i)->text(0);

            preferences.insert(Ogre::NameValuePairList::value_type(disabledPluginPrefString.toStdString(),
                Ogre::String(treeWidget->topLevelItem(i)->data(0, Qt::UserRole).toString().toStdString())));
        }
    }
}
//----------------------------------------------------------------------------------------
void *OgitorPreferencesWidget::getPreferencesWidget()
{
    QSettings settings;
    settings.beginGroup(getPrefsSectionName().c_str());

    styleSheetList->setCurrentIndex(styleSheetList->findText(settings.value("customStyleSheet").toString()));;  

    loadLastCheckBox->setChecked(settings.value("loadLastLoadedScene", false).toBool());
    splashscreenCheckBox->setChecked(settings.value("skipSplash", false).toBool());
    azertyCheckBox->setChecked(settings.value("useAZERTY", false).toBool());

    QString style = settings.value("customStyleSheet").toString();
    int result = styleSheetList->findText(style);
    if(result > -1)
        styleSheetList->setCurrentIndex(result);
    else {
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        styleSheetList->setCurrentIndex(1);
#else
        styleSheetList->setCurrentIndex(0);
#endif
    }

    QString lang = settings.value("customLanguage").toString();
    QMapIterator<QString, QString> i(mLanguageMap);
    while (i.hasNext()) 
    {
        i.next();
        if(lang == i.value())
        {
            languageFileList->setCurrentIndex(languageFileList->findText(i.key()));
            break;
        }
    }
    mLanguageChanged = false;
    mPluginsChanged = false;

    fontSizeSpinBox->setValue(settings.value("fontSize", 10).toUInt());
    lineBreakCheckBox->setChecked(settings.value("lineWrapping", false).toBool());

    if(antiAlliasingComboBox->findText(settings.value("antiAliasing").toString()) >= 0)
        antiAlliasingComboBox->setCurrentIndex(antiAlliasingComboBox->findText(settings.value("antiAliasing").toString()));
    if(renderSystemComboBox->findText(settings.value("renderSystem").toString()) >= 0)
        renderSystemComboBox->setCurrentIndex(renderSystemComboBox->findText(settings.value("renderSystem").toString()));

    useVSyncCheckBox->setChecked(settings.value("useVSync").toBool());

    // Load plugin usage
    const Ogitors::PluginEntryMap* pPluginMap = Ogitors::OgitorsRoot::getSingletonPtr()->GetPluginMap();
    Ogitors::PluginEntryMap::const_iterator it = pPluginMap->begin();
    std::vector<std::pair<Ogre::String, void*> > vec;
    while(it != pPluginMap->end())
    {
        vec.push_back(std::pair<Ogre::String, void*>(it->second.mPluginPath, it->first));
        it++;
    }

    settings.beginGroup("/disabledPlugins");
    std::vector<std::pair<Ogre::String, void*> >::const_iterator it_vec;
    for(int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {
        it_vec = vec.begin();
        while(it_vec != vec.end())
        {
            if(vec.begin()->first == Ogre::String(treeWidget->topLevelItem(i)->data(0, Qt::UserRole).toString().toStdString()))
            {
                it = pPluginMap->find(it_vec->second);

                if(settings.value(it->second.mPluginPath.c_str(), true).toBool() == true)
                    treeWidget->topLevelItem(i)->setCheckState(0, Qt::Checked);
                else
                    treeWidget->topLevelItem(i)->setCheckState(0, Qt::Unchecked);
                break;
            }
            else
                it_vec++;
        }          
    }

    settings.endGroup();
    applyPreferences();

    connect(azertyCheckBox,             SIGNAL(stateChanged(int)),                  this, SLOT(keyboardLayoutChanged()));
    connect(splashscreenCheckBox,       SIGNAL(stateChanged(int)),                  this, SLOT(setDirty()));
    connect(loadLastCheckBox,           SIGNAL(stateChanged(int)),                  this, SLOT(setDirty()));
    connect(styleSheetList,             SIGNAL(currentIndexChanged(int)),           this, SLOT(setDirty()));
    connect(languageFileList,           SIGNAL(currentIndexChanged(int)),           this, SLOT(setDirty()));
    connect(languageFileList,           SIGNAL(currentIndexChanged(int)),           this, SLOT(languageChanged()));
    connect(fontSizeSpinBox,            SIGNAL(valueChanged(int)),                  this, SLOT(setDirty()));
    connect(lineBreakCheckBox,          SIGNAL(stateChanged(int)),                  this, SLOT(setDirty()));
    connect(renderSystemComboBox,       SIGNAL(currentIndexChanged(int)),           this, SLOT(renderSystemChanged()));
    connect(useVSyncCheckBox,           SIGNAL(stateChanged(int)),                  this, SLOT(VSyncChanged()));
    connect(antiAlliasingComboBox,      SIGNAL(currentIndexChanged(int)),           this, SLOT(setDirty()));
    connect(treeWidget,                 SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(treeChanged(QTreeWidgetItem*, int)));

    return this;
}
//----------------------------------------------------------------------------------------
bool OgitorPreferencesWidget::applyPreferences()
{
    // Use default should always be at index 0
    if(styleSheetList->currentIndex() != 0)
    {
        if(!QFile::exists(styleSheetList->currentText()))
        {
            QMessageBox::warning(QApplication::activeWindow(),tr("Preferences"), tr("Cannot find the requested StyleSheet"), QMessageBox::Ok);
            return false;
        }
        QFile file(styleSheetList->currentText());
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }
    else
    {
        QFile file(styleSheetList->currentText());
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
    }

    QList<QPlainTextEdit*> list = mOgitorMainWindow->getGenericTextEditor()->findChildren<QPlainTextEdit*>();
    QFont font;
    QPlainTextEdit::LineWrapMode mode;

    if(list.size() > 0)
    {
        font = list.at(0)->font();
        font.setPointSize(fontSizeSpinBox->value());
    }

    if(lineBreakCheckBox->isChecked())
        mode = QPlainTextEdit::WidgetWidth;
    else
        mode = QPlainTextEdit::NoWrap;

    for(int i = 0; i < list.size(); i++)
    {
        list.at(i)->setFont(font);
        list.at(i)->setLineWrapMode(mode);
    }

    if(mLanguageChanged)
        QMessageBox::warning(QApplication::activeWindow(), tr("Preferences"), tr("Language will be changed when Ogitor is restarted!"), QMessageBox::Ok);
    mLanguageChanged = false;

    if(mPluginsChanged)
        QMessageBox::warning(QApplication::activeWindow(), tr("Preferences"), tr("Plugin usage will be changed when Ogitor is restarted!"), QMessageBox::Ok);
    mPluginsChanged = false;

    if(mRenderSystemChanged)
        QMessageBox::warning(QApplication::activeWindow(), tr("Preferences"), tr("Render System will be changed when Ogitor is restarted!"), QMessageBox::Ok);
    mRenderSystemChanged = false;

    if(mVSyncChanged)
        QMessageBox::warning(QApplication::activeWindow(), tr("Preferences"), tr("VSync usage will be changed when Ogitor is restarted!"), QMessageBox::Ok);
    mVSyncChanged = false;

    if(mKeyboardLayoutChanged)
    {
        Ogitors::OgitorsSpecialKeys keys = Ogitors::CViewportEditor::GetKeyboard();

        if(azertyCheckBox->isChecked())
        {
            // AZERTY keys
            keys.SPK_LEFT = Qt::Key_Q;
            keys.SPK_FORWARD = Qt::Key_Z;
            keys.SPK_DOWN = Qt::Key_A;
        }
        else
        {
            // normal keys
            keys.SPK_LEFT = Qt::Key_A;
            keys.SPK_FORWARD = Qt::Key_W;
            keys.SPK_DOWN = Qt::Key_Q;
        }

        Ogitors::CViewportEditor::SetKeyboard(ViewKeyboard, keys);
    }

    /*// Unload plugins
    const Ogitors::PluginEntryMap* pPluginMap = Ogitors::OgitorsRoot::getSingletonPtr()->GetPluginMap();
    Ogitors::PluginEntryMap::const_iterator it = pPluginMap->begin();
    std::vector<std::pair<Ogre::String, void*> > vec;
    while(it != pPluginMap->end())
    {
    vec.push_back(std::pair<Ogre::String, void*>(it->second.mPluginPath, it->first));
    it++;
    }

    std::vector<std::pair<Ogre::String, void*> >::const_iterator it_vec;
    for(int i = 0; i < treeWidget->topLevelItemCount(); i++)
    {    
    if(treeWidget->topLevelItem(i)->checkState(0) == Qt::Unchecked)
    {
    // Search the matching pluginPath that was passed on via the tree user data
    it_vec = vec.begin();
    while(it_vec != vec.end())
    {
    if(vec.begin()->first == Ogre::String(treeWidget->topLevelItem(i)->data(0, Qt::UserRole).toString().toStdString()))
    {
    it = pPluginMap->find(it_vec->second);
    Ogitors::OgitorsRoot::getSingletonPtr()->UnLoadPlugin(it->first);
    break;
    }
    else
    it_vec++;
    }   
    }
    }*/

    return true;
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::fillOgreTab()
{
    Ogre::RenderSystemList::const_iterator pRend = Ogre::Root::getSingletonPtr()->getAvailableRenderers().begin();
    while (pRend != Ogre::Root::getSingletonPtr()->getAvailableRenderers().end())
    {
        renderSystemComboBox->addItem((*pRend)->getName().c_str());
        pRend++;
    }

    antiAlliasingComboBox->addItem("0");
    antiAlliasingComboBox->addItem("2");
    antiAlliasingComboBox->addItem("4");
    antiAlliasingComboBox->addItem("8");
    antiAlliasingComboBox->addItem("16");
    antiAlliasingComboBox->setCurrentIndex(0);
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::setDirty()
{
    emit isDirty();
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::keyboardLayoutChanged()
{
    mKeyboardLayoutChanged = true;
    setDirty();
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::languageChanged()
{
    mLanguageChanged = true;
    setDirty();
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::treeChanged(QTreeWidgetItem* item, int row)
{
    mPluginsChanged = true;
    setDirty();
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::renderSystemChanged()
{
    mRenderSystemChanged = true;
    setDirty();
}
//----------------------------------------------------------------------------------------
void OgitorPreferencesWidget::VSyncChanged()
{
    mVSyncChanged = true;
    setDirty();
}
//----------------------------------------------------------------------------------------