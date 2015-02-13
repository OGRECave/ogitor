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
#ifndef OGRE_VIEWPORTGRID_H
#define OGRE_VIEWPORTGRID_H

namespace Ogitors
{
    //! Viewport grid class
    /**
        A viewport grid class that renders a dynamically adjusting grid inside an Ogre viewport.
        @todo Make the grid work with an arbitrary rotated orthogonal camera (e.g. for working in object space mode).
    */
    class OgitorExport ViewportGrid: public Ogre::RenderTargetListener, public Ogre::GeneralAllocatedObject
    {
    public:
        /** The render layer of the grid in orthogonal view */
        enum RenderLayer
        {
            RL_BEHIND, /** Behind all objects */
            RL_INFRONT /** In front of all objects */
        };

    protected:
        Ogre::SceneManager *m_pSceneMgr;            /** Handle to scene manager */
        Ogre::Viewport *m_pViewport;                /** Handle to current viewport */
        CBaseEditor *m_trackObject;                 /** Currently selected object */
        bool m_enabled;                             /** Enabled/disabled flag */
        RenderLayer m_layer;                        /** Render in front or behind flag */

        Ogre::Camera *m_pPrevCamera;                /** Previously used camera handle */
        Ogre::Vector3 m_prevPos;                    /** Previous position of selected object */
        Ogre::Real    m_SpacingMult;                /** Grid spacing multiplier (size of a single cell) */
        bool m_forceUpdate;                         /** Force update flag */

        Ogre::ManualObject *m_pGrid;                /** Ore::ManualObject-based handle to the grid object */
        bool m_created;                             /** Grid creation flag */
        Ogre::SceneNode *m_pNode;                   /** Scene node handle */

        Ogre::ColourValue m_colour1, m_colour2;     /** Horizontal/vertical axis colours */
        unsigned int m_division;                    /** Sub-division of lines when camera is zooming in */
        Ogre::Real m_perspSize;                     /** Current grid size in perspective view*/
        bool m_renderScale, m_renderMiniAxes;       /** Additional rendering flags */

    public:
        /**
        * Constructor
        * @param pSceneMgr handle to scene manager
        * @param pViewport handle to active viewport
        */
        ViewportGrid(Ogre::SceneManager *pSceneMgr, Ogre::Viewport *pViewport);
        /**
        * Destructor
        */
        virtual ~ViewportGrid();
        /**
        * Sets currently selected object
        * @param object new selected object
        */
        void setTrackingObject(CBaseEditor *object);
        
        /**
        * Fetches horizontal colour of the grid
        * @return horizontal colour of the grid
        */
        const Ogre::ColourValue &getColour() const { return m_colour1; }
        /**
        * Sets horizontal colour of the grid
        * @param colour horizontal colour of the grid
        */
        void setColour(const Ogre::ColourValue &colour);

        /**
        * Fetches division value (the number of new lines that are created when zooming in)
        * @return division value
        */
        unsigned int getDivision() const { return m_division; }
        /**
        * Sets the division value (the number of new lines that are created when zooming in)
        * @param division division value
        */
        void setDivision(unsigned int division);

        /**
        * Fetches render layer flag (in front or behind)
        * @return render layer flag
        */
        RenderLayer getRenderLayer() const { return m_layer; }
        /**
        * Sets render layer flag (in front or behind)
        * @param layer render layer flag
        */
        void setRenderLayer(RenderLayer layer);

        /**
        * Fetches size of the grid in perspective view
        * @return size of the grid in perspective view
        */
        Ogre::Real getPerspectiveSize() const { return m_perspSize; }
        /**
        * Sets size of the grid in perspective view
        * @param size size of the grid in perspective view
        */
        void setPerspectiveSize(Ogre::Real size);

        /**
        * Fetches rendering scale flag
        * @return rendering scale flag
        */
        bool getRenderScale() const { return m_renderScale; }
        /**
        * Sets rendering scale flag
        * @param enable rendering scale flag
        */
        void setRenderScale(bool enabled = true);

        /**
        * Fetches rendering of mini-axes flag
        * @return rendering of mini-axes flag
        */
        bool getRenderMiniAxes() const { return m_renderMiniAxes; }
        /**
        * Sets rendering of mini-axes flag
        * @param enabled rendering of mini-axes flag
        */
        void setRenderMiniAxes(bool enabled = true);

        /**
        * Fetches grid (rendering) flag
        * @return grid (rendering) flag
        */
        bool isEnabled() const;
        /**
        * Enables grid (rendering)
        */
        void enable();
        /**
        * Disables grid (rendering)
        */
        void disable();
        /**
        * Toggles grid (rendering)
        */
        void toggle();
        /**
        * Sets grid (rendering)
        * @param enabled grid (rendering) flag
        */
        void setEnabled(bool enabled);
        /**
        * Forces the update of the grid
        */
        void forceUpdate() { m_forceUpdate = true; }
        /**
        * Sets the grid spacing
        * @param newspacing the new value for grid spacing
        */
        static void setGridSpacing(Ogre::Real newspacing) 
        {
            if(newspacing > 0)
                m_GridSpacing = newspacing;
        };
        /**
        * Gets the grid spacing
        * @return the value for grid spacing
        */
        static Ogre::Real getGridSpacing() 
        {
            return m_GridSpacing;
        };
        /**
        * Sets the grid colour
        * @param gridcolour the new value for grid colour
        */
        static void setGridColour(Ogre::ColourValue gridcolour) 
        {
            m_GlobalGridColour = gridcolour;
        };
        /**
        * Gets the grid colour
        * @return the value for grid colour
        */
        static Ogre::ColourValue getGridColour() 
        {
            return m_GlobalGridColour;
        };

    protected:
        static Ogre::Real          m_GridSpacing;   /** The space between two grid points */
        static Ogre::ColourValue   m_GlobalGridColour; /** The Colour for the grid */
        /**
        * Delegate function that is called before viewport has been updated
        * @param evt viewport update event structure
        */
        void preViewportUpdate(const Ogre::RenderTargetViewportEvent &evt);
        /**
        * Delegate function that is called after viewport has been updated
        * @param evt viewport update event structure
        */
        void postViewportUpdate(const Ogre::RenderTargetViewportEvent &evt);

        /**
        * Creates the grid object
        */
        void createGrid();
        /**
        * Destroys the grid object
        */
        void destroyGrid();
        /**
        * Updates the grid object
        */
        void update();
        /**
        * Updates perspective view
        */
        void updatePersp();
        /**
        * Checks if an update is due
        */
        bool checkUpdate();
    };
}

#endif // OGRE_VIEWPORTGRID_H