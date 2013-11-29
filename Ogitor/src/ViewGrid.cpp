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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "MultiSelEditor.h"
#include "ViewGrid.h"

using namespace Ogitors;

// Constants
static const Ogre::String sMatName = "ViewportGrid";
Ogre::Real        ViewportGrid::m_GridSpacing = 1.0f;
Ogre::ColourValue ViewportGrid::m_GlobalGridColour(0.7f, 0.7f, 0.7f);


/******************************
* Constructors and destructor *
******************************/

ViewportGrid::ViewportGrid(Ogre::SceneManager *pSceneMgr, Ogre::Viewport *pViewport)
: m_pSceneMgr(pSceneMgr), m_pViewport(pViewport), m_trackObject(0), m_enabled(false)
, m_pPrevCamera(0), m_forceUpdate(true), m_pGrid(0), m_created(false), m_pNode(0)
, m_division(10), m_perspSize(100)
, m_renderScale(true), m_renderMiniAxes(true)
{
    assert(m_pSceneMgr);
    assert(m_pViewport);

    m_SpacingMult = m_GridSpacing;
    m_colour1 = m_colour2 = m_GlobalGridColour;

    createGrid();
    setRenderLayer(RL_BEHIND);

    // Add this as a render target listener
    m_pViewport->getTarget()->addListener(this);
}

ViewportGrid::~ViewportGrid()
{
    // Remove this as a render target listener
    m_pViewport->getTarget()->removeListener(this);

    destroyGrid();
}


/************************
* Get and set functions *
************************/


/** Sets the object to track position of
*/
void ViewportGrid::setTrackingObject(CBaseEditor *object)
{
    m_trackObject = object;
    forceUpdate();
}

/** Sets the colour of the major grid lines (the minor lines are alpha-faded out/in when zooming out/in)
    @note The alpha value is automatically set to one
*/
void ViewportGrid::setColour(const Ogre::ColourValue &colour)
{
    // Force alpha = 1 for the primary colour
    m_colour1 = colour; m_colour1.a = 1.0f;
    m_colour2 = m_colour1;
    forceUpdate();
}

/** Sets in how many lines a grid has to be divided when zoomed in.
    Defaults to 10.
*/
void ViewportGrid::setDivision(unsigned int division)
{
    m_division = division;
    forceUpdate();
}

/** Sets the render layer of the grid
    @note Ignored in perspective view.
    @see Ogre::ViewportGrid::RenderLayer
*/
void ViewportGrid::setRenderLayer(RenderLayer layer)
{
    m_layer = layer;

    switch(m_layer)
    {
    default:
    case RL_BEHIND:
        // Render just before the world geometry
        m_pGrid->setRenderQueueGroup(Ogre::RENDER_QUEUE_WORLD_GEOMETRY_1 - 1);
        break;

    case RL_INFRONT:
        // Render just before the overlays
        m_pGrid->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);
        break;
    }
}

/** Sets the size of the grid in perspective view.
    Defaults to 100 units.
    @note Ignored in orthographic view.
*/
void ViewportGrid::setPerspectiveSize(Ogre::Real size)
{
    m_perspSize = size;
    forceUpdate();
}

/** Sets whether to render scaling info in an overlay.
    This looks a bit like the typical scaling info on a map.
*/
void ViewportGrid::setRenderScale(bool enabled)
{
    m_renderScale = enabled;
    forceUpdate();
}

/** Sets whether to render mini-axes in an overlay.
*/
void ViewportGrid::setRenderMiniAxes(bool enabled)
{
    m_renderMiniAxes = enabled;
    forceUpdate();
}


/******************
* Other functions *
******************/

bool ViewportGrid::isEnabled() const
{
    return m_enabled;
}

void ViewportGrid::enable()
{
    m_enabled = true;

    if(!m_pGrid->isAttached())
        m_pNode->attachObject(m_pGrid);

    forceUpdate();
}

void ViewportGrid::disable()
{
    m_enabled = false;

    if(m_pGrid->isAttached())
        m_pNode->detachObject(m_pGrid);
}

void ViewportGrid::toggle()
{
    setEnabled(!m_enabled);
}

void ViewportGrid::setEnabled(bool enabled)
{
    if(enabled)
        enable();
    else
        disable();
}


/***********************
* RenderTargetListener *
***********************/

void ViewportGrid::preViewportUpdate(const Ogre::RenderTargetViewportEvent &evt)
{
    if(evt.source != m_pViewport) return;

    m_pGrid->setVisible(true);
    
    if(m_enabled)
        update();
}

void ViewportGrid::postViewportUpdate(const Ogre::RenderTargetViewportEvent &evt)
{
    if(evt.source != m_pViewport) return;

    m_pGrid->setVisible(false);
}

/****************************
* Other protected functions *
****************************/

