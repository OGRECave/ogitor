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

#include <QtCore/QUrl>
#include <QtCore/QEvent>
#include <QtCore/QDirIterator>
#include <QtCore/QTimer>

#include <QtGui/QPainter>
#include <QtGui/QDragEnterEvent>

#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QColorDialog>

#include "settingsdialog.hxx"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "ViewGrid.h"
#include "mainwindow.hxx"
#include "ofstreewidget.hxx"

using namespace Ogitors;

const int RES_LOC_DIR = 1;
const int RES_LOC_ZIP = 2;

extern QString ConvertToQString(Ogre::UTFString& value);

//----------------------------------------------------------------------------------
SettingsDialog::SettingsDialog(QWidget *parent, PROJECTOPTIONS *options) :
QDialog(parent, Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
{
    setupUi(this);

    mOptions = options;

    mProjectDirTextBox->setText(mOptions->ProjectDir.c_str());
    mProjectNameTextBox->setText(mOptions->ProjectName.c_str());
    mProjectNameTextBox->setFocus();
    mSceneMgrNameMenu->addItem("OctreeSceneManager");
    mSceneMgrNameMenu->setCurrentIndex(0);
    mConfigFileTextBox->setText(mOptions->SceneManagerConfigFile.c_str());
    mTerrainDirTextBox->setText(mOptions->TerrainDirectory.c_str());

    if(!mOptions->IsNewProject)
    {
      mProjectNameTextBox->setText(QApplication::translate("QtOgitorSystem", "<Enter New Name>"));         
      mProjectDirTextBox->setEnabled(false);
      mProjectNameTextBox->setEnabled(false);
      mSceneMgrNameMenu->setEnabled(false);
      mConfigFileTextBox->setEnabled(false);
      mTerrainDirTextBox->setEnabled(false);
      mBrowseProjectDirButton->setEnabled(false);
    }

    unsigned int i;
    QString value;
    for(i = 0;i < mOptions->ResourceDirectories.size();i++)
    {
      value = mOptions->ResourceDirectories[i].c_str();

      addResourceLocation(RES_LOC_DIR, value);
    }
    mResourceListBox->installEventFilter(this);

    mSelRectColourWidget = new ColourPickerWidget(this, QColor(mOptions->SelectionRectColour.r * 255, mOptions->SelectionRectColour.g * 255, mOptions->SelectionRectColour.b * 255));   
    mSelColourWidget = new ColourPickerWidget(this, QColor(mOptions->SelectionBBColour.r * 255, mOptions->SelectionBBColour.g * 255, mOptions->SelectionBBColour.b * 255));   
    mHighlightColourWidget = new ColourPickerWidget(this, QColor(mOptions->HighlightBBColour.r * 255, mOptions->HighlightBBColour.g * 255, mOptions->HighlightBBColour.b * 255));   
    mSelectHighlightColourWidget = new ColourPickerWidget(this, QColor(mOptions->SelectHighlightBBColour.r * 255, mOptions->SelectHighlightBBColour.g * 255, mOptions->SelectHighlightBBColour.b * 255));   
    mSelectionGridLayout->addWidget(mSelRectColourWidget,0,1,1,1);
    mSelectionGridLayout->addWidget(mSelColourWidget,1,1,1,1);
    mSelectionGridLayout->addWidget(mHighlightColourWidget,2,1,1,1);
    mSelectionGridLayout->addWidget(mSelectHighlightColourWidget,3,1,1,1);

    mGridColourWidget = new ColourPickerWidget(this, QColor(mOptions->GridColour.r * 255, mOptions->GridColour.g * 255, mOptions->GridColour.b * 255));   
    mGridAppearanceLayout->addWidget(mGridColourWidget,1,1,1,1);

    mGridSpacingMenu->setValue(ViewportGrid::getGridSpacing());
    mSnapAngleMenu->setValue(0);
    mSelectionDepthMenu->setValue(mOptions->VolumeSelectionDepth);

    mSelRectColourWidget->setMaximumSize(40,20);
    mSelRectColourWidget->setMinimumSize(40,20);
    mSelColourWidget->setMaximumSize(40,20);
    mSelColourWidget->setMinimumSize(40,20);
    mHighlightColourWidget->setMaximumSize(40,20);
    mHighlightColourWidget->setMinimumSize(40,20);
    mSelectHighlightColourWidget->setMaximumSize(40,20);
    mSelectHighlightColourWidget->setMinimumSize(40,20);
    mGridColourWidget->setMaximumSize(40,20);
    mGridColourWidget->setMinimumSize(40,20);

    // Initialize auto backup settings
    if(mOptions->AutoBackupEnabled == true)
        enableAutoBackupBox->setChecked(true);
    else
    {
        enableAutoBackupBox->setChecked(false);
        autoBackupStateChanged(Qt::Unchecked);
    }

    autoBackupTimeSpinBox->setValue(mOptions->AutoBackupPeriod);
    autoBackupPeriodTypeComboBox->setCurrentIndex(mOptions->AutoBackupPeriodType);
    autoBackupPathEdit->setText(mOptions->AutoBackupFolder.c_str());
    autoBackupMaxStoredSpinBox->setValue(mOptions->AutoBackupNumber);

    tabWidget->setCurrentIndex(0);

    // Enable dropping on the widget
    setAcceptDrops(true);

    connect(buttonBox,                      SIGNAL(accepted()),                         this, SLOT(onAccept()));
    connect(buttonBox,                      SIGNAL(rejected()),                         this, SLOT(reject()));
    connect(mBrowseProjectDirButton,        SIGNAL(clicked()),                          this, SLOT(browse()));
    connect(mSceneMgrNameMenu,              SIGNAL(currentIndexChanged(int)),           this, SLOT(setDirty()));
    connect(mConfigFileTextBox,             SIGNAL(textChanged(const QString&)),        this, SLOT(setDirty()));
    connect(mTerrainDirTextBox,             SIGNAL(textChanged(const QString&)),        this, SLOT(setDirty()));
    connect(mSelectionDepthMenu,            SIGNAL(valueChanged(double)),               this, SLOT(setDirty()));
    connect(mGridSpacingMenu,               SIGNAL(valueChanged(double)),               this, SLOT(setDirty()));
    connect(mSnapAngleMenu,                 SIGNAL(valueChanged(double)),               this, SLOT(setDirty()));
    connect(mSelRectColourWidget,           SIGNAL(colourChanged(Ogre::ColourValue)),   this, SLOT(setDirty()));
    connect(mSelColourWidget,               SIGNAL(colourChanged(Ogre::ColourValue)),   this, SLOT(setDirty()));
    connect(mHighlightColourWidget,         SIGNAL(colourChanged(Ogre::ColourValue)),   this, SLOT(setDirty()));
    connect(mSelectHighlightColourWidget,   SIGNAL(colourChanged(Ogre::ColourValue)),   this, SLOT(setDirty()));
    connect(mGridColourWidget,              SIGNAL(colourChanged(Ogre::ColourValue)),   this, SLOT(setDirty()));
    // Auto backup mapping
    connect(enableAutoBackupBox,            SIGNAL(stateChanged(int)),                  this, SLOT(autoBackupStateChanged(int)));
    connect(autoBackupTimeSpinBox,          SIGNAL(valueChanged(int)),                  this, SLOT(onAutoBackupValueChanged()));
    connect(autoBackupPeriodTypeComboBox,   SIGNAL(currentIndexChanged(int)),           this, SLOT(onAutoBackupValueChanged()));
    connect(autoBackupPathEdit,             SIGNAL(textChanged(const QString&)),        this, SLOT(onAutoBackupValueChanged()));
    connect(autoBackupMaxStoredSpinBox,     SIGNAL(valueChanged(int)),                  this, SLOT(onAutoBackupValueChanged()));
    connect(autoBackupPathButton,           SIGNAL(clicked()),                          this, SLOT(onSelectPathForBackup()));
}
//----------------------------------------------------------------------------------
SettingsDialog::~SettingsDialog()
{
}
//----------------------------------------------------------------------------------
void SettingsDialog::browse()
{
   QString path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), "", QApplication::applicationDirPath()
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#else
      );
