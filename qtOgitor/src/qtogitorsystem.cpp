/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "qtogitorsystem.hxx"
#include "mainwindow.hxx"
#include "ogrewidget.hxx"
#include "sceneview.hxx"
#include "layerview.hxx"
#include "imageconverter.hxx"
#include "OgitorsRoot.h"

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "propertiesviewgeneral.hxx"
#include "propertiesviewcustom.hxx"
#include "manageTerrainDialog.hxx"
#include "importheightmapdialog.hxx"
#include "eucliddialog.hxx"
#include "calculateblendmapdialog.hxx"
#include "terraintoolswidget.hxx"

#include "BaseEditor.h"
#include "MultiSelEditor.h"

//-------------------------------------------------------------------------------
QString ConvertToQString(Ogre::UTFString& value)
{
    QByteArray encodedString((const char*)value.data(), value.size() * 2);
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    return codec->toUnicode(encodedString);
}
//-------------------------------------------------------------------------------
Ogre::UTFString ConvertFromQString(QString& value)
{
    return Ogre::UTFString(value.toUtf8().constData());
}
//-------------------------------------------------------------------------------
//---PLACE ALL CORE MESSAGES HERE SO LINGUIST CAN PICK THEM----------------------
//-------------------------------------------------------------------------------
void QtOgitorSystem::DummyTranslationFunction()
{
#define tr(a) QString(a)

    QString dummyStr;
    dummyStr = tr("Parsing Scene File");
    dummyStr = tr("Applying Post Load Updates");
    dummyStr = tr("Cannot delete the main viewport!!");
    dummyStr = tr("Do you want to save your current project?");
    dummyStr = tr("Do you want to remove %s?");
    dummyStr = tr("No free slots to create a page. Please increase rows or columns.");
    dummyStr = tr("The top row is not empty!\nPlease remove the pages at top row before removing the row!");
    dummyStr = tr("The bottom row is not empty!\nPlease remove the pages at bottom row before removing the row!");
    dummyStr = tr("The left column is not empty!\nPlease remove the pages at left column before removing the row!");
    dummyStr = tr("The right column is not empty!\nPlease remove the pages at right column before removing the row!");
    dummyStr = tr("Changing Map Size can not be undone. The scene will be saved after the changes.\nDo you want to continue?");
    dummyStr = tr("Renaming a Scene Manager requires Save and Reload of the Scene.\nDo you want to continue?");
    dummyStr = tr("Are you sure you want to delete %s and all of its children?");
    dummyStr = tr("Are you sure you want to delete all selected objects?");
    dummyStr = tr("Are you sure you want to delete %s?");
    dummyStr = tr("Lock");
    dummyStr = tr("UnLock");
    dummyStr = tr("Import Heightmap");
    dummyStr = tr("Export Heightmap");
    dummyStr = tr("Export Heightmaps");
    dummyStr = tr("Select a Directory to Export");
    dummyStr = tr("Import Blendmap");
    dummyStr = tr("Import Blendmaps");
    dummyStr = tr("Remove Page");
    dummyStr = tr("Manage Terrain Pages");
    dummyStr = tr("Add Row Top");
    dummyStr = tr("Add Row Bottom");
    dummyStr = tr("Add Column Left");
    dummyStr = tr("Add Column Right");
    dummyStr = tr("Remove Row Top");
    dummyStr = tr("Remove Row Bottom");
    dummyStr = tr("Remove Column Left");
    dummyStr = tr("Remove Column Right");
    dummyStr = tr("Add Billboard");
    dummyStr = tr("Remove Billboard");
    dummyStr = tr("Add Compositor");
    dummyStr = tr("Remove Compositor");
    dummyStr = tr("Open");
    dummyStr = tr("Save");
    dummyStr = tr("Save As");
    dummyStr = tr("Ogitor Scene File");
    dummyStr = tr("DotScene File");
    dummyStr = tr("Import DotScene File");
    dummyStr = tr("Only File Version 1.0 is supported!");
    dummyStr = tr("The Scene Manager Type is not supported!");
    dummyStr = tr("<Enter New Name>");
    dummyStr = tr("%1 can not contain (\"<>,\"#?&;%2\")");
    dummyStr = tr("%1 does not contain a valid value!");
    dummyStr = tr("Parsing project file");
    dummyStr = tr("Parsing project options");
    dummyStr = tr("Creating scene objects");
    dummyStr = tr("Loading scene objects");
    dummyStr = tr("Please load a Scene File...");
    dummyStr = tr("Load in progress...");
    dummyStr = tr("Rendering...");
    dummyStr = tr("Missing Colour Map : ");
    dummyStr = tr("You must first define a SkyBox Material!");
    dummyStr = tr("The Material supplied is not compatible with Sky Box!");
    dummyStr = tr("The Material supplied is not compatible with Sky Dome!");
    dummyStr = tr("Changing Max Pixel Error requires Terrain to re-load.\nDo you want to continue?");
    dummyStr = tr("Changing Min Batch Size requires Terrain to re-load.\nDo you want to continue?");
    dummyStr = tr("Changing Max Batch Size requires Terrain to re-load.\nDo you want to continue?");
    dummyStr = tr("Add Technique");
    dummyStr = tr("Delete Technique");
    dummyStr = tr("Add Pass");
    dummyStr = tr("Delete Pass");
    dummyStr = tr("Raw 32bit Float File");
    dummyStr = tr("PNG Grayscale");
    dummyStr = tr("Image Files");
    dummyStr = tr("Import Terrain From Heightmap");
    dummyStr = tr("Calculate Blendmap");
    dummyStr = tr("Re-Light");
    dummyStr = tr("You must first specify a plant material!!");
#undef tr
}
//-------------------------------------------------------------------------------
QtOgitorSystem::QtOgitorSystem(): mGeneralPropsWidget(0), mCustomPropsWidget(0), mSceneTreeWidget(0), mLayerTreeWidget(0), mRenderViewWidget(0), mProgressDialog(0)
{
    mIconList.clear();
    Ogitors::OgitorsUtils::SetExePath(QApplication::applicationDirPath().toStdString());
    QDir directory(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QString("\\OgitorProjects"));
    if(!directory.exists())
#if defined(Q_WS_X11)
    directory.setPath(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
#else
    directory.setPath("../Projects/");
#endif
    mProjectsDirectory = directory.absolutePath();
    mCalcBlendmapDlg = 0;
}
//-------------------------------------------------------------------------------
QtOgitorSystem::~QtOgitorSystem(void)
{

}
//-------------------------------------------------------------------------------
Ogre::String QtOgitorSystem::GetProjectsDirectory()
{
    return mProjectsDirectory.toStdString();
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::InitTreeIcons()
{
    Ogitors::EditorObjectFactoryMap objects = Ogitors::OgitorsRoot::getSingletonPtr()->GetEditorObjectFactories();
    Ogitors::EditorObjectFactoryMap::iterator it = objects.begin();

    while(it != objects.end())
    {
        if(it->second)
        {
            QString filenm(it->second->mIcon.c_str());

            if(filenm != QString(""))
            {
                filenm = std::string( Ogitors::Globals::OGITOR_PLUGIN_ICON_PATH + "/" + filenm.toStdString()).c_str();
                mIconList[it->second->mTypeID] = filenm;
            }
            else
                mIconList[it->second->mTypeID] = QString( std::string(Ogitors::Globals::OGITOR_PLUGIN_ICON_PATH + "/project.svg").c_str());
        }
        it++;
    }
}
//-------------------------------------------------------------------------------
void Mkdir(const QString& path)
{
   QDir directory(path);
   directory.mkpath(path);
}
//-------------------------------------------------------------------------------
bool CopyDir(const QString& src, const QString& target, const QString& targetRoot)
{
    QString SLASH = QDir::toNativeSeparators("/");

    QString from(src);
    QString to(target);

    if (to.endsWith("/") || to.endsWith("\\")) {
        to.resize(to.length() - 1);
    }
    if (from.endsWith("/") || from.endsWith("\\")) {
        from.resize(from.length() - 1);
    }

    QString targetDir(targetRoot);
    if (targetDir.endsWith("/") || targetDir.endsWith("\\")) {
        targetDir.resize(targetDir.length() - 1);
    }

    to = QDir::toNativeSeparators(to);
    from = QDir::toNativeSeparators(from);

    // first make sure that the source dir exists
    if (!QDir(from).exists())
        return false;

    if (!QDir(to).exists())
        Mkdir(to);

    QString filename;
    QDirIterator it(from, QDir::AllDirs | QDir::Files, QDirIterator::NoIteratorFlags);

    while(it.hasNext())
    {
        filename = it.next();
        if(filename.compare(targetDir) == 0)    // infinite cycle would happen if the target dir is within the source dir
            continue;
        if (QDir(filename).exists())
        {
            if(!filename.endsWith(".")) {
                Mkdir(to + SLASH + QFileInfo(filename).fileName());
                CopyDir(filename, to + SLASH + QFileInfo(filename).fileName(), targetRoot);
            }
        } else {
            // change the umask for files only
            QFile::copy(filename, to + SLASH + QFileInfo(filename).fileName());
        }
    }

    return true;
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::CopyFilesEx(Ogre::String source, Ogre::String destination)
{
    QString SLASH = QDir::toNativeSeparators("/");

    QString filePath(Ogitors::OgitorsUtils::ExtractFilePath(source).c_str());
    QString filespec(source.c_str());
    filespec = filespec.mid(filePath.length());
    QString fileDest(destination.c_str());

    if (filePath.endsWith("/") || filePath.endsWith("\\")) {
        filePath.resize(filePath.length() - 1);
    }
    if (fileDest.endsWith("/") || fileDest.endsWith("\\")) {
        fileDest.resize(fileDest.length() - 1);
    }

    if (!QDir(fileDest).exists()) {
        Mkdir(fileDest);
    }

    QDir::Filters filter = QDir::Files;
    if(filespec == "*")
    {
        filter = QDir::AllDirs;
        filespec = QString("*.*");
    }
    else if(filespec == "*.*")
    {
        filter = QDir::Files | QDir::NoSymLinks;
    }

    QStringList list;
    list << filespec;

    if(filter.testFlag(QDir::Files))
    {
        QDirIterator it(filePath, list, filter);

        while(it.hasNext())
        {
            QString fname = it.next();
            QFile::copy(fname, fileDest + SLASH + QFileInfo(fname).fileName());
        }
        return true;
    }
    else
        return CopyDir(filePath, fileDest, fileDest);

    return false;
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::CopyFile(Ogre::String source, Ogre::String destination)
{
    QString fname = source.c_str();
    QString fdest = destination.c_str();

    QFile::remove(fdest);
    return QFile::copy(fname, fdest);
}
//-------------------------------------------------------------------------------
Ogre::UTFString QtOgitorSystem::GetSetting(Ogre::UTFString group, Ogre::UTFString name, Ogre::UTFString defaultValue)
{
    QSettings settings;
    settings.beginGroup(ConvertToQString(group));
    QString value = settings.value(ConvertToQString(name), ConvertToQString(defaultValue)).toString();
    settings.endGroup();

    return ConvertFromQString(value);
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::SetSetting(Ogre::UTFString group, Ogre::UTFString name, Ogre::UTFString value)
{
    QSettings settings;
    settings.beginGroup(ConvertToQString(group));
    settings.setValue(ConvertToQString(name), ConvertToQString(value));
    
    settings.sync();

    bool ret = settings.status() == QSettings::NoError;
    settings.endGroup();
    return ret;
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::FileExists(const Ogre::String& filename)
{
    return QFile(QString(filename.c_str())).exists();
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::MakeDirectory(Ogre::String dirname)
{
    Mkdir(dirname.c_str());
    return true;
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::DeleteFile(const Ogre::String &file)
{
    QFile::remove(file.c_str());
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::RenameFile(const Ogre::String &oldname, const Ogre::String &newname)
{
    QFile::rename(oldname.c_str(), newname.c_str());
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::GetFileList(Ogre::String path, Ogre::StringVector &list)
{
    QString srcpath(Ogitors::OgitorsUtils::ExtractFilePath(path).c_str());
    QString filespec = path.c_str();
    filespec = filespec.mid(srcpath.length(), filespec.length() - srcpath.length());

    if(filespec.isEmpty())
        filespec = "*.*";

    QStringList speclist;
    speclist << filespec;
    QDirIterator it(srcpath,speclist,QDir::Files);
    while(it.hasNext())
    {
        QString filename = it.next();
        list.push_back(filename.toStdString());
    }
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::GetDirList(Ogre::String path, Ogre::StringVector &list)
{
    QString srcpath(Ogitors::OgitorsUtils::ExtractFilePath(path).c_str());

    QDirIterator it(srcpath, QDir::Dirs);
    while(it.hasNext())
    {
        QFileInfo file(it.next());

        QString filename = file.baseName();

        if(!filename.isEmpty() && filename != QString(".") && filename != QString(".."))
            list.push_back(filename.toStdString());
    }
}
//-------------------------------------------------------------------------------
Ogitors::DIALOGRET QtOgitorSystem::DisplayMessageDialog(Ogre::UTFString msg, Ogitors::DIALOGTYPE dlgType)
{
    QMessageBox::StandardButtons buttons;
    switch(dlgType)
    {
    case Ogitors::DLGTYPE_OK:
        buttons = QMessageBox::Ok;
        break;
    case Ogitors::DLGTYPE_YESNO:
        buttons = QMessageBox::Yes | QMessageBox::No;
        break;
    case Ogitors::DLGTYPE_YESNOCANCEL:
        buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
        break;
    default:
        buttons = QMessageBox::Ok;
        break;
    }

    switch(QMessageBox::information(QApplication::activeWindow(), "qtOgitor", ConvertToQString(msg), buttons))
    {
    case QMessageBox::Ok:
    case QMessageBox::Yes:
        return Ogitors::DLGRET_YES;
        break;
    case QMessageBox::No:
        return Ogitors::DLGRET_NO;
        break;
    case QMessageBox::Cancel:
        return Ogitors::DLGRET_CANCEL;
        break;
    }
    return Ogitors::DLGRET_CANCEL;
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::DisplayProgressDialog(Ogre::UTFString title, int min, int max, int value)
{
    if(mProgressDialog != 0)
    {
        delete mProgressDialog;
        mProgressDialog = 0;
    }

    QString labeltext = ConvertToQString(title);
    mProgressDialog = new QProgressDialog(labeltext, "", min, max, QApplication::activeWindow());
    mProgressDialog->setValue(value);
    mProgressDialog->setCancelButton(0);
    mProgressDialog->setWindowModality(Qt::WindowModal);
    Qt::WindowFlags flags = Qt::Popup;
    mProgressDialog->setWindowFlags(flags);
    mProgressDialog->show();
    mProgressDialog->repaint();
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::HideProgressDialog()
{
    if(mProgressDialog != 0)
    {
        delete mProgressDialog;
        mProgressDialog = 0;
    }
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::UpdateProgressDialog(int value)
{
    mProgressDialog->setValue(value);
}
//-------------------------------------------------------------------------------
Ogre::String QtOgitorSystem::DisplayDirectorySelector(Ogre::UTFString title, Ogre::UTFString defaultPath)
{
    mOgitorMainWindow->getOgreWidget()->stopRendering(true);

    // Is default path is empty then use the project location directory    
    if(defaultPath.empty())
    {
        defaultPath = Ogitors::OgitorsUtils::ExtractFilePath(GetProjectsDirectory());
        defaultPath = GetSetting("system", "oldOpenPath", defaultPath);
    }
    
    QString oldOpenPath = ConvertToQString(defaultPath);

    QString path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), ConvertToQString(title), oldOpenPath
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#else
      );
#endif

    mOgitorMainWindow->getOgreWidget()->stopRendering(false);
    return path.toStdString();
}
//-------------------------------------------------------------------------------
Ogre::String QtOgitorSystem::DisplayOpenDialog(Ogre::UTFString title, Ogitors::UTFStringVector ExtensionList, Ogre::UTFString defaultPath)
{
    mOgitorMainWindow->getOgreWidget()->stopRendering(true);

    QString theList;
    QString selectedFilter;

    // Is default path is empty then use the project location directory    
    if(defaultPath.empty())
    {
        defaultPath = Ogitors::OgitorsUtils::ExtractFilePath(GetProjectsDirectory());
        defaultPath = GetSetting("system", "oldOpenPath", defaultPath);
    }
    
    QString oldOpenPath = ConvertToQString(defaultPath);

    for(unsigned int i = 0; i < ExtensionList.size(); i+=2)
    {
        if(i)
            theList += QString(";;");

        theList += ConvertToQString(ExtensionList[i]) + QString(" (") + ConvertToQString(ExtensionList[i + 1]) + QString(")");
    }

    if(theList.contains("xml", Qt::CaseInsensitive) || theList.contains(".scene", Qt::CaseInsensitive))
    {
        QSettings settings;
        settings.beginGroup("system");
        if (oldOpenPath.isEmpty()) 
            oldOpenPath = settings.value("oldDotsceneOpenPath", mProjectsDirectory).toString();

        selectedFilter = settings.value("selectedDotsceneOpenFilter", "").toString();
        settings.endGroup();
    }

    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), ConvertToQString(title), oldOpenPath, theList , &selectedFilter
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif

    mOgitorMainWindow->getOgreWidget()->stopRendering(false);
    return path.toStdString();
}
//-------------------------------------------------------------------------------
Ogre::String QtOgitorSystem::DisplaySaveDialog(Ogre::UTFString title, Ogitors::UTFStringVector ExtensionList, Ogre::UTFString defaultPath)
{
    mOgitorMainWindow->getOgreWidget()->stopRendering(true);

    QString theList;
    QString selectedFilter = "";

    // Is default path is empty then use the project location directory    
    if(defaultPath.empty())
    {
        defaultPath = Ogitors::OgitorsUtils::ExtractFilePath(GetProjectsDirectory());
        defaultPath = GetSetting("system", "oldOpenPath", defaultPath);
    }
    
    QString oldSavePath = ConvertToQString(defaultPath);

    for(unsigned int i = 0; i < ExtensionList.size(); i+=2)
    {
        if(i) 
            theList += QString(";;");

        theList += ConvertToQString(ExtensionList[i]) + QString(" (") + ConvertToQString(ExtensionList[i + 1]) + QString(")");
    }

    if(theList.contains("xml", Qt::CaseInsensitive))
    {
        QSettings settings;
        settings.beginGroup("system");
        if (oldSavePath.isEmpty())
            oldSavePath = settings.value("oldDotsceneSavePath", mProjectsDirectory).toString();

        settings.endGroup();
    }

    QString path = QFileDialog::getSaveFileName(QApplication::activeWindow(), ConvertToQString(title), oldSavePath, theList, &selectedFilter
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif

    mOgitorMainWindow->getOgreWidget()->stopRendering(false);
    return path.toStdString();
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::UpdateLoadProgress(float percentage, Ogre::UTFString msg)
{
    if(percentage > 0)
        mRenderViewWidget->setOverlayMessage(QApplication::translate("QtOgitorSystem","Loading") + QString(" %") + QString("%1").arg(percentage) + "\n" + ConvertToQString(msg));
    else
        mRenderViewWidget->setOverlayMessage(ConvertToQString(msg));
}
//-------------------------------------------------------------------------------
Ogre::UTFString QtOgitorSystem::Translate(Ogre::String& str)
{
    QString result = QApplication::translate("QtOgitorSystem", str.c_str(), 0, QApplication::UnicodeUTF8);

    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    QByteArray encodedString = codec->fromUnicode(result);

    Ogre::ushort *data = (Ogre::ushort*)encodedString.data();
    data++;

    Ogre::UTFString retStr(data,(encodedString.size() / 2) - 1);
    return retStr;
}
//-------------------------------------------------------------------------------
Ogre::UTFString QtOgitorSystem::Translate(const char * str)
{
    QString result =  QApplication::translate("QtOgitorSystem", str, 0, QApplication::UnicodeUTF8);

    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    QByteArray encodedString = codec->fromUnicode(result);

    Ogre::ushort *data = (Ogre::ushort*)encodedString.data();
    data++;

    Ogre::UTFString retStr(data,(encodedString.size() / 2) - 1);
    return retStr;
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::SetMouseCursor(unsigned int cursor)
{
    if(cursor == Ogitors::MOUSE_ARROW)
        mRenderViewWidget->setCursor(Qt::ArrowCursor);
    else if(cursor == Ogitors::MOUSE_CROSS)
        mRenderViewWidget->setCursor(Qt::CrossCursor);
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::SetMousePosition(Ogre::Vector2 position)
{
    QPoint pos = mRenderViewWidget->mapToGlobal(QPoint(position.x, position.y));
    QCursor::setPos(pos);
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::ShowMouseCursor(bool bShow)
{
    mRenderViewWidget->showCursorEx(bShow);
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::DisplayTerrainDialog()
{
    ManageTerrainDialog* dlg = new ManageTerrainDialog(QApplication::activeWindow());
    dlg->show();
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::DisplayImportHeightMapDialog(Ogre::NameValuePairList &params)
{
    ImportHeightMapDialog dlg(QApplication::activeWindow());
    params.clear();

    if(dlg.exec() == QDialog::Accepted)
    {
        params["scale"] = dlg.mInputScale->text().toStdString();
        params["bias"] = dlg.mInputBias->text().toStdString();
        params["diffuse"] = dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex()).toStdString();
        params["normal"] = dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex()).toStdString();
        params["inverted"] = Ogre::StringConverter::toString(dlg.mInverted->isChecked());
        return true;
    }
    else
        return false;
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::DisplayEuclidDialog(Ogre::NameValuePairList &params)
{
    EuclidDialog dlg(QApplication::activeWindow());

    Ogre::NameValuePairList::iterator it;

    if((it = params.find("title")) != params.end())
        dlg.setWindowTitle(params["title"].c_str());

    if((it = params.find("input1")) != params.end())
        dlg.input1label->setText(params["input1"].c_str());

    if((it = params.find("input2")) != params.end())
        dlg.input2label->setText(params["input2"].c_str());

    if((it = params.find("input3")) != params.end())
        dlg.input3Label->setText(params["input3"].c_str());

    if((it = params.find("input1value")) != params.end())
        dlg.mInputPitch->setText(params["input1value"].c_str());

    if((it = params.find("input2value")) != params.end())
        dlg.mInputYaw->setText(params["input2value"].c_str());

    if((it = params.find("input3value")) != params.end())
        dlg.mInputRoll->setText(params["input3value"].c_str());

    params.clear();

    if(dlg.exec() == QDialog::Accepted)
    {
        params["input1"] = dlg.mInputPitch->text().toStdString();
        params["input2"] = dlg.mInputYaw->text().toStdString();
        params["input3"] = dlg.mInputRoll->text().toStdString();
        return true;
    }
    else
        return false;
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::DisplayCalculateBlendMapDialog(Ogre::NameValuePairList &params)
{
    if(!mCalcBlendmapDlg)
    {
        Ogitors::OgitorsRoot *ogitorRoot = Ogitors::OgitorsRoot::getSingletonPtr();
        mCalcBlendmapDlg = new CalculateBlendMapDialog(QApplication::activeWindow());
        
        ImageConverter imageConverter(128,128);
        Ogitors::PropertyOptionsVector* diffuseList = ogitorRoot->GetTerrainDiffuseTextureNames();

        for (Ogitors::PropertyOptionsVector::const_iterator diffItr = diffuseList->begin(); diffItr != diffuseList->end(); ++diffItr)
        {
            Ogitors::PropertyOption opt = (*diffItr);
            Ogre::String name = Ogre::any_cast<Ogre::String>(opt.mValue);
            
            QPixmap pixmap;
            if (!pixmap.convertFromImage(imageConverter.fromOgreImageName(name, "TerrainGroupDiffuseSpecular")))
                continue;

            QIcon witem(pixmap);
            QString texname(name.c_str());
            QVariant data(texname);
            mCalcBlendmapDlg->tex1->addItem(witem, QString(texname), data);
            mCalcBlendmapDlg->tex2->addItem(witem, QString(texname), data);
            mCalcBlendmapDlg->tex3->addItem(witem, QString(texname), data);
            mCalcBlendmapDlg->tex4->addItem(witem, QString(texname), data);
            mCalcBlendmapDlg->tex5->addItem(witem, QString(texname), data);
        }
    }

    params.clear();

    if(mCalcBlendmapDlg->exec() == QDialog::Accepted)
    {
        int position = 0;

        if(mCalcBlendmapDlg->enable1->checkState() == Qt::Checked)
        {
            position++;
            Ogre::String keyst = Ogre::StringConverter::toString(position) + "::";

            QString str = qVariantValue<QString>(mCalcBlendmapDlg->tex1->itemData(mCalcBlendmapDlg->tex1->currentIndex()));

            params[keyst + "img"] = str.toStdString();
            params[keyst + "hs"] = mCalcBlendmapDlg->hs1->text().toStdString();
            params[keyst + "he"] = mCalcBlendmapDlg->he1->text().toStdString();
            params[keyst + "ss"] = mCalcBlendmapDlg->ss1->text().toStdString();
            params[keyst + "se"] = mCalcBlendmapDlg->se1->text().toStdString();
            params[keyst + "skw"] = mCalcBlendmapDlg->sk1->text().toStdString();
            params[keyst + "skwazm"] = mCalcBlendmapDlg->ska1->text().toStdString();
            params[keyst + "hr"] = mCalcBlendmapDlg->hr1->text().toStdString();
            params[keyst + "sr"] = mCalcBlendmapDlg->sr1->text().toStdString();
        }

        if(mCalcBlendmapDlg->enable2->checkState() == Qt::Checked)
        {
            position++;
            Ogre::String keyst = Ogre::StringConverter::toString(position) + "::";

            QString str = qVariantValue<QString>(mCalcBlendmapDlg->tex2->itemData(mCalcBlendmapDlg->tex2->currentIndex()));

            params[keyst + "img"] = str.toStdString();
            params[keyst + "hs"] = mCalcBlendmapDlg->hs2->text().toStdString();
            params[keyst + "he"] = mCalcBlendmapDlg->he2->text().toStdString();
            params[keyst + "ss"] = mCalcBlendmapDlg->ss2->text().toStdString();
            params[keyst + "se"] = mCalcBlendmapDlg->se2->text().toStdString();
            params[keyst + "skw"] = mCalcBlendmapDlg->sk2->text().toStdString();
            params[keyst + "skwazm"] = mCalcBlendmapDlg->ska2->text().toStdString();
            params[keyst + "hr"] = mCalcBlendmapDlg->hr2->text().toStdString();
            params[keyst + "sr"] = mCalcBlendmapDlg->sr2->text().toStdString();
        }

        if(mCalcBlendmapDlg->enable3->checkState() == Qt::Checked)
        {
            position++;
            Ogre::String keyst = Ogre::StringConverter::toString(position) + "::";

            QString str = qVariantValue<QString>(mCalcBlendmapDlg->tex3->itemData(mCalcBlendmapDlg->tex3->currentIndex()));

            params[keyst + "img"] = str.toStdString();
            params[keyst + "hs"] = mCalcBlendmapDlg->hs3->text().toStdString();
            params[keyst + "he"] = mCalcBlendmapDlg->he3->text().toStdString();
            params[keyst + "ss"] = mCalcBlendmapDlg->ss3->text().toStdString();
            params[keyst + "se"] = mCalcBlendmapDlg->se3->text().toStdString();
            params[keyst + "skw"] = mCalcBlendmapDlg->sk3->text().toStdString();
            params[keyst + "skwazm"] = mCalcBlendmapDlg->ska3->text().toStdString();
            params[keyst + "hr"] = mCalcBlendmapDlg->hr3->text().toStdString();
            params[keyst + "sr"] = mCalcBlendmapDlg->sr3->text().toStdString();
        }

        if(mCalcBlendmapDlg->enable4->checkState() == Qt::Checked)
        {
            position++;
            Ogre::String keyst = Ogre::StringConverter::toString(position) + "::";

            QString str = qVariantValue<QString>(mCalcBlendmapDlg->tex4->itemData(mCalcBlendmapDlg->tex4->currentIndex()));

            params[keyst + "img"] = str.toStdString();
            params[keyst + "hs"] = mCalcBlendmapDlg->hs4->text().toStdString();
            params[keyst + "he"] = mCalcBlendmapDlg->he4->text().toStdString();
            params[keyst + "ss"] = mCalcBlendmapDlg->ss4->text().toStdString();
            params[keyst + "se"] = mCalcBlendmapDlg->se4->text().toStdString();
            params[keyst + "skw"] = mCalcBlendmapDlg->sk4->text().toStdString();
            params[keyst + "skwazm"] = mCalcBlendmapDlg->ska4->text().toStdString();
            params[keyst + "hr"] = mCalcBlendmapDlg->hr4->text().toStdString();
            params[keyst + "sr"] = mCalcBlendmapDlg->sr4->text().toStdString();
        }

        if(mCalcBlendmapDlg->enable5->checkState() == Qt::Checked)
        {
            position++;
            Ogre::String keyst = Ogre::StringConverter::toString(position) + "::";

            QString str = qVariantValue<QString>(mCalcBlendmapDlg->tex5->itemData(mCalcBlendmapDlg->tex5->currentIndex()));

            params[keyst + "img"] = str.toStdString();
            params[keyst + "hs"] = mCalcBlendmapDlg->hs5->text().toStdString();
            params[keyst + "he"] = mCalcBlendmapDlg->he5->text().toStdString();
            params[keyst + "ss"] = mCalcBlendmapDlg->ss5->text().toStdString();
            params[keyst + "se"] = mCalcBlendmapDlg->se5->text().toStdString();
            params[keyst + "skw"] = mCalcBlendmapDlg->sk5->text().toStdString();
            params[keyst + "skwazm"] = mCalcBlendmapDlg->ska5->text().toStdString();
            params[keyst + "hr"] = mCalcBlendmapDlg->hr5->text().toStdString();
            params[keyst + "sr"] = mCalcBlendmapDlg->sr5->text().toStdString();
        }

        return (position > 0);
    }
    else
        return false;
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::HasPropertiesView()
{
    return (mGeneralPropsWidget != 0 && mCustomPropsWidget != 0);
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::PresentPropertiesView(Ogitors::CBaseEditor *object)
{
    if(object)
        object->prepareBeforePresentProperties();

    mGeneralPropsWidget->PresentPropertiesView(object);
    mCustomPropsWidget->PresentPropertiesView(object);

    if(object && object->getCustomToolsWindow())
    {
        QWidget *widget = static_cast<QWidget*>(object->getCustomToolsWindow());
        try
        {
            mOgitorMainWindow->mToolsDockWidget->setWidget(widget);
            mOgitorMainWindow->mToolsDockWidget->show();
            mOgitorMainWindow->mToolsDockWidget->raise();

            return;
        }
        catch(...)
        {
            mOgitorMainWindow->mToolsDockWidget->setWidget(NULL);
        }
    }
    else
    {
        mOgitorMainWindow->mToolsDockWidget->setWidget(NULL);
    }

    mOgitorMainWindow->mToolsDockWidget->hide();
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::HasTreeView()
{
    return (mSceneTreeWidget != 0);
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::SelectTreeItem(Ogitors::CBaseEditor *object)
{
    if(object)
    {
        mSceneTreeWidget->setSelectionUpdate(true);
        mLayerTreeWidget->setSelectionUpdate(true);

        QList<QTreeWidgetItem*> list1 = mSceneTreeWidget->getTreeWidget()->selectedItems();
        QList<QTreeWidgetItem*> list2 = mLayerTreeWidget->getTreeWidget()->selectedItems();

        if(object->getEditorType() != Ogitors::ETYPE_MULTISEL)
        {
            for(int i = 0;i < list1.size();i++)
                list1[i]->setSelected(false);

            for(int i = 0;i < list2.size();i++)
                list2[i]->setSelected(false);

            QTreeWidgetItem* handle = (QTreeWidgetItem*)object->getSceneTreeItemHandle();

            if(handle)
            {
                mSceneTreeWidget->getTreeWidget()->scrollToItem(handle, QTreeWidget::EnsureVisible);
                mSceneTreeWidget->getTreeWidget()->setItemSelected(handle, true);
            }

            handle = (QTreeWidgetItem*)object->getLayerTreeItemHandle();

            if(handle)
            {
                mLayerTreeWidget->getTreeWidget()->scrollToItem(handle, QTreeWidget::EnsureVisible);
                mLayerTreeWidget->getTreeWidget()->setItemSelected(handle, true);
            }
        }
        else
        {
            Ogitors::NameObjectPairList objList = static_cast<Ogitors::CMultiSelEditor*>(object)->getSelection();
            for(int i = 0;i < list1.size();i++)
            {
                if(objList.find(list1[i]->text(0).toStdString()) == objList.end())
                    list1[i]->setSelected(false);
            }

            for(int i = 0;i < list2.size();i++)
            {
                if(objList.find(list2[i]->text(0).toStdString()) == objList.end())
                    list2[i]->setSelected(false);
            }

            list1 = mSceneTreeWidget->getTreeWidget()->selectedItems();
            list2 = mLayerTreeWidget->getTreeWidget()->selectedItems();

            QTreeWidgetItem* handle = 0;
            QTreeWidgetItem* lasthandle1 = 0;
            QTreeWidgetItem* lasthandle2 = 0;

            Ogitors::NameObjectPairList::const_iterator it;
            for(it = objList.begin();it != objList.end();it++)
            {
                handle = (QTreeWidgetItem*)it->second->getSceneTreeItemHandle();
                if(!handle)
                    continue;

                if(list1.indexOf(handle) == -1)
                {
                    mSceneTreeWidget->getTreeWidget()->setItemSelected(handle, true);
                    lasthandle1 = handle;
                }

                handle = (QTreeWidgetItem*)it->second->getLayerTreeItemHandle();
                if(!handle)
                    continue;

                if(list2.indexOf(handle) == -1)
                {
                    mLayerTreeWidget->getTreeWidget()->setItemSelected(handle, true);
                    lasthandle2 = handle;
                }
            }

            if(lasthandle1)
                mSceneTreeWidget->getTreeWidget()->scrollToItem(lasthandle1, QTreeWidget::EnsureVisible);

            if(lasthandle2)
                mLayerTreeWidget->getTreeWidget()->scrollToItem(lasthandle2, QTreeWidget::EnsureVisible);
        }

        mSceneTreeWidget->setSelectionUpdate(false);
        mLayerTreeWidget->setSelectionUpdate(false);
    }
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::InsertTreeItem(Ogitors::CBaseEditor *parent, Ogitors::CBaseEditor *object, int iconid, unsigned int colour, bool order)
{
    if(!parent || !object)
        return;

    QTreeWidgetItem* item = 0;

    item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(object->getName().c_str())));
    object->setSceneTreeItemHandle(item);
    item->setIcon(0, QIcon( mIconList[iconid]));
    item->setTextColor(0, QColor(GetRED(colour), GetGREEN(colour), GetBLUE(colour)));

    QTreeWidgetItem *parent_item = static_cast<QTreeWidgetItem*>(parent->getSceneTreeItemHandle());

    if( order )
    {
        int pos = -1;

        unsigned int obj_order = Ogitors::OgitorsRoot::getSingletonPtr()->GetDisplayOrder( object );

        for( int j = 0; j < parent_item->childCount();j++)
        {
            unsigned int dest_order = Ogitors::OgitorsRoot::getSingletonPtr()->GetDisplayOrder( parent_item->child(j)->text(0).toStdString() );
            if( dest_order < obj_order )
                pos = j;
            else if( dest_order == obj_order && parent_item->child(j)->text(0).compare(QString(object->getName().c_str())) < 0  )
                pos = j;
            else
                break;
        }

        parent_item->insertChild( pos + 1, item );
    }
    else
        parent_item->addChild(item);

    const Ogitors::OgitorsPropertyDef *definition = object->getProperties()->getProperty("layer")->getDefinition();

    if(definition->canRead())
    {
        item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(object->getName().c_str())));
        object->setLayerTreeItemHandle(item);
        item->setIcon(0, QIcon( mIconList[iconid]));
        item->setTextColor(0,QColor(GetRED(colour),GetGREEN(colour),GetBLUE(colour)));

        int objectlayer = object->getLayer();

        if(objectlayer >= Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->LayerCount)
        {
             Ogitors::OgitorsRoot::getSingletonPtr()->GetProjectOptions()->LayerCount = objectlayer + 1;
        }

        QTreeWidgetItem *widgetitem = mLayerTreeWidget->getRootItem(objectlayer);
        if(widgetitem)
            widgetitem->addChild(item);
    }
}
//-------------------------------------------------------------------------------
void *QtOgitorSystem::MoveTreeItem(void *newparent, void *object)
{
    if(!object)
        return 0;

    QTreeWidgetItem* item = static_cast<QTreeWidgetItem*>(object);
    if(item)
    {
        if(item->parent())
            item->parent()->removeChild(item);

        if(newparent)
        {
            QTreeWidgetItem* parent = static_cast<QTreeWidgetItem*>(newparent);
            parent->addChild(item);
        }
    }

    return object;
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::MoveLayerTreeItem(int newparent, Ogitors::CBaseEditor *object)
{
    if(!object)
        return;

    QTreeWidgetItem* item = static_cast<QTreeWidgetItem*>(object->getLayerTreeItemHandle());

    if(item)
    {
        QTreeWidgetItem* parent = mLayerTreeWidget->getRootItem(newparent);
        if(item->parent() == parent)
            return;

        bool selected = object->getSelected();
        if(item->parent())
            item->parent()->removeChild(item);

        parent->addChild(item);

        object->setSelected(selected);
    }
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::SetTreeItemText(Ogitors::CBaseEditor *object, Ogre::String newname)
{
    if(!object)
        return;

    QTreeWidgetItem* witem = static_cast<QTreeWidgetItem*>(object->getSceneTreeItemHandle());
    if(witem)
    {
        witem->setText(0, QString(newname.c_str()) );
    }

    witem = static_cast<QTreeWidgetItem*>(object->getLayerTreeItemHandle());
    if(witem)
    {
        witem->setText(0, QString(newname.c_str()) );
    }
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::DeleteTreeItem(Ogitors::CBaseEditor *object)
{
    if(!object)
        return;

    QTreeWidgetItem* witem = static_cast<QTreeWidgetItem*>(object->getSceneTreeItemHandle());
    if(witem && witem->parent())
    {
        witem->parent()->removeChild( witem );
        object->setSceneTreeItemHandle(0);
    }

    witem = static_cast<QTreeWidgetItem*>(object->getLayerTreeItemHandle());
    if(witem)
    {
        witem->parent()->removeChild( witem );
        object->setLayerTreeItemHandle(0);
    }
}
//-------------------------------------------------------------------------------
void *QtOgitorSystem::CreateTreeRoot(Ogre::String name)
{
    QTreeWidgetItem* item = 0;

    item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(name.c_str())));
    item->setIcon(0, QIcon( mIconList[0]));
    QFont fnt = item->font(0);
    fnt.setBold(true);
    item->setFont(0, fnt);
    mSceneTreeWidget->getTreeWidget()->insertTopLevelItem(0, item);

    return (void*)item;
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::SetTreeItemColour(Ogitors::CBaseEditor *object, unsigned int colour)
{
    if(!object)
        return;

    QTreeWidgetItem* witem = static_cast<QTreeWidgetItem*>(object->getSceneTreeItemHandle());
    if(witem)
    {
        witem->setTextColor(0,QColor(GetRED(colour), GetGREEN(colour), GetBLUE(colour)));

        if(object->getLocked())
            witem->setData(0, Qt::UserRole, ":/icons/lock.svg");
        else
            witem->setData(0, Qt::UserRole, "");
    }

    witem = static_cast<QTreeWidgetItem*>(object->getLayerTreeItemHandle());
    if(witem)
    {
        witem->setTextColor(0,QColor(GetRED(colour), GetGREEN(colour), GetBLUE(colour)));
    }
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::ClearTreeItems()
{
    mSceneTreeWidget->getTreeWidget()->clear();

    mOgitorMainWindow->getLayersViewWidget()->resetLayerNames();
}
//-------------------------------------------------------------------------------