void ViewportGrid::createGrid()
{
    Ogre::String name = m_pViewport->getTarget()->getName() + "::";
    name += Ogre::StringConverter::toString(m_pViewport->getZOrder()) + "::ViewportGrid";

    // Create the manual object
    m_pGrid = m_pSceneMgr->createManualObject(name);
    m_pGrid->setDynamic(true);

    // Create the scene node (not attached yet)
    m_pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(name);
    m_enabled = false;

    // Make sure the material is created
    //! @todo Should we destroy the material somewhere?
    Ogre::MaterialManager &matMgr = Ogre::MaterialManager::getSingleton();
    if(!matMgr.resourceExists(sMatName))
    {
        Ogre::MaterialPtr pMaterial = matMgr.create(sMatName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        pMaterial->setLightingEnabled(false);
        pMaterial->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    }
}

void ViewportGrid::destroyGrid()
{
    // Destroy the manual object
    m_pSceneMgr->destroyManualObject(m_pGrid);
    m_pGrid = 0;

    // Destroy the scene node
    m_pSceneMgr->destroySceneNode(m_pNode->getName());
    m_pNode = 0;
}

void ViewportGrid::update()
{
    if(!m_enabled) return;

    Ogre::Camera *pCamera = m_pViewport->getCamera();
    if(!pCamera) return;

    // Check if an update is necessary
    if(!checkUpdate() && !m_forceUpdate)
        return;

    updatePersp();

    m_forceUpdate = false;
}

void ViewportGrid::updatePersp()
{
    //! @todo Interpolate alpha
    m_colour2.a = 0.4f;
    //if(m_colour2.a > 1.0f) m_colour2.a = 1.0f;

    // Calculate the horizontal zero-axis color
    const Ogre::ColourValue &horAxisColor = m_colour1;

    // Calculate the vertical zero-axis color
    const Ogre::ColourValue &vertAxisColor = m_colour1;

    // The number of lines
    int numLines = (int) (m_perspSize / m_SpacingMult) + 1;

    // Start creating or updating the grid
    m_pGrid->estimateVertexCount(4 * numLines);
    if(m_created)
        m_pGrid->beginUpdate(0);
    else
    {
        m_pGrid->begin(sMatName, Ogre::RenderOperation::OT_LINE_LIST);
        m_created = true;
    }

    // Vertical lines
    Ogre::Real start = m_SpacingMult * (int) (-m_perspSize / 2 / m_SpacingMult);
    Ogre::Real x = start;
    while(x <= m_perspSize / 2)
    {
        // Get the right color for this line
        int multX = (x == 0) ? x : (x < 0) ? (int) (x / m_SpacingMult - 0.5f) : (int) (x / m_SpacingMult + 0.5f);
        const Ogre::ColourValue &colour = (multX == 0) ? vertAxisColor : (multX % (int) m_division) ? m_colour2 : m_colour1;

        // Add the line
        m_pGrid->position(x, 0, -m_perspSize / 2);
        m_pGrid->colour(colour);
        m_pGrid->position(x, 0, m_perspSize / 2);
        m_pGrid->colour(colour);

        x += m_SpacingMult;
    }

    // Horizontal lines
    Ogre::Real y = start;
    while(y <= m_perspSize / 2)
    {
        // Get the right color for this line
        int multY = (y == 0) ? y : (y < 0) ? (int) (y / m_SpacingMult - 0.5f) : (int) (y / m_SpacingMult + 0.5f);
        const Ogre::ColourValue &colour = (multY == 0) ? horAxisColor : (multY % (int) m_division) ? m_colour2 : m_colour1;

        // Add the line
        m_pGrid->position(-m_perspSize / 2, 0, y);
        m_pGrid->colour(colour);
        m_pGrid->position(m_perspSize / 2, 0, y);
        m_pGrid->colour(colour);

        y += m_SpacingMult;
    }

    m_pGrid->end();

    // Normal orientation, grid in the X-Z plane
    m_pNode->resetOrientation();
}

/* Checks if an update is necessary */
bool ViewportGrid::checkUpdate()
{
    bool update = false;

    Ogre::Camera *pCamera = m_pViewport->getCamera();
    if(!pCamera) return false;

    if(pCamera != m_pPrevCamera)
    {
        m_pPrevCamera = pCamera;
        update = true;
    }

    if(OgitorsRoot::getSingletonPtr()->GetSelection()->getAsSingle() != m_trackObject)
        m_trackObject = 0;
 
    if(m_GlobalGridColour != m_colour1)
    {
        m_colour1 = m_GlobalGridColour;
        m_colour2 = m_GlobalGridColour;
        update = true;
    }

    if(m_trackObject)
    {
        if(m_SpacingMult != m_GridSpacing)
        {
            m_SpacingMult = m_GridSpacing;
            m_perspSize = 20.0f * m_GridSpacing;
            update = true;
        }

        if(m_trackObject->getDerivedPosition() != m_prevPos)
        {
            m_prevPos = m_trackObject->getDerivedPosition();
            Ogre::Real posX = (int)(m_prevPos.x / (m_GridSpacing * 2.0f));
            Ogre::Real posZ = (int)(m_prevPos.z / (m_GridSpacing * 2.0f));

            m_pGrid->getParentSceneNode()->setPosition(posX * (m_GridSpacing * 2.0f), m_prevPos.y, posZ * (m_GridSpacing * 2.0f));
        }
    }
    else
    {
        if(pCamera->getDerivedPosition() != m_prevPos)
        {
            m_prevPos = pCamera->getDerivedPosition();
            float height = fabs(m_prevPos.y);

            Ogre::Real newmult = (1 << (int)((log(height) / log(2.0f)) + 1)) * m_GridSpacing;

            if(m_SpacingMult != newmult)
            {
                m_SpacingMult = newmult;
                m_perspSize = 20.0f * m_SpacingMult;
                update = true;
            }

            int posX = m_prevPos.x / (int)(m_division * m_SpacingMult);
            int posZ = m_prevPos.z / (int)(m_division * m_SpacingMult);

            m_pGrid->getParentSceneNode()->setPosition(posX * (int)m_division,0,posZ * (int)m_division);
        }
    }

    return update;
}