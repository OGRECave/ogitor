///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include "Ogitors.h"
#include "ofs.h"
#include "HydraxWaterEditor.h"

using namespace Ogitors;
using namespace Ogre;

PropertyOptionsVector CHydraxEditorFactory::mShaderModes;
PropertyOptionsVector CHydraxEditorFactory::mTextureSizes;

//---------------------------------------------------------------------------------
CHydraxEditor::CHydraxEditor(CBaseEditorFactory *factory) : CBaseEditor(factory)
{
    mHandle = 0;
    mModule = 0;
    mName->init("HydraxWater");
}
//---------------------------------------------------------------------------------
CHydraxEditor::~CHydraxEditor()
{
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::getObjectContextMenu(UTFStringVector &menuitems)
{
    menuitems.clear();
    menuitems.push_back(OTR("Load Hydrax config file") + ";:/icons/import.svg");

    return true;
}
//-----------------------------------------------------------------------------------------
void CHydraxEditor::onObjectContextMenu(int menuresult)
{
    switch(menuresult)
    {
        // Load landscape definitions from XML
        case 0:
            UTFStringVector extlist;
            extlist.push_back(OTR("Hydrax Config File"));
            extlist.push_back("*.hdx");

            Ogre::String filename = mSystem->DisplayOpenDialog(OTR("Open"), extlist, "");
            if(filename == "") 
                return;

            mConfigFile->set( filename );
            mSystem->SetSetting("system", "oldOpenPath", OgitorsUtils::ExtractFilePath(filename));

        break;
    }
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow)
{
    ITerrainEditor *ed = mOgitorsRoot->GetTerrainEditor();
    if(ed)
    {
        StringVector matnames = ed->getMaterialNames();
        for(unsigned int i = 0;i < matnames.size();i++)
        {
             addDepthTechnique(matnames[i]);
        }
    }
    return false;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setConfigFile(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mHandle)
    {
        mHandle->loadCfg( value );
        _initAndSignalProperties();
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
     if(mHandle)
     {
         mHandle->setPosition(value);
     }
     return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setWaterColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mHandle)
    {
        mHandle->setWaterColor(Ogre::Vector3(value.r,value.g,value.b));
    }

    mOriginalWaterColour->set(value);

    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPlanesError(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setPlanesError(value);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setShaderMode(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        Hydrax::MaterialManager::ShaderMode smtype = (Hydrax::MaterialManager::ShaderMode)value;
        mHandle->setShaderMode(smtype);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setFullReflectionDistance(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFullReflectionDistance(value);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setGlobalTransparency(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setGlobalTransparency(value);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNormalDistortion(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setNormalDistortion(value);
    }
    return true;
}

void CHydraxEditor::__setComponent( Hydrax::HydraxComponent hc, const bool &value )
{
    unsigned int eComp = static_cast<unsigned int>(mHandle->getComponents());
    if(value) eComp |= hc; else eComp &= ~(hc);
    mHandle->setComponents(static_cast<Hydrax::HydraxComponent>(eComp));
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentSun(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        __setComponent( Hydrax::HYDRAX_COMPONENT_SUN, value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentFoam(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        __setComponent( Hydrax::HYDRAX_COMPONENT_FOAM, value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentDepth(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        __setComponent( Hydrax::HYDRAX_COMPONENT_DEPTH, value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentSmooth(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        __setComponent( Hydrax::HYDRAX_COMPONENT_SMOOTH, value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentCaustics(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        __setComponent( Hydrax::HYDRAX_COMPONENT_CAUSTICS, value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentUnderwater(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        __setComponent( Hydrax::HYDRAX_COMPONENT_UNDERWATER, value );
        if(!value)
        {
            __setComponent( Hydrax::HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS, false );
            __setComponent( Hydrax::HYDRAX_COMPONENT_UNDERWATER_GODRAYS, false );
        }
    }
    if(!value)
    {
        mComponentUnderwaterReflections->initAndSignal(false);
        mComponentUnderwaterGodrays->initAndSignal(false);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentUnderwaterReflections(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        if( mComponentUnderwater->get() )
            __setComponent( Hydrax::HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS, value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setComponentUnderwaterGodRays(OgitorsPropertyBase* property, const bool &value)
{
    if(mHandle)
    {
        if( mComponentUnderwater->get() )
            __setComponent( Hydrax::HYDRAX_COMPONENT_UNDERWATER_GODRAYS, value );
    }
    return true;
}
//----------------------------------------------------------------------------
bool CHydraxEditor::_setSunPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(mHandle)
    {
        mHandle->setSunPosition(value);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setSunStrength(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setSunStrength(value);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setSunArea(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setSunArea(value);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setSunColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mHandle)
    {
        mHandle->setSunColor(Ogre::Vector3(value.r,value.g,value.b));
    }
    return true;
}


//---------------------------------------------------------------------------------
bool CHydraxEditor::_setFoamStart(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFoamStart( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setFoamMaxDistance(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFoamMaxDistance( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setFoamScale(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFoamScale( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setFoamTransparency(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFoamTransparency( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setDepthLimit(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setDepthLimit( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setSmoothPower(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setSmoothPower( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setCausticsScale(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setCausticsScale( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setCausticsPower(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setCausticsPower( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setCausticsEnd(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setCausticsEnd( value );
    }
    return true;
}
//----------------------------------------------------------------------------
bool CHydraxEditor::_setGodraysExposure(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(mHandle)
    {
        mHandle->setGodRaysExposure(value);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setGodraysIntensity(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setGodRaysIntensity( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setGodraysSpeed(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle && mHandle->getGodRaysManager())
    {
        mHandle->getGodRaysManager()->setSimulationSpeed( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setGodraysNumberOfRays(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle && mHandle->getGodRaysManager())
    {
        mHandle->getGodRaysManager()->setNumberOfRays( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setGodraysSize(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle && mHandle->getGodRaysManager())
    {
        mHandle->getGodRaysManager()->setRaysSize( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setGodraysIntersections(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle && mHandle->getGodRaysManager())
    {
        mHandle->getGodRaysManager()->setObjectIntersectionsEnabled( value );
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setRttReflection(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->getRttManager()->setTextureSize(Hydrax::RttManager::RTT_REFLECTION, Hydrax::Size(value, value));
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setRttRefraction(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->getRttManager()->setTextureSize(Hydrax::RttManager::RTT_REFRACTION, Hydrax::Size(value, value));
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setRttDepth(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->getRttManager()->setTextureSize(Hydrax::RttManager::RTT_DEPTH, Hydrax::Size(value, value));
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setRttDepthReflection(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->getRttManager()->setTextureSize(Hydrax::RttManager::RTT_DEPTH_REFLECTION, Hydrax::Size(value, value));
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setRttDepthAIP(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->getRttManager()->setTextureSize(Hydrax::RttManager::RTT_DEPTH_AIP, Hydrax::Size(value, value));
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setRttGpuNormalMap(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->getRttManager()->setTextureSize(Hydrax::RttManager::RTT_GPU_NORMAL_MAP, Hydrax::Size(value, value));
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setModuleName(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mHandle)
    {
        // TODO
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPGChoppyStrength(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Module::ProjectedGrid::Options oVal = mModule->getOptions();
        oVal.ChoppyStrength = value;
        mModule->setOptions(oVal);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPGChoppyWaves(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        Hydrax::Module::ProjectedGrid::Options oVal = mModule->getOptions();
        oVal.ChoppyWaves = value;
        mModule->setOptions(oVal);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPGComplexity(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        Hydrax::Module::ProjectedGrid::Options oVal = mModule->getOptions();
        oVal.Complexity = value;
        mModule->setOptions(oVal);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPGElevation(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Module::ProjectedGrid::Options oVal = mModule->getOptions();
        oVal.Elevation = value;
        mModule->setOptions(oVal);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPGForceRecalculateGeometry(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        Hydrax::Module::ProjectedGrid::Options oVal = mModule->getOptions();
        oVal.ForceRecalculateGeometry = value;
        mModule->setOptions(oVal);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPGSmooth(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        Hydrax::Module::ProjectedGrid::Options oVal = mModule->getOptions();
        oVal.Smooth = value;
        mModule->setOptions(oVal);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setPGStrength(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Module::ProjectedGrid::Options oVal = mModule->getOptions();
        oVal.Strength = value;
        mModule->setOptions(oVal);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseModuleName(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mHandle)
    {
        // TODO
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseOctaves(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        noiseOptions.Octaves = value;
        nModule->setOptions(noiseOptions);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseScale(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        noiseOptions.Scale = value;
        nModule->setOptions(noiseOptions);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseFallOff(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        noiseOptions.Falloff = value;
        nModule->setOptions(noiseOptions);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseAnimSpeed(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        noiseOptions.Animspeed = value;
        nModule->setOptions(noiseOptions);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseTimeMulti(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        noiseOptions.Timemulti = value;
        nModule->setOptions(noiseOptions);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseGpuStrength(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        noiseOptions.GPU_Strength = value;
        nModule->setOptions(noiseOptions);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setNoiseGpuLod(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(mHandle)
    {
        Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        noiseOptions.GPU_LODParameters = value;
        nModule->setOptions(noiseOptions);
    }
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setAddDepthTechnique(OgitorsPropertyBase* property, const Ogre::String& value)
{
    addDepthTechnique(value);
    static_cast<OgitorsProperty<Ogre::String>*>(property)->init("");
    return true;
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::_setRemoveDepthTechnique(OgitorsPropertyBase* property, const Ogre::String& value)
{
    removeDepthTechnique(value);
    static_cast<OgitorsProperty<Ogre::String>*>(property)->init("");
    return true;
}
//---------------------------------------------------------------------------------
void CHydraxEditor::_createWaterPlane(Ogre::SceneManager *mSceneMgr, Ogre::Camera *mCamera, Ogre::Viewport *mViewport)
{
    Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
    Ogre::String value = mOgitorsRoot->GetProjectFile()->getFileSystemName() + "::/" + mOgitorsRoot->GetProjectOptions()->HydraxDirectory + "/";
    mngr->addResourceLocation(value,"Ofs","Hydrax");

    mHandle = new Hydrax::Hydrax(mSceneMgr, mCamera, mViewport);

    // Create our projected grid module
    mModule = new Hydrax::Module::ProjectedGrid(// Hydrax parent pointer
                                                mHandle,
                                                // Noise module
                                                new Hydrax::Noise::Perlin(/*Generic one*/),
                                                // Base plane
                                                Ogre::Plane(Ogre::Vector3(0,1,0), Ogre::Vector3(0,0,0)),
                                                // Normal mode
                                                Hydrax::MaterialManager::NM_VERTEX,
                                                // Projected grid options
                                                Hydrax::Module::ProjectedGrid::Options(64));

    // Set our module
    mHandle->setModule(static_cast<Hydrax::Module::Module*>(mModule));

    // Load all parameters from config file
    // Remarks: The config file must be in Hydrax resource group.
    // All parameters can be set/updated directly by code(Like previous versions),
    // but due to the high number of customizable parameters, Hydrax 0.4 allows save/load config files.
    mHandle->loadCfg(mConfigFile->get());
    // Create water
    mHandle->create();
    mHandle->getMesh()->getEntity()->setQueryFlags(0);
}
//---------------------------------------------------------------------------------
bool CHydraxEditor::update(float timePassed)
{
    if(mHandle)
    {
        if(mHandle->getCamera() != mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera())
        {
            if(mHandle->isCreated()) mHandle->remove();
            delete mHandle;

            _createWaterPlane(mOgitorsRoot->GetSceneManager(),mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera(),(Ogre::Viewport*)mOgitorsRoot->GetViewport()->getHandle());
            if(mOgitorsRoot->GetTerrainEditor())
            {
                StringVector matNames = mOgitorsRoot->GetTerrainEditor()->getMaterialNames();
                for(unsigned int i = 0;i < matNames.size();i++)
                {
                    removeDepthTechnique(matNames[i]);
                    addDepthTechnique(matNames[i]);
                }
            }
        }

        if(mCaelumIntegration->get())
        {
            CBaseEditor *CaelumED = mOgitorsRoot->FindObject("CaelumSky", ETYPE_SKY_MANAGER);
            if(CaelumED)
            {
                Ogre::Vector3 value;
                Ogre::ColourValue cval;
                CaelumED->getProperties()->getValue("sun::position",value);
                mHandle->setSunPosition(value);
                CaelumED->getProperties()->getValue("sun::colour",cval);
                mHandle->setSunColor(Ogre::Vector3(cval.r,cval.g,cval.b));
                CaelumED->getProperties()->getValue("sun::lightcolour",cval);
                mHandle->setWaterColor(Ogre::Vector3(cval.r,cval.g,cval.b));
            }

            Vector3 col = mHandle->getWaterColor();
            Ogre::ColourValue colOrig = mOriginalWaterColour->get();
            Vector3 original = Ogre::Vector3(colOrig.r, colOrig.g, colOrig.b);
            float height = mHandle->getSunPosition().y / 10.0f;

            if(height < -99.0f)
            {
                col = original * 0.1f;
                height = 9999.0f;
            }
            else if(height < 1.0f)
            {
                col = original * (0.1f + (0.009f * (height + 99.0f)));
                height = 100.0f / (height + 99.001f);
            }
            else if(height < 2.0f)
            {
                col += original;
                col    /= 2.0f;
                float percent = (height - 1.0f);
                col = (col * percent) + (original * (1.0f - percent));
            }
            else
            {
                col += original;
                col    /= 2.0f;
            }
            mHandle->setWaterColor(col);
            mHandle->setSunArea(height);
        }
        mHandle->update(timePassed);
    }
    return false;
}
//---------------------------------------------------------------------------------
void CHydraxEditor::addDepthTechnique(const Ogre::String& matname)
{
    if(!mHandle)
        return;

    Ogre::MaterialPtr pMat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(matname));
    
    if(pMat.isNull())
        return;
    
    for(unsigned int i = 0;i < pMat->getNumTechniques();i++)
    {
        if(pMat->getTechnique(i)->getSchemeName() == "HydraxDepth") return;
    }

    mHandle->getMaterialManager()->addDepthTechnique(pMat->createTechnique());
}
//---------------------------------------------------------------------------------
void CHydraxEditor::removeDepthTechnique(const Ogre::String& matname)
{
    Ogre::MaterialPtr pMat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName(matname));

    for(unsigned int i = 0;i < pMat->getNumTechniques();i++)
    {
        if(pMat->getTechnique(i)->getSchemeName() == "HydraxDepth")
        {
            pMat->removeTechnique(i);
            return;
        }
    }
}
//---------------------------------------------------------------------------------
Ogre::SceneManager *CHydraxEditor::getSceneManager()
{
    return mHandle->getSceneManager();
}
//---------------------------------------------------------------------------------
void CHydraxEditor::refresh()
{
    if(!mHandle) return;
    mHandle->remove();
    mHandle->create();
}
//---------------------------------------------------------------------------------
void CHydraxEditor::onSave(bool forced)
{
    Ogre::String hydraxpath = "/" + mOgitorsRoot->GetProjectOptions()->HydraxDirectory + "/";
    Ogre::ColourValue colOrig = mOriginalWaterColour->get();
    Ogre::Vector3 original = Ogre::Vector3(colOrig.r, colOrig.g, colOrig.b);
    mHandle->setWaterColor(original);

    Ogre::String cfgData;
    mHandle->saveCfg(cfgData);

    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();
    Ogre::String path =  "/" + OgitorsRoot::getSingletonPtr()->GetProjectOptions()->HydraxDirectory + "/" + mConfigFile->get();

    OFS::OFSHANDLE handle;
    mFile->createFile(handle, path.c_str(), cfgData.size(), cfgData.size(), cfgData.c_str());
    mFile->closeFile(handle);
}
//---------------------------------------------------------------------------------
void CHydraxEditor::_initAndSignalProperties( void )
{
    mShaderMode->init((int)mHandle->getShaderMode());
    mFullReflectionDistance->initAndSignal(mHandle->getFullReflectionDistance());
    mGlobalTransparency->initAndSignal(mHandle->getGlobalTransparency());
    mNormalDistortion->initAndSignal(mHandle->getNormalDistortion());
    mSunPosition->initAndSignal(mHandle->getSunPosition());
    mSunStrength->initAndSignal(mHandle->getSunStrength());
    mSunArea->initAndSignal(mHandle->getSunArea());
    mSunColour->initAndSignal(Ogre::ColourValue( mHandle->getSunColor().x, mHandle->getSunColor().y, mHandle->getSunColor().z));

    Hydrax::HydraxComponent Comp = mHandle->getComponents();
    mComponentSun->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_SUN) > 0);
    mComponentFoam->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_FOAM) > 0);
    mComponentDepth->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_DEPTH) > 0);
    mComponentSmooth->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_DEPTH) > 0);
    mComponentCaustics->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_CAUSTICS) > 0);
    mComponentUnderwater->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_UNDERWATER) > 0);
    mComponentUnderwaterReflections->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_UNDERWATER_REFLECTIONS) > 0);
    mComponentUnderwaterGodrays->initAndSignal((Comp & Hydrax::HYDRAX_COMPONENT_UNDERWATER_GODRAYS) > 0);

    mFoamStart->initAndSignal(mHandle->getFoamStart());
    mFoamMaxDistance->initAndSignal(mHandle->getFoamMaxDistance());
    mFoamScale->initAndSignal(mHandle->getFoamScale());
    mFoamTransparency->initAndSignal(mHandle->getFoamTransparency());

    mDepthLimit->initAndSignal(mHandle->getDepthLimit());

    mCausticsScale->initAndSignal(mHandle->getCausticsScale());
    mCausticsPower->initAndSignal(mHandle->getCausticsPower());
    mCausticsEnd->initAndSignal(mHandle->getCausticsEnd());

    mGodraysExposure->initAndSignal(mHandle->getGodRaysExposure());
    mGodraysIntensity->initAndSignal(mHandle->getGodRaysIntensity());

    if(mHandle->getGodRaysManager())
    {
        mGodraysSpeed->initAndSignal(mHandle->getGodRaysManager()->getSimulationSpeed());
        mGodraysNumberOfRays->initAndSignal(mHandle->getGodRaysManager()->getNumberOfRays());
        mGodraysSize->initAndSignal(mHandle->getGodRaysManager()->getRaysSize());
        mGodraysIntersections->initAndSignal(mHandle->getGodRaysManager()->areObjectsIntersectionsEnabled());
    }

    mRttReflection->initAndSignal(mHandle->getRttManager()->getTextureSize(Hydrax::RttManager::RTT_REFLECTION).Width);
    mRttRefraction->initAndSignal(mHandle->getRttManager()->getTextureSize(Hydrax::RttManager::RTT_REFRACTION).Width);
    mRttDepth->initAndSignal(mHandle->getRttManager()->getTextureSize(Hydrax::RttManager::RTT_DEPTH).Width);
    mRttDepthReflection->initAndSignal(mHandle->getRttManager()->getTextureSize(Hydrax::RttManager::RTT_DEPTH_REFLECTION).Width);
    mRttDepthAIP->initAndSignal(mHandle->getRttManager()->getTextureSize(Hydrax::RttManager::RTT_DEPTH_AIP).Width);
    mRttGpuNormalMap->initAndSignal(mHandle->getRttManager()->getTextureSize(Hydrax::RttManager::RTT_GPU_NORMAL_MAP).Width);

    mModuleName->initAndSignal(mModule->getName());
    mPGComplexity->initAndSignal(mModule->getOptions().Complexity);
    mPGElevation->initAndSignal(mModule->getOptions().Elevation);
    mPGSmooth->initAndSignal(mModule->getOptions().Smooth);
    mPGStrength->initAndSignal(mModule->getOptions().Strength);
    mPGForceRecalculateGeometry->initAndSignal(mModule->getOptions().ForceRecalculateGeometry);
    mPGChoppyWaves->initAndSignal(mModule->getOptions().ChoppyWaves);
    mPGChoppyStrength->initAndSignal(mModule->getOptions().ChoppyStrength);

    Hydrax::Noise::Perlin *nModule = static_cast<Hydrax::Noise::Perlin *>(mModule->getNoise());
    mNoiseModuleName->initAndSignal(nModule->getName());
    if( nModule )
    {
        Hydrax::Noise::Perlin::Options noiseOptions = nModule->getOptions();
        mNoiseOctaves->initAndSignal(noiseOptions.Octaves);
        mNoiseScale->initAndSignal(noiseOptions.Scale);
        mNoiseFallOff->initAndSignal(noiseOptions.Falloff);
        mNoiseAnimSpeed->initAndSignal(noiseOptions.Animspeed);
        mNoiseTimeMulti->initAndSignal(noiseOptions.Timemulti);
        mNoiseGpuStrength->initAndSignal(noiseOptions.GPU_Strength);
        mNoiseGpuLod->initAndSignal(noiseOptions.GPU_LODParameters);
    }

    Ogre::Vector3 value = mHandle->getWaterColor();
    Ogre::ColourValue watcol = Ogre::ColourValue(value.x, value.y, value.z);
    mOriginalWaterColour->initAndSignal(watcol);

    value = mHandle->getPosition();
    mPosition->initAndSignal(value);
}
//----------------------------------------------------------------------------
bool CHydraxEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    _createWaterPlane(mOgitorsRoot->GetSceneManager(),mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera(),(Ogre::Viewport*)mOgitorsRoot->GetViewport()->getHandle());

    _initAndSignalProperties();

    if(mOgitorsRoot->GetTerrainEditor())
    {
        Ogre::StringVector matNames = mOgitorsRoot->GetTerrainEditor()->getMaterialNames();
        for(unsigned int i = 0;i < matNames.size();i++)
        {
            addDepthTechnique(matNames[i]);
        }
    }

    registerForUpdates();

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CHydraxEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unRegisterForUpdates();

    NameObjectPairList list = mOgitorsRoot->GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::iterator it = list.begin();
    while(it != list.end())
    {
        Ogre::CompositorManager::getSingleton().removeCompositor(mHandle->getViewport(), "_Hydrax_Underwater_Compositor_Name");
        it++;
    }

    if(mHandle)
    {
        if(mHandle->isCreated()) mHandle->remove();
        delete mHandle;
    }

    mOgitorsRoot->DestroyResourceGroup("Hydrax");

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CHydraxEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mPosition, "position", Ogre::Vector3, Ogre::Vector3::ZERO, 0, SETTER(Ogre::Vector3, CHydraxEditor, _setPosition));
    PROPERTY_PTR(mConfigFile, "configfile", Ogre::String, "", 0, SETTER(Ogre::String, CHydraxEditor, _setConfigFile));
    PROPERTY_PTR(mCaelumIntegration, "caelumintegration", bool, false, 0, 0);
    PROPERTY_PTR(mPlanesError, "planes_error", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setPlanesError));
    PROPERTY_PTR(mShaderMode, "shader_mode", int, Hydrax::MaterialManager::SM_CG, 0, SETTER(int, CHydraxEditor, _setShaderMode));
    PROPERTY_PTR(mFullReflectionDistance, "full_reflection_distance", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setFullReflectionDistance));
    PROPERTY_PTR(mGlobalTransparency, "global_transparency", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setGlobalTransparency));
    PROPERTY_PTR(mNormalDistortion, "normal_distortion", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setNormalDistortion));
    PROPERTY_PTR(mOriginalWaterColour, "watercolour", Ogre::ColourValue, Ogre::ColourValue(0,0,0), 0, SETTER(Ogre::ColourValue, CHydraxEditor, _setWaterColour));

    PROPERTY_PTR(mComponentSun, "components::sun", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentSun));
    PROPERTY_PTR(mComponentFoam, "components::foam", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentFoam));
    PROPERTY_PTR(mComponentDepth, "components::depth", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentDepth));
    PROPERTY_PTR(mComponentSmooth, "components::smooth", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentSmooth));
    PROPERTY_PTR(mComponentCaustics, "components::caustics", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentCaustics));
    PROPERTY_PTR(mComponentUnderwater, "components::underwater", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentUnderwater));
    PROPERTY_PTR(mComponentUnderwaterReflections, "components::underwater_reflections", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentUnderwaterReflections));
    PROPERTY_PTR(mComponentUnderwaterGodrays, "components::underwater_godrays", bool, 0, 0, SETTER(bool, CHydraxEditor, _setComponentUnderwaterGodRays));

    PROPERTY_PTR(mSunPosition, "sun::position", Ogre::Vector3, Ogre::Vector3::ZERO, 0, SETTER(Ogre::Vector3, CHydraxEditor, _setSunPosition));
    PROPERTY_PTR(mSunStrength, "sun::strength", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setSunStrength));
    PROPERTY_PTR(mSunArea, "sun::area", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setSunArea));
    PROPERTY_PTR(mSunColour, "sun::colour", Ogre::ColourValue, Ogre::ColourValue(0,0,0), 0, 0);

    PROPERTY_PTR(mFoamStart, "foam::start", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setFoamStart));
    PROPERTY_PTR(mFoamMaxDistance, "foam::max_distance", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setFoamMaxDistance));
    PROPERTY_PTR(mFoamScale, "foam::scale", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setFoamScale));
    PROPERTY_PTR(mFoamTransparency, "foam::transparency", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setFoamTransparency));

    PROPERTY_PTR(mDepthLimit, "depth::limit", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setDepthLimit));

    PROPERTY_PTR(mSmoothPower, "smooth::power", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setSmoothPower));

    PROPERTY_PTR(mCausticsScale, "caustics::scale", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setCausticsScale));
    PROPERTY_PTR(mCausticsPower, "caustics::power", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setCausticsPower));
    PROPERTY_PTR(mCausticsEnd, "caustics::end", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setCausticsEnd));

    PROPERTY_PTR(mGodraysExposure, "godrays::exposure", Ogre::Vector3, Ogre::Vector3::ZERO, 0, SETTER(Ogre::Vector3, CHydraxEditor, _setGodraysExposure));
    PROPERTY_PTR(mGodraysIntensity, "godrays::intensity", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setGodraysIntensity));
    PROPERTY_PTR(mGodraysSpeed, "godrays::speed", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setGodraysSpeed));
    PROPERTY_PTR(mGodraysNumberOfRays, "godrays::number_of_rays", int, 0, 0, SETTER(int, CHydraxEditor, _setGodraysNumberOfRays));
    PROPERTY_PTR(mGodraysSize, "godrays::rays_size", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setGodraysSize));
    PROPERTY_PTR(mGodraysIntersections, "godrays::intersections", bool, 0, 0, SETTER(bool, CHydraxEditor, _setGodraysIntersections));

    PROPERTY_PTR(mRttReflection, "rtt_quality::reflection", int, 0, 0, SETTER(int, CHydraxEditor, _setRttReflection));
    PROPERTY_PTR(mRttRefraction, "rtt_quality::refraction", int, 0, 0, SETTER(int, CHydraxEditor, _setRttRefraction));
    PROPERTY_PTR(mRttDepth, "rtt_quality::depth", int, 0, 0, SETTER(int, CHydraxEditor, _setRttDepth));
    PROPERTY_PTR(mRttDepthReflection, "rtt_quality::depth_reflection", int, 0, 0, SETTER(int, CHydraxEditor, _setRttDepthReflection));
    PROPERTY_PTR(mRttDepthAIP, "rtt_quality::depth_aip", int, 0, 0, SETTER(int, CHydraxEditor, _setRttDepthAIP));
    PROPERTY_PTR(mRttGpuNormalMap, "rtt_quality::gpu_normal_map", int, 0, 0, SETTER(int, CHydraxEditor, _setRttGpuNormalMap));

    PROPERTY_PTR(mModuleName, "module_name", Ogre::String, "", 0, SETTER(Ogre::String, CHydraxEditor, _setModuleName));
    PROPERTY_PTR(mPGChoppyStrength, "pgmodule::choppy_strength", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setPGChoppyStrength));
    PROPERTY_PTR(mPGChoppyWaves, "pgmodule::choppy_waves", bool, 0, 0, SETTER(bool, CHydraxEditor, _setPGChoppyWaves));
    PROPERTY_PTR(mPGComplexity, "pgmodule::complexity", int, 0, 0, SETTER(int, CHydraxEditor, _setPGComplexity));
    PROPERTY_PTR(mPGElevation, "pgmodule::elevation", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setPGElevation));
    PROPERTY_PTR(mPGForceRecalculateGeometry, "pgmodule::force_recalculate_geometry", bool, 0, 0, SETTER(bool, CHydraxEditor, _setPGForceRecalculateGeometry));
    PROPERTY_PTR(mPGSmooth, "pgmodule::smooth", bool, 0, 0, SETTER(bool, CHydraxEditor, _setPGSmooth));
    PROPERTY_PTR(mPGStrength, "pgmodule::strength", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setPGStrength));

    PROPERTY_PTR(mNoiseModuleName, "noise_module_name", Ogre::String, "", 0, SETTER(Ogre::String, CHydraxEditor, _setNoiseModuleName));
    PROPERTY_PTR(mNoiseOctaves, "perlin::octaves", int, 0, 0, SETTER(int, CHydraxEditor, _setNoiseOctaves));
    PROPERTY_PTR(mNoiseScale, "perlin::scale", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setNoiseScale));
    PROPERTY_PTR(mNoiseFallOff, "perlin::falloff", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setNoiseFallOff));
    PROPERTY_PTR(mNoiseAnimSpeed, "perlin::anim_speed", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setNoiseAnimSpeed));
    PROPERTY_PTR(mNoiseTimeMulti, "perlin::time_multi", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setNoiseTimeMulti));
    PROPERTY_PTR(mNoiseGpuStrength, "perlin::gpu_strength", Ogre::Real, 0, 0, SETTER(Ogre::Real, CHydraxEditor, _setNoiseGpuStrength));
    PROPERTY_PTR(mNoiseGpuLod, "perlin::gpu_lod", Ogre::Vector3, Ogre::Vector3::ZERO, 0, SETTER(Ogre::Vector3, CHydraxEditor, _setNoiseGpuLod));

    PROPERTY("technique_add", Ogre::String, "", 0, SETTER(Ogre::String, CHydraxEditor, _setAddDepthTechnique));
    PROPERTY("technique_remove", Ogre::String, "", 0, SETTER(Ogre::String, CHydraxEditor, _setRemoveDepthTechnique));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
TiXmlElement *CHydraxEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pHydrax = pParent->Parent()->InsertEndChild(TiXmlElement("hydrax"))->ToElement();
    pHydrax->SetAttribute("configFile", mConfigFile->get().c_str());
    pHydrax->SetAttribute("caelumIntegration", Ogre::StringConverter::toString(mCaelumIntegration->get()).c_str());

    return pHydrax;
}
//-----------------------------------------------------------------------------------------


//----------------------------------------------------------------------------
//----HYDRAXEDITORFACTORY-----------------------------------------------------
//----------------------------------------------------------------------------
CHydraxEditorFactory::CHydraxEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Hydrax";
    mEditorType = ETYPE_WATER_MANAGER;
    mAddToObjectList = true;
    mIcon = "hydrax.svg";
    mCapabilities = CAN_DELETE;
    mUsesGizmos = false;
    mUsesHelper = false;

    mShaderModes.clear();
    mShaderModes.push_back(PropertyOption("SM_HLSL",Ogre::Any((int)Hydrax::MaterialManager::SM_HLSL)));
    mShaderModes.push_back(PropertyOption("SM_CG",Ogre::Any((int)Hydrax::MaterialManager::SM_CG)));
    mShaderModes.push_back(PropertyOption("SM_GLSL",Ogre::Any((int)Hydrax::MaterialManager::SM_GLSL)));

    mTextureSizes.clear();
    mTextureSizes.push_back(PropertyOption("Auto", Ogre::Any((int)0)));
    for( int i=1; i <= 1024; i*=2 )
        mTextureSizes.push_back(PropertyOption(Ogre::StringConverter::toString(i)+"x"+Ogre::StringConverter::toString(i),Ogre::Any(i)));

    OgitorsPropertyDef *definition;

    AddPropertyDefinition("position","Main::Position","The Position of Water Surface.",PROP_VECTOR3, true, true);
    AddPropertyDefinition("configfile","Main::Config File","The configuration file.",PROP_STRING, true, false);
    AddPropertyDefinition("caelumintegration","Main::Caelum Integration","Adjust water colour according to Caelum Sun Light?",PROP_BOOL, true, true);
    AddPropertyDefinition("planes_error","Main::Planes Error","",PROP_REAL, true, true);
    definition = AddPropertyDefinition("shader_mode","Main::Shader Mode","0=HLSL, 1=CG, 2=GLSL",PROP_INT, true, true);
    definition->setOptions(&mShaderModes);
    AddPropertyDefinition("full_reflection_distance","Main::Full Reflection Distance","",PROP_REAL, true, true);
    AddPropertyDefinition("global_transparency","Main::Global Transparency","",PROP_REAL, true, true);
    AddPropertyDefinition("normal_distortion","Main::Normal Distortion","",PROP_REAL, true, true);
    AddPropertyDefinition("watercolour","Main::Water Colour","",PROP_COLOUR, true, true);

    AddPropertyDefinition("technique_add","","Adds depth technique.",PROP_STRING, false, false, false);
    AddPropertyDefinition("technique_remove","","Removes depth technique.",PROP_STRING, false, false, false);

    AddPropertyDefinition("components::sun","Components::Sun","",PROP_BOOL, true, true);
    AddPropertyDefinition("components::foam","Components::Foam","",PROP_BOOL, true, true);
    AddPropertyDefinition("components::depth","Components::Depth","",PROP_BOOL, true, true); //FIXME: crashes when turned off
    AddPropertyDefinition("components::smooth","Components::Smooth","",PROP_BOOL, true, true);
    AddPropertyDefinition("components::caustics","Components::Caustics","",PROP_BOOL, true, true);
    AddPropertyDefinition("components::underwater","Components::Underwater","",PROP_BOOL, true, true);
    AddPropertyDefinition("components::underwater_reflections","Components::Underwater Reflections","",PROP_BOOL, true, true);
    AddPropertyDefinition("components::underwater_godrays","Components::Underwater Godrays","",PROP_BOOL, true, true);

    AddPropertyDefinition("sun::position","Sun::Position","",PROP_VECTOR3, false, false, false);
    AddPropertyDefinition("sun::strength","Sun::Strength","",PROP_REAL, true, true);
    AddPropertyDefinition("sun::area","Sun::Area","",PROP_REAL, false, false, false);
    AddPropertyDefinition("sun::colour","Sun::Colour","",PROP_COLOUR, false, false, false);

    AddPropertyDefinition("foam::max_distance","Foam::Max Distance","",PROP_REAL, true, true);
    AddPropertyDefinition("foam::scale","Foam::Scale","",PROP_REAL, true, true);
    AddPropertyDefinition("foam::start","Foam::Start","",PROP_REAL, true, true);
    AddPropertyDefinition("foam::transparency","Foam::Transparency","",PROP_REAL, true, true);

    AddPropertyDefinition("depth::limit","Depth::Limit","",PROP_REAL, true, true);

    AddPropertyDefinition("smooth::power","Smooth::Power","",PROP_REAL, true, true);

    AddPropertyDefinition("caustics::scale","Caustics::Scale","",PROP_REAL, true, true);
    AddPropertyDefinition("caustics::power","Caustics::Power","",PROP_REAL, true, true);
    AddPropertyDefinition("caustics::end","Caustics::End","",PROP_REAL, true, true);

    AddPropertyDefinition("godrays::exposure","God Rays::Exposure","",PROP_VECTOR3, true, true);
    AddPropertyDefinition("godrays::intensity","God Rays::Intensity","",PROP_REAL, true, true);
    AddPropertyDefinition("godrays::speed","God Rays::Speed","",PROP_REAL, true, true);
    AddPropertyDefinition("godrays::number_of_rays","God Rays::Number of Rays","",PROP_INT, true, true);
    AddPropertyDefinition("godrays::rays_size","God Rays::Rays Size","",PROP_REAL, true, true);
    AddPropertyDefinition("godrays::intersections","God Rays::Intersections","",PROP_BOOL, true, true);

    definition = AddPropertyDefinition("rtt_quality::reflection","Rtt Quality::Reflection","",PROP_INT, true, true);
    definition->setOptions(&mTextureSizes);
    definition = AddPropertyDefinition("rtt_quality::refraction","Rtt Quality::Refraction","",PROP_INT, true, true);
    definition->setOptions(&mTextureSizes);
    definition = AddPropertyDefinition("rtt_quality::depth","Rtt Quality::Depth","",PROP_INT, true, true);
    definition->setOptions(&mTextureSizes);
    definition = AddPropertyDefinition("rtt_quality::depth_reflection","Rtt Quality::Depth Reflection","",PROP_INT, true, true);
    definition->setOptions(&mTextureSizes);
    definition = AddPropertyDefinition("rtt_quality::depth_aip","Rtt Quality::Depth AIP","",PROP_INT, true, true);
    definition->setOptions(&mTextureSizes);
    definition = AddPropertyDefinition("rtt_quality::gpu_normal_map","Rtt Quality::GPU Normal Map","",PROP_INT, true, true);
    definition->setOptions(&mTextureSizes);

    AddPropertyDefinition("module_name","Module Name","",PROP_STRING, true, false);
    AddPropertyDefinition("pgmodule::choppy_strength","PGModule::Choppy Strength","",PROP_REAL, true, true);
    AddPropertyDefinition("pgmodule::choppy_waves","PGModule::Waves","",PROP_BOOL, true, true);
    AddPropertyDefinition("pgmodule::complexity","PGModule::Complexity","",PROP_INT, true, true);
    AddPropertyDefinition("pgmodule::elevation","PGModule::Elevation","",PROP_REAL, true, true);
    AddPropertyDefinition("pgmodule::force_recalculate_geometry","PGModule::Force Recalculate Geometry","",PROP_BOOL, true, true);
    AddPropertyDefinition("pgmodule::smooth","PGModule::Smooth","",PROP_BOOL, true, true);
    AddPropertyDefinition("pgmodule::strength","PGModule::Strength","",PROP_REAL, true, true);

    AddPropertyDefinition("noise_module_name","Noise Module Name","",PROP_STRING, true, false);
    AddPropertyDefinition("perlin::octaves","Perlin::Octaves","",PROP_INT, true, true);
    AddPropertyDefinition("perlin::scale","Perlin::Scale","",PROP_REAL, true, true);
    AddPropertyDefinition("perlin::falloff","Perlin::Falloff","",PROP_REAL, true, true);
    AddPropertyDefinition("perlin::anim_speed","Perlin::Animation Speed","",PROP_REAL, true, true);
    AddPropertyDefinition("perlin::time_multi","Perlin::Time Multiplier","",PROP_REAL, true, true);
    AddPropertyDefinition("perlin::gpu_strength","Perlin::GPU Strength","",PROP_REAL, true, true);
    AddPropertyDefinition("perlin::gpu_lod","Perlin::GPU LoD","",PROP_VECTOR3, true, true);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);

    it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//----------------------------------------------------------------------------
CBaseEditorFactory *CHydraxEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CHydraxEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CHydraxEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
  OgitorsRoot *ogroot = OgitorsRoot::getSingletonPtr();
  Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
  Ogre::String value = "/" + OgitorsRoot::getSingletonPtr()->GetProjectOptions()->HydraxDirectory;
  OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();
    
  CHydraxEditor *object = OGRE_NEW CHydraxEditor(this);

  if(params.find("init") != params.end())
  {
      mFile->createDirectory(value.c_str());   
      Ogre::String copydir = OgitorsUtils::GetEditorResourcesPath() + "/Hydrax/";
      OgitorsUtils::CopyDirOfs(copydir, value + "/");

      OgitorsPropertyValue propValue;
      propValue.propType = PROP_STRING;
      propValue.val = Ogre::Any(Ogre::String("Hydrax.hdx"));
      params["configfile"] = propValue;
      params.erase(params.find("init"));
  }

  object->createProperties(params);
  object->mParentEditor->init(*parent);
  object->load();
  object->update(0);
  object->registerForPostSceneUpdates();

  mInstanceCount++;
  return object;
}
//----------------------------------------------------------------------------
void CHydraxEditorFactory::DestroyObject(CBaseEditor *object)
{
    ITerrainEditor *terrain = OgitorsRoot::getSingletonPtr()->GetTerrainEditor();
    CHydraxEditor *HYDRAXOBJECT = static_cast<CHydraxEditor*>(object);

    if(terrain)
    {
        StringVector matnames = terrain->getMaterialNames();
        for(unsigned int i = 0;i < matnames.size();i++)
        {
            HYDRAXOBJECT->removeDepthTechnique(matnames[i]);
        }
    }

    HYDRAXOBJECT->unLoad();
    HYDRAXOBJECT->destroyAllChildren();
    if(HYDRAXOBJECT->getName() != "")
        OgitorsRoot::getSingletonPtr()->UnRegisterObjectName(HYDRAXOBJECT->getName(), HYDRAXOBJECT);

    OGRE_DELETE HYDRAXOBJECT;
    mInstanceCount--;
}

//----------------------------------------------------------------------------
bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Hydrax Plugin";
    OgitorsRoot::getSingletonPtr()->RegisterEditorFactory(identifier, OGRE_NEW CHydraxEditorFactory());
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "Hydrax Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    return true;
}
//----------------------------------------------------------------------------
