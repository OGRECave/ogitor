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

#include "OgrePlugin.h"
#include "OgreRoot.h"
#include "OgreArchiveManager.h"
#include "OfsFileSystem.h"

namespace Ogre
{

	/** Plugin instance for OFS */
	class OFSPlugin : public Plugin
	{
	public:
		OFSPlugin();

		/// @copydoc Plugin::getName
		const String& getName() const;

		/// @copydoc Plugin::install
		void install();

		/// @copydoc Plugin::initialise
		void initialise();

		/// @copydoc Plugin::shutdown
		void shutdown();

		/// @copydoc Plugin::uninstall
		void uninstall();
	protected:
		OFSArchiveFactory* mOFSArchiveFactory;


	};
}


namespace Ogre 
{
	const String sPluginName = "OFS Archive";
	//---------------------------------------------------------------------
	OFSPlugin::OFSPlugin()
		:mOFSArchiveFactory(0)
	{

	}
	//---------------------------------------------------------------------
	const String& OFSPlugin::getName() const
	{
		return sPluginName;
	}
	//---------------------------------------------------------------------
	void OFSPlugin::install()
	{
		// Create new factory
		mOFSArchiveFactory = OGRE_NEW OFSArchiveFactory();
		// Register
        ArchiveManager::getSingletonPtr()->addArchiveFactory(mOFSArchiveFactory);
	}
	//---------------------------------------------------------------------
	void OFSPlugin::initialise()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void OFSPlugin::shutdown()
	{
		// nothing to do
	}
	//---------------------------------------------------------------------
	void OFSPlugin::uninstall()
	{
        if (mOFSArchiveFactory)
        {
		    OGRE_DELETE mOFSArchiveFactory;
		    mOFSArchiveFactory = 0;
        }
	}

    OFSPlugin *ofsPlugin = NULL;

    extern "C" void PluginExport dllStartPlugin(void)
    {
        // Create new plugin
        ofsPlugin = OGRE_NEW OFSPlugin();

        // Register
        Root::getSingleton().installPlugin(ofsPlugin);
    }

    extern "C" void PluginExport dllStopPlugin(void)
    {
		Root::getSingleton().uninstallPlugin(ofsPlugin);
		OGRE_DELETE ofsPlugin;
    }

}