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

#pragma once

namespace Ogitors
{
    //! Visual helper class
    /*!  
        A class that provides visual assistance for entities that have none
    */
    class OgitorExport CVisualHelper: public Ogre::GeneralAllocatedObject
    {
    public:
        /**
        * Constructor
        * @param parent Parent editor handle
        */
        CVisualHelper(CBaseEditor *parent);
        /**
        * Destructor
        */
        virtual ~CVisualHelper();
        /**
        * Displays helper visually
        * @param bShow draws helper if true, otherwise does not
        */
        virtual void Show(bool bShow) = 0;
        /**
        * Fetches AABB box associated with visual helper
        * @return AABB box associated with visual helper
        */
        virtual Ogre::AxisAlignedBox getAABB() = 0;
        /**
        * Fetches World AABB box associated with visual helper
        * @return World AABB box associated with visual helper
        */
        virtual Ogre::AxisAlignedBox getWorldAABB() = 0;
        /**
        * sets visibility flags for helper
        */
        virtual void setVisiblityFlags(unsigned int flags) = 0;
        /**
        * Fetches Ogre scene node associated with visual helper
        * @return Ogre scene node associated with visual helper
        */
        inline  Ogre::SceneNode *getNode() {return mNode;};
        /**
        * Destroys visual helper and cleans up
        */
        virtual void Destroy() = 0;

    protected:
        CBaseEditor     *mParent;       /** Parent editor handle */
        Ogre::SceneNode *mNode;         /** Ogre scene node handle */
        bool             mVisible;      /** Visibility flag */
    };

    //! Visual helper entity class
    /*!  
        A class that draws specified entity for those that have nothing to draw on their own
    */
    class OgitorExport CEntityVisualHelper: public CVisualHelper
    {
    public:
        /**
        * Constructor
        * @param parent Parent editor handle
        * @param scalesensitive flag to maintain internal scale
        */
        CEntityVisualHelper(CBaseEditor *parent, bool scalesensitive = true);
        /**
        * Destructor
        */
        virtual ~CEntityVisualHelper();
        /**
        * Sets entity to be drawn for visual aid
        * @param model name of entity
        */
        void SetModel(Ogre::String model);
        /**
        * Sets material for an entity to be drawn
        * @param material material handle
        */
        void SetMaterial(Ogre::MaterialPtr material);
        /**
        * Sets material for an entity to be drawn
        * @param matname name of the material
        */
        void SetMaterialName(const Ogre::String& matname);
        /**
        * Sets visibility flag for the visual aid entity to be drawn
        * @param bShow draws visual aid entity if true, otherwise does not
        */
        /** @copydoc CVisualHelper::setVisiblityFlags(unsigned int) */
        virtual void setVisiblityFlags(unsigned int flags);
        virtual void Show(bool bShow);
        /** @copydoc CVisualHelper::getAABB() */
        virtual Ogre::AxisAlignedBox getAABB();
        /** @copydoc CVisualHelper::getWorldAABB() */
        virtual Ogre::AxisAlignedBox getWorldAABB();
        /** @copydoc CVisualHelper::Destroy() */
        virtual void Destroy();

        /**
        EDITOROBSERVER OVERRIDES
        **/
        void OnParentsParentChange(const OgitorsPropertyBase* property, Ogre::Any value);
        void OnParentsNodeScaleChange(const OgitorsPropertyBase* property, Ogre::Any value);
        void OnParentPositionChange(const OgitorsPropertyBase* property, Ogre::Any value);
        void OnParentOrientationChange(const OgitorsPropertyBase* property, Ogre::Any value);

    protected:
        Ogre::Entity *mHandle;                      /** Visual aid entity handle */
        bool          mScaleSensitive;              /** Internal scale flag */
        OgitorsScopedConnection mConnections[4];    /** Changes connections */
    };
}