#endif
   if(!path.isEmpty())
      mProjectDirTextBox->setText(path);
}
//----------------------------------------------------------------------------------
bool IsValidName(QString strName, QString strDisplayName, QString exkeys = "")
{
   strName = strName.trimmed();
   bool found = false;
   QString mask = "%\"<>#,?&;" + exkeys;
   for(int i = 0;i < mask.size();i++)
   {
      if(strName.contains(mask[i]))
         found = true;
   }

   if(found)
   {
      Ogre::UTFString msgTemp = OgitorsSystem::getSingletonPtr()->Translate("%1 can not contain (\"<>,\"#?&;%2\")");

      QString msg = ConvertToQString(msgTemp).arg(strDisplayName).arg(exkeys);
      QMessageBox::warning(QApplication::activeWindow(),"qtOgitor", msg, QMessageBox::Ok);
      return false;
   }
   if(strName.isEmpty())
   {
      Ogre::UTFString msgTemp = OgitorsSystem::getSingletonPtr()->Translate("%1 does not contain a valid value!");

      QString msg = ConvertToQString(msgTemp).arg(strDisplayName);
      QMessageBox::warning(QApplication::activeWindow(),"qtOgitor", msg, QMessageBox::Ok);
      return false;
   }
   return true;
}
//----------------------------------------------------------------------------------
QString lastDirPath = "";

void SettingsDialog::addResourceLocation(int loctype, QString path)
{
   bool found = false;

   for(int i = 0;i < mResourceListBox->count();i++)
   {
      if(mResourceListBox->item(i)->text() == path)
      {
         found = true;
         break;
      }
   }

   if(!found)
   {
      mResourceListBox->addItem(path);
      mResourceFileTypes.push_back(loctype);
   }

   setDirty();
}
//----------------------------------------------------------------------------------
void SettingsDialog::onAddDirectory()
{
   QString path;
   if(lastDirPath == "")    
      path = "/";
   else
      path = lastDirPath;

   OfsTreeWidget *ofsTreeWidget = new OfsTreeWidget(0, OfsTreeWidget::CAP_SHOW_DIRS, QStringList(lastDirPath));
   QGridLayout *layout = new QGridLayout();

   QPushButton *butOk = new QPushButton(QString("OK"));
   QPushButton *butCancel = new QPushButton(QString("Cancel"));

   layout->addWidget(ofsTreeWidget, 0, 0, 1, 3);
   layout->addWidget(butOk, 1, 0);
   layout->addWidget(butCancel, 1, 2);

   layout->setRowStretch(0, 1);
   layout->setRowStretch(1, 0);
   layout->setColumnStretch(0, 0);
   layout->setColumnStretch(1, 1);
   layout->setColumnStretch(2, 0);
   
   QDialog dlg;
   dlg.setLayout(layout);
   dlg.setMinimumSize(QSize(300,400));

   connect(butOk, SIGNAL(clicked()), &dlg, SLOT(accept()));
   connect(butCancel, SIGNAL(clicked()), &dlg, SLOT(reject()));

   if(dlg.exec() == QDialog::Accepted)
   {
       path = ofsTreeWidget->getSelectedItems().at(0);
       if(!path.isEmpty())
       {
          addResourceLocation(RES_LOC_DIR, path);
          lastDirPath = path;
       }
   }
}

