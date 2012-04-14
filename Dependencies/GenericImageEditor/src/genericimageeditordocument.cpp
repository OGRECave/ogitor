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

#include <QtGui/QtGui>
#include "genericimageeditordocument.hxx"
#include "genericimageeditor.hxx"
#include "heightimageeditorcodec.hxx"

#include "Ogitors.h"
#include "OgitorsDefinitions.h"
#include "DefaultEvents.h"
#include "EventManager.h"
#include "OFSDataStream.h"

GenericImageEditorDocument::GenericImageEditorDocument(QWidget *parent) : QScrollArea(parent), 
mCodec(0), mDocName(""), mFilePath(""), mFile(0), mIsOfsFile(false)
{
    mLabel = new ToolTipLabel(this);
    mLabel->setScaledContents(true);
}
//-----------------------------------------------------------------------------------------
GenericImageEditorDocument::~GenericImageEditorDocument()
{
    mFile.close();
    mOfsPtr.unmount();
    delete mCodec;
    mCodec = 0;
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::displayImageFromFile(QString docName, QString filePath)
{
    mDocName = docName;
    mFilePath = filePath;

    int pos = filePath.indexOf("::");
    if(pos > 0)
    {
        QString ofsFile = filePath.mid(0, pos);
        filePath.remove(0, pos + 2);
        
        if(mOfsPtr.mount(ofsFile.toStdString().c_str()) != OFS::OFS_OK)
            return;

        OFS::OFSHANDLE *handle = new OFS::OFSHANDLE();

        if(mOfsPtr->openFile(*handle, filePath.toStdString().c_str(), OFS::OFS_READ) != OFS::OFS_OK)
        {
            mOfsPtr.unmount();
            return;
        }

        mIsOfsFile = true;
        Ogre::DataStreamPtr stream(new Ogitors::OfsDataStream(mOfsPtr, handle));
        displayImage(docName, stream);
    }
    else
    {
        mIsOfsFile = false;

        std::ifstream inpstr(filePath.toStdString().c_str());
        Ogre::DataStreamPtr stream(new Ogre::FileStreamDataStream(&inpstr, false));

        displayImage(docName, stream);
    }   
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::displayImage(QString docName, Ogre::DataStreamPtr stream)
{
    mLabel->setMouseTracking(true);
    mLabel->setPixmap(mCodec->onBeforeDisplay(stream));
    mLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    setWidget(mLabel);

    QString tabTitle = docName;
    if(tabTitle.length() > 25)
        tabTitle = tabTitle.left(12) + "..." + tabTitle.right(10);
    setWindowTitle(tabTitle + QString("[*]"));
    setWindowModified(false);

    mCodec->onAfterDisplay();
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::contextMenuEvent(QContextMenuEvent *event)
{
    mCodec->onContextMenu(event);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::mousePressEvent(QMouseEvent *event)
{
    // Rewrite the right click mouse event to a left button event so the cursor is moved to the location of the click
    if(event->button() == Qt::RightButton)
        event = new QMouseEvent(QEvent::MouseButtonPress, event->pos(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QScrollArea::mousePressEvent(event);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::setModified(bool modified)
{
    mModified = modified; 
    setWindowModified(modified);
    Ogitors::OgitorsRoot::getSingletonPtr()->ChangeSceneModified(modified);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::save()
{
    if(isModified())
    {
        if(!mCodec->isUseDefaultSaveLogic())
            mCodec->onSave();
        else
            if(!saveDefaultLogic())
                QMessageBox::information(QApplication::activeWindow(), "qtOgitor", QObject::tr("Error saving file %1").arg(mFilePath));

        mCodec->onAfterSave();
    }

    setModified(false);
}
//-----------------------------------------------------------------------------------------
bool GenericImageEditorDocument::saveDefaultLogic()
{
    // unimplemented
    return false;
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::closeEvent(QCloseEvent* event)
{
    if(isWindowModified())
    {
        int result = QMessageBox::information(QApplication::activeWindow(), "qtOgitor", "Document has been modified. Should the changes be saved?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(result)
        {
        case QMessageBox::Yes:      save(); break;
        case QMessageBox::No:       break;
        case QMessageBox::Cancel:   event->ignore(); return;
        }
    }

    event->accept();
    getCodec()->onClose();
    close();
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::showEvent(QShowEvent* event)
{
    GenericImageEditor* editor = (GenericImageEditor*) ((QMdiSubWindow*)parent())->mdiArea();
    editor->setActiveDocument(this);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::wheelEvent(QWheelEvent* event)
{
    int numDegrees = event->delta() / 8;
    float numSteps = numDegrees / 15;

    scaleImage(1 + (float)(numSteps / 10));
    event->accept();
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::scaleImage(float factor)
{  
    // Currently limited to 5120px to prevent huge slowdowns when trying to zoom in any further.
    // Changing the logic to only deal with the part of the image / pixmap that is actually displayed 
    // might help to reduce the load in future revisions.
    if(getCodec()->getScaleFactor() * factor * mLabel->pixmap()->size().height() < 5120)
    {
        QPixmap map = getCodec()->onScaleImage(factor);
        qDebug(QString::number(map.height()).toAscii());
        mLabel->resize(getCodec()->onScaleImage(factor).size());
    }
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::onZoomIn()
{
    scaleImage(1.1f);
}
//-----------------------------------------------------------------------------------------
void GenericImageEditorDocument::onZoomOut()
{
    scaleImage(0.9f);
}
/************************************************************************/
ToolTipLabel::ToolTipLabel(GenericImageEditorDocument* genImgEdDoc, QWidget *parent) : QLabel(parent),
mGenImgEdDoc(genImgEdDoc)
{
    setMouseTracking(true);
}
//-----------------------------------------------------------------------------------------
void ToolTipLabel::mouseMoveEvent(QMouseEvent *event)
{
    QToolTip::showText(event->globalPos(), mGenImgEdDoc->getCodec()->onToolTip(event), this);
}
//-----------------------------------------------------------------------------------------
