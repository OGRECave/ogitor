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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "ScreenEffectEditor.h"
#include "OgreMaterialManager.h"
#include "ViewportEditor.h"

namespace Ogitors
{

    //-----------------------------------------------------------------------------------------
    CScreenEffectEditor::CScreenEffectEditor(Ogre::Viewport* viewport, CBaseEditorFactory *factory) : CBaseEditor(factory)
    {
        mHandle = viewport;
        mHelper = 0;
        mUsesGizmos = false;
        mUsesHelper = false;
    }
    //-----------------------------------------------------------------------------------------
    CScreenEffectEditor::~CScreenEffectEditor()
    {

    }
    //-----------------------------------------------------------------------------------------
    bool CScreenEffectEditor::getObjectContextMenu(UTFStringVector &menuitems) 
    {
        return false;
    }
    //-------------------------------------------------------------------------------
    void CScreenEffectEditor::onObjectContextMenu(int menuresult) 
    {
    }
    //-----------------------------------------------------------------------------------------
    void CScreenEffectEditor::addCompositor(const Ogre::String& name, int position)
    {
        if(mHandle)
        {
            try
            {
                Ogre::CompositorManager::getSingletonPtr()->addCompositor(mHandle, name, position);
            }
            catch(...)
            {
                return;
            }
        }

        /*Ogre::String sCount1 = "Compositors::Compositor" + Ogre::StringConverter::toString(count);
        Ogre::String sCount2 = "compositor" + Ogre::StringConverter::toString(count);

        mFactory->AddPropertyDefinition(sCount2 + "::enabled", sCount1 + "::Enabled", "Is the compositor enabled?", PROP_BOOL, true, true);
        OgitorsPropertyDef *definition = mFactory->AddPropertyDefinition(sCount2 + "::name", sCount1 + "::Name", "Compositor Name", PROP_STRING, true, true);
        definition->setOptions(CViewportEditorFactory::GetCompositorNames());

        if(position == count)
        {
        PROPERTY(sCount2 + "::enabled", bool, false, position, SETTER(bool, CViewportEditor, _setCompositorEnabled)); 
        PROPERTY(sCount2 + "::name", Ogre::String, name, position, SETTER(Ogre::String, CViewportEditor, _setCompositorName)); 
        }
        else
        {
        for(int c = count - 1;c >= position;c--)
        {
        propName = "compositor" + Ogre::StringConverter::toString(c);
        propName2 = "compositor" + Ogre::StringConverter::toString(c + 1);
        mProperties.getValue(propName + "::name", compName);
        mProperties.getValue(propName + "::enabled", enabled);

        if(c == (count - 1))
        {            
        PROPERTY(sCount2 + "::enabled", bool, enabled, c + 1, SETTER(bool, CViewportEditor, _setCompositorEnabled)); 
        PROPERTY(sCount2 + "::name", Ogre::String, compName, c + 1, SETTER(Ogre::String, CViewportEditor, _setCompositorName)); 
        }
        else
        {
        static_cast<OgitorsProperty<Ogre::String>*>(mProperties.getProperty(propName2 + "::name"))->initAndSignal(compName);
        static_cast<OgitorsProperty<bool>*>(mProperties.getProperty(propName2 + "::enabled"))->initAndSignal(enabled);
        }
        }

        propName = "compositor" + Ogre::StringConverter::toString(position);
        static_cast<OgitorsProperty<Ogre::String>*>(mProperties.getProperty(propName + "::name"))->initAndSignal(name);
        static_cast<OgitorsProperty<bool>*>(mProperties.getProperty(propName + "::enabled"))->initAndSignal(false);
        }

        mCompositorCount->set(mCompositorCount->get() + 1);*/
    }
    //-------------------------------------------------------------------------------
    void CScreenEffectEditor::removeCompositor(const Ogre::String& name)
    {
        if(!mHandle)
            return;

        Ogre::CompositorManager *comMngr = Ogre::CompositorManager::getSingletonPtr();
        Ogre::CompositorChain *chain = comMngr->getCompositorChain(mHandle);
        Ogre::CompositorChain::InstanceIterator it = chain->getCompositors();

        while(it.hasMoreElements())
        {
            Ogre::CompositorInstance *compositor = it.getNext();
            Ogre::Compositor *sub = compositor->getCompositor();
            if(sub && sub->getName() == name)
            {
                comMngr->removeCompositor(mHandle, name);
                return;
            }
        }
    }
    //-----------------------------------------------------------------------------------------    
    bool CScreenEffectEditor::_setCompositorEnabled(OgitorsPropertyBase* property, const bool& value)
    {
        Ogre::String name;
        mProperties.getValue("compositor::name", name);
        enableCompositor(name, value);

        return true;
    }
    //-------------------------------------------------------------------------------
    void CScreenEffectEditor::enableCompositor(const Ogre::String& name, const bool& value)
    {
        if(mHandle)
        {
            Ogre::CompositorManager *comMngr = Ogre::CompositorManager::getSingletonPtr();
            Ogre::CompositorChain *chain = comMngr->getCompositorChain(mHandle);

            comMngr->setCompositorEnabled(mHandle, name, value);
        }
    }
    //-------------------------------------------------------------------------------
    void CScreenEffectEditor::_restoreCompositors()
    {
        if(!mHandle) 
            return;

        Ogre::CompositorManager *comMngr = Ogre::CompositorManager::getSingletonPtr();

        Ogre::String compName;
        bool enabled;
        mProperties.getValue("compositor::name", compName);
        mProperties.getValue("compositor::enabled", enabled);
        comMngr->addCompositor(mHandle, compName);
        if(enabled)
            comMngr->setCompositorEnabled(mHandle, compName, enabled);
    }
    //-----------------------------------------------------------------------------------------
    bool CScreenEffectEditor::_setCompositorName(OgitorsPropertyBase* property, const Ogre::String& value)
    {
        if(mHandle)
        {
            Ogre::CompositorManager *comMngr = Ogre::CompositorManager::getSingletonPtr();
            Ogre::CompositorChain *chain = comMngr->getCompositorChain(mHandle);
            Ogre::String name = static_cast<OgitorsProperty<Ogre::String>*>(property)->getOld();
            Ogre::CompositorInstance *comInstance;
            try
            {
                comInstance = chain->getCompositor(value);
                if(comInstance)
                    return false;

                comMngr->removeCompositor(mHandle, name);

                comInstance = comMngr->addCompositor(mHandle, value, -1);
                bool enabled;
                mProperties.getValue("compositor::enabled", enabled);
                if(enabled)
                    comMngr->setCompositorEnabled(mHandle, value, enabled);
            }
            catch(...)
            {
                comMngr->removeCompositor(mHandle, value);
                return false;
            }

            //Ogre::GpuProgramParametersSharedPtr params=comInstance->getTechnique()->getOutputTargetPass()->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
            //if( params->hasNamedParameters() )
            //{
            //    Ogre::GpuNamedConstants gnc = params->getConstantDefinitions();
            //    for(Ogre::GpuConstantDefinitionMap::iterator it = gnc.map.begin(); it != gnc.map.end(); ++it)
            //    {
            //        Ogre::String name = it->first;

            //        // FIXME: what are those duplicate names ending in "[0]"?
            //        if( name.rfind("[0]") != std::string::npos )
            //            continue;

            //        Ogre::GpuConstantDefinition gcd = it->second;

            //        Ogre::String newProp = "compositor" + Ogre::StringConverter::toString(property->getTag()) + "::" + name;
            //        Ogre::String newProp2 = "Compositors::Compositor" + Ogre::StringConverter::toString(property->getTag()) + "::" + name;
            //        switch(gcd.constType)
            //        {
            //        case Ogre::GCT_FLOAT1:
            //            {
            //                mFactory->AddPropertyDefinition(newProp, newProp2, "", PROP_REAL, true, true);
            //                float value = *params->getFloatPointer( params->getConstantDefinition(name).physicalIndex );
            //                PROPERTY(newProp, Ogre::Real, value, 0, SETTER(Ogre::Real, CViewportEditor, _setCompositorNamedConstant)); 
            //            }
            //            break;
            //        case Ogre::GCT_FLOAT3:
            //            mFactory->AddPropertyDefinition(newProp, newProp2, "", PROP_COLOUR, true, true);
            //            float *value = params->getFloatPointer(params->getConstantDefinition(name).physicalIndex);
            //            PROPERTY(newProp, Ogre::ColourValue, Ogre::ColourValue(value[0], value[1], value[2]), 0, SETTER(Ogre::ColourValue, CViewportEditor, _setCompositorNamedConstantColourValue)); 
            //            break;
            //        }
            //    }
            //}
        }
        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CScreenEffectEditor::_setCompositorNamedConstant(OgitorsPropertyBase* property, const float& value)
    {
        Ogre::CompositorManager *comMngr = Ogre::CompositorManager::getSingletonPtr();
        Ogre::CompositorChain *chain = comMngr->getCompositorChain(mHandle);

        Ogre::String name;
        mProperties.getValue("compositor" + Ogre::StringConverter::toString(property->getTag()) + "::name", name);
        Ogre::CompositorInstance *comInstance = chain->getCompositor( name );
        Ogre::GpuProgramParametersSharedPtr params=comInstance->getTechnique()->getOutputTargetPass()->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

        int pos = property->getName().rfind("::");
        if( pos == std::string::npos )
            return true;

        Ogre::String constantName = property->getName().substr(pos+2, std::string::npos);
        params->setNamedConstant(constantName, value);

        comMngr->setCompositorEnabled(mHandle, name, false);
        comMngr->setCompositorEnabled(mHandle, name, true);

        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CScreenEffectEditor::_setCompositorNamedConstantColourValue(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
    {
        Ogre::CompositorManager *comMngr = Ogre::CompositorManager::getSingletonPtr();
        Ogre::CompositorChain *chain = comMngr->getCompositorChain(mHandle);

        Ogre::String name;
        mProperties.getValue("compositor" + Ogre::StringConverter::toString(property->getTag()) + "::name", name);
        Ogre::CompositorInstance *comInstance = chain->getCompositor( name );
        Ogre::GpuProgramParametersSharedPtr params=comInstance->getTechnique()->getOutputTargetPass()->getPass(0)->getMaterial()->getTechnique(0)->getPass(0)->getFragmentProgramParameters();

        int pos = property->getName().rfind("::");
        if( pos == std::string::npos )
            return true;

        Ogre::String constantName = property->getName().substr(pos+2, std::string::npos);
        params->setNamedConstant(constantName, value);

        comMngr->setCompositorEnabled(mHandle, name, false);
        comMngr->setCompositorEnabled(mHandle, name, true);

        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CScreenEffectEditor::load(bool async)
    {
        if(mLoaded->get())
            return true;

        if(getParent()->load())
            _restoreCompositors();

        mLoaded->set(true);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    bool CScreenEffectEditor::unLoad()
    {
        if(!mLoaded->get())
            return true;

        mHandle = 0;

        mLoaded->set(false);
        return true;
    }
    //-----------------------------------------------------------------------------------------
    void CScreenEffectEditor::createProperties(OgitorsPropertyValueMap &params)
    {
        PROPERTY_PTR(mCompositorPath, "compositor::name", Ogre::String, "", 0, SETTER(Ogre::String, CScreenEffectEditor, _setCompositorName)); 
        PROPERTY("compositor::enabled", bool, false, 0, SETTER(bool, CScreenEffectEditor, _setCompositorEnabled)); 

        mProperties.initValueMap(params);
    }
    //-----------------------------------------------------------------------------------------

    //-----------------------------------------------------------------------------------------
    //------CMATERIALEDITORFACTORY-----------------------------------------------------------------
    //-----------------------------------------------------------------------------------------
    CScreenEffectEditorFactory::CScreenEffectEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
    {
        mTypeName = "Screen Effect";
        mEditorType = ETYPE_SCREEN_EFFECT;
        mAddToObjectList = true;
        mRequirePlacement = false;
        mIcon = "screen_effect.svg";
        mCapabilities = CAN_UNDO | CAN_DELETE;

        OgitorsPropertyDef *definition;
        definition = AddPropertyDefinition("compositor::name", "Compositor", "", Ogitors::PROP_STRING);
        definition->setOptions(OgitorsRoot::GetCompositorNames());
        definition = AddPropertyDefinition("compositor::enabled", "Enabled", "Switch compositor effect on or off", Ogitors::PROP_BOOL);
    }
    //-----------------------------------------------------------------------------------------
    CBaseEditorFactory *CScreenEffectEditorFactory::duplicate(OgitorsView *view)
    {
        CBaseEditorFactory *ret = new CScreenEffectEditorFactory(view);
        ret->mTypeID = mTypeID;

        return ret;
    }
    //-----------------------------------------------------------------------------------------
    CBaseEditor *CScreenEffectEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
    {
        *parent = OgitorsRoot::getSingletonPtr()->GetViewport();
        
        CScreenEffectEditor *object = new CScreenEffectEditor((Ogre::Viewport*)OgitorsRoot::getSingletonPtr()->GetViewport()->getHandle(), this);
        OgitorsPropertyValueMap::iterator ni;

        if((ni = params.find("init")) != params.end())
        {
            OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
            value.val = Ogre::Any(CreateUniqueID("ScreenEffect"));
            params["name"] = value;
            params.erase(ni);
        }

        object->createProperties(params);
        object->mParentEditor->init(*parent);

        mInstanceCount++;
        return object;
    }
    //-----------------------------------------------------------------------------------------
}