//----------------------------------------------------------------------------------
void SettingsDialog::onRemoveEntry()
{
    if(mResourceListBox->currentIndex().isValid())
    {
        int index = mResourceListBox->currentIndex().row();
        if(index >= 0)
        {
            mResourceFileTypes.erase(mResourceFileTypes.begin() + index);
            mResourceListBox->takeItem(index);
        }
    }
}
//----------------------------------------------------------------------------------
bool SettingsDialog::eventFilter(QObject * watched, QEvent * e)
{
    if(watched == mResourceListBox)
    {
        if(e->type() == QEvent::ContextMenu)
        {
            QMenu *menu = new QMenu(this);
            QAction *tempAct = menu->addAction(tr("Add Directory"), this, SLOT(onAddDirectory()));
            tempAct->setEnabled(!mOptions->IsNewProject);
            tempAct = menu->addAction(tr("Remove Entry"), this, SLOT(onRemoveEntry()));
            tempAct->setEnabled(mResourceListBox->currentIndex().row() >= 0);
            
            menu->exec(QCursor::pos());
            delete menu;
            e->ignore();
            return true;
        }
        else if(e->type() == QEvent::KeyRelease)
        {
            QKeyEvent *evt = static_cast<QKeyEvent*>(e);

            if(evt->key() == Qt::Key_Delete)
            {
                onRemoveEntry();
                return true;
            }
        }
    }
    return false;
}
//----------------------------------------------------------------------------------
void SettingsDialog::onAccept()
{
    if(mOptions->IsNewProject)
    {
        QString ProjectDir = mProjectDirTextBox->text();
        QString ProjectName = mProjectNameTextBox->text();
        QString SceneManagerName = mSceneMgrNameMenu->itemText(mSceneMgrNameMenu->currentIndex());
        QString SceneManagerConfigFile = mConfigFileTextBox->text();
        QString TerrainDir = mTerrainDirTextBox->text();

        if(!IsValidName(ProjectDir, qApp->translate("SettingsDialog", "Project Directory")))
            return;
        if(!IsValidName(ProjectName, qApp->translate("SettingsDialog", "Project Name"), "\\/"))
            return;
        if(!IsValidName(TerrainDir, qApp->translate("SettingsDialog", "Terrain Directory")))
            return;

        Ogre::String filename = OgitorsUtils::QualifyPath(QString(ProjectDir + QString("/") + ProjectName).toStdString());

        QFile file(filename.c_str());
        if(file.exists())
        {
            int result = QMessageBox::information(QApplication::activeWindow(), "qtOgitor", "Project file with that name already exists. Would you like to overwrite it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (result != QMessageBox::Yes)
                return;
        }

        Ogre::String sProjectDir = OgitorsUtils::QualifyPath(QString(ProjectDir + QString("/") + ProjectName).toStdString());

        OgitorsSystem::getSingletonPtr()->MakeDirectory(sProjectDir);

        mOptions->CreatedIn = ""; 
        mOptions->ProjectDir = sProjectDir;
        mOptions->ProjectName = ProjectName.toStdString();
        mOptions->SceneManagerName = SceneManagerName.toStdString();
        mOptions->SceneManagerConfigFile = SceneManagerConfigFile.toStdString();
        mOptions->TerrainDirectory = TerrainDir.toStdString();
    }

    mOptions->ResourceDirectories.clear();

    Ogre::String pathTo = mOptions->ProjectDir;

    OGRE_HashMap<Ogre::String, int> resDirMap;

    unsigned int i;
    unsigned int itemcount = mResourceListBox->count();
    for(i = 0;i < itemcount;i++)
    { 
        Ogre::String strTemp = mResourceListBox->item(i)->text().toStdString();
        int stype = mResourceFileTypes[i];

        Ogre::String val;
        if(stype == RES_LOC_DIR)
        {
            val = strTemp;
            if(resDirMap.find(val) == resDirMap.end())
            {
                resDirMap[val] = 0;
            }
        }
    }

    OGRE_HashMap<Ogre::String, int>::const_iterator rit = resDirMap.begin();
    while(rit != resDirMap.end())
    {
        mOptions->ResourceDirectories.push_back(rit->first);
        rit++;
    }

    mOptions->SelectionRectColour = mSelRectColourWidget->getColour();
    mOptions->SelectionBBColour = mSelColourWidget->getColour();
    mOptions->HighlightBBColour = mHighlightColourWidget->getColour();
    mOptions->SelectHighlightBBColour = mSelectHighlightColourWidget->getColour();
    mOptions->GridColour = mGridColourWidget->getColour();
    mOptions->GridSpacing = mGridSpacingMenu->value();
    mOptions->SnapAngle = mSnapAngleMenu->value();
    mOptions->VolumeSelectionDepth = mSelectionDepthMenu->value();

    if(enableAutoBackupBox->checkState() == Qt::Checked && autoBackupPathEdit->text().length() == 0)
    {
        QMessageBox::information(QApplication::activeWindow(), "Ogitor", tr("No auto backup location specified.") + "\n" + tr("Auto backup will therefore be disabled."));
        mOptions->AutoBackupEnabled = false;
    }
    else if(enableAutoBackupBox->checkState() == Qt::Checked)
    {
        QDir dir = QDir(autoBackupPathEdit->text());
        if(dir.exists())
            mOptions->AutoBackupEnabled = true;
        else
        {
            QMessageBox::information(QApplication::activeWindow(), "Ogitor", tr("The specified directory does not exist.") + "\n" + tr("Auto backup will therefore be disabled."));
            mOptions->AutoBackupEnabled = false;
        }
    }

    mOptions->AutoBackupFolder = autoBackupPathEdit->text().toStdString();
    mOptions->AutoBackupNumber = autoBackupMaxStoredSpinBox->value();
    mOptions->AutoBackupPeriodType = autoBackupPeriodTypeComboBox->currentIndex();
    mOptions->AutoBackupPeriod = autoBackupTimeSpinBox->value();

    // Adjust auto backup timer
    if(mOptions->AutoBackupPeriodType == 0)
        // Minutes -> value (in minutes) * 60 (to convert to seconds) * 1000 (to convert to milli-seconds)
        mOgitorMainWindow->getAutoBackupTimer()->setInterval(mOptions->AutoBackupPeriod * 60 * 1000);
    else if(mOptions->AutoBackupPeriodType == 1)
        // Hours -> value (in hours) * 60 (to convert to minutes ) * 60 (to convert to seconds) * 1000 (to convert to milli-seconds)
        mOgitorMainWindow->getAutoBackupTimer()->setInterval(mOptions->AutoBackupPeriod * 60 *  60 * 1000);

    if(mOptions->AutoBackupEnabled)
        mOgitorMainWindow->getAutoBackupTimer()->start();
    else
        mOgitorMainWindow->getAutoBackupTimer()->stop();

    accept();
}
//----------------------------------------------------------------------------------
void SettingsDialog::autoBackupStateChanged(int state)
{
    if(state == Qt::Unchecked)
    {
        autoBackupTimeSpinBox->setEnabled(false);
        autoBackupPeriodTypeComboBox->setEnabled(false);
        autoBackupPathEdit->setEnabled(false);
        autoBackupPathButton->setEnabled(false);
        autoBackupMaxStoredSpinBox->setEnabled(false);

        mOptions->AutoBackupEnabled = false;
    }
    else if(state == Qt::Checked)
    {
        autoBackupTimeSpinBox->setEnabled(true);
        autoBackupPeriodTypeComboBox->setEnabled(true);
        autoBackupPathEdit->setEnabled(true);
        autoBackupPathButton->setEnabled(true);
        autoBackupMaxStoredSpinBox->setEnabled(true);

        mOptions->AutoBackupEnabled = true;
    }

    setDirty();
}
//----------------------------------------------------------------------------------------
void SettingsDialog::onAutoBackupValueChanged()
{
    mOptions->AutoBackupFolder      = autoBackupPathEdit->text().toStdString();
    mOptions->AutoBackupNumber      = autoBackupMaxStoredSpinBox->value();
    mOptions->AutoBackupPeriod      = autoBackupTimeSpinBox->value();
    mOptions->AutoBackupPeriodType  = autoBackupPeriodTypeComboBox->currentIndex();

    setDirty();
}
//----------------------------------------------------------------------------------
void SettingsDialog::setDirty()
{
    OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
}
//----------------------------------------------------------------------------------
void SettingsDialog::onSelectPathForBackup()
{
    const QString dir = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir.c_str();
    QString path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), "Backup storage location", dir
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
        , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#else
        );
#endif
    if(!path.isEmpty())
        autoBackupPathEdit->setText(path);
}
//----------------------------------------------------------------------------------
