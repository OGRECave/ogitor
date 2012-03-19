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

#pragma once

namespace Ogitors
{
    //! Camera visual helper class
    /* 
        This class encapsulates the visual appearance of camera(s) in Ogitor.
    */
    class OgitorExport CCameraVisualHelper : public CEntityVisualHelper
    {
    public:

        /**
        * Constructor
        */
        CCameraVisualHelper(CBaseEditor *parent);
        /**
        * Destructor
        */
        virtual ~CCameraVisualHelper();
        /**
        * Displays or hides camera helper depending on the flag value
        * @param bShow flag to show/hide camera helper
        */
        virtual void Show(bool bShow);

    private:

        /**
        * Creates camera for the helper
        */
        void _createCamera();

        Ogre::ColourValue       mColour;                /** Camera helper colour */

        // frustum properties
        Ogre::Real              mFrustumLeft;           /** Camera' frustum left boundary */
        Ogre::Real              mFrustumRight;          /** Camera' frustum right boundary */
        Ogre::Real              mFrustumTop;            /** Camera' frustum top boundary */
        Ogre::Real              mFrustumBottom;         /** Camera' frustum bottom boundary */
        Ogre::Real              mNearClipDistance;      /** Camera' frustum clipping distance (near)*/
        Ogre::Real              mFarClipDistance;       /** Camera' frustum clipping distance (far)*/

        Ogre::Real              mQuadNearOffset;        /** @todo Figure this one out */
        Ogre::Real              mQuadFarOffset;         /** @todo Figure this one out */
    
        Ogre::ManualObject*     mVisualHelperObject;    /** Handle to OGRE based manual object that represents camera */

        // scenemanager where our visual object belongs to
        Ogre::SceneManager*     mSceneManager;          /** Handle of scene manager */

        // visual helper materials
        Ogre::MaterialPtr       mMaterial;              /** Helper' material handle */

        // resource group of the internal material
        Ogre::String            mResourceGroup;         /** Resource group for the camera helper */

        OgitorsScopedConnection mPropConnections[3];   /** Properties for camera helper */

        /**
        * Delegate function that is called when Field of View (FOV) is changed (i.e. when camera is displaced)
        * @param value new FOV value 
        */
        void OnFOVChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when clipping distance (near or far) is changed
        * @param value new near/far clipping value(s)
        */
        void OnClipDistanceChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when visual helper is selected/deselected
        * @param value selection/deselection flag
        */
        void OnSelectionChange(const OgitorsPropertyBase* property, Ogre::Any value);
    };
}