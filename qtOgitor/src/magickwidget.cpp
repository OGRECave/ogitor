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
#ifdef OGITOR_MESHMAGICK_INTERFACE

#include "qtogitorsystem.h"
#include "magickwidget.hxx"
#include "mainwindow.hxx"
#include "OgitorsPrerequisites.h"
#include "OgitorsSystem.h"
#include "entityview.hxx"

using namespace Ogitors;

//-------------------------------------------------------------------------------------------
MagickDisplayWidget::MagickDisplayWidget(QWidget *parent): QWidget(parent), mImage(0)
{
    setAcceptDrops(true);
}
//----------------------------------------------------------------------------------------
MagickDisplayWidget::~MagickDisplayWidget()
{
}
//-------------------------------------------------------------------------------------------
void MagickDisplayWidget::setImage(const QString& file)
{
    if(file.isEmpty())
    {
        delete mImage;
        mImage = 0;
    }
    else
    {
        if(!mImage)
            mImage = new QPixmap(file);
        else
            mImage->load(file);
    }
    update();
}
//-------------------------------------------------------------------------------------------
void MagickDisplayWidget::paintEvent(QPaintEvent* evt)
{
    QPainter painter(this);
    painter.setClipRect(0,0,width(),height());
    painter.setBrush(QBrush(QColor(0,0,0)));
    painter.fillRect(QRectF(0,0,width(),height()), QColor(0,0,0));
    
    if(mImage)
    {
        int w = width();
        int h = height();

        if(w >= h)
            painter.drawPixmap(QRect((w - h) / 2, 0, h, h), *mImage);
        else
            painter.drawPixmap(QRect(0, (h - w) / 2, w, w), *mImage);
    }
    else
    {
        painter.setPen(QColor(210,210,210));
        painter.drawText(QRectF(0,0,width(),height()),tr("Please load a mesh file..."),QTextOption(Qt::AlignVCenter | Qt::AlignHCenter));
    }
}
//-------------------------------------------------------------------------------------------
void MagickDisplayWidget::dragEnterEvent(QDragEnterEvent *evt)
{
    void *source = (void*)(evt->source());
    void *lw = (void*)(mOgitorMainWindow->getEntityViewWidget()->getListWidget());
    
   if(source == lw)
    {
        evt->acceptProposedAction();
    }
}
//-------------------------------------------------------------------------------------------
void MagickDisplayWidget::dropEvent(QDropEvent *evt)
{
    void *source = (void*)(evt->source());
    void *lw = (void*)(mOgitorMainWindow->getEntityViewWidget()->getListWidget());
    
    if(source == lw)
    {
        QList<QListWidgetItem*> selected = mOgitorMainWindow->getEntityViewWidget()->getListWidget()->selectedItems();
        if(selected.size())
        {
            QString stxt = selected[0]->text() + ".mesh";
            Ogre::MeshPtr pmesh = Ogre::MeshManager::getSingletonPtr()->load(stxt.toStdString(), PROJECT_RESOURCE_GROUP);
            static_cast<MagickWidget*>(parentWidget())->loadMesh(pmesh);
        }
    }
}
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
MagickWidget::MagickWidget(QWidget *parent): QWidget(parent), mDisplayWidget(0), mMeshFileName("")
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setMargin(0);

    /*groupManager = new QtGroupPropertyManager(this);
    boolManager = new QtBoolPropertyManager(this);
    intManager = new QtIntPropertyManager(this);
    doubleManager = new QtDoublePropertyManager(this);
    stringManager = new QtStringPropertyManager(this);
    enumManager = new QtEnumPropertyManager(this);
    colourManager = new QtColorPropertyManager(this);
    quaternionManager = new QuaternionManager(this);
    vector2Manager = new Vector2Manager(this);
    vector3Manager = new Vector3Manager(this);
    vector4Manager = new Vector4Manager(this);
    QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(this);
    QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);
    QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);
    QtColorEditorFactory *colourEditFactory = new QtColorEditorFactory(this);
    QtVariantEditorFactory *variantEditFactory = new QtVariantEditorFactory(this);
    QtVariantPropertyManager *varMan1 = quaternionManager;
    QtVariantPropertyManager *varMan2 = vector2Manager;
    QtVariantPropertyManager *varMan3 = vector3Manager;
    QtVariantPropertyManager *varMan4 = vector4Manager;
    
    propertiesWidget = new QtTreePropertyBrowser(this);
    propertiesWidget->setFactoryForManager(boolManager, checkBoxFactory);
    propertiesWidget->setFactoryForManager(intManager, spinBoxFactory);
    propertiesWidget->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
    propertiesWidget->setFactoryForManager(enumManager, comboBoxFactory);
    propertiesWidget->setFactoryForManager(stringManager, lineEditFactory);
    propertiesWidget->setFactoryForManager(colourManager, colourEditFactory);
    propertiesWidget->setAlternatingRowColors(true);
    propertiesWidget->setIndentation(10);
    propertiesWidget->setFactoryForManager(varMan1, variantEditFactory);
    propertiesWidget->setFactoryForManager(varMan2, variantEditFactory);
    propertiesWidget->setFactoryForManager(varMan3, variantEditFactory);
    propertiesWidget->setFactoryForManager(varMan4, variantEditFactory);*/

    mDisplayWidget = new MagickDisplayWidget(this);
    mToolBar = new QToolBar(this);
    mToolBar->setMinimumHeight(30);
    mToolBar->setFixedHeight(30);
    mToolBar->setIconSize(QSize(24,24));
    mToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    
    createActions();

    /*layout2->addWidget(propertiesWidget);*/
    layout2->addWidget(mDisplayWidget);
    layout2->setStretch(0, 0);
    layout2->setStretch(1, 1);

    layout->addWidget(mToolBar);
    layout->addLayout(layout2);
    layout->setStretch(0, 0);
    layout->setStretch(1, 1);
    setLayout(layout);
}
//----------------------------------------------------------------------------------------
MagickWidget::~MagickWidget()
{
}
//-------------------------------------------------------------------------------------------
void MagickWidget::createActions()
{
    actOpen = new QAction(tr("Open"), this);
    actOpen->setStatusTip(tr("Open a mesh file"));
    actOpen->setIcon(QIcon(":/icons/fileopen.svg"));
    
    actSave = new QAction(tr("Save"), this);
    actSave->setStatusTip(tr("Save mesh file"));
    actSave->setIcon(QIcon(":/icons/filesave.svg"));
    
    actSaveAs = new QAction(tr("Save As"), this);
    actSaveAs->setStatusTip(tr("Save mesh file as"));
    actSaveAs->setIcon(QIcon(":/icons/filesaveas.svg"));

    actMove = new QAction(tr("Move"), this);
    actMove->setStatusTip(tr("Move Mesh"));
    actMove->setIcon(QIcon(":/icons/translate.svg"));
    actMove->setCheckable(true);

    actRotate = new QAction(tr("Rotate"), this);
    actRotate->setStatusTip(tr("Rotate Mesh"));
    actRotate->setIcon(QIcon(":/icons/rotate.svg"));
    actRotate->setCheckable(true);

    actScale = new QAction(tr("Scale"), this);
    actScale->setStatusTip(tr("Scale Mesh"));
    actScale->setIcon(QIcon(":/icons/scale.svg"));
    actScale->setCheckable(true);

    mToolBar->addAction(actOpen);
    mToolBar->addAction(actSave);
    mToolBar->addAction(actSaveAs);
    mToolBar->addSeparator();
    mToolBar->addAction(actMove);
    mToolBar->addAction(actRotate);
    mToolBar->addAction(actScale);

    connect(actOpen,    SIGNAL(triggered()), this, SLOT(openMesh()));
    connect(actSave,    SIGNAL(triggered()), this, SLOT(saveMesh()));
    connect(actSaveAs,  SIGNAL(triggered()), this, SLOT(saveMeshAs()));
    connect(actMove,    SIGNAL(triggered()), this, SLOT(toolTransform()));
    connect(actRotate,  SIGNAL(triggered()), this, SLOT(toolRotate()));
    connect(actScale,   SIGNAL(triggered()), this, SLOT(toolScale()));

    actSave->setEnabled(false);
    actSaveAs->setEnabled(false);
    actMove->setEnabled(false);
    actScale->setEnabled(false);
    actRotate->setEnabled(false);
}
//-------------------------------------------------------------------------------------------
void MagickWidget::openMesh()
{
    UTFStringVector extension;
    QString ext = tr("Mesh Files ");
    extension.push_back(ext.toStdString());
    extension.push_back("*.mesh");

    Ogitors::OgitorsSystem *mSystem = Ogitors::OgitorsSystem::getSingletonPtr();
    QString path = mSystem->DisplayOpenDialog(OTR("Open mesh file"), extension).c_str();

    if(path != "")
    {
        QSettings settings;
        settings.beginGroup("OgitorSystem");
        settings.setValue("oldMeshOpenPath", path);
        settings.endGroup();

        Ogre::String meshpath = path.toStdString();
        meshpath = OgitorsUtils::ExtractFilePath(meshpath);
        Ogre::ResourceGroupManager::getSingletonPtr()->addResourceLocation(meshpath, "FileSystem", "MeshMagickGroup");
        
        Ogre::MeshPtr pmesh = Ogre::MeshManager::getSingletonPtr()->load(path.toStdString(), "MeshMagickGroup");
        loadMesh(pmesh);
    }
}
//-------------------------------------------------------------------------------------------
void MagickWidget::saveMesh()
{
}
//-------------------------------------------------------------------------------------------
void MagickWidget::saveMeshAs()
{
}
//-------------------------------------------------------------------------------------------
void MagickWidget::toolTransform()
{
}
//-------------------------------------------------------------------------------------------
void MagickWidget::toolScale()
{
}
//-------------------------------------------------------------------------------------------
void MagickWidget::toolRotate()
{
}
//-------------------------------------------------------------------------------------------
void MagickWidget::loadMesh(Ogre::MeshPtr pMesh)
{
    QString directory(OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir.c_str());
    if(directory.isEmpty())
        directory = "./";

    QDir(directory).mkpath("entitycache");

    Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual("MeshMagickTex", 
                   Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, Ogre::TEX_TYPE_2D, 
                   512, 512, 0, Ogre::PF_R8G8B8A8 , Ogre::TU_RENDERTARGET );

    Ogre::RenderTexture *rttTex = texture->getBuffer()->getRenderTarget();
    Ogre::SceneManager *mSceneMgr = Ogre::Root::getSingletonPtr()->createSceneManager("OctreeSceneManager", "MeshMagickTexMgr");

    Ogre::Light *dirl = mSceneMgr->createLight("DisplayLight");
    dirl->setDirection(-1,-1,-1);
    dirl->setDiffuseColour(1,1,1);
    dirl->setType(Ogre::Light::LT_DIRECTIONAL);

    Ogre::Camera* RTTCam = mSceneMgr->createCamera("MeshMagickCam");
    RTTCam->setNearClipDistance(0.01F);
    RTTCam->setFarClipDistance(0);
    RTTCam->setAspectRatio(1);
    RTTCam->setFOVy(Ogre::Degree(90));
    RTTCam->setPosition(0, 0, 1);
    RTTCam->lookAt(0, 0, 0);

    Ogre::Viewport *v = rttTex->addViewport( RTTCam );
    v->setClearEveryFrame( true );
    v->setBackgroundColour(Ogre::ColourValue(0, 0, 0));

    Ogre::Entity *mEntity;

    mEntity = mSceneMgr->createEntity("scbDisplay", pMesh->getName());
    mSceneMgr->getRootSceneNode()->attachObject(mEntity);

    Ogre::Vector3 vSize = mEntity->getBoundingBox().getHalfSize();
    Ogre::Vector3 vCenter = mEntity->getBoundingBox().getCenter(); 
    
    vSize += Ogre::Vector3(vSize.z, vSize.z, vSize.z);

    float maxsize = std::max(std::max(vSize.x, vSize.y), vSize.z);
    
    vSize = Ogre::Vector3(0, 0, maxsize * 1.15f) + vCenter;
    
    RTTCam->setPosition(vSize.x, vSize.y, vSize.z);
    RTTCam->lookAt(vCenter.x, vCenter.y, vCenter.z);

    rttTex->update();
    Ogre::String imagefile = OgitorsUtils::QualifyPath(directory.toStdString() + "/entitycache/meshmagick.png");
    rttTex->writeContentsToFile(imagefile);

    mEntity->detachFromParent();
    mSceneMgr->destroyEntity(mEntity);
    rttTex->removeAllViewports();
    Ogre::Root::getSingletonPtr()->destroySceneManager(mSceneMgr);
    mDisplayWidget->setImage(QString(imagefile.c_str()));

    actSave->setEnabled(true);
    actSaveAs->setEnabled(true);
    actMove->setEnabled(true);
    actScale->setEnabled(true);
    actRotate->setEnabled(true);
}
//-------------------------------------------------------------------------------------------
#endif