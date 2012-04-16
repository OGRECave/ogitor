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

#include "mainwindow.hxx"
#include "terraintoolswidget.hxx"
#include "colourpicker.hxx"

#include "BaseEditor.h"
#include "TerrainEditor.h"
#include "OgitorsRoot.h"
#include "DefaultEvents.h"
#include "EventManager.h"

#define GRID_SIZE_X 52
#define GRID_SIZE_Y 64
#define ICON_SIZE 48


using namespace Ogitors;

int BrushValuesTable[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,24,28,32,36,40,44,48,52,56,60,64,72,80,88,96,104,112,120,128};

//----------------------------------------------------------------------------------------
TerrainToolsWidget::TerrainToolsWidget(QWidget *parent) :
    QWidget(parent)
{
    toolBar = new QToolBar("", this);
    toolBar->setObjectName("terraintoolstoolbar");
    toolBar->setIconSize(QSize(20,20));
    toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolBar->addAction(mOgitorMainWindow->actDeform);
    toolBar->addAction(mOgitorMainWindow->actSmooth);
    toolBar->addAction(mOgitorMainWindow->actSplat);
    toolBar->addAction(mOgitorMainWindow->actPaint);
    toolBar->addAction(mOgitorMainWindow->actSplatGrass);
    toolBar->addAction(mOgitorMainWindow->actReLight);

    mBrushSizeLabel = new QLabel(tr("Size (1)"));
    mBrushSizeSlider = new QSlider(Qt::Horizontal);
    mBrushSizeSlider->setRange(0, 34);
    mBrushSizeSlider->setTickInterval(1);
    mBrushSizeSlider->setTickPosition(QSlider::TicksBelow);

    QFontMetrics fm(mBrushSizeLabel->font());
    int minwidth = fm.width(tr("Intensity (999)"));

    mBrushIntensityLabel = new QLabel(tr("Intensity (1)"));
    mBrushIntensityLabel->setMinimumWidth(minwidth);
    mBrushIntensitySlider = new QSlider(Qt::Horizontal);
    mBrushIntensitySlider->setRange(0, 34);
    mBrushIntensitySlider->setTickInterval(1);
    mBrushIntensitySlider->setTickPosition(QSlider::TicksBelow);

    mPaintColour = new ColourPickerWidget(this, QColor(255,255,255));
    mPaintColour->setMinimumHeight(20);

    brushWidget = new QListWidget(this);
    brushWidget->setViewMode(QListView::IconMode);
    brushWidget->setGridSize(QSize(GRID_SIZE_X, GRID_SIZE_Y));
    brushWidget->setFlow(QListView::LeftToRight);
    brushWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    brushWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

    texturesWidget = new QListWidget(this);
    texturesWidget->setViewMode(QListView::IconMode);
    texturesWidget->setGridSize(QSize(GRID_SIZE_X, GRID_SIZE_Y));
    texturesWidget->setFlow(QListView::LeftToRight);
    texturesWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    texturesWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

    plantsWidget = new QListWidget(this);
    plantsWidget->setSelectionRectVisible(true);
    plantsWidget->setViewMode(QListView::IconMode);
    plantsWidget->setGridSize(QSize(GRID_SIZE_X, GRID_SIZE_Y));
    plantsWidget->setFlow(QListView::LeftToRight);
    plantsWidget->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    plantsWidget->setDragDropMode(QAbstractItemView::NoDragDrop);

    toolBox = new QToolBox(this);
    toolBox->setObjectName("terraintoolstoolbox");
    toolBox->addItem(brushWidget, tr("Brush"));
    toolBox->addItem(texturesWidget, tr("Layer Textures"));
    toolBox->addItem(plantsWidget, tr("Plant Textures"));

    QGridLayout *mLayout = new QGridLayout();
    mLayout->addWidget(toolBar, 0, 0, 1, 2);
    mLayout->addWidget(mBrushSizeLabel, 1, 0);
    mLayout->addWidget(mBrushSizeSlider, 1, 1);
    mLayout->addWidget(mBrushIntensityLabel, 2, 0);
    mLayout->addWidget(mBrushIntensitySlider, 2, 1);
    mLayout->addWidget(new QLabel(tr("Paint Colour")), 3, 0);
    mLayout->addWidget(mPaintColour, 3, 1);
    mLayout->addWidget(toolBox, 4, 0, 1, 2);
    mLayout->setColumnStretch(0, 0);
    mLayout->setColumnStretch(1, 1);
    mLayout->setRowStretch(0, 0);
    mLayout->setRowStretch(1, 0);
    mLayout->setRowStretch(2, 0);
    mLayout->setRowStretch(3, 0);
    mLayout->setRowStretch(4, 1);
    mLayout->setSpacing(0);
    mLayout->setContentsMargins(0,0,5,0);

    setLayout(mLayout);

    EventManager::getSingletonPtr()->connectEvent(EventManager::LOAD_STATE_CHANGE, this, true, 0, true, 0, EVENT_CALLBACK(TerrainToolsWidget, onSceneLoadStateChange));

    connect(texturesWidget, SIGNAL(  itemSelectionChanged() ), this, SLOT( textureIndexChanged()));
    connect(plantsWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( plantIndexChanged()));
    connect(brushWidget, SIGNAL( itemSelectionChanged() ), this, SLOT( brushIndexChanged()));
    connect(mBrushSizeSlider, SIGNAL( valueChanged( int )), this, SLOT( brushSizeValueChanged( int )));
    connect(mBrushIntensitySlider, SIGNAL( valueChanged( int )), this, SLOT( brushIntensityValueChanged( int )));
    connect(mPaintColour, SIGNAL(colourChanged( Ogre::ColourValue )), this, SLOT(paintColourChanged( Ogre::ColourValue )));
}
//----------------------------------------------------------------------------------------
TerrainToolsWidget::~TerrainToolsWidget()
{
    EventManager::getSingletonPtr()->disconnectEvent(EventManager::LOAD_STATE_CHANGE, this);
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::resizeEvent(QResizeEvent* evt)
{
    if(brushWidget)
        brushWidget->setGridSize(QSize(GRID_SIZE_X, GRID_SIZE_Y));

    if(texturesWidget)
        texturesWidget->setGridSize(QSize(GRID_SIZE_X, GRID_SIZE_Y));

    if(plantsWidget)
        plantsWidget->setGridSize(QSize(GRID_SIZE_X, GRID_SIZE_Y));
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::updateTerrainOptions(ITerrainEditor *terrain)
{
    if(texturesWidget->currentItem())
    {
        QString str = texturesWidget->currentItem()->whatsThis();
        terrain->setTexture(str.replace(QString("_diffuse.png"),QString("_diffusespecular.dds"), Qt::CaseInsensitive).toStdString());
    }

    if(plantsWidget->currentItem())
    {
        QString str = plantsWidget->currentItem()->whatsThis();
        terrain->setGrassTexture(str.toStdString());
    }

    if(brushWidget->currentItem())
    {
        QString str = brushWidget->currentItem()->whatsThis();
        terrain->setBrushName(str.toStdString());
    }

    terrain->setBrushSize(BrushValuesTable[mBrushSizeSlider->sliderPosition()]);
    terrain->setBrushIntensity(BrushValuesTable[mBrushIntensitySlider->sliderPosition()]);
    terrain->setColour(mPaintColour->getColour());
    terrain->setEditMode(0);
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::switchToolWidget( const unsigned int tool )
{
    if (tool == TOOL_SMOOTH || tool == TOOL_DEFORM)
        toolBox->setCurrentWidget(brushWidget);

    if (tool == TOOL_SPLAT)
        toolBox->setCurrentWidget(texturesWidget);

    if (tool == TOOL_SPLATGRASS)
        toolBox->setCurrentWidget(plantsWidget);
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::updateTools()
{
    populateBrushes();
    populateTextures();
    populatePlants();
}
//----------------------------------------------------------------------------------------
typedef std::map<QString, QString> ComboData;

void TerrainToolsWidget::populateBrushes()
{
    brushWidget->clear();

    ComboData items;
    items.clear();

    Ogre::FileInfoListPtr resList = Ogre::ResourceGroupManager::getSingleton().listResourceFileInfo("Brushes");

    for (Ogre::FileInfoList::const_iterator it = resList->begin(); it != resList->end(); ++it)
    {
        Ogre::FileInfo fInfo = (*it);
        if(fInfo.archive->getType() == "FileSystem")
        {
            if(fInfo.filename.find(".png") == -1) continue;
            Ogre::String filename = fInfo.archive->getName() + "/";
            filename += fInfo.filename;

            if(items.find(QString(filename.c_str())) == items.end())
            {
                items.insert(ComboData::value_type(QString(filename.c_str()), QString(fInfo.filename.c_str())));
            }
        }
    }
    resList.setNull();

    for(ComboData::iterator ct = items.begin();ct != items.end();ct++)
    {
        QImage pImg(ct->first);
        pImg.invertPixels(QImage::InvertRgb);
        QPixmap pmap = QPixmap::fromImage(pImg);

        QString name = ct->second.mid(0, ct->second.length() - 4);;
        QListWidgetItem *witem = new QListWidgetItem(QIcon(pmap), name);
        witem->setWhatsThis(ct->second);
        witem->setToolTip(name);
        brushWidget->addItem(witem);
    }

    brushWidget->setCurrentItem(brushWidget->item(0));
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::populateTextures()
{
    texturesWidget->clear();

    ComboData items;
    items.clear();

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

            if(items.find(QString(fname_diffuse.c_str())) == items.end())
            {
                items.insert(ComboData::value_type(QString(fname_diffuse.c_str()), result));
            }
        }
    }

    resList2.setNull();

    for(ComboData::iterator ct = items.begin();ct != items.end();ct++)
    {
        QString name = ct->second.mid(0, (ct->second.length() / 2) - 3);
        name.replace("_diffuse.png","");
        QListWidgetItem *witem = new QListWidgetItem(QIcon(ct->first), name);
        witem->setWhatsThis(ct->second);
        witem->setToolTip(name);
        texturesWidget->addItem(witem);
    }

    texturesWidget->setCurrentItem(texturesWidget->item(0));
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::populatePlants()
{
    PropertyOptionsVector *list = OgitorsRoot::GetTerrainPlantMaterialNames();

    plantsWidget->clear();

    ComboData items;
    items.clear();

    for(unsigned int i = 1;i < list->size();i++)
    {
        Ogre::String matName = (*list)[i].mKey;
        Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingletonPtr()->getByName(matName);

        if(matPtr.isNull())
            continue;

        Ogre::String texName = matPtr->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName();

        Ogre::FileInfoListPtr resPtr = Ogre::ResourceGroupManager::getSingletonPtr()->findResourceFileInfo("Plants", texName);
        Ogre::FileInfo fInfo = (*(resPtr->begin()));

        Ogre::String fname = fInfo.archive->getName() + "/";
        fname += fInfo.filename;

        if(items.find(QString(fname.c_str())) == items.end())
        {
            items.insert(ComboData::value_type(QString(fname.c_str()), QString(matName.c_str())));
        }

        resPtr.setNull();
    }

    for(ComboData::iterator ct = items.begin();ct != items.end();ct++)
    {
        QListWidgetItem *witem = new QListWidgetItem(QIcon(ct->first), ct->second);
        witem->setWhatsThis(ct->second);
        witem->setToolTip(ct->second);
        plantsWidget->addItem(witem);
    }

    if(list->size() > 0)
        plantsWidget->setCurrentItem(plantsWidget->item(0));
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::onSceneLoadStateChange(Ogitors::IEvent* evt)
{
    LoadStateChangeEvent *change_event = Ogitors::event_cast<LoadStateChangeEvent*>(evt);

    if(change_event)
    {
        //reload the zone selection widget when a scene is loaded
        LoadState state = change_event->getType();

        if(state == LS_LOADED)
        {
            updateTools();

            ITerrainEditor *terrain = OgitorsRoot::getSingletonPtr()->GetTerrainEditor();
            if(terrain)
                updateTerrainOptions(terrain);

            mOgitorMainWindow->menuTerrainTools->setEnabled(true);
        }
        else if(state == LS_UNLOADED)
        {
            brushWidget->clear();
            texturesWidget->clear();
            plantsWidget->clear();
            mOgitorMainWindow->menuTerrainTools->setEnabled(false);
        }
    }
}
//----------------------------------------------------------------------------------------
void TerrainToolsWidget::textureIndexChanged()
{
    if(!texturesWidget->currentItem())
        return;

    QString str = texturesWidget->currentItem()->whatsThis();

    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
        OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setTexture(str.replace(QString("_diffuse.png"),QString("_diffusespecular.dds"), Qt::CaseInsensitive).toStdString());
}
//------------------------------------------------------------------------------
void TerrainToolsWidget::plantIndexChanged()
{
    if(!plantsWidget->currentItem())
        return;

    QString str = plantsWidget->currentItem()->whatsThis();

    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
        OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setGrassTexture(str.toStdString());
}
//------------------------------------------------------------------------------
void TerrainToolsWidget::brushIndexChanged()
{
    if(!brushWidget->currentItem())
        return;

    QString str = brushWidget->currentItem()->whatsThis();

    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
        OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setBrushName(str.toStdString());
}
//------------------------------------------------------------------------------
void TerrainToolsWidget::brushSizeValueChanged ( int value )
{
    int state = BrushValuesTable[value];

    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
        OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setBrushSize(state);

    mBrushSizeLabel->setText(tr("Size (%1)").arg(state));
}
//------------------------------------------------------------------------------
void TerrainToolsWidget::brushIntensityValueChanged ( int value )
{
    int state = BrushValuesTable[value];

    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
        OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setBrushIntensity(state);

    mBrushIntensityLabel->setText(tr("Intensity (%1)").arg(state));
}
//------------------------------------------------------------------------------
void TerrainToolsWidget::paintColourChanged( Ogre::ColourValue value)
{
    if(OgitorsRoot::getSingletonPtr()->GetTerrainEditor())
        OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->setColour(value);
}
//------------------------------------------------------------------------------

#undef GRID_SIZE
#undef ICON_SIZE
