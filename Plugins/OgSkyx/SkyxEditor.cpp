///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include "Ogitors.h"
#include "ofs.h"
#include "SkyxEditor.h"

using namespace Ogitors;
using namespace Ogre;

//---------------------------------------------------------------------------------
CSkyxEditor::CSkyxEditor(CBaseEditorFactory *factory) : CBaseEditor(factory)
{
    mHandle = 0;
    mName->init("SkyX");

    mUsesGizmos = false;
    mUsesHelper = false;
}
//---------------------------------------------------------------------------------
CSkyxEditor::~CSkyxEditor()
{
}
//---------------------------------------------------------------------------------
bool CSkyxEditor::update(float timePassed)
{
    mHandle->update(timePassed);
    mHandle->notifyCameraRender(mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera());
    return false;
}
//---------------------------------------------------------------------------------
Ogre::SceneManager *CSkyxEditor::getSceneManager()
{
    return mHandle->getSceneManager();
}
//---------------------------------------------------------------------------------
void CSkyxEditor::refresh()
{
    if(!mHandle) 
        return;
}
//---------------------------------------------------------------------------------
void CSkyxEditor::_restoreState()
{
    mHandle->setTimeMultiplier(mTimeMultiplier->get());
    
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();    
    opt.RayleighMultiplier  = mRayleighMultiplier->get();
    opt.MieMultiplier       = mMieMultiplier->get();
    opt.NumberOfSamples     = mSampleCount->get();
    opt.HeightPosition      = mHeightPosition->get();
    opt.Exposure            = mExposure->get();
    opt.InnerRadius         = mInnerRadius->get();
    opt.OuterRadius         = mOuterRadius->get();
    opt.SunIntensity        = mSunIntensity->get();
    opt.WaveLength          = mWaveLength->get();
    opt.G                   = mG->get();
    mHandle->getAtmosphereManager()->setOptions(opt);

    mBasicController->setTime(mTime->get());
    mBasicController->setEastDirection(mEastPosition->get());
    mBasicController->setMoonPhase(mMoonPhase->get());

    if(mVCEnable->get() == true)
        if (!mHandle->getVCloudsManager()->isCreated())
            mHandle->getVCloudsManager()->create(mHandle->getMeshManager()->getSkydomeRadius(mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera()));
    else
        if(mHandle->getVCloudsManager()->isCreated())
            mHandle->getVCloudsManager()->remove();
 
    mHandle->getVCloudsManager()->setAutoupdate(mVCAutoUpdate->get());
    mHandle->getVCloudsManager()->getVClouds()->setWindSpeed(mVCWindSpeed->get());
    mHandle->getVCloudsManager()->getVClouds()->setWindDirection(Ogre::Degree(mVCWindDirection->get()));
    mHandle->getVCloudsManager()->getVClouds()->setNoiseScale(mVCNoiseScale->get());
    Ogre::ColourValue colour = mVCAmbientColor->get();
    mHandle->getVCloudsManager()->getVClouds()->setAmbientColor(Ogre::Vector3(colour.r,colour.g, colour.b));
    mHandle->getVCloudsManager()->getVClouds()->setLightResponse(mVCLightReponse->get());
    mHandle->getVCloudsManager()->getVClouds()->setAmbientFactors(mVCAmbientFactors->get());
    mHandle->getVCloudsManager()->getVClouds()->setWheater(mVCWeather->get().x, mVCWeather->get().y, false);
}
//---------------------------------------------------------------------------------
void CSkyxEditor::onSave(bool forced)
{
}
//---------------------------------------------------------------------------------
bool CSkyxEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
    Ogre::String value = mOgitorsRoot->GetProjectFile()->getFileSystemName() + "::/SkyX/";
    mngr->addResourceLocation(value, "Ofs", "SkyX");
    mngr->initialiseResourceGroup("SkyX");

	// Create SkyX
    mBasicController = new SkyX::BasicController(true);
    mHandle = new SkyX::SkyX(mOgitorsRoot->GetSceneManager(), mBasicController);
	mHandle->create();

    mHandle->getVCloudsManager()->getVClouds()->registerCamera(mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera());

    mHandle->getVCloudsManager()->getVClouds()->setDistanceFallingParams(Ogre::Vector2(2,-1));

    _restoreState();

    registerForUpdates();

    mLoaded->set(true);
    return true;
}
//----------------------------------------------------------------------------------------
bool CSkyxEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unRegisterForUpdates();

    if(mHandle)
    {
        if(mHandle->isCreated()) mHandle->remove();
        delete mHandle;
    }

    mOgitorsRoot->DestroyResourceGroup("SkyX");

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CSkyxEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mTimeMultiplier        , "options::timemultiplier"     , Ogre::Real,           0.08f,                                  0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsTimeMultiplier));
    
    // SkyX Basic Controller parameters
    PROPERTY_PTR(mTime                  , "options::time"               , Ogre::Vector3,        Ogre::Vector3(8.80f, 7.50f, 20.50f),    0, SETTER(Ogre::Vector3,        CSkyxEditor, _setOptionsTime));
    PROPERTY_PTR(mEastPosition          , "options::eastposition"       , Ogre::Vector2,        Ogre::Vector2(0, 1),                    0, SETTER(Ogre::Vector2,        CSkyxEditor, _setOptionsEastPosition));
    PROPERTY_PTR(mMoonPhase             , "options::moonphase"          , Ogre::Real,           0,                                      0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsMoonPhase));

    // SkyX Atmosphere parameters
    PROPERTY_PTR(mInnerRadius           , "options::innerradius"        , Ogre::Real,           9.77501f,                               0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsInnerRadius));
    PROPERTY_PTR(mOuterRadius           , "options::outerradius"        , Ogre::Real,           10.2963f,                               0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsOuterRadius));
    PROPERTY_PTR(mHeightPosition        , "options::height"             , Ogre::Real,           0.1f,                                   0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsHeight));
    PROPERTY_PTR(mRayleighMultiplier    , "options::rayleighmultiplier" , Ogre::Real,           0.0022f,                                0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsRayleighMultiplier));
    PROPERTY_PTR(mMieMultiplier         , "options::miemultiplier"      , Ogre::Real,           0.000675f,                              0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsMieMultiplier));
    PROPERTY_PTR(mSunIntensity          , "options::sunintensity"       , Ogre::Real,           30,                                     0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsSunIntensity));
    PROPERTY_PTR(mWaveLength            , "options::wavelength"         , Ogre::Vector3,        Ogre::Vector3(0.57f, 0.54f, 0.44f),     0, SETTER(Ogre::Vector3,        CSkyxEditor, _setOptionsWaveLength));
    PROPERTY_PTR(mG                     , "options::g"                  , Ogre::Real,           -0.991f,                                0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsG));
    PROPERTY_PTR(mExposure              , "options::exposure"           , Ogre::Real,           3.0f,                                   0, SETTER(Ogre::Real,           CSkyxEditor, _setOptionsExposure));
    PROPERTY_PTR(mSampleCount           , "options::samplecount"        , int,                  4,                                      0, SETTER(int,                  CSkyxEditor, _setOptionsSampleCount));

    // SkyX Volumetric Clouds parameters
    PROPERTY_PTR(mVCEnable              , "vclouds::enable"             , bool,                 true,                                   0, SETTER(bool,                 CSkyxEditor, _setVCEnable));
    PROPERTY_PTR(mVCAutoUpdate          , "vclouds::autoupdate"         , bool,                 false,                                  0, SETTER(bool,                 CSkyxEditor, _setVCAutoUpdate));
    PROPERTY_PTR(mVCWindSpeed           , "vclouds::windspeed"          , Ogre::Real,           80,                                     0, SETTER(Ogre::Real,           CSkyxEditor, _setVCWindSpeed));
    PROPERTY_PTR(mVCWindDirection       , "vclouds::winddirection"      , Ogre::Real,           0,                                      0, SETTER(Ogre::Real,           CSkyxEditor, _setVCWindDirection));
    PROPERTY_PTR(mVCNoiseScale          , "vclouds::noisescale"         , Ogre::Real,           4.2f,                                   0, SETTER(Ogre::Real,           CSkyxEditor, _setVCNoiseScale));
    PROPERTY_PTR(mVCAmbientColor        , "vclouds::ambientcolor"       , Ogre::ColourValue,    Ogre::ColourValue(0.6f, 0.6f, 0.7f),    0, SETTER(Ogre::ColourValue,    CSkyxEditor, _setVCAmbientColor));
    PROPERTY_PTR(mVCLightReponse        , "vclouds::lightresponse"      , Ogre::Vector4,        Ogre::Vector4(0.3f, 0.2f, 0.9f, 0.1f),  0, SETTER(Ogre::Vector4,        CSkyxEditor, _setVCLightResponse));
    PROPERTY_PTR(mVCAmbientFactors      , "vclouds::ambientfactors"     , Ogre::Vector4,        Ogre::Vector4(0.4f, 0.7f, 0.0f, 0.0f),  0, SETTER(Ogre::Vector4,        CSkyxEditor, _setVCAmbientFactors));
    PROPERTY_PTR(mVCWeather             , "vclouds::weather"            , Ogre::Vector2,        Ogre::Vector2(0.8f, 0.5f),              0, SETTER(Ogre::Vector2,        CSkyxEditor, _setVCWeather));
    
    // SkyX Volumetric Clouds Lightning parameters
    PROPERTY_PTR(mVCLightningEnable     , "vclightning::enable"         , bool,                 false,                                  0, SETTER(bool,                 CSkyxEditor, _setVCLightningEnable));
    PROPERTY_PTR(mVCLightningAT         , "vclightning::at"             , Ogre::Real,           0.5f,                                   0, SETTER(Ogre::Real,           CSkyxEditor, _setVCLightningAT));
    PROPERTY_PTR(mVCLightningColor      , "vclightning::color"          , Ogre::ColourValue,    Ogre::ColourValue(0.9f, 1.0f, 1.0f),    0, SETTER(Ogre::ColourValue,    CSkyxEditor, _setVCLightningColor));
    PROPERTY_PTR(mVCLightningTM         , "vclightning::tm"             , Ogre::Real,           2.0f,                                   0, SETTER(Ogre::Real,           CSkyxEditor, _setVCLightningTM));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsTimeMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->setTimeMultiplier(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsRayleighMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.RayleighMultiplier = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsMieMultiplier(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.MieMultiplier = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsSampleCount(OgitorsPropertyBase* property, const int& value)
{
    if(value < 1)
        return false;
    
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.NumberOfSamples = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsHeight(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.HeightPosition = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsExposure(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.Exposure = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsInnerRadius(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(value >= mOuterRadius->get())
        return false;

    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.InnerRadius = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsOuterRadius(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(value <= mInnerRadius->get())
        return false;

    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.OuterRadius = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsTime(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    Ogre::Vector3 checkvalue = value;
    if(checkvalue.x < 0.0f)
        checkvalue.x = 0.0f;
    else if(checkvalue.x > 24.0f)
        checkvalue.x = 24.0f;

    if(checkvalue.y < 0.0f)
        checkvalue.y = 0.0f;
    else if(checkvalue.y > 24.0f)
        checkvalue.y = 24.0f;

    if(checkvalue.z < 0.0f)
        checkvalue.z = 0.0f;
    else if(checkvalue.z > 24.0f)
        checkvalue.z = 24.0f;

    mTime->init(checkvalue);

    mBasicController->setTime(checkvalue);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsEastPosition(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{
    mBasicController->setEastDirection(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsSunIntensity(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.SunIntensity = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsWaveLength(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.WaveLength = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setOptionsG(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    SkyX::AtmosphereManager::Options opt = mHandle->getAtmosphereManager()->getOptions();
    opt.G = value;
    mHandle->getAtmosphereManager()->setOptions(opt);
    return true;
}
//----------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setOptionsMoonPhase(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mBasicController->setMoonPhase(value);
    return true;
}
//----------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCEnable(OgitorsPropertyBase* property, const bool& value)
{
    if(value == true)
    {
        if (!mHandle->getVCloudsManager()->isCreated())
            mHandle->getVCloudsManager()->create();
    }
    else
    {
        if(mHandle->getVCloudsManager()->isCreated())
            mHandle->getVCloudsManager()->remove();
    }   

    return true;
}
//----------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCAutoUpdate(OgitorsPropertyBase* property, const bool& value)
{
    mHandle->getVCloudsManager()->setAutoupdate(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setVCWindSpeed(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getVCloudsManager()->getVClouds()->setWindSpeed(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSkyxEditor::_setVCWindDirection(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getVCloudsManager()->getVClouds()->setWindDirection(Ogre::Degree(value));
    return true;
}
//----------------------------------------------------------------------------
bool CSkyxEditor::_setVCNoiseScale(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getVCloudsManager()->getVClouds()->setNoiseScale(value);
    return true;
}
//-----------------------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCAmbientColor(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    Ogre::ColourValue colour = mVCAmbientColor->get();
    mHandle->getVCloudsManager()->getVClouds()->setAmbientColor(Ogre::Vector3(colour.r,colour.g, colour.b));
    return true;
}
//----------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCLightResponse(OgitorsPropertyBase* property, const Ogre::Vector4& value)
{
    mHandle->getVCloudsManager()->getVClouds()->setLightResponse(value);
    return true;
}
//----------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCAmbientFactors(OgitorsPropertyBase* property, const Ogre::Vector4& value)
{
    mHandle->getVCloudsManager()->getVClouds()->setAmbientFactors(value);
    return true;
}
//-------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCWeather(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{   
    mHandle->getVCloudsManager()->getVClouds()->setWheater(value.x, value.y, false);
    return true;
}
//-------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCLightningEnable(OgitorsPropertyBase* property, const bool& value)
{
    mHandle->getVCloudsManager()->getVClouds()->getLightningManager()->setEnabled(value);
    return true;
}
//-------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCLightningAT(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getVCloudsManager()->getVClouds()->getLightningManager()->setAverageLightningApparitionTime(value);
    return true;
}
//-------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCLightningColor(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    Ogre::ColourValue colour = mVCAmbientColor->get();    
    mHandle->getVCloudsManager()->getVClouds()->getLightningManager()->setLightningColor(Ogre::Vector3(colour.r,colour.g, colour.b));
    return true;
}
//-------------------------------------------------------------------------
bool Ogitors::CSkyxEditor::_setVCLightningTM(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    mHandle->getVCloudsManager()->getVClouds()->getLightningManager()->setLightningTimeMultiplier(value);
    return true;
}

//-----------------------------------------------------------------------------------------
TiXmlElement *CSkyxEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pSkyX = pParent->Parent()->InsertEndChild(TiXmlElement("skyx"))->ToElement();

    pSkyX->SetAttribute("rayleighMultiplier", Ogre::StringConverter::toString(mRayleighMultiplier->get()).c_str());
    pSkyX->SetAttribute("mieMultiplier", Ogre::StringConverter::toString(mMieMultiplier->get()).c_str());
    pSkyX->SetAttribute("exposure", Ogre::StringConverter::toString(mExposure->get()).c_str());
    pSkyX->SetAttribute("innerRadius", Ogre::StringConverter::toString(mInnerRadius->get()).c_str());
    pSkyX->SetAttribute("outerRadius", Ogre::StringConverter::toString(mOuterRadius->get()).c_str());
    pSkyX->SetAttribute("sampleCount", Ogre::StringConverter::toString(mSampleCount->get()).c_str());
    pSkyX->SetAttribute("height", Ogre::StringConverter::toString(mHeightPosition->get()).c_str());
    pSkyX->SetAttribute("sunIntensity", Ogre::StringConverter::toString(mSunIntensity->get()).c_str());
    pSkyX->SetAttribute("G", Ogre::StringConverter::toString(mG->get()).c_str());

    TiXmlElement *pTime = pSkyX->InsertEndChild(TiXmlElement("time"))->ToElement();
    pTime->SetAttribute("multiplier", Ogre::StringConverter::toString(mTimeMultiplier->get()).c_str());
    pTime->SetAttribute("current", Ogre::StringConverter::toString(mTime->get().x).c_str());
    pTime->SetAttribute("sunRise", Ogre::StringConverter::toString(mTime->get().y).c_str());
    pTime->SetAttribute("sunSet",  Ogre::StringConverter::toString(mTime->get().z).c_str());
    
    TiXmlElement *pEastPosition = pSkyX->InsertEndChild(TiXmlElement("eastPosition"))->ToElement();
    pEastPosition->SetAttribute("X", Ogre::StringConverter::toString(mEastPosition->get().x).c_str());
    pEastPosition->SetAttribute("Y", Ogre::StringConverter::toString(mEastPosition->get().y).c_str());

    TiXmlElement *pWaveLength = pSkyX->InsertEndChild(TiXmlElement("waveLength"))->ToElement();
    pWaveLength->SetAttribute("R", Ogre::StringConverter::toString(mWaveLength->get().x).c_str());
    pWaveLength->SetAttribute("G", Ogre::StringConverter::toString(mWaveLength->get().y).c_str());
    pWaveLength->SetAttribute("B", Ogre::StringConverter::toString(mWaveLength->get().z).c_str());

    TiXmlElement *pvClouds = pSkyX->InsertEndChild(TiXmlElement("vClouds"))->ToElement();
    pvClouds->SetAttribute("windSpeed", Ogre::StringConverter::toString(mVCWindSpeed->get()).c_str());
    pvClouds->SetAttribute("windDirection", Ogre::StringConverter::toString(mVCWindDirection->get()).c_str());
    pvClouds->SetAttribute("noiseScale", Ogre::StringConverter::toString(mVCNoiseScale->get()).c_str());

    return pSkyX;
}

//----------------------------------------------------------------------------
//----HYDRAXEDITORFACTORY-----------------------------------------------------
//----------------------------------------------------------------------------
CSkyxEditorFactory::CSkyxEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Skyx Object";
    mEditorType = ETYPE_SKY_MANAGER;
    mAddToObjectList = true;
    mIcon = "caelum.svg";
    mCapabilities = CAN_DELETE;

    OgitorsPropertyDef * definition = 0;

    AddPropertyDefinition("options::timemultiplier",                    "Options::Time Mult.",          "", PROP_REAL);
    definition = AddPropertyDefinition("options::rayleighmultiplier",   "Options::Rayleigh Mult.",      "", PROP_REAL);
    definition->setStepSize(Ogre::Any(0.0001f));
    definition = AddPropertyDefinition("options::miemultiplier",        "Options::Mie Mult.",           "", PROP_REAL);
    definition->setStepSize(Ogre::Any(0.000001f));
    AddPropertyDefinition("options::samplecount",                       "Options::Sample Count",        "", PROP_INT);
    definition = AddPropertyDefinition("options::height",               "Options::Height",              "0=InnerRadius, 1=OuterRadius", PROP_REAL);
    definition->setRange(Ogre::Any(0.0f), Ogre::Any(1.0f), Ogre::Any(0.05f));
    AddPropertyDefinition("options::exposure",                          "Options::Exposure",            "", PROP_REAL);
    definition = AddPropertyDefinition("options::innerradius",          "Options::Inner Radius",        "", PROP_REAL);
    definition->setStepSize(Ogre::Any(0.00001f));
    definition = AddPropertyDefinition("options::outerradius",          "Options::Outer Radius",        "", PROP_REAL);
    definition->setStepSize(Ogre::Any(0.0001f));
    definition = AddPropertyDefinition("options::time",                 "Options::Time",                "", PROP_VECTOR3);
    definition->setFieldNames("Current", "Sun Rise", "Sun Set");
    AddPropertyDefinition("options::eastposition",                      "Options::East Position",       "", PROP_VECTOR2);
    AddPropertyDefinition("options::sunintensity",                      "Options::Sun Intensity",       "", PROP_REAL);
    definition = AddPropertyDefinition("options::wavelength",           "Options::Wave Length",         "", PROP_VECTOR3);
    definition->setFieldNames("R Mult.", "G Mult.", "B Mult.");
    AddPropertyDefinition("options::g",                                 "Options::G",                   "", PROP_REAL);
    definition = AddPropertyDefinition("options::moonphase",            "Options::Moon Phase",          "", PROP_REAL);
    definition->setRange(Ogre::Any(-1.0f), Ogre::Any(1.0f));

    // SkyX Volumetric Clouds
    AddPropertyDefinition("vclouds::enable",                            "Volumetric Clouds::Enable",            "", PROP_BOOL);
    AddPropertyDefinition("vclouds::autoupdate",                        "Volumetric Clouds::Auto Update",       "", PROP_BOOL);
    AddPropertyDefinition("vclouds::noisescale",                        "Volumetric Clouds::Noise Scale",       "", PROP_REAL);
    AddPropertyDefinition("vclouds::windspeed",                         "Volumetric Clouds::Wind Speed",        "", PROP_REAL);
    AddPropertyDefinition("vclouds::winddirection",                     "Volumetric Clouds::Wind Direction",    "", PROP_REAL);
    AddPropertyDefinition("vclouds::noisescale",                        "Volumetric Clouds::Noise Scale",       "", PROP_REAL);
    definition = AddPropertyDefinition("vclouds::ambientcolor",         "Volumetric Clouds::Ambient Color",     "", PROP_COLOUR);
    definition = AddPropertyDefinition("vclouds::lightresponse",        "Volumetric Clouds::Light Response",    "", PROP_VECTOR4);
    definition->setFieldNames("Sun light power", "Sun beta multiplier", "Ambient color multiplier", "Distance attenuation");
    definition = AddPropertyDefinition("vclouds::ambientfactors",       "Volumetric Clouds::Ambient Factors",   "", PROP_VECTOR4);
    definition->setFieldNames("Constant", "Linear", "Quadratic", "Cubic");    
    definition = AddPropertyDefinition("vclouds::weather",              "Volumetric Clouds::Weather",           "", PROP_VECTOR2);
    definition->setFieldNames("Humidity", "Average Cloud Size");
    definition->setRange(Ogre::Any(Ogre::Vector2(0, 0)), Ogre::Any(Ogre::Vector2(1, 1)), Ogre::Any(Ogre::Vector2(0.05, 0.05)));
    
    // SkyX Volumetric Clouds Lightning
    AddPropertyDefinition("vclightning::enable",                        "Lightning::Enable",                    "", PROP_BOOL);
    AddPropertyDefinition("vclightning::at",                            "Lightning::Avg. Aparition Time",       "in seconds", PROP_REAL);
    AddPropertyDefinition("vclightning::color",                         "Lightning::Color",                     "", PROP_COLOUR);
    AddPropertyDefinition("vclightning::tm",                            "Lightning::Time Multiplier",           "", PROP_REAL);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);

    it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//----------------------------------------------------------------------------
CBaseEditorFactory *CSkyxEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CSkyxEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CSkyxEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
  OgitorsRoot *ogroot = OgitorsRoot::getSingletonPtr();
  Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
  Ogre::String value = "/SkyX";
  OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();
    
  CSkyxEditor *object = OGRE_NEW CSkyxEditor(this);

  if(params.find("init") != params.end())
  {
      mFile->createDirectory(value.c_str());   
      Ogre::String copydir = OgitorsUtils::GetEditorResourcesPath() + "/SKYX/";
      OgitorsUtils::CopyDirOfs(copydir, value + "/");

      params.erase(params.find("init"));
  }

  object->createProperties(params);
  object->mParentEditor->init(*parent);
  object->load();
  object->update(0);

  mInstanceCount++;
  return object;
}
//----------------------------------------------------------------------------
void CSkyxEditorFactory::DestroyObject(CBaseEditor *object)
{
    CSkyxEditor *SKYXOBJECT = static_cast<CSkyxEditor*>(object);

    SKYXOBJECT->unLoad();
    SKYXOBJECT->destroyAllChildren();
    if(SKYXOBJECT->getName() != "")
        OgitorsRoot::getSingletonPtr()->UnRegisterObjectName(SKYXOBJECT->getName(), SKYXOBJECT);

    OGRE_DELETE SKYXOBJECT;
    mInstanceCount--;
}

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "SkyX Plugin";
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, OGRE_NEW CSkyxEditorFactory());
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "SkyX Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    return true;
}
//----------------------------------------------------------------------------
