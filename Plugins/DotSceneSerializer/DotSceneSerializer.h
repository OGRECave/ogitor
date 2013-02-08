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

#ifndef DOTSCENE_SERIALIZER_H
#define DOTSCENE_SERIALIZER_H

#include "Ogitors.h"

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

    class PluginExport CDotSceneSerializer: public CBaseSerializer
    {
    public:
        CDotSceneSerializer();
        virtual ~CDotSceneSerializer() {};

        /// Does the serializer need a TerminateScene before Import?
        virtual bool RequiresTerminateScene() {return true;};
        /// The function to Export Data
        virtual int  Export(bool SaveAs = false, Ogre::String exportfile = "");
        /// The function to Import Data
        virtual int  Import(Ogre::String importfile = "");
    protected:
        int RecurseReadObjects(TiXmlElement *parentelement,CBaseEditor* parentobject);

        int ReadSceneNode(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret);
        int ReadEntity(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret);
        int ReadSubEntity(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret);
        int ReadLight(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret);
        int ReadCamera(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret);
        int ReadParticle(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret);
        int ReadPlane(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret);
    };

}

extern "C" bool PluginExport dllStartPlugin(void *identifier, Ogre::String& name);

extern "C" bool PluginExport dllGetPluginName(Ogre::String& name);

extern "C" bool PluginExport dllStopPlugin(void);

#endif