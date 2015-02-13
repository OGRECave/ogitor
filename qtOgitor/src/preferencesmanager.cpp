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

#include <iostream>

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QApplication>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QMessageBox>

#include "preferencesmanager.hxx"

//--------------------------------------------------------------------------------
PreferencesManager::PreferencesManager(QWidget *parent) : QObject(parent)
{
    mParentWidget = parent;
    createPreferencesDialog(parent);
    mCurrentSection = 0;
    mSectionCount = 0;

    mOgitorPrefWidget = new OgitorPreferencesWidget("preferences", parent);
    mOgitorPrefWidget->getPreferencesWidget();
    addCoreSection(tr("Ogitor"), ":/icons/qtOgitor.png", "preferences", mOgitorPrefWidget);

    //mShortCutSettings = new ShortCutSettings(parent);
    //addCoreSection(tr("Controls"), ":/icons/controls.svg", mShortCutSettings);

    mPluginsRootItem = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Plugins")));
    mPluginsRootItem->setIcon(0, QIcon(":/icons/additional.svg"));
    mTreeWidgetRoot->addChild(mPluginsRootItem);

    setupSections();

    mTreeWidget->expandAll();

    connect(mTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));

    mPreferencesSections[tr("Ogitor")]->treeItem->setSelected(true);
}
//--------------------------------------------------------------------------------
PreferencesManager::~PreferencesManager()
{
    std::map<QString, PreferencesSection*>::iterator it = mPreferencesSections.begin();
    while(it != mPreferencesSections.end())
    {
        delete it->second;
        it++;
    }
    mPreferencesSections.clear();
}
//--------------------------------------------------------------------------------
void PreferencesManager::addCoreSection(QString identifier, QString sectionImagePath, QString sectionName, QWidget *widget)
{
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(identifier));
    item->setIcon(0, QIcon(sectionImagePath));
    item->setWhatsThis(0, QString::number((qlonglong)widget));
    mTreeWidgetRoot->addChild(item);
    connect(widget, SIGNAL(isDirty()), this, SLOT(onIsDirty()));
  
    PreferencesSection *sec = new PreferencesSection();
    sec->identifier = identifier;
    sec->sectionName = sectionName;
    sec->widget = widget;
    sec->treeItem = item;
    
    mCurrentSection = sec;
    mContentsLayout->addWidget(sec->widget);
    
    mPreferencesSections[identifier] = sec;
    mSectionCount++;
}
//--------------------------------------------------------------------------------
void PreferencesManager::addPluginSection(QString identifier, QString sectionImagePath, QString sectionName, QWidget *widget)
{
    QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(identifier));
    item->setIcon(0, QIcon(sectionImagePath));
    item->setWhatsThis(0, QString::number((qlonglong)widget));
    mPluginsRootItem->addChild(item);
    connect(widget, SIGNAL(isDirty()), this, SLOT(onIsDirty()));

    PreferencesSection *sec = new PreferencesSection();
    sec->identifier = identifier;
    sec->sectionName = sectionName;
    sec->widget = widget;
    sec->treeItem = item;
        
    mCurrentSection = sec;
    mContentsLayout->addWidget(sec->widget);
                
    mPreferencesSections[identifier] = sec;
    mSectionCount++;
}
//--------------------------------------------------------------------------------
void PreferencesManager::createPreferencesDialog(QWidget *parent)
{
    mTreeWidget = new QTreeWidget();
    mTreeWidget->setHeaderHidden(true);
    mTreeWidget->setColumnCount(1);
    mTreeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    mTreeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    mTreeWidget->setMinimumWidth(210);
    mTreeWidget->setMaximumWidth(210);

    mTreeWidgetRoot = new QTreeWidgetItem((QTreeWidget*)0, QStringList(tr("Preferences")));
    mTreeWidgetRoot->setIcon(0, QIcon(":/icons/preferences.svg"));
    QFont fnt = mTreeWidgetRoot->font(0);
    fnt.setBold(true);
    mTreeWidgetRoot->setFont(0, fnt);
    mTreeWidget->addTopLevelItem(mTreeWidgetRoot);
    
    mBtnBox = new QDialogButtonBox();
    mBtnBox->centerButtons();
    QPushButton *applybutton = mBtnBox->addButton(QDialogButtonBox::Apply);
    mBtnBox->addButton(QDialogButtonBox::Ok);
    mBtnBox->addButton(QDialogButtonBox::Cancel);

    mPrefDlg = new QDialog(parent);
    mPrefDlg->setMinimumWidth(500);
    mPrefDlg->setMinimumHeight(300);

    QHBoxLayout *mMainLayout = new QHBoxLayout();
    mMainLayout->addWidget(mTreeWidget);

    QVBoxLayout *mainVerticalLayout = new QVBoxLayout();
    mContentsLayout = new QHBoxLayout();
    mainVerticalLayout->addLayout(mContentsLayout);
    mainVerticalLayout->addWidget(mBtnBox);
  
    mMainLayout->addLayout(mainVerticalLayout);
    mMainLayout->setStretch(0, 0);
    mMainLayout->setStretch(1, 1);

    connect(mBtnBox, SIGNAL(accepted()), this, SLOT(onAccept()));
    connect(mBtnBox, SIGNAL(rejected()), mPrefDlg, SLOT(reject()));
    connect(applybutton, SIGNAL(clicked()), this, SLOT(onApply()));
    applybutton->setEnabled(false);
    
    mPrefDlg->setLayout(mMainLayout);
    mPrefDlg->setMinimumSize(710, 350);
}
//--------------------------------------------------------------------------------
void PreferencesManager::setupSections()
{
    mPreferencesEditors = Ogitors::OgitorsRoot::getSingletonPtr()->GetPreferencesEditorList();

    QSettings settings;

    Ogitors::PreferencesEditorDataList::iterator iter;
    for(iter=mPreferencesEditors.begin(); iter!=mPreferencesEditors.end(); iter++)
    {
        Ogitors::PreferencesEditorData str = (Ogitors::PreferencesEditorData)(*iter);
        QWidget *widg = (QWidget*)static_cast<Ogitors::PreferencesEditor*>(str.mHandle)->getPreferencesWidget();
        widg->setObjectName(str.mCaption.c_str());
        addPluginSection(QString(str.mCaption.c_str()), QString(str.mIcon.c_str()), QString(str.mSectionName.c_str()), widg);
    }
}
//--------------------------------------------------------------------------------
void PreferencesManager::showDialog()
{
    mPrefDlg->exec();
}
//--------------------------------------------------------------------------------
void PreferencesManager::showSection(QString identifier)
{
    if(mPreferencesSections[identifier])
    {
        mContentsLayout->removeWidget(mCurrentSection->widget);
        mCurrentSection->widget->hide();
        mCurrentSection = mPreferencesSections[identifier];
        mContentsLayout->addWidget(mCurrentSection->widget);
        mCurrentSection->widget->show();
    }
}
//--------------------------------------------------------------------------------
void PreferencesManager::onApply()
{
    bool result = true;

    result &= mOgitorPrefWidget->applyPreferences();

    Ogitors::PreferencesEditorDataList::iterator iter;
    for(iter = mPreferencesEditors.begin(); iter != mPreferencesEditors.end(); iter++)
    {
        Ogitors::PreferencesEditorData str = (Ogitors::PreferencesEditorData)(*iter);
        result &= static_cast<Ogitors::PreferencesEditor*>(str.mHandle)->applyPreferences();
    }

    // If we reach this line it is safe to disable the button
    mBtnBox->button(QDialogButtonBox::Apply)->setEnabled(!result); 
}
//--------------------------------------------------------------------------------
void PreferencesManager::onAccept()
{
    if(mBtnBox->button(QDialogButtonBox::Apply)->isEnabled())
        onApply();

    savePreferences();

    mPrefDlg->accept();
}
//--------------------------------------------------------------------------------
void PreferencesManager::onIsDirty()
{
    mBtnBox->button(QDialogButtonBox::Apply)->setEnabled(true);
}
//--------------------------------------------------------------------------------
void PreferencesManager::selectionChanged()
{
    QList<QTreeWidgetItem*> list = mTreeWidget->selectedItems();
    if(list.size() == 0)
        return;

    if(list[0]->parent() == 0)
        return;
    
    QString name = list[0]->text(0);
    showSection(name);    
}
//--------------------------------------------------------------------------------
void PreferencesManager::savePreferences()
{
    ///////// Ogitors Preferences Widget //////////////////
    Ogre::NameValuePairList preferences;
    mOgitorPrefWidget->getPreferences(preferences);

    QSettings settings;
    settings.beginGroup(mPreferencesSections[tr("Ogitor")]->sectionName);
    Ogre::NameValuePairList::const_iterator na;
    for(na=preferences.begin(); na!=preferences.end(); na++)
        settings.setValue(na->first.c_str(), na->second.c_str());
    settings.endGroup();

    ///////////// Plugin Widgets ///////////////////////////
    Ogitors::PreferencesEditorDataList::iterator iter;
    for(iter=mPreferencesEditors.begin(); iter!=mPreferencesEditors.end(); iter++)
    {
        Ogre::NameValuePairList preferences;
        Ogitors::PreferencesEditorData str = (Ogitors::PreferencesEditorData)(*iter);
        static_cast<Ogitors::PreferencesEditor*>(str.mHandle)->getPreferences(preferences);

        Ogre::NameValuePairList::const_iterator ni;

        settings.beginGroup(str.mSectionName.c_str());
        for(ni=preferences.begin(); ni!=preferences.end(); ni++)
        {
            settings.setValue(ni->first.c_str(), ni->second.c_str());
        }
        settings.endGroup();
    }
}
//--------------------------------------------------------------------------------