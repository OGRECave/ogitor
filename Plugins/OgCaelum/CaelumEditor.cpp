///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include "Ogitors.h"
#include "ofs.h"
#include "CaelumEditor.h"

using namespace Ogitors;

PropertyOptionsVector CCaelumEditorFactory::mMonths;

//--------------------------------------------------------------------------------------------------
CCaelumEditor::CCaelumEditor(CBaseEditorFactory *factory) : CBaseEditor(factory),
    _mUpdateCounter(0)
{
    mHandle = 0;
    mSceneManager = 0;
    
    mName->init("CaelumSky");

    Ogre::ResourceGroupManager *resmngr = Ogre::ResourceGroupManager::getSingletonPtr();
    Ogre::String value = mOgitorsRoot->GetProjectFile()->getFileSystemName() + "::/" + mOgitorsRoot->GetProjectOptions()->CaelumDirectory + "/";
    resmngr->addResourceLocation(value,"Ofs","Caelum");
    resmngr->initialiseResourceGroup("Caelum");
}
//--------------------------------------------------------------------------------------------------
CCaelumEditor::~CCaelumEditor()
{
    mOgitorsRoot->DestroyResourceGroup("Caelum");
}
//--------------------------------------------------------------------------------------------------
bool CCaelumEditor::_createSky(Ogre::SceneManager *mngr, Ogre::Camera *cam)
{
    Caelum::CaelumSystem::CaelumComponent componentMask;
    componentMask = static_cast<Caelum::CaelumSystem::CaelumComponent> (
                Caelum::CaelumSystem::CAELUM_COMPONENT_SUN |                
                Caelum::CaelumSystem::CAELUM_COMPONENT_MOON |
                Caelum::CaelumSystem::CAELUM_COMPONENT_SKY_DOME |
                Caelum::CaelumSystem::CAELUM_COMPONENT_POINT_STARFIELD |
                 Caelum::CaelumSystem::CAELUM_COMPONENT_CLOUDS |
             0);

    mSceneManager = mngr;
    mHandle = new Caelum::CaelumSystem(Ogre::Root::getSingletonPtr(), mngr,componentMask);

    mHandle->setManageSceneFog(false);
    mHandle->notifyCameraChanged(cam);
    mHandle->getSkyDome()->setQueryFlags(0);
    mHandle->getSkyDome()->setVisibilityFlags(0x80000000);
    mHandle->forceSubcomponentQueryFlags(0);
    mHandle->forceSubcomponentVisibilityFlags(0x80000000);

    mHandle->getUniversalClock()->setGregorianDateTime(mClockYear->get(),mClockMonth->get(),mClockDay->get(),mClockHour->get(),mClockMinute->get(),mClockSecond->get());
    mHandle->getUniversalClock()->setTimeScale(mClockSpeed->get());
    mHandle->setObserverLongitude(Ogre::Degree(mLongitude->get()));
    mHandle->setObserverLatitude(Ogre::Degree(mLatitude->get()));
    mHandle->setManageSceneFog(mFogManage->get());
    mHandle->setGlobalFogDensityMultiplier(mFogDensityMultiplier->get());
    
    Caelum::BaseSkyLight *sun = mHandle->getSun();
    sun->setAmbientMultiplier(mSunAmbientMultiplier->get());
    sun->setDiffuseMultiplier(mSunDiffuseMultiplier->get());
    sun->setSpecularMultiplier(mSunSpecularMultiplier->get());
    mHandle->getSun()->setAutoDisable(mSunAutoDisable->get());
    mHandle->getSun()->getMainLight()->setCastShadows(mSunCastShadow->get());
    mHandle->getSun()->getMainLight()->setAttenuation(mSunAttenuationDistance->get(),mSunAttenuationConstantMultiplier->get(),mSunAttenuationLinearMultiplier->get(),mSunAttenuationConstantMultiplier->get());

    Caelum::BaseSkyLight *moon = mHandle->getMoon();
    moon->setAmbientMultiplier(mMoonAmbientMultiplier->get());
    moon->setDiffuseMultiplier(mMoonDiffuseMultiplier->get());
    moon->setSpecularMultiplier(mMoonSpecularMultiplier->get());
    mHandle->getMoon()->setAutoDisable(mMoonAutoDisable->get());
    mHandle->getMoon()->getMainLight()->setCastShadows(mMoonCastShadow->get());
    mHandle->getMoon()->getMainLight()->setAttenuation(mMoonAttenuationDistance->get(),mMoonAttenuationConstantMultiplier->get(),mMoonAttenuationLinearMultiplier->get(),mMoonAttenuationConstantMultiplier->get());

    
    Caelum::PointStarfield *stars = mHandle->getPointStarfield();
    stars->setMagnitudeScale( mStarsMagnitudeScale->get() );
    stars->setMag0PixelSize( mStarsMag0PixelSize->get() );
    stars->setMinPixelSize( mStarsMinPixelSize->get() );
    stars->setMaxPixelSize( mStarsMaxPixelSize->get() );
    mHandle->setEnsureSingleLightSource( mLightingSingleLightSource->get() );
    mHandle->setEnsureSingleShadowSource( mLightingSingleShadowSource->get() );
    mHandle->setManageAmbientLight( mLightingManageAmbientLight->get() );
    mHandle->setMinimumAmbientLight( mLightingMinimumAmbientLight->get() );

    Caelum::CloudSystem *clouds = mHandle->getCloudSystem();
    if(!clouds) 
    {
        clouds = new Caelum::CloudSystem(mSceneManager,mHandle->getCaelumCameraNode());
        mHandle->setCloudSystem(clouds);
    }

    clouds->clearLayers();
    clouds->getLayerVector().clear();

    for(int i = 0;i < 3;i++)
    {
        Caelum::FlatCloudLayer *layer = clouds->createLayer();
        layer->setVisibilityFlags(mCloudsEnabled[i]->get() == true?0xFFFFFFFF:0);
        layer->setQueryFlags(0);
        layer->setCloudCover(mCloudsCoverage[i]->get());
        layer->setHeight(mCloudsHeight[i]->get());
        layer->setCloudSpeed(mCloudsSpeed[0]->get());
    }

    return true;
}
//--------------------------------------------------------------------------------------------------
bool CCaelumEditor::update(float timePassed)
{
    if(!mHandle) 
        return false;

    mHandle->updateSubcomponents(timePassed);
    mHandle->notifyCameraChanged(mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera());
    
    // FIXME - should this be implemented in property getters??
    if( mHandle->getSun() )
    {
        mSunPosition->set( mHandle->getSun()->getSceneNode()->getPosition() );
        mSunColour->set( mHandle->getSun()->getBodyColour() );
        Caelum::LongReal mJulian = mHandle->getUniversalClock()->getJulianDay();
        Ogre::ColourValue mCol = mHandle->getSunLightColour(mJulian, mHandle->getSunDirection(mJulian));
        mSunLightColour->set( Ogre::ColourValue(mCol.r - 0.3, mCol.g - 0.2, mCol.b) );
    }

    // update clock properties once per second
    if( (_mUpdateCounter+=timePassed) >= 1. )
    {
        _initClockProperties();
        _mUpdateCounter = 0;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
bool CCaelumEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    _createSky(mOgitorsRoot->GetSceneManager(), mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera());

    registerForUpdates();

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unRegisterForUpdates();

    delete mHandle;

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setClock(OgitorsPropertyBase* property, const int& value)
{
    mHandle->getUniversalClock()->setGregorianDateTime(mClockYear->get(),mClockMonth->get(),mClockDay->get(),mClockHour->get(),mClockMinute->get(),mClockSecond->get());

    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setClockSpeed(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getUniversalClock()->setTimeScale(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setLongitude(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->setObserverLongitude(Ogre::Degree(value));
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setLatitude(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->setObserverLatitude(Ogre::Degree(value));
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setFogManage(OgitorsPropertyBase* property, const bool& value)
{
    mHandle->setManageSceneFog(value);
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setFogDensityMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->setGlobalFogDensityMultiplier(value);
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setSunEnable(OgitorsPropertyBase* property, const bool& value)
{
    if( value )
    {
        Caelum::SphereSun *sun = new Caelum::SphereSun(mSceneManager, mHandle->getCaelumCameraNode());
        sun->setQueryFlags(0);
        mHandle->setSun( sun );
        Ogre::ColourValue cval;
        _setSun(0, cval);
    } else
        mHandle->setSun( 0 );
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setSun(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    Caelum::BaseSkyLight *sun = mHandle->getSun();
    sun->setAmbientMultiplier(mSunAmbientMultiplier->get());
    sun->setDiffuseMultiplier(mSunDiffuseMultiplier->get());
    sun->setSpecularMultiplier(mSunSpecularMultiplier->get());
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setSunAutoDisable(OgitorsPropertyBase* property, const bool& value)
{
    mHandle->getSun()->setAutoDisable(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setSunCastShadow(OgitorsPropertyBase* property, const bool& value)
{
    mHandle->getSun()->getMainLight()->setCastShadows(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setSunAttenuation(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getSun()->getMainLight()->setAttenuation(mSunAttenuationDistance->get(),mSunAttenuationConstantMultiplier->get(),mSunAttenuationLinearMultiplier->get(),mSunAttenuationConstantMultiplier->get());
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setMoon(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    Caelum::BaseSkyLight *moon = mHandle->getMoon();
    moon->setAmbientMultiplier(mMoonAmbientMultiplier->get());
    moon->setDiffuseMultiplier(mMoonDiffuseMultiplier->get());
    moon->setSpecularMultiplier(mMoonSpecularMultiplier->get());
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setMoonEnable(OgitorsPropertyBase* property, const bool& value)
{
    if( value )
    {
        Caelum::Moon *moon = new Caelum::Moon(mSceneManager, mHandle->getCaelumCameraNode());
        moon->setQueryFlags( 0 );
        mHandle->setMoon( moon );
        Ogre::ColourValue cval;
        _setMoon(0, cval);
    } else
        mHandle->setMoon( 0 );
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setMoonAutoDisable(OgitorsPropertyBase* property, const bool& value)
{
    mHandle->getMoon()->setAutoDisable(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setMoonCastShadow(OgitorsPropertyBase* property, const bool& value)
{
    mHandle->getMoon()->getMainLight()->setCastShadows(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setMoonAttenuation(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getMoon()->getMainLight()->setAttenuation(mMoonAttenuationDistance->get(),mMoonAttenuationConstantMultiplier->get(),mMoonAttenuationLinearMultiplier->get(),mMoonAttenuationConstantMultiplier->get());
    return true;
}
//-----------------------------------------------------------------------------------------
bool CCaelumEditor::_setStars(OgitorsPropertyBase* property, const float& value)
{
    Caelum::PointStarfield *stars = mHandle->getPointStarfield();
    stars->setMagnitudeScale( mStarsMagnitudeScale->get() );
    stars->setMag0PixelSize( mStarsMag0PixelSize->get() );
    stars->setMinPixelSize( mStarsMinPixelSize->get() );
    stars->setMaxPixelSize( mStarsMaxPixelSize->get() );
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setStarsEnable(OgitorsPropertyBase* property, const bool& value)
{
    if( value )
    {
        Caelum::PointStarfield *stars = new Caelum::PointStarfield (mSceneManager, mHandle->getCaelumCameraNode ());
        stars->setQueryFlags( 0 );
        mHandle->setPointStarfield( stars );
        float fval;
        _setStars(0, fval);
    } else
        mHandle->setPointStarfield( 0 );
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setLighting(OgitorsPropertyBase* property, const bool &value)
{
    mHandle->setEnsureSingleLightSource( mLightingSingleLightSource->get() );
    mHandle->setEnsureSingleShadowSource( mLightingSingleShadowSource->get() );
    mHandle->setManageAmbientLight( mLightingManageAmbientLight->get() );
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setLightingMinimumAmbientLight(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    mHandle->setMinimumAmbientLight( value );
    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setCloudsEnable(OgitorsPropertyBase* property, const bool& value)
{
    Caelum::CloudSystem *clouds = mHandle->getCloudSystem();
    Caelum::FlatCloudLayer *layer = clouds->getLayer(property->getTag());
    layer->setVisibilityFlags(value == true?0xFFFFFFFF:0);

    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setCloudsCoverage(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    Caelum::CloudSystem *clouds = mHandle->getCloudSystem();
    Caelum::FlatCloudLayer *layer = clouds->getLayer(property->getTag());
    layer->setCloudCover(value);

    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setCloudsHeight(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    Caelum::CloudSystem *clouds = mHandle->getCloudSystem();
    Caelum::FlatCloudLayer *layer = clouds->getLayer(property->getTag());
    layer->setHeight(value);

    return true;
}
//---------------------------------------------------------------------------------
bool CCaelumEditor::_setCloudsSpeed(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{
    Caelum::CloudSystem *clouds = mHandle->getCloudSystem();
    Caelum::FlatCloudLayer *layer = clouds->getLayer(property->getTag());
    layer->setCloudSpeed(value);

    return true;
}
//---------------------------------------------------------------------------------
void CCaelumEditor::_initClockProperties( void )
{
    int year,month,day,hour,minute;
    double second;
    Caelum::LongReal jd = mHandle->getUniversalClock()->getJulianDay();
    Caelum::Astronomy::getGregorianDateTimeFromJulianDay(jd,year,month,day,hour,minute,second);

    bool modified = mOgitorsRoot->IsSceneModified();
    mClockYear->initAndSignal(year);
    mClockMonth->initAndSignal(month);
    mClockDay->initAndSignal(day);
    mClockHour->initAndSignal(hour);
    mClockMinute->initAndSignal(minute);
    mClockSecond->initAndSignal(second);
    mOgitorsRoot->SetSceneModified(modified);
}
//---------------------------------------------------------------------------------
void CCaelumEditor::createProperties(Ogitors::OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mSunPosition, "sun::position", Ogre::Vector3, Ogre::Vector3(0,0,0), 0, 0);
    PROPERTY_PTR(mSunColour, "sun::colour", Ogre::ColourValue, Ogre::ColourValue(1,1,1), 0, 0);
    PROPERTY_PTR(mSunLightColour, "sun::lightcolour", Ogre::ColourValue, Ogre::ColourValue(1,1,1), 0, 0);

    PROPERTY_PTR(mClockYear  , "clock::year"  , int, 2000, 0, SETTER(int, CCaelumEditor, _setClock));
    PROPERTY_PTR(mClockMonth , "clock::month" , int, 1, 0, SETTER(int, CCaelumEditor, _setClock));
    PROPERTY_PTR(mClockDay   , "clock::day"   , int, 1, 0, SETTER(int, CCaelumEditor, _setClock));
    PROPERTY_PTR(mClockHour  , "clock::hour"  , int, 12, 0, SETTER(int, CCaelumEditor, _setClock));
    PROPERTY_PTR(mClockMinute, "clock::minute", int, 0, 0, SETTER(int, CCaelumEditor, _setClock));
    PROPERTY_PTR(mClockSecond, "clock::second", int, 0, 0, SETTER(int, CCaelumEditor, _setClock));
    PROPERTY_PTR(mClockSpeed , "clock::speed" , Ogre::Real, 0, 0, SETTER(Ogre::Real, CCaelumEditor, _setClockSpeed));

    PROPERTY_PTR(mLongitude, "observer::longitude", Ogre::Real, 0, 0, SETTER(Ogre::Real, CCaelumEditor, _setLongitude));
    PROPERTY_PTR(mLatitude, "observer::latitude", Ogre::Real, 45, 0, SETTER(Ogre::Real, CCaelumEditor, _setLatitude));

    PROPERTY_PTR(mLightingSingleLightSource, "lighting::ensure_single_lightsource", bool, false, 0, SETTER(bool, CCaelumEditor, _setLighting));
    PROPERTY_PTR(mLightingSingleShadowSource, "lighting::ensure_single_shadowsource", bool, false, 0, SETTER(bool, CCaelumEditor, _setLighting));
    PROPERTY_PTR(mLightingManageAmbientLight, "lighting::manage_ambient_light", bool, true, 0, SETTER(bool, CCaelumEditor, _setLighting));
    PROPERTY_PTR(mLightingMinimumAmbientLight, "lighting::minimum_ambient_light", Ogre::ColourValue, Ogre::ColourValue(0.1f,0.1f,0.3f), 0, SETTER(Ogre::ColourValue, CCaelumEditor, _setLightingMinimumAmbientLight));

    PROPERTY_PTR(mFogManage, "fog::manage", bool, false, 0, SETTER(bool, CCaelumEditor, _setFogManage));
    PROPERTY_PTR(mFogDensityMultiplier, "fog::density_multiplier", Ogre::Real, 0, 0, SETTER(Ogre::Real, CCaelumEditor, _setFogDensityMultiplier));

    PROPERTY_PTR(mSunEnable, "sun::enable", bool, true, 0, SETTER(bool, CCaelumEditor, _setSunEnable));
    PROPERTY_PTR(mSunAmbientMultiplier, "sun::ambient_multiplier", Ogre::ColourValue, Ogre::ColourValue(0.5f,0.5f,0.5f), 0, SETTER(Ogre::ColourValue, CCaelumEditor, _setSun));
    PROPERTY_PTR(mSunDiffuseMultiplier, "sun::diffuse_multiplier", Ogre::ColourValue, Ogre::ColourValue(0,0,0), 0, SETTER(Ogre::ColourValue, CCaelumEditor, _setSun));
    PROPERTY_PTR(mSunSpecularMultiplier, "sun::specular_multiplier", Ogre::ColourValue, Ogre::ColourValue(0,0,0), 0, SETTER(Ogre::ColourValue, CCaelumEditor, _setSun));
    PROPERTY_PTR(mSunAutoDisable, "sun::auto_disable", bool, true, 0, SETTER(bool, CCaelumEditor, _setSunAutoDisable));
    PROPERTY_PTR(mSunCastShadow, "sun::cast_shadow", bool, true, 0, SETTER(bool, CCaelumEditor, _setSunCastShadow));
    PROPERTY_PTR(mSunAttenuationDistance, "sun::attenuation::distance", Ogre::Real, 100000.0f, 0, SETTER(Ogre::Real, CCaelumEditor, _setSunAttenuation));
    PROPERTY_PTR(mSunAttenuationConstantMultiplier, "sun::attenuation::constant_multiplier", Ogre::Real, 1.0f, 0, SETTER(Ogre::Real, CCaelumEditor, _setSunAttenuation));
    PROPERTY_PTR(mSunAttenuationLinearMultiplier, "sun::attenuation::linear_multiplier", Ogre::Real, 0, 0, SETTER(Ogre::Real, CCaelumEditor, _setSunAttenuation));
    PROPERTY_PTR(mSunAttenuationQuadricMultiplier, "sun::attenuation::quadric_multiplier",Ogre::Real, 0, 0, SETTER(Ogre::Real, CCaelumEditor, _setSunAttenuation));

    PROPERTY_PTR(mMoonEnable, "moon::enable", bool, true, 0, SETTER(bool, CCaelumEditor, _setMoonEnable));
    PROPERTY_PTR(mMoonAmbientMultiplier, "moon::ambient_multiplier", Ogre::ColourValue, Ogre::ColourValue(0.2f,0.2f,0.2f), 0, SETTER(Ogre::ColourValue, CCaelumEditor, _setMoon));
    PROPERTY_PTR(mMoonDiffuseMultiplier, "moon::diffuse_multiplier", Ogre::ColourValue, Ogre::ColourValue(1,1,0.9f), 0, SETTER(Ogre::ColourValue, CCaelumEditor, _setMoon));
    PROPERTY_PTR(mMoonSpecularMultiplier, "moon::specular_multiplier", Ogre::ColourValue, Ogre::ColourValue(1,1,1), 0, SETTER(Ogre::ColourValue, CCaelumEditor, _setMoon));
    PROPERTY_PTR(mMoonAutoDisable, "moon::auto_disable", bool, true, 0, SETTER(bool, CCaelumEditor, _setMoonAutoDisable));
    PROPERTY_PTR(mMoonCastShadow, "moon::cast_shadow", bool, true, 0, SETTER(bool, CCaelumEditor, _setMoonCastShadow));
    PROPERTY_PTR(mMoonAttenuationDistance, "moon::attenuation::distance", Ogre::Real, 100000.0f, 0, SETTER(Ogre::Real, CCaelumEditor, _setMoonAttenuation));
    PROPERTY_PTR(mMoonAttenuationConstantMultiplier, "moon::attenuation::constant_multiplier", Ogre::Real, 1, 0, SETTER(Ogre::Real, CCaelumEditor, _setMoonAttenuation));
    PROPERTY_PTR(mMoonAttenuationLinearMultiplier, "moon::attenuation::linear_multiplier", Ogre::Real, 0, 0, SETTER(Ogre::Real, CCaelumEditor, _setMoonAttenuation));
    PROPERTY_PTR(mMoonAttenuationQuadricMultiplier, "moon::attenuation::quadric_multiplier",Ogre::Real, 0, 0, SETTER(Ogre::Real, CCaelumEditor, _setMoonAttenuation));

    PROPERTY_PTR(mStarsEnable, "stars::enable", bool, true, 0, SETTER(bool, CCaelumEditor, _setStarsEnable));
    PROPERTY_PTR(mStarsMagnitudeScale, "stars::magnitude_scale", Ogre::Real, 2.51189, 0, SETTER(Ogre::Real, CCaelumEditor, _setStars));
    PROPERTY_PTR(mStarsMag0PixelSize, "stars::mag0_pixel_size", Ogre::Real, 16, 0, SETTER(Ogre::Real, CCaelumEditor, _setStars));
    PROPERTY_PTR(mStarsMinPixelSize, "stars::min_pixel_size", Ogre::Real, 4, 0, SETTER(Ogre::Real, CCaelumEditor, _setStars));
    PROPERTY_PTR(mStarsMaxPixelSize, "stars::max_pixel_size", Ogre::Real, 6, 0, SETTER(Ogre::Real, CCaelumEditor, _setStars));

    PROPERTY_PTR(mCloudsEnabled[0], "clouds::layer0::enable", bool, true, 0, SETTER(bool, CCaelumEditor, _setCloudsEnable));
    PROPERTY_PTR(mCloudsCoverage[0], "clouds::layer0::coverage", Ogre::Real, 0.1f, 0, SETTER(Ogre::Real, CCaelumEditor, _setCloudsCoverage));
    PROPERTY_PTR(mCloudsHeight[0], "clouds::layer0::height", Ogre::Real, 1000, 0, SETTER(Ogre::Real, CCaelumEditor, _setCloudsHeight));
    PROPERTY_PTR(mCloudsSpeed[0], "clouds::layer0::speed", Ogre::Vector2, Ogre::Vector2(0.01f, 0.01f), 0, SETTER(Ogre::Vector2, CCaelumEditor, _setCloudsSpeed));

    PROPERTY_PTR(mCloudsEnabled[1], "clouds::layer1::enable", bool, false, 1, SETTER(bool, CCaelumEditor, _setCloudsEnable));
    PROPERTY_PTR(mCloudsCoverage[1], "clouds::layer1::coverage", Ogre::Real, 0.1f, 1, SETTER(Ogre::Real, CCaelumEditor, _setCloudsCoverage));
    PROPERTY_PTR(mCloudsHeight[1], "clouds::layer1::height", Ogre::Real, 2000, 1, SETTER(Ogre::Real, CCaelumEditor, _setCloudsHeight));
    PROPERTY_PTR(mCloudsSpeed[1], "clouds::layer1::speed", Ogre::Vector2, Ogre::Vector2(0.01f, 0.01f), 1, SETTER(Ogre::Vector2, CCaelumEditor, _setCloudsSpeed));

    PROPERTY_PTR(mCloudsEnabled[2], "clouds::layer2::enable", bool, false, 2, SETTER(bool, CCaelumEditor, _setCloudsEnable));
    PROPERTY_PTR(mCloudsCoverage[2], "clouds::layer2::coverage", Ogre::Real, 0.1f, 2, SETTER(Ogre::Real, CCaelumEditor, _setCloudsCoverage));
    PROPERTY_PTR(mCloudsHeight[2], "clouds::layer2::height", Ogre::Real, 3000, 2, SETTER(Ogre::Real, CCaelumEditor, _setCloudsHeight));
    PROPERTY_PTR(mCloudsSpeed[2], "clouds::layer2::speed", Ogre::Vector2, Ogre::Vector2(0.01f, 0.01f), 2, SETTER(Ogre::Vector2, CCaelumEditor, _setCloudsSpeed));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
TiXmlElement *CCaelumEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pCaelum = pParent->Parent()->InsertEndChild(TiXmlElement("caelum"))->ToElement();

    TiXmlElement *pSun = pCaelum->InsertEndChild(TiXmlElement("sun"))->ToElement();
    pSun->SetAttribute("enable", Ogre::StringConverter::toString(mSunEnable->get()).c_str());
    pSun->SetAttribute("autoDisable", Ogre::StringConverter::toString(mSunAutoDisable->get()).c_str());
    pSun->SetAttribute("position", Ogre::StringConverter::toString(mSunPosition->get()).c_str());
    pSun->SetAttribute("colour", Ogre::StringConverter::toString(mSunColour->get()).c_str());
    pSun->SetAttribute("lightColour", Ogre::StringConverter::toString(mSunLightColour->get()).c_str());
    pSun->SetAttribute("ambientMultiplier", Ogre::StringConverter::toString(mSunAmbientMultiplier->get()).c_str());
    pSun->SetAttribute("diffuseMultiplier", Ogre::StringConverter::toString(mSunDiffuseMultiplier->get()).c_str());
    pSun->SetAttribute("specularMultiplier", Ogre::StringConverter::toString(mSunSpecularMultiplier->get()).c_str());
    pSun->SetAttribute("castShadow", Ogre::StringConverter::toString(mSunCastShadow->get()).c_str());

    TiXmlElement *pSunAttenuation = pSun->InsertEndChild(TiXmlElement("attenuation"))->ToElement();
    pSunAttenuation->SetAttribute("distance", Ogre::StringConverter::toString(mSunAttenuationDistance->get()).c_str());
    pSunAttenuation->SetAttribute("constantMultiplier", Ogre::StringConverter::toString(mSunAttenuationConstantMultiplier->get()).c_str());
    pSunAttenuation->SetAttribute("linearMultiplier", Ogre::StringConverter::toString(mSunAttenuationLinearMultiplier->get()).c_str());
    pSunAttenuation->SetAttribute("quadricMultiplier", Ogre::StringConverter::toString(mSunAttenuationQuadricMultiplier->get()).c_str());

    TiXmlElement *pMoon = pCaelum->InsertEndChild(TiXmlElement("moon"))->ToElement();
    pMoon->SetAttribute("enable", Ogre::StringConverter::toString(mMoonEnable->get()).c_str());
    pMoon->SetAttribute("autoDisable", Ogre::StringConverter::toString(mMoonAutoDisable->get()).c_str());
    pMoon->SetAttribute("ambientMultiplier", Ogre::StringConverter::toString(mMoonAmbientMultiplier->get()).c_str());
    pMoon->SetAttribute("diffuseMultiplier", Ogre::StringConverter::toString(mMoonDiffuseMultiplier->get()).c_str());
    pMoon->SetAttribute("specularMultiplier", Ogre::StringConverter::toString(mMoonSpecularMultiplier->get()).c_str());
    pMoon->SetAttribute("castShadow", Ogre::StringConverter::toString(mMoonCastShadow->get()).c_str());

    TiXmlElement *pMoonAttenuation = pMoon->InsertEndChild(TiXmlElement("attenuation"))->ToElement();
    pMoonAttenuation->SetAttribute("distance", Ogre::StringConverter::toString(mMoonAttenuationDistance->get()).c_str());
    pMoonAttenuation->SetAttribute("constantMultiplier", Ogre::StringConverter::toString(mMoonAttenuationConstantMultiplier->get()).c_str());
    pMoonAttenuation->SetAttribute("linearMultiplier", Ogre::StringConverter::toString(mMoonAttenuationLinearMultiplier->get()).c_str());
    pMoonAttenuation->SetAttribute("quadricMultiplier", Ogre::StringConverter::toString(mMoonAttenuationQuadricMultiplier->get()).c_str());

    TiXmlElement *pClock = pCaelum->InsertEndChild(TiXmlElement("clock"))->ToElement();
    pClock->SetAttribute("year", Ogre::StringConverter::toString(mClockYear->get()).c_str());
    pClock->SetAttribute("month", Ogre::StringConverter::toString(mClockMonth->get()).c_str());
    pClock->SetAttribute("day", Ogre::StringConverter::toString(mClockDay->get()).c_str());
    pClock->SetAttribute("hour", Ogre::StringConverter::toString(mClockHour->get()).c_str());
    pClock->SetAttribute("minute", Ogre::StringConverter::toString(mClockMinute->get()).c_str());
    pClock->SetAttribute("second", Ogre::StringConverter::toString(mClockSecond->get()).c_str());
    pClock->SetAttribute("speed", Ogre::StringConverter::toString(mClockSpeed->get()).c_str());

    TiXmlElement *pObserver = pCaelum->InsertEndChild(TiXmlElement("observer"))->ToElement();
    pObserver->SetAttribute("longitude", Ogre::StringConverter::toString(mLongitude->get()).c_str());
    pObserver->SetAttribute("latitude", Ogre::StringConverter::toString(mLatitude->get()).c_str());

    TiXmlElement *pLighting = pCaelum->InsertEndChild(TiXmlElement("lighting"))->ToElement();
    pLighting->SetAttribute("singleLightsource", Ogre::StringConverter::toString(mLightingSingleLightSource->get()).c_str());
    pLighting->SetAttribute("singleShadowsource", Ogre::StringConverter::toString(mLightingSingleShadowSource->get()).c_str());
    pLighting->SetAttribute("manageAmbientLight", Ogre::StringConverter::toString(mLightingManageAmbientLight->get()).c_str());
    pLighting->SetAttribute("minimumAmbientLight", Ogre::StringConverter::toString(mLightingMinimumAmbientLight->get()).c_str());

    TiXmlElement *pFog = pCaelum->InsertEndChild(TiXmlElement("fog"))->ToElement();
    pFog->SetAttribute("manage", Ogre::StringConverter::toString(mFogManage->get()).c_str());
    pFog->SetAttribute("densityMultiplier", Ogre::StringConverter::toString(mFogDensityMultiplier->get()).c_str());

    TiXmlElement *pStars = pCaelum->InsertEndChild(TiXmlElement("stars"))->ToElement();
    pStars->SetAttribute("enable", Ogre::StringConverter::toString(mStarsEnable->get()).c_str());
    pStars->SetAttribute("magnitudeScale", Ogre::StringConverter::toString(mStarsMagnitudeScale->get()).c_str());
    pStars->SetAttribute("mag0PixelSize", Ogre::StringConverter::toString(mStarsMag0PixelSize->get()).c_str());
    pStars->SetAttribute("minPixelSize", Ogre::StringConverter::toString(mStarsMinPixelSize->get()).c_str());
    pStars->SetAttribute("maxPixelSize", Ogre::StringConverter::toString(mStarsMaxPixelSize->get()).c_str());

    TiXmlElement *pClouds = pCaelum->InsertEndChild(TiXmlElement("clouds"))->ToElement();
    for(int i = 0; i < 3; i++)
    {
        TiXmlElement *pCloudsLayer = pClouds->InsertEndChild(TiXmlElement("layer"))->ToElement();
        pCloudsLayer->SetAttribute("enable", Ogre::StringConverter::toString(mCloudsEnabled[i]->get()).c_str());
        pCloudsLayer->SetAttribute("coverage", Ogre::StringConverter::toString(mCloudsCoverage[i]->get()).c_str());
        pCloudsLayer->SetAttribute("height", Ogre::StringConverter::toString(mCloudsHeight[i]->get()).c_str());
        pCloudsLayer->SetAttribute("speed", Ogre::StringConverter::toString(mCloudsSpeed[i]->get()).c_str());
    }

    return pCaelum;
}



//--------------------------------------------------------------------------------------------------
//-----CAELUMEDITORFACTORY--------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
CCaelumEditorFactory::CCaelumEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Caelum";
    mEditorType = ETYPE_SKY_MANAGER;
    mAddToObjectList = true;
    mIcon = "caelum.svg";
    mCapabilities = CAN_DELETE;

    mMonths.clear();
    mMonths.push_back(PropertyOption("January",Ogre::Any((int)1)));
    mMonths.push_back(PropertyOption("Febuary",Ogre::Any((int)2)));
    mMonths.push_back(PropertyOption("March",Ogre::Any((int)3)));
    mMonths.push_back(PropertyOption("April",Ogre::Any((int)4)));
    mMonths.push_back(PropertyOption("May",Ogre::Any((int)5)));
    mMonths.push_back(PropertyOption("June",Ogre::Any((int)6)));
    mMonths.push_back(PropertyOption("July",Ogre::Any((int)7)));
    mMonths.push_back(PropertyOption("August",Ogre::Any((int)8)));
    mMonths.push_back(PropertyOption("September",Ogre::Any((int)9)));
    mMonths.push_back(PropertyOption("October",Ogre::Any((int)10)));
    mMonths.push_back(PropertyOption("November",Ogre::Any((int)11)));
    mMonths.push_back(PropertyOption("December",Ogre::Any((int)12)));

    OgitorsPropertyDef *definition;

    AddPropertyDefinition("clock::year","UniversalClock::Date::Year","Specifies the current year",PROP_INT, true, true);
    definition = AddPropertyDefinition("clock::month","UniversalClock::Date::Month","Specifies the current month",PROP_INT, true, true);
    definition->setOptions(&mMonths);
    AddPropertyDefinition("clock::day","UniversalClock::Date::Day","Specifies the current day",PROP_INT, true, true);
    AddPropertyDefinition("clock::hour","UniversalClock::Time::Hour","Specifies the current hour",PROP_INT, true, true);
    AddPropertyDefinition("clock::minute","UniversalClock::Time::Minute","Specifies the current minute",PROP_INT, true, true);
    AddPropertyDefinition("clock::second","UniversalClock::Time::Second","Specifies the current second",PROP_INT, true, true);
    AddPropertyDefinition("clock::speed","UniversalClock::Speed","Specifies the current speed multiplier",PROP_REAL, true, true);

    AddPropertyDefinition("observer::longitude","Observer::Longitude","",PROP_REAL, true, true);
    AddPropertyDefinition("observer::latitude","Observer::Latitude","",PROP_REAL, true, true);

    AddPropertyDefinition("lighting::ensure_single_lightsource","Lighting::SingleLightSource","Force single light",PROP_BOOL, true, true);
    AddPropertyDefinition("lighting::ensure_single_shadowsource","Lighting::SingleShadowSource","Force single shadow source",PROP_BOOL, true, true);
    AddPropertyDefinition("lighting::manage_ambient_light","Lighting::ManageAmbientLight","Manage ambient light",PROP_BOOL, true, true);
    AddPropertyDefinition("lighting::minimum_ambient_light","Lighting::MinimumAmbientLight","Manage ambient light",PROP_COLOUR, true, true);

    AddPropertyDefinition("fog::manage","Fog::Manage","",PROP_BOOL, true, true);
    AddPropertyDefinition("fog::density_multiplier","Fog::DensityMultiplier","",PROP_REAL, true, true);

    AddPropertyDefinition("sun::enable","Sun::Enable","",PROP_BOOL, true, true);
    AddPropertyDefinition("sun::ambient_multiplier","Sun::AmbientMultiplier","",PROP_COLOUR, true, true);
    AddPropertyDefinition("sun::diffuse_multiplier","Sun::DiffuseMultiplier","",PROP_COLOUR, true, true);
    AddPropertyDefinition("sun::specular_multiplier","Sun::SpecularMultiplier","",PROP_COLOUR, true, true);
    AddPropertyDefinition("sun::auto_disable","Sun::AutoDisable","",PROP_BOOL, true, true);
    AddPropertyDefinition("sun::cast_shadow","Sun::CastShadow","",PROP_BOOL, true, true);
    AddPropertyDefinition("sun::attenuation::distance","Sun::Attenuation::Distance","",PROP_REAL, true, true);
    AddPropertyDefinition("sun::attenuation::constant_multiplier","Sun::Attenuation::ConstantMultiplier","",PROP_REAL, true, true);
    AddPropertyDefinition("sun::attenuation::linear_multiplier","Sun::Attenuation::LinearMultiplier","",PROP_REAL, true, true);
    AddPropertyDefinition("sun::attenuation::quadric_multiplier","Sun::Attenuation::QuadricMultiplier","",PROP_REAL, true, true);

    AddPropertyDefinition("sun::position","Sun::Position","The position of the sun.",PROP_VECTOR3, false, false, false);
    AddPropertyDefinition("sun::colour","Sun::Colour","The colour of the sun.",PROP_COLOUR, false, false, false);
    AddPropertyDefinition("sun::lightcolour","Sun::LightColour","The colour of the sun light.",PROP_COLOUR, false, false, false);

    AddPropertyDefinition("moon::enable","Moon::Enable","",PROP_BOOL, true, true);
    AddPropertyDefinition("moon::ambient_multiplier","Moon::AmbientMultiplier","",PROP_COLOUR, true, true);
    AddPropertyDefinition("moon::diffuse_multiplier","Moon::DiffuseMultiplier","",PROP_COLOUR, true, true);
    AddPropertyDefinition("moon::specular_multiplier","Moon::SpecularMultiplier","",PROP_COLOUR, true, true);
    AddPropertyDefinition("moon::auto_disable","Moon::AutoDisable","",PROP_BOOL, true, true);
    AddPropertyDefinition("moon::cast_shadow","Moon::CastShadow","",PROP_BOOL, true, true);
    AddPropertyDefinition("moon::attenuation::distance","Moon::Attenuation::Distance","",PROP_REAL, true, true);
    AddPropertyDefinition("moon::attenuation::constant_multiplier","Moon::Attenuation::ConstantMultiplier","",PROP_REAL, true, true);
    AddPropertyDefinition("moon::attenuation::linear_multiplier","Moon::Attenuation::LinearMultiplier","",PROP_REAL, true, true);
    AddPropertyDefinition("moon::attenuation::quadric_multiplier","Moon::Attenuation::QuadricMultiplier","",PROP_REAL, true, true);

    AddPropertyDefinition("stars::enable","Stars::Enable","",PROP_BOOL, true, true);
    AddPropertyDefinition("stars::magnitude_scale","Stars::MagnitudeScale","",PROP_REAL, true, true);
    AddPropertyDefinition("stars::mag0_pixel_size","Stars::Mag0PixelSize","",PROP_REAL, true, true);
    AddPropertyDefinition("stars::min_pixel_size","Stars::MinPixelSize","",PROP_REAL, true, true);
    AddPropertyDefinition("stars::max_pixel_size","Stars::MinPixelSize","",PROP_REAL, true, true);

    AddPropertyDefinition("clouds::layer0::enable","Clouds::Layer0::Enable","",PROP_BOOL, true, true);
    AddPropertyDefinition("clouds::layer0::coverage","Clouds::Layer0::Coverage","",PROP_REAL, true, true);
    AddPropertyDefinition("clouds::layer0::height","Clouds::Layer0::Height","",PROP_REAL, true, true);
    AddPropertyDefinition("clouds::layer0::speed","Clouds::Layer0::Speed","",PROP_VECTOR2, true, true);

    AddPropertyDefinition("clouds::layer1::enable","Clouds::Layer1::Enable","",PROP_BOOL, true, true);
    AddPropertyDefinition("clouds::layer1::coverage","Clouds::Layer1::Coverage","",PROP_REAL, true, true);
    AddPropertyDefinition("clouds::layer1::height","Clouds::Layer1::Height","",PROP_REAL, true, true);
    AddPropertyDefinition("clouds::layer1::speed","Clouds::Layer1::Speed","",PROP_VECTOR2, true, true);

    AddPropertyDefinition("clouds::layer2::enable","Clouds::Layer2::Enable","",PROP_BOOL, true, true);
    AddPropertyDefinition("clouds::layer2::coverage","Clouds::Layer2::Coverage","",PROP_REAL, true, true);
    AddPropertyDefinition("clouds::layer2::height","Clouds::Layer2::Height","",PROP_REAL, true, true);
    AddPropertyDefinition("clouds::layer2::speed","Clouds::Layer2::Speed","",PROP_VECTOR2, true, true);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);

    it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CCaelumEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CCaelumEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CCaelumEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
  Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
  Ogre::String value = "/" + OgitorsRoot::getSingletonPtr()->GetProjectOptions()->CaelumDirectory;
  OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();
    
  if(params.find("init") != params.end())
  {
      mFile->createDirectory(value.c_str());   
      Ogre::String copydir = OgitorsUtils::GetEditorResourcesPath() + "/Caelum/";
      OgitorsUtils::CopyDirOfs(copydir, value + "/");

      params.erase(params.find("init"));
  }

  CCaelumEditor *object = OGRE_NEW CCaelumEditor(this);
  object->createProperties(params);
  object->mParentEditor->init(*parent);
  object->load();
  object->update(0);
//  object->registerForPostSceneUpdates();

  mInstanceCount++;
  return object;
}
//--------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Caelum Plugin";
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, OGRE_NEW CCaelumEditorFactory());
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "Caelum Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    return true;
}
//----------------------------------------------------------------------------