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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "SceneManagerEditor.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "tinyxml.h"

using namespace Ogitors;

PropertyOptionsVector CSceneManagerEditorFactory::mFogModes;
PropertyOptionsVector CSceneManagerEditorFactory::mTextureMapSizes;
PropertyOptionsVector CSceneManagerEditorFactory::mShadowTechniqueNames;

//-------------------------------------------------------------------------------
CSceneManagerEditor::CSceneManagerEditor(CBaseEditorFactory *factory) : CBaseEditor(factory)
{
    mHandle = 0;
    mUsesGizmos = false;
    mUsesHelper = false;

    mSphereSceneQuery = 0;
    mRaySceneQuery = 0;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::update(float timePassed)
{
    if(mShadowsEnabled->get())
    {
        Ogre::SceneManager::MovableObjectIterator mit = mHandle->getMovableObjectIterator("Light");
        if(!(mit.hasMoreElements()))
            mShadowsEnabled->set(false);
    }

    if(mHandle)
        mAmbient->set(mHandle->getAmbientLight());

    return false;
}
//-------------------------------------------------------------------------------
Ogre::MaterialPtr CSceneManagerEditor::buildDepthShadowMaterial(const Ogre::String& textureName)
{
    Ogre::String matName = "DepthShadows/" + textureName;

    Ogre::MaterialPtr ret = Ogre::MaterialManager::getSingleton().getByName(matName);
    if (ret.isNull())
    {
        Ogre::MaterialPtr baseMat = Ogre::MaterialManager::getSingleton().getByName("Ogre/shadow/depth/integrated/pssm");
        ret = baseMat->clone(matName);
        Ogre::Pass* p = ret->getTechnique(0)->getPass(0);
        p->getTextureUnitState("diffuse")->setTextureName(textureName);

        Ogre::Vector4 splitPoints;
        const Ogre::PSSMShadowCameraSetup::SplitPointList& splitPointList = 
            static_cast<Ogre::PSSMShadowCameraSetup*>(mPSSMSetup.get())->getSplitPoints();
        for (int i = 0; i < 3; ++i)
        {
            splitPoints[i] = splitPointList[i];
        }
        p->getFragmentProgramParameters()->setNamedConstant("pssmSplitPoints", splitPoints);
    }

    return ret;
}
//-------------------------------------------------------------------------------
Ogre::MaterialPtr CSceneManagerEditor::buildDepthShadowMaterial(Ogre::MaterialPtr cpyMat)
{
    if(mShadowsTechnique->get() >= (int)Ogre::SHADOWTYPE_TEXTURE_ADDITIVE && mShadowsTechnique->get() <= (int)Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED)
    {
        Ogre::String matName = "DepthShadows/" + cpyMat->getName();

        Ogre::MaterialPtr ret = Ogre::MaterialManager::getSingleton().getByName(matName);
        if (ret.isNull())
        {
            ret = cpyMat->clone(matName);

            Ogre::Technique *t = ret->getTechnique(0);
            t->setShadowCasterMaterial("Ogre/shadow/depth/caster");
            Ogre::Pass *p = t->getPass(0);
            p->setVertexProgram("Ogre/shadow/receiver/depth/pssm3/vp");
            p->setFragmentProgram("Ogre/shadow/receiver/depth/pssm3/fp");

            Ogre::TextureUnitState *tu = p->createTextureUnitState();
            tu->setName("shadow0");
            tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
            tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
            tu->setTextureBorderColour(Ogre::ColourValue(1,1,1,1));
            
            tu = p->createTextureUnitState();
            tu->setName("shadow1");
            tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
            tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
            tu->setTextureBorderColour(Ogre::ColourValue(1,1,1,1));
            
            tu = p->createTextureUnitState();
            tu->setName("shadow2");
            tu->setContentType(Ogre::TextureUnitState::CONTENT_SHADOW);
            tu->setTextureAddressingMode(Ogre::TextureUnitState::TAM_BORDER);
            tu->setTextureBorderColour(Ogre::ColourValue(1,1,1,1));

            Ogre::Vector4 splitPoints;
            const Ogre::PSSMShadowCameraSetup::SplitPointList& splitPointList = 
                static_cast<Ogre::PSSMShadowCameraSetup*>(mPSSMSetup.get())->getSplitPoints();
            for (int i = 0; i < 3; ++i)
            {
                splitPoints[i] = splitPointList[i];
            }
            p->getFragmentProgramParameters()->setNamedConstant("pssmSplitPoints", splitPoints);
        }
        
        return ret;
    }
    else
        return cpyMat;
}

//-------------------------------------------------------------------------------
void CSceneManagerEditor::setupPSSM()
{
    if(mShadowsTechnique->get() >= (int)Ogre::SHADOWTYPE_TEXTURE_ADDITIVE && mShadowsTechnique->get() <= (int)Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED)
    {
        Ogre::Camera *mCamera = mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera();
        mPSSMSetup.setNull();

        // shadow camera setup
        Ogre::PSSMShadowCameraSetup* pssmSetup = OGRE_NEW Ogre::PSSMShadowCameraSetup();
        pssmSetup->setSplitPadding(mCamera->getNearClipDistance());
        pssmSetup->calculateSplitPoints(3, mCamera->getNearClipDistance(), mHandle->getShadowFarDistance());
        pssmSetup->setOptimalAdjustFactor(0, 2);
        pssmSetup->setOptimalAdjustFactor(1, 1);
        pssmSetup->setOptimalAdjustFactor(2, 0.5);

        mPSSMSetup.bind(pssmSetup);

        mHandle->setShadowCameraSetup(mPSSMSetup);
    }
}
//-------------------------------------------------------------------------------
void CSceneManagerEditor::configureShadows()
{
    if(mShadowsEnabled->get())
    {
        if(mShadowsTechnique->get() >= (int)Ogre::SHADOWTYPE_TEXTURE_ADDITIVE && mShadowsTechnique->get() <= (int)Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED)
        {
            // 3 textures per directional light (PSSM)
            mHandle->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);

            mHandle->setShadowTextureCount(3);
            mHandle->setShadowTextureConfig(0, mShadowsResolutionNear->get(), mShadowsResolutionNear->get(), Ogre::PF_FLOAT32_R);
            mHandle->setShadowTextureConfig(1, mShadowsResolutionMiddle->get(), mShadowsResolutionMiddle->get(), Ogre::PF_FLOAT32_R);
            mHandle->setShadowTextureConfig(2, mShadowsResolutionFar->get(), mShadowsResolutionFar->get(), Ogre::PF_FLOAT32_R);
            mHandle->setShadowTextureSelfShadow(true);
            mHandle->setShadowCasterRenderBackFaces(true);
            mHandle->setShadowTextureCasterMaterial("PSSM/shadow_caster");
        }

        // General scene setup
        mHandle->setShadowTechnique((Ogre::ShadowTechnique)mShadowsTechnique->get());
        mHandle->setShadowFarDistance(mShadowsRenderingDistance->get());
    }
    else
    {
        mHandle->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
    }
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::_setShadowsEnabled(OgitorsPropertyBase* property, const bool& value)
{
    configureShadows();
    setupPSSM();

    return true;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::_setShadowsTechnique(OgitorsPropertyBase* property, const int& value)
{
    configureShadows();
    setupPSSM();

    return true;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::_setShadowsRenderingDistance(OgitorsPropertyBase* property, const int& value)
{
    if(mShadowsEnabled->get())
    {
        configureShadows();
        setupPSSM();
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::_setShadowsResolutionNear(OgitorsPropertyBase* property, const int& value)
{
    if(mShadowsEnabled->get())
    {
        mHandle->setShadowTextureConfig(0, value, value, Ogre::PF_FLOAT32_R);
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::_setShadowsResolutionMiddle(OgitorsPropertyBase* property, const int& value)
{
    if(mShadowsEnabled->get())
    {
        mHandle->setShadowTextureConfig(1, value, value, Ogre::PF_FLOAT32_R);
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::_setShadowsResolutionFar(OgitorsPropertyBase* property, const int& value)
{
    if(mShadowsEnabled->get())
    {
        mHandle->setShadowTextureConfig(2, value, value, Ogre::PF_FLOAT32_R);
    }

    return true;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::getObjectContextMenu(UTFStringVector &menuitems) 
{
    menuitems.clear();
    return false;
}
//-------------------------------------------------------------------------------
void CSceneManagerEditor::onObjectContextMenu(int menuresult) 
{
    if(menuresult == 0)
    {
    }
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::setNameImpl(Ogre::String name)
{
    if(mSystem->DisplayMessageDialog(OTR("Renaming a Scene Manager requires Save and Reload of the Scene.\nDo you want to continue?"),DLGTYPE_YESNO) == DLGRET_YES)
    {
        mOgitorsRoot->SaveScene();
        Ogre::String filename = mOgitorsRoot->GetProjectOptions()->ProjectDir + mOgitorsRoot->GetProjectOptions()->ProjectName + ".ogscene";
        mOgitorsRoot->TerminateScene();
        mOgitorsRoot->LoadScene(filename);
        return true;
    }
    else
        return false;
}
//-------------------------------------------------------------------------------
bool CSceneManagerEditor::_setConfigFile(OgitorsPropertyBase* property, const Ogre::String& value)
{
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setAmbient(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mHandle)
    {
        mHandle->setAmbientLight(value);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setSkyboxMaterial(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(value == "")
    {
        mSkyBoxActive->set(false);
        return true;
    }

    if(mHandle)
    {
        mHandle->setSkyBox(mSkyBoxActive->get(), value,mSkyBoxDistance->get());        
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setSkyboxDistance(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setSkyBox(mSkyBoxActive->get(),mSkyBoxMaterial->get(), value);        
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setSkyboxActive(OgitorsPropertyBase* property, const bool& value)
{
    if(value && mSkyDomeActive->get())
        mSkyDomeActive->set(false);

    if(value && mSkyBoxMaterial->get() == "")
    {
        mSystem->DisplayMessageDialog(OTR("You must first define a SkyBox Material!"), DLGTYPE_OK); 
        return false;
    }

    if(mHandle)
    {
        try
        {
            mHandle->setSkyBox(value ,mSkyBoxMaterial->get(),mSkyBoxDistance->get());        
            if(value)
            {
                Ogre::SceneNode *node = mHandle->getSkyBoxNode();
                Ogre::SceneNode::ObjectIterator it = node->getAttachedObjectIterator();
                while(it.hasMoreElements())
                {
                    Ogre::MovableObject *object = it.getNext();
                    object->setVisibilityFlags(0x7F000000);
                }
            }
        }
        catch(...)
        {
            mSystem->DisplayMessageDialog(OTR("The Material supplied is not compatible with Sky Box!"), DLGTYPE_OK); 
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setSkydomeMaterial(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(value == "")
    {
        mSkyDomeActive->set(false);
        return true;
    }

    if(mHandle)
    {
        mHandle->setSkyDome(mSkyDomeActive->get(), value);        
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setSkydomeActive(OgitorsPropertyBase* property, const bool& value)
{
    if(value && mSkyBoxActive->get())
        mSkyBoxActive->set(false);

    if(value && mSkyDomeMaterial->get() == "")
    {
        mSystem->DisplayMessageDialog(OTR("You must first define a SkyDome Material!"), DLGTYPE_OK); 
        return false;
    }

    if(mHandle)
    {
        try
        {
            mHandle->setSkyDome(value ,mSkyDomeMaterial->get());        
            if(value)
            {
                Ogre::SceneNode *node = mHandle->getSkyDomeNode();
                Ogre::SceneNode::ObjectIterator it = node->getAttachedObjectIterator();
                while(it.hasMoreElements())
                {
                    Ogre::MovableObject *object = it.getNext();
                    object->setVisibilityFlags(0x7F000000);
                }
            }
        }
        catch(...)
        {
            mSystem->DisplayMessageDialog(OTR("The Material supplied is not compatible with Sky Dome!"), DLGTYPE_OK); 
            return false;
        }
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setRenderingDistance(OgitorsPropertyBase* property, const Ogre::Real& distance)
{
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setFogMode(OgitorsPropertyBase* property, const int& value)
{
    if(mHandle)
    {
        mHandle->setFog((Ogre::FogMode)value,mFogColour->get(),mFogDensity->get(),mFogStart->get(),mFogEnd->get());
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setFogStart(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFog((Ogre::FogMode)mFogMode->get(),mFogColour->get(),mFogDensity->get(), value,mFogEnd->get());
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setFogEnd(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFog((Ogre::FogMode)mFogMode->get(),mFogColour->get(),mFogDensity->get(),mFogStart->get(), value);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setFogColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mHandle)
    {
        mHandle->setFog((Ogre::FogMode)mFogMode->get(), value,mFogDensity->get(),mFogStart->get(),mFogEnd->get());
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::_setFogDensity(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setFog((Ogre::FogMode)mFogMode->get(),mFogColour->get(),value,mFogStart->get(),mFogEnd->get());
    }
    return true;
}
//-----------------------------------------------------------------------------------------
void CSceneManagerEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mSceneManagerType, "scenemanagertype",Ogre::String     ,"OctreeSceneManager",0, 0);
    PROPERTY_PTR(mPosition        , "position"        ,Ogre::Vector3    ,Ogre::Vector3::ZERO ,0, SETTER(Ogre::Vector3, CSceneManagerEditor, _setPosition));
    PROPERTY_PTR(mScale           , "scale"           ,Ogre::Vector3    ,Ogre::Vector3(1,1,1),0, SETTER(Ogre::Vector3, CSceneManagerEditor, _setScale));
    PROPERTY_PTR(mOrientation     , "orientation"     ,Ogre::Quaternion ,Ogre::Quaternion::IDENTITY,0, SETTER(Ogre::Quaternion, CSceneManagerEditor, _setOrientation));
    PROPERTY_PTR(mConfigFile      , "configfile"      ,Ogre::String     ,""                  ,0, SETTER(Ogre::String, CSceneManagerEditor, _setConfigFile));
    PROPERTY_PTR(mAmbient         , "ambient"         ,Ogre::ColourValue,Ogre::ColourValue(0.5f,0.5f,0.5f),0, SETTER(Ogre::ColourValue, CSceneManagerEditor, _setAmbient));
    PROPERTY_PTR(mSkyBoxActive    , "skybox::active"  ,bool             ,false               ,0, SETTER(bool, CSceneManagerEditor, _setSkyboxActive));
    PROPERTY_PTR(mSkyBoxDistance  , "skybox::distance",Ogre::Real       ,5000.0f             ,0, SETTER(Ogre::Real, CSceneManagerEditor, _setSkyboxDistance));
    PROPERTY_PTR(mSkyBoxMaterial  , "skybox::material",Ogre::String     ,""                  ,0, SETTER(Ogre::String, CSceneManagerEditor, _setSkyboxMaterial));
    PROPERTY_PTR(mSkyDomeActive   , "skydome::active"  ,bool            ,false               ,0, SETTER(bool, CSceneManagerEditor, _setSkydomeActive));
    PROPERTY_PTR(mSkyDomeMaterial , "skydome::material",Ogre::String    ,""                  ,0, SETTER(Ogre::String, CSceneManagerEditor, _setSkydomeMaterial));
    PROPERTY_PTR(mFogMode         , "fog::mode"       ,int              ,Ogre::FOG_NONE      ,0, SETTER(int, CSceneManagerEditor, _setFogMode));
    PROPERTY_PTR(mFogColour       , "fog::colour"     ,Ogre::ColourValue,Ogre::ColourValue(0.5f,0.5f,0.5f),0, SETTER(Ogre::ColourValue, CSceneManagerEditor, _setFogColour));
    PROPERTY_PTR(mFogStart        , "fog::start"      ,Ogre::Real       ,0                   ,0, SETTER(Ogre::Real, CSceneManagerEditor, _setFogStart));
    PROPERTY_PTR(mFogEnd          , "fog::end"        ,Ogre::Real       ,1000                ,0, SETTER(Ogre::Real, CSceneManagerEditor, _setFogEnd));
    PROPERTY_PTR(mFogDensity      , "fog::density"    ,Ogre::Real       ,0                   ,0, SETTER(Ogre::Real, CSceneManagerEditor, _setFogDensity));
    PROPERTY_PTR(mRenderingDistance, "renderingdistance",Ogre::Real     ,5000                ,0, SETTER(Ogre::Real, CSceneManagerEditor, _setRenderingDistance));
    PROPERTY_PTR(mShadowsEnabled            , "shadows::enabled"          ,bool              ,false               ,0, SETTER(bool, CSceneManagerEditor, _setShadowsEnabled));
    PROPERTY_PTR(mShadowsTechnique          , "shadows::technique"        ,int , (int)Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED,0, SETTER(int, CSceneManagerEditor, _setShadowsTechnique));
    PROPERTY_PTR(mShadowsRenderingDistance  , "shadows::renderingdistance",int , 1000        ,0, SETTER(int, CSceneManagerEditor, _setShadowsRenderingDistance));
    PROPERTY_PTR(mShadowsResolutionNear     , "shadows::resolutionnear"   ,int , 1024        ,0, SETTER(int, CSceneManagerEditor, _setShadowsResolutionNear));
    PROPERTY_PTR(mShadowsResolutionMiddle   , "shadows::resolutionmiddle" ,int , 512         ,0, SETTER(int, CSceneManagerEditor, _setShadowsResolutionMiddle));
    PROPERTY_PTR(mShadowsResolutionFar      , "shadows::resolutionfar"    ,int , 512         ,0, SETTER(int, CSceneManagerEditor, _setShadowsResolutionFar));

    mProperties.initValueMap(params);

    //Temporary Fix
    mShadowsTechnique->init((int)Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(!getParent()->load())
        return false;

    mHandle = Ogre::Root::getSingletonPtr()->createSceneManager(mSceneManagerType->get(),mName->get());
    if(mSkyBoxActive->get())
    {
        if(Ogre::MaterialManager::getSingletonPtr()->getByName(mSkyBoxMaterial->get()).isNull())
            mSkyBoxActive->set(false);
        else
            mHandle->setSkyBox(mSkyBoxActive->get(),mSkyBoxMaterial->get(),mSkyBoxDistance->get());    
    }
    else if(mSkyDomeActive->get())
    {
        if(Ogre::MaterialManager::getSingletonPtr()->getByName(mSkyDomeMaterial->get()).isNull())
            mSkyDomeActive->set(false);
        else
            mHandle->setSkyDome(mSkyDomeActive->get(),mSkyDomeMaterial->get());
    }

    mHandle->setFog((Ogre::FogMode)mFogMode->get(),mFogColour->get(),mFogDensity->get(),mFogStart->get(),mFogEnd->get());
    mHandle->setAmbientLight(mAmbient->get());

    mRaySceneQuery = mHandle->createRayQuery(Ogre::Ray());
    mSphereSceneQuery = mHandle->createSphereQuery(Ogre::Sphere());

    configureShadows();

    registerForUpdates();

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CSceneManagerEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unRegisterForUpdates();

    unLoadAllChildren();
    destroyBoundingBox();

    if(mHandle)
    {
        mHandle->destroyQuery(mRaySceneQuery);
        mRaySceneQuery = 0;
        mHandle->destroyQuery(mSphereSceneQuery);
        mSphereSceneQuery = 0;
        
        Ogre::Root::getSingletonPtr()->destroySceneManager(mHandle);
        mHandle = 0;
    }

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CSceneManagerEditor::exportDotScene(TiXmlElement *pParent)
{
    TiXmlElement *pSceneManager = pParent->InsertEndChild(TiXmlElement("scenemanager"))->ToElement();
    pSceneManager->SetAttribute("name", mName->get().c_str());
    pSceneManager->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    pSceneManager->SetAttribute("type", mSceneManagerType->get().c_str());
    //TODO: hmm - this used to belong directly in the environment tag - does it belong in the scenemanager tag?
    TiXmlElement *pAmbient = pParent->InsertEndChild(TiXmlElement("colourAmbient"))->ToElement();
    pAmbient->SetAttribute("r", Ogre::StringConverter::toString(mAmbient->get().r).c_str());
    pAmbient->SetAttribute("g", Ogre::StringConverter::toString(mAmbient->get().g).c_str());
    pAmbient->SetAttribute("b", Ogre::StringConverter::toString(mAmbient->get().b).c_str());
    pAmbient->SetAttribute("a", Ogre::StringConverter::toString(mAmbient->get().a).c_str());
    
    if(mSkyBoxActive->get() && !mSkyBoxMaterial->get().empty())
    {
        TiXmlElement *pSkybox = pParent->InsertEndChild(TiXmlElement("skyBox"))->ToElement();
        pSkybox->SetAttribute("active", Ogre::StringConverter::toString(mSkyBoxActive->get()).c_str());
        pSkybox->SetAttribute("distance", Ogre::StringConverter::toString(mSkyBoxDistance->get()).c_str());
        pSkybox->SetAttribute("material", mSkyBoxMaterial->get().c_str());
    }

    if(mSkyDomeActive->get() && !mSkyDomeMaterial->get().empty())
    {
        TiXmlElement *pSkydome = pParent->InsertEndChild(TiXmlElement("skyDome"))->ToElement();
        pSkydome->SetAttribute("active", Ogre::StringConverter::toString(mSkyDomeActive->get()).c_str());
        pSkydome->SetAttribute("material", mSkyDomeMaterial->get().c_str());
    }

    Ogre::String fogModes[] = {"none", "exp", "exp2", "linear", "GUARD"};
    TiXmlElement *pFog = pParent->InsertEndChild(TiXmlElement("fog"))->ToElement();
    pFog->SetAttribute("mode", fogModes[mFogMode->get()].c_str());
    pFog->SetAttribute("start", Ogre::StringConverter::toString(mFogStart->get()).c_str());
    pFog->SetAttribute("end", Ogre::StringConverter::toString(mFogEnd->get()).c_str());
    pFog->SetAttribute("density", Ogre::StringConverter::toString(mFogDensity->get()).c_str());
    
    TiXmlElement *pFogColour = pFog->InsertEndChild(TiXmlElement("colour"))->ToElement();
    pFogColour->SetAttribute("r", Ogre::StringConverter::toString(mFogColour->get().r).c_str());
    pFogColour->SetAttribute("g", Ogre::StringConverter::toString(mFogColour->get().g).c_str());
    pFogColour->SetAttribute("b", Ogre::StringConverter::toString(mFogColour->get().b).c_str());
    pFogColour->SetAttribute("a", Ogre::StringConverter::toString(mFogColour->get().a).c_str());

    return pSceneManager;
}

//-------------------------------------------------------------------------------
//--------CSCENEMANAGEREDITORFACTORY---------------------------------------------
//-------------------------------------------------------------------------------
CSceneManagerEditorFactory::CSceneManagerEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "OctreeSceneManager";
    mEditorType = ETYPE_SCENEMANAGER;
    mIcon = "Icons/tsm.svg";
    mCapabilities = CAN_DROP | CAN_UNDO | CAN_ACCEPTPASTE;

    mFogModes.clear();
    mFogModes.push_back(PropertyOption("FOG_NONE",Ogre::Any((int)Ogre::FOG_NONE)));
    mFogModes.push_back(PropertyOption("FOG_LINEAR",Ogre::Any((int)Ogre::FOG_LINEAR)));
    mFogModes.push_back(PropertyOption("FOG_EXP",Ogre::Any((int)Ogre::FOG_EXP)));
    mFogModes.push_back(PropertyOption("FOG_EXP2",Ogre::Any((int)Ogre::FOG_EXP2)));

    mTextureMapSizes.clear();
    mTextureMapSizes.push_back(PropertyOption("128x128",Ogre::Any((int)128)));
    mTextureMapSizes.push_back(PropertyOption("256x256",Ogre::Any((int)256)));
    mTextureMapSizes.push_back(PropertyOption("512x512",Ogre::Any((int)512)));
    mTextureMapSizes.push_back(PropertyOption("1024x1024",Ogre::Any((int)1024)));
    mTextureMapSizes.push_back(PropertyOption("2048x2048",Ogre::Any((int)2048)));
    mTextureMapSizes.push_back(PropertyOption("4096x4096",Ogre::Any((int)4096)));

    mShadowTechniqueNames.clear();
    mShadowTechniqueNames.push_back(PropertyOption("NONE",Ogre::Any((int)Ogre::SHADOWTYPE_NONE)));
    mShadowTechniqueNames.push_back(PropertyOption("STENCIL_ADDITIVE",Ogre::Any((int)Ogre::SHADOWTYPE_STENCIL_ADDITIVE)));
    mShadowTechniqueNames.push_back(PropertyOption("STENCIL_MODULATIVE",Ogre::Any((int)Ogre::SHADOWTYPE_STENCIL_MODULATIVE)));
    mShadowTechniqueNames.push_back(PropertyOption("TEXTURE_ADDITIVE",Ogre::Any((int)Ogre::SHADOWTYPE_TEXTURE_ADDITIVE)));
    mShadowTechniqueNames.push_back(PropertyOption("TEXTURE_ADDITIVE_INTEGRATED",Ogre::Any((int)Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED)));
    mShadowTechniqueNames.push_back(PropertyOption("TEXTURE_MODULATIVE",Ogre::Any((int)Ogre::SHADOWTYPE_TEXTURE_MODULATIVE)));
    mShadowTechniqueNames.push_back(PropertyOption("TEXTURE_MODULATIVE_INTEGRATED",Ogre::Any((int)Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED)));

    AddPropertyDefinition("scenemanagertype","Manager Type","The typename of the Scene Manager.",PROP_STRING, true, false);
    AddPropertyDefinition("configfile","Config File","The configuration file of the object.",PROP_STRING,true,false);
    AddPropertyDefinition("ambient","Ambient","The ambient colour of the scene.",PROP_COLOUR);
    
    OgitorsPropertyDef *definition = AddPropertyDefinition("skybox::material","SkyBox::Material","The material of the skybox.",PROP_STRING);
    definition->setOptions(OgitorsRoot::GetSkyboxMaterials());
    AddPropertyDefinition("skybox::active","SkyBox::Active","Is the skybox active?",PROP_BOOL);
    AddPropertyDefinition("skybox::distance","SkyBox::Distance","The distance of the skybox.",PROP_REAL);
    
    definition = AddPropertyDefinition("skydome::material","SkyDome::Material","The material of the skydome.",PROP_STRING);
    definition->setOptions(OgitorsRoot::GetSkyboxMaterials());
    AddPropertyDefinition("skydome::active","SkyDome::Active","Is the skydome active?",PROP_BOOL);

    definition = AddPropertyDefinition("fog::mode","Fog::Mode","The fog mode of the scene.",PROP_INT);
    definition->setOptions(&mFogModes);

    AddPropertyDefinition("fog::colour","Fog::Colour","The colour of the fog.",PROP_COLOUR);
    AddPropertyDefinition("fog::start","Fog::Start","The distance at which fog starts.",PROP_REAL);
    AddPropertyDefinition("fog::end","Fog::End","The distance at which fog ends.",PROP_REAL);
    AddPropertyDefinition("fog::density","Fog::Density","The density of fog.",PROP_REAL);
    AddPropertyDefinition("position","Position","The position of the object.",PROP_VECTOR3, true, false);
    AddPropertyDefinition("scale","Scale","The scale of the object.",PROP_VECTOR3);
    AddPropertyDefinition("orientation","Orientation","The orientation of the object.",PROP_QUATERNION, true, false);
    AddPropertyDefinition("renderingdistance","Rendering Distance","The maximum distance to render meshes.",PROP_REAL);

    AddPropertyDefinition("shadows::enabled","Shadows::Enabled","Enable/Disable Shadows",PROP_BOOL);
    definition = AddPropertyDefinition("shadows::technique","Shadows::Technique","Shadow Technique to use",PROP_INT, true, false);
    definition->setOptions(&mShadowTechniqueNames);
    AddPropertyDefinition("shadows::renderingdistance","Shadows::Rendering Distance","The distance up to which shadows will be rendered",PROP_INT);
    definition = AddPropertyDefinition("shadows::resolutionnear","Shadows::Near Map Res.","Resolution of the nearest shadow map",PROP_INT);
    definition->setOptions(&mTextureMapSizes);
    definition = AddPropertyDefinition("shadows::resolutionmiddle","Shadows::Middle Map Res.","Resolution of the middle shadow map",PROP_INT);
    definition->setOptions(&mTextureMapSizes);
    definition = AddPropertyDefinition("shadows::resolutionfar","Shadows::Far Map Res.","Resolution of the far shadow map",PROP_INT);
    definition->setOptions(&mTextureMapSizes);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(false, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CSceneManagerEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CSceneManagerEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CSceneManagerEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CSceneManagerEditor *object = OGRE_NEW CSceneManagerEditor(this);

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    object->load();

    mInstanceCount++;
    return object;
}
//-------------------------------------------------------------------------------
