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

#include "qtogitorsystem.h"
#include "mainwindow.hxx"
#include "ogrewidget.hxx"
#include "sceneview.hxx"
#include "layerview.hxx"

#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "propertiesviewgeneral.hxx"
#include "propertiesviewcustom.hxx"
#include "addterraindialog.hxx"
#include "importheightmapdialog.hxx"
#include "calculateblendmapdialog.hxx"
#include "terraintoolswidget.hxx"

#include "BaseEditor.h"
#include "MultiSelEditor.h"
//-------------------------------------------------------------------------------
//---PLACE ALL CORE MESSAGES HERE SO LINGUIST CAN PICK THEM----------------------
//-------------------------------------------------------------------------------
QString ConvertToQString(Ogre::UTFString& value)
{
    QByteArray encodedString((const char*)value.data(), value.size() * 2);
    QTextCodec *codec = QTextCodec::codecForName("UTF-16");
    return codec->toUnicode(encodedString);
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::DummyTranslationFunction()
{
#define tr(a) QString(a)

    QString dummyStr;
    dummyStr = tr("Parsing Scene File");
    dummyStr = tr("Applying Post Load Updates");
    dummyStr = tr("Can not delete the main viewport!!");
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
    dummyStr = tr("Add Page");
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
QtOgitorSystem::QtOgitorSystem(): mGeneralPropsWidget(0), mCustomPropsWidget(0), mSceneTreeWidget(0), mLayerTreeWidget(0), mRenderViewWidget(0)
{
    mIconList.clear();
    Ogitors::OgitorsUtils::SetExePath(QApplication::applicationDirPath().toStdString());
    QDir directory(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QString("\\OgitorProjects"));
    if(!directory.exists())
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
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
Ogre::String QtOgitorSystem::getProjectsDirectory()
{
    return mProjectsDirectory.toStdString();
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::initTreeIcons()
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
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
                filenm = Ogitors::OgitorsUtils::QualifyPath("/usr/share/qtOgitor/Plugins/" + filenm.toStdString()).c_str();
#else
                filenm = Ogitors::OgitorsUtils::QualifyPath("../Plugins/" + filenm.toStdString()).c_str();
#endif
                mIconList[it->second->mTypeID] = filenm;
            }
            else
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
                mIconList[it->second->mTypeID] = QString(Ogitors::OgitorsUtils::QualifyPath("/usr/share/qtOgitor/Plugins/Icons/project.svg").c_str());
#else
                mIconList[it->second->mTypeID] = QString(Ogitors::OgitorsUtils::QualifyPath("../Plugins/Icons/project.svg").c_str());
#endif

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


    if (!QDir(to).exists()) {
        Mkdir(to);
    }

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

    switch(QMessageBox::information(QApplication::activeWindow(),"qtOgitor", ConvertToQString(msg), buttons))
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
Ogre::String QtOgitorSystem::DisplayDirectorySelector(Ogre::UTFString title)
{
    mOgitorMainWindow->getOgreWidget()->stopRendering(true);

    QString path = QFileDialog::getExistingDirectory(QApplication::activeWindow(), ConvertToQString(title), QApplication::applicationDirPath()
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
      , QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly);
#else
      );
#endif

    mOgitorMainWindow->getOgreWidget()->stopRendering(false);

    return path.toStdString();
}
//-------------------------------------------------------------------------------
Ogre::String QtOgitorSystem::DisplayOpenDialog(Ogre::UTFString title, Ogitors::UTFStringVector ExtensionList)
{
    mOgitorMainWindow->getOgreWidget()->stopRendering(true);

    QSettings settings;
    QString theList;
    QString selectedFilter;
    QString oldOpenPath;
    for(unsigned int i = 0; i < ExtensionList.size(); i+=2)
    {
        if(i)
            theList += QString(";;");
        theList += ConvertToQString(ExtensionList[i]) + QString(" (") + ConvertToQString(ExtensionList[i + 1]) + QString(")");
    }

    settings.beginGroup("OgitorSystem");
    if( theList.contains("xml", Qt::CaseInsensitive) || theList.contains(".scene", Qt::CaseInsensitive) )
    {
        oldOpenPath = settings.value("oldDotsceneOpenPath", mProjectsDirectory).toString();
        selectedFilter = settings.value("selectedDotsceneOpenFilter", "").toString();
    }
    else
    {
        oldOpenPath = settings.value("oldOpenPath", mProjectsDirectory).toString();
    }
    settings.endGroup();

    QString path = QFileDialog::getOpenFileName(QApplication::activeWindow(), ConvertToQString(title), oldOpenPath, theList , &selectedFilter
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif

    mOgitorMainWindow->getOgreWidget()->stopRendering(false);

    if(path != "")
    {
        settings.beginGroup("OgitorSystem");
        if((theList.contains("xml", Qt::CaseInsensitive))&&(theList.contains(".scene", Qt::CaseInsensitive)))
        {
            settings.setValue("oldDotsceneOpenPath", path);
            settings.setValue("selectedDotsceneOpenFilter", selectedFilter);
        }
        else
        {
            settings.setValue("oldOpenPath", path);
        }
        settings.endGroup();
    }

    return path.toStdString();
}
//-------------------------------------------------------------------------------
Ogre::String QtOgitorSystem::DisplaySaveDialog(Ogre::UTFString title, Ogitors::UTFStringVector ExtensionList)
{
    mOgitorMainWindow->getOgreWidget()->stopRendering(true);

    QSettings settings;
    QString theList;
    QString selectedFilter;
    QString oldSavePath;

    for(unsigned int i = 0; i < ExtensionList.size(); i+=2)
    {
        if(i)
            theList += QString(";;");
        theList += ConvertToQString(ExtensionList[i]) + QString(" (") + ConvertToQString(ExtensionList[i + 1]) + QString(")");
    }

    settings.beginGroup("OgitorSystem");
    if(theList.contains("xml", Qt::CaseInsensitive))
    {
        oldSavePath = settings.value("oldDotsceneSavePath", mProjectsDirectory).toString();
        selectedFilter = settings.value("selectedDotsceneFilter", "").toString();
    }
    else
    {
        oldSavePath = settings.value("oldSavePath", mProjectsDirectory).toString();
        selectedFilter = settings.value("selectedFilter", "").toString();
    }
    settings.endGroup();

    QString path = QFileDialog::getSaveFileName(QApplication::activeWindow(), ConvertToQString(title), oldSavePath, theList, &selectedFilter
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    , QFileDialog::DontUseNativeDialog );
#else
    );
#endif

    mOgitorMainWindow->getOgreWidget()->stopRendering(false);

    if(path != "")
    {
        settings.beginGroup("OgitorSystem");
        if(theList.contains("xml", Qt::CaseInsensitive))
        {
            settings.setValue("oldDotsceneSavePath", path);
            settings.setValue("selectedDotsceneFilter", selectedFilter);
        }
        else
        {
            settings.setValue("oldSavePath", path);
            settings.setValue("selectedFilter", selectedFilter);
        }
        settings.endGroup();
    }
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
bool QtOgitorSystem::DisplayTerrainDialog(Ogre::NameValuePairList &params)
{
    QStringList list;
    Ogre::NameValuePairList::const_iterator it = params.begin();
    while(it != params.end())
    {
        list.push_back(QString(it->second.c_str()));
        it++;
    }

    AddTerrainDialog dlg(QApplication::activeWindow(), list);
    params.clear();

    if(dlg.exec() == QDialog::Accepted)
    {
        Ogre::String pos = dlg.mPositionCombo->itemText(dlg.mPositionCombo->currentIndex()).toStdString();
        std::replace(pos.begin(), pos.end(), 'x', ' ');
        Ogre::Vector2 vpos = Ogre::StringConverter::parseVector2(pos);
        params["init"] = "true";
        params["pagex"] = Ogre::StringConverter::toString((int)vpos.x);
        params["pagey"] = Ogre::StringConverter::toString((int)vpos.y);
        params["diffuse"] = dlg.mDiffuseCombo->itemText(dlg.mDiffuseCombo->currentIndex()).toStdString();
        params["normal"] = dlg.mNormalCombo->itemText(dlg.mNormalCombo->currentIndex()).toStdString();
        return true;
    }
    else
        return false;
}
//-------------------------------------------------------------------------------
bool QtOgitorSystem::DisplayImportHeightMapDialog(Ogre::NameValuePairList &params)
{
    ImportHeightMapDialog dlg(QApplication::activeWindow());

    Ogre::NameValuePairList::iterator it;

    if((it = params.find("title")) != params.end())
        dlg.setWindowTitle(params["title"].c_str());

    if((it = params.find("input1")) != params.end())
        dlg.input1label->setText(params["input1"].c_str());

    if((it = params.find("input2")) != params.end())
        dlg.input2label->setText(params["input2"].c_str());

    if((it = params.find("check1")) != params.end())
        dlg.checkV->setVisible(true);
    else
        dlg.checkV->setVisible(false);

    if((it = params.find("input1value")) != params.end())
        dlg.mInputScale->setText(params["input1value"].c_str());

    if((it = params.find("input2value")) != params.end())
        dlg.mInputBias->setText(params["input2value"].c_str());

    params.clear();

    if(dlg.exec() == QDialog::Accepted)
    {
        params["input1"] = dlg.mInputScale->text().toStdString();
        params["input2"] = dlg.mInputBias->text().toStdString();
        params["inputCheckV"] = "false";
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
        mCalcBlendmapDlg = new CalculateBlendMapDialog(QApplication::activeWindow());

        Ogre::FileInfoListPtr resList2 = Ogre::ResourceGroupManager::getSingleton().listResourceFileInfo("TerrainTextures");

        Ogre::String fname_diffuse;
        Ogre::String fname_normal;
        for (Ogre::FileInfoList::const_iterator it = resList2->begin(); it != resList2->end(); ++it)
        {
            Ogre::FileInfo fInfo = (*it);
            if(fInfo.archive->getType() == "FileSystem")
            {
                if(fInfo.filename.find("_diffuse.png") == -1) continue;
                fname_diffuse = fInfo.archive->getName() + "/";
                fname_diffuse += fInfo.filename;
                fname_normal = fInfo.filename;
                fname_normal.erase(fname_normal.length() - 12,12);
                fname_normal += "_normalheight.dds";

                QString result = fInfo.filename.c_str();
                if(QFile::exists(QString(Ogre::String(fInfo.archive->getName() + "/" + fname_normal).c_str())))
                {
                    result += QString(";") + QString(fname_normal.c_str());
                }

                QVariant data(result);
                mCalcBlendmapDlg->tex1->addItem(QIcon(QString(fname_diffuse.c_str())),"",data);
                mCalcBlendmapDlg->tex2->addItem(QIcon(QString(fname_diffuse.c_str())),"",data);
                mCalcBlendmapDlg->tex3->addItem(QIcon(QString(fname_diffuse.c_str())),"",data);
                mCalcBlendmapDlg->tex4->addItem(QIcon(QString(fname_diffuse.c_str())),"",data);
                mCalcBlendmapDlg->tex5->addItem(QIcon(QString(fname_diffuse.c_str())),"",data);
            }
        }
        resList2.setNull();
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
            str.replace(QString("_diffuse.png"),QString("_diffusespecular.dds"), Qt::CaseInsensitive);

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
            str.replace(QString("_diffuse.png"),QString("_diffusespecular.dds"), Qt::CaseInsensitive);

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
            str.replace(QString("_diffuse.png"),QString("_diffusespecular.dds"), Qt::CaseInsensitive);

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
            str.replace(QString("_diffuse.png"),QString("_diffusespecular.dds"), Qt::CaseInsensitive);

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
            str.replace(QString("_diffuse.png"),QString("_diffusespecular.dds"), Qt::CaseInsensitive);

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
            mOgitorMainWindow->toolsDockWidget->setWidget(widget);
            mOgitorMainWindow->toolsDockWidget->raise();
        }
        catch(...)
        {
            mOgitorMainWindow->toolsDockWidget->setWidget(NULL);
        }
    }
    else
    {
        mOgitorMainWindow->toolsDockWidget->setWidget(NULL);
    }
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
void QtOgitorSystem::InsertTreeItem(Ogitors::CBaseEditor *parent, Ogitors::CBaseEditor *object,int iconid, unsigned int colour)
{
    if(!parent || !object)
        return;

    QTreeWidgetItem* item = 0;

    item = new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString(object->getName().c_str())));
    object->setSceneTreeItemHandle(item);
    item->setIcon(0, QIcon( mIconList[iconid]));
    item->setTextColor(0,QColor(GetRED(colour),GetGREEN(colour),GetBLUE(colour)));

    static_cast<QTreeWidgetItem*>(parent->getSceneTreeItemHandle())->addChild(item);

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
void QtOgitorSystem::SetTreeItemColour(Ogitors::CBaseEditor *object, unsigned int colour )
{
    if(!object)
        return;

    QTreeWidgetItem* witem = static_cast<QTreeWidgetItem*>(object->getSceneTreeItemHandle());
    if(witem)
    {
        witem->setTextColor(0,QColor(GetRED(colour),GetGREEN(colour),GetBLUE(colour)));
    }

    witem = static_cast<QTreeWidgetItem*>(object->getLayerTreeItemHandle());
    if(witem)
    {
        witem->setTextColor(0,QColor(GetRED(colour),GetGREEN(colour),GetBLUE(colour)));
    }
}
//-------------------------------------------------------------------------------
void QtOgitorSystem::ClearTreeItems()
{
    mSceneTreeWidget->getTreeWidget()->clear();

    mOgitorMainWindow->getLayersViewWidget()->resetLayerNames();
}
//-------------------------------------------------------------------------------