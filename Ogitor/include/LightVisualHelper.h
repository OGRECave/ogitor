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
    //! Light visual helper class
    /* 
        This class encapsulates the visual appearance of light(s) in Ogitor.
    */
    class OgitorExport CLightVisualHelper : public CEntityVisualHelper
    {
    public:

        /**
        * Constructor
        */
        CLightVisualHelper(CBaseEditor *parent);
        /**
        * Destructor
        */
        virtual ~CLightVisualHelper();
        /**
        * Displays or hides light helper depending on the flag value
        * @param bShow flag to show/hide light helper
        */
        virtual void Show(bool bShow);
    private:

        /**
        * Creates point light for visual helper
        */
        void _createPointLight();

        /**
        * Creates directional light for visual helper
        */
        void _createDirectionalLight();

        /**
        * Creates spotlight for visual helper
        */
        void _createSpotLight();

        /**
        * Draws a circle around Z axis
        * @param radius circle radius
        * @param zOffset how far in to draw the circle on (for example, in directional light, the light has 'length' associated with arrow showing the direction)
        * @param colour circle' colour
        */
        void _drawCircle(const Ogre::Real radius, const Ogre::Real zOffset, const Ogre::ColourValue& colour);

        Ogre::Light::LightTypes mLightType;                     /** Type of the light */
        Ogre::ColourValue       mColour1;                       /** Primary colour */
        Ogre::ColourValue       mColour2;                       /** Secondary colour */
        Ogre::Radian            mSpotlightInnerAngle;           /** Spotlight inner angle value */
        Ogre::Radian            mSpotlightOuterAngle;           /** Spotlight outer angle value */
        Ogre::Real              mAttenuationRange;              /** Attennuation range */

        Ogre::Real              mDirectionalLightCircleRadius;  /** Directional light' circle radius */
        Ogre::Real              mDirectionalLightLength;        /** Directional light length (see _drawCircle) */
        Ogre::Real              mSpotlightStartCircleOffset;    /** The start offset of the spotlight */

        // this manual object represents our actual visual representation of the light depending 
        // on the given attributes
        Ogre::ManualObject*     mVisualHelperObject;            /** Handle to OGRE based manual object that represents camera */

        // scenemanager where our visual object belongs to
        Ogre::SceneManager*     mSceneManager;                  /** Handle of scene manager */

        // visual helper materials
        Ogre::MaterialPtr       mMaterial;                      /** Helper' material handle */

        // resource group of the internal material
        Ogre::String            mResourceGroup;                 /** Resource group for the light helper */

        OgitorsScopedConnection mPropConnections[6];           /** Properties for light helper */

        /**
        * Delegate function that is called when diffuse value of the light is changed
        * @param value new diffuse value 
        */
        void OnDiffuseChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when specular value of the light is changed
        * @param value new specular value 
        */
        void OnSpecularChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when light type is changed
        * @param value new light type 
        */
        void OnLightTypeChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when range of light is changed
        * @param value new light range
        */
        void OnRangeChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when attennuation value of the light is changed
        * @param value new attennuation value 
        */
        void OnAttenuationChange(const OgitorsPropertyBase* property, Ogre::Any value);
        /**
        * Delegate function that is called when visual helper is selected/deselected
        * @param value selection/deselection flag
        */
        void OnSelectionChange(const OgitorsPropertyBase* property, Ogre::Any value);
    };
}