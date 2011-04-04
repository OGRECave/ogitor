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

#pragma once

#include "ParticleUniverse/ParticleUniversePrerequisites.h"
#include "ParticleUniverse/ParticleUniversePlugin.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifdef PLUGIN_EXPORT
#define PluginExport __declspec (dllexport)
#else
#define PluginExport __declspec (dllimport)
#endif
#else
#define PluginExport
#endif

namespace Ogitors
{
	class PluginExport CParticleUniverseEditor : public CBaseEditor
	{
		 friend class CParticleUniverseEditorFactory;

	public:
		static OgitorsPropertyDefMap propertyDefs;
		/// Returns the Factory associated with this object
		static CBaseEditorFactory *getFactory();
		/// Returns the Factory associated with this object can be overridden
		virtual CBaseEditorFactory *getFactoryDynamic();
		
		virtual Ogre::String	getParticleSystemName();

		virtual bool			getParticleSystemEnabled();
		virtual void			setParticleSystemEnabled(const bool& bEnabled, bool display);

		virtual void			setParticleSystemState(const int& state, bool display);
		virtual int				getParticleSystemState();
		#pragma endregion

		virtual void            processParameters(Ogre::NameValuePairList &params);
		/// Virtual Function that loads (Creates) underlying object 
		virtual bool            load();
		/// Virtual Function that unloads (Deletes) underlying object 
		virtual bool            unLoad();

		virtual void            getObjectProperties(Ogre::NameValuePairList &retList);
		virtual bool			setupPropertiesWnd();
		/// Overridden by Editor Objects to Import Object Dependant Properties
		virtual void            restore(Ogre::NameValuePairList &retList);
		/// Gets the Handle to encapsulated object
		inline virtual void    *getHandle() {return static_cast<void*>(m_pParticleSystemHandle);};

		virtual bool			_createParticleUniverse();
		virtual void             onDropMaterial(Ogre::Vector3 position, const Ogre::String& materialname) {};
	protected:
		int m_iParticleSystemState;
		bool m_bEnabled;
		ParticleUniverse::ParticleSystem *m_pParticleSystemHandle;
		Ogre::String m_ParticleSystemName;

		ParticleUniverse::ParticleSystem::ParticleSystemState _convertParticleSystemState(Ogre::String val);
		Ogre::String	getParticleSystemStateString();
		CParticleUniverseEditor(Ogre::String typestring = "ParticleUniverse Object", EDITORTYPE etype = ETYPE_MOVABLE);
		virtual     ~CParticleUniverseEditor() {};
		virtual bool handleCustomFunction(int BIND_ID,Ogre::String val);
	};

	class PluginExport CParticleUniverseEditorFactory: public CBaseEditorFactory
	{
	public:
		CParticleUniverseEditorFactory() {};
		virtual ~CParticleUniverseEditorFactory() {};
		virtual void Init();
		virtual CBaseEditor *CreateObject(CBaseEditor **parent, Ogre::NameValuePairList &params);
		virtual bool CanInstantiate() {return (m_InstanceCount == 0);};
	};
}

extern "C" void PluginExport dllStartPlugin(void);

extern "C" void PluginExport dllStopPlugin(void);