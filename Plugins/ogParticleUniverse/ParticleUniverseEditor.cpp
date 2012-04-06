/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2009 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "Ogitors.h"
#include "NodeEditor.h"
#include "ParticleUniverseEditor.h"

using namespace Ogitors;

static CParticleUniverseEditorFactory ParticleEditorFactory;
OgitorsPropertyDefMap CParticleUniverseEditor::propertyDefs;

//-------------------------------------------------------------------------------
CParticleUniverseEditor::CParticleUniverseEditor(Ogre::String typestring, EDITORTYPE etype) : CBaseEditor(typestring, etype)
{
	m_Name = "ParticleUniverse";
	m_UsesGizmos = true;
	m_UsesHelper = false;
	m_EditFlags = CAN_MOVE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO;
	registerObjectName();

	IMPLEMENT_REQUIRED_PROPERTIES(CParticleUniverseEditor)
}
//-------------------------------------------------------------------------------
CBaseEditorFactory *CParticleUniverseEditor::getFactory()
{
	return &ParticleEditorFactory;
}
//-------------------------------------------------------------------------------
CBaseEditorFactory *CParticleUniverseEditor::getFactoryDynamic()
{
	return &ParticleEditorFactory;
}
//-------------------------------------------------------------------------------
void CParticleUniverseEditor::setParticleSystemState(const int& state, bool display)
{
	ParticleUniverse::ParticleSystem::ParticleSystemState estate = (ParticleUniverse::ParticleSystem::ParticleSystemState)state;

	if(m_pParticleSystemHandle != 0)
	{
		if(estate == ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_PAUSED)
		m_pParticleSystemHandle->pause();
		else if(estate == ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_PREPARED)
			m_pParticleSystemHandle->prepare();
		else if(estate == ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_STARTED)
			m_pParticleSystemHandle->start();
		else
			m_pParticleSystemHandle->stop();
	}

	m_iParticleSystemState = estate;

	if(display)
		m_System->SetProperty(this, CUSTOM_BINDING_START + 1, getParticleSystemStateString());

	setModified(true);

	SIGNAL_PROPERTY("particlesystemstate", m_iParticleSystemState)
}
//-------------------------------------------------------------------------------
int CParticleUniverseEditor::getParticleSystemState()
{
	return m_iParticleSystemState;
}
//-------------------------------------------------------------------------------
bool CParticleUniverseEditor::getParticleSystemEnabled()
{
	return m_bEnabled;
}
//-------------------------------------------------------------------------------
void CParticleUniverseEditor::setParticleSystemEnabled(const bool& bEnabled, bool display)
{
	m_bEnabled = bEnabled;

	if(display)
		m_System->SetProperty(this, CUSTOM_BINDING_START + 2, Ogre::StringConverter::toString(m_bEnabled));

	setModified(true);
	SIGNAL_PROPERTY("particlesystemenabled", m_bEnabled)
}
//-------------------------------------------------------------------------------
bool CParticleUniverseEditor::setupPropertiesWnd()
{
	if(!CBaseEditor::setupPropertiesWnd()) 
		return false;
	
	if(m_pParticleSystemHandle == 0)
		return false;

	m_System->SetProperty(this,CUSTOM_BINDING_START, m_ParticleSystemName);
	m_System->SetProperty(this,CUSTOM_BINDING_START + 1,getParticleSystemStateString());
	m_System->SetProperty(this,CUSTOM_BINDING_START + 2,Ogre::StringConverter::toString(m_bEnabled));
	return true;
}
//-------------------------------------------------------------------------------
void CParticleUniverseEditor::getObjectProperties(Ogre::NameValuePairList &retList)
{
	retList.clear();
	CBaseEditor::getObjectProperties(retList);

	retList["name"] = m_pParticleSystemHandle->getName();
	retList["enabled"] = Ogre::StringConverter::toString(m_pParticleSystemHandle->isEnabled());
	retList["state"] = getParticleSystemStateString();
}
//-------------------------------------------------------------------------------
bool CParticleUniverseEditor::handleCustomFunction(int BIND_ID,Ogre::String val)
{
	switch(BIND_ID)
	{
		case 1:
			{
				Ogre::String strParticleSystem = val;
				ParticleUniverse::ParticleSystem *pSystem = ParticleUniverse::ParticleSystemManager::getSingletonPtr()->getParticleSystem(strParticleSystem);
				if(pSystem != 0)
				{
					m_pParticleSystemHandle = pSystem;
					m_ParticleSystemName = strParticleSystem;
				}

				break;
			}

		case 2:
			setParticleSystemState(_convertParticleSystemState(val), false);
		break;

		case 3:
			setParticleSystemEnabled(Ogre::StringConverter::parseBool(val), false);
		break;
	}

	return true;
}
//-------------------------------------------------------------------------------
void CParticleUniverseEditor::restore(Ogre::NameValuePairList &retList)
{
	Ogre::NameValuePairList theList;
	getObjectProperties(theList);
	Ogre::NameValuePairList dummy = retList;

	Ogre::NameValuePairList::const_iterator ni;

	CBaseEditor::restore(dummy);

	if ((ni = retList.find("particlesystemstate")) != retList.end())
		setParticleSystemState(_convertParticleSystemState(ni->second), true);

	if ((ni = retList.find("particlesystemenabled")) != retList.end())
		setParticleSystemEnabled(Ogre::StringConverter::parseBool(ni->second), true);

	if ((ni = retList.find("particlesystemname")) != retList.end())
	{
		Ogre::String strOldParticleSystem = ni->second;
		ParticleUniverse::ParticleSystem *pOldSystem = ParticleUniverse::ParticleSystemManager::getSingletonPtr()->getParticleSystem(strOldParticleSystem);
		if(pOldSystem != 0)
		{
			m_pParticleSystemHandle = pOldSystem;
			m_ParticleSystemName = strOldParticleSystem;
		}
	}
}
//-----------------------------------------------------------------------------------------
void CParticleUniverseEditor::processParameters(Ogre::NameValuePairList &params)
{
	Ogre::NameValuePairList::const_iterator ni;

	CBaseEditor::processParameters(params);

	if ((ni = params.find("particlesystemstate")) != params.end())
		m_iParticleSystemState = _convertParticleSystemState(ni->second);

	if ((ni = params.find("particlesystemenabled")) != params.end())
		m_bEnabled = Ogre::StringConverter::parseBool(ni->second);

	if ((ni = params.find("particlesystemname")) != params.end())
	{
		Ogre::String strNewParticleSystem = ni->second;
		ParticleUniverse::ParticleSystem *pNewSystem = ParticleUniverse::ParticleSystemManager::getSingletonPtr()->getParticleSystem(strNewParticleSystem);
		if(pNewSystem != 0)
		{
			m_pParticleSystemHandle = pNewSystem;
			m_ParticleSystemName = strNewParticleSystem;
		}
	}
}    
//-----------------------------------------------------------------------------------------
bool CParticleUniverseEditor::load()
{
	if(m_isLoaded)
		return true;

	_createParticleUniverse();
	registerForUpdates();

	m_isLoaded = true;
	return true;
}
//-----------------------------------------------------------------------------------------
bool CParticleUniverseEditor::unLoad()
{
	if(!m_isLoaded)
		return true;

	unRegisterForUpdates();
	Ogre::ResourceGroupManager::getSingletonPtr()->destroyResourceGroup("ParticleUniverse");

	// uninstall();

	m_isLoaded = false;
	return true;
}
//-----------------------------------------------------------------------------------------
bool CParticleUniverseEditor::_createParticleUniverse()
{
	Ogre::ResourceGroupManager *resmngr = Ogre::ResourceGroupManager::getSingletonPtr();
	Ogre::String value = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ParticleUniverseDirectory;
	resmngr->addResourceLocation(value,"FileSystem","ParticleUniverse");
	resmngr->initialiseResourceGroup("ParticleUniverse");

	return true;
}
//-----------------------------------------------------------------------------------------
Ogre::String CParticleUniverseEditor::getParticleSystemName()
{
	return m_ParticleSystemName;
}
//-----------------------------------------------------------------------------------------
Ogre::String CParticleUniverseEditor::getParticleSystemStateString()
{
	ParticleUniverse::ParticleSystem::ParticleSystemState state = (ParticleUniverse::ParticleSystem::ParticleSystemState)m_iParticleSystemState;
	if(state == ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_PREPARED)
		return "PSS_PREPARED";
	else if(state == ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_STARTED)
		return "PSS_STARTED";
	else if(state == ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_STOPPED)
		return "PSS_STOPPED";

	return "PSS_PAUSED";
}
//-----------------------------------------------------------------------------------------
ParticleUniverse::ParticleSystem::ParticleSystemState CParticleUniverseEditor::_convertParticleSystemState(Ogre::String val)
{
	if(val == "PSS_PREPARED")
		return ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_PREPARED;
	else if(val == "PSS_STARTED")
		return ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_STARTED;
	else if(val == "PSS_STOPPED")
		return ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_STOPPED;
	
	return ParticleUniverse::ParticleSystem::ParticleSystemState::PSS_PAUSED;
}
//-------------------------------------------------------------------------------
//--------------CPARTICLEEDITORFACTORY-------------------------------------------
//-------------------------------------------------------------------------------
void CParticleUniverseEditorFactory::Init()
{
	DEFINE_REQUIRED_PROPERTIES(CParticleUniverseEditor)
}
//-------------------------------------------------------------------------------
CBaseEditor *CParticleUniverseEditorFactory::CreateObject(CBaseEditor **parent, Ogre::NameValuePairList &params)
{
	Ogre::ResourceGroupManager *mngr = Ogre::ResourceGroupManager::getSingletonPtr();
	Ogre::String value = OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ProjectDir + OgitorsRoot::getSingletonPtr()->GetProjectOptions()->ParticleUniverseDirectory + "/";

	if(params.find("init") != params.end())
	{
		Ogre::String dirname = OgitorsSystem::getSingletonPtr()->QualifyPath(value);	
		OgitorsSystem::getSingletonPtr()->MakeDirectory(dirname);	
		Ogre::String copydir = "../Media/EditorResources/ParticleUniverse/*";
		OgitorsSystem::getSingletonPtr()->CopyFilesEx(OgitorsSystem::getSingletonPtr()->QualifyPath(copydir),dirname);
	}

	CParticleUniverseEditor *object = new CParticleUniverseEditor();
	object->m_Parent = (*parent);

	m_InstanceCount++;
	return object;
}
//-------------------------------------------------------------------------------
//----------------------------------------------------------------------------
void dllStartPlugin(void)
{
	OGITORREGISTRATIONSTRUCT stReg;

	stReg.PropertySheet = "PropertySheets/ParticleUniverse.xml";
	stReg.Factory = CParticleUniverseEditor::getFactory();
	stReg.Icon = "Icons/particleuniverse.svg";
	stReg.RequirePlacement = false;
	stReg.AddToObjectList = true;
	OgitorsRoot::getSingletonPtr()->RegisterObjectType("ParticleUniverse Object",stReg);
}
//----------------------------------------------------------------------------
void dllStopPlugin(void)
{
}
//----------------------------------------------------------------------------
