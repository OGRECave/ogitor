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

/*////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////*/

#pragma once

#include <string>

#include <OgitorsExports.h>

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable:4251)   // needs to have dll-interface to be used by clients..
#endif

namespace Ogitors
{
    class OgitorExport Globals
    {
    public:
        static const std::string OGITOR_VERSION;
        static const std::string INSTALL_PREFIX;
        static const std::string BIN_PATH;
        static const std::string RUN_PATH;
        static const std::string RESOURCE_PATH;
        static const std::string LANGUAGE_PATH;
        static const std::string MEDIA_PATH;
        static const std::string PROJECTS_PATH;
        static const std::string SCRIPTS_PATH;
        static const std::string OGITOR_PLUGIN_PATH;
        static const std::string OGITOR_PLUGIN_ICON_PATH;
        static const std::string OGRE_PLUGIN_PATH;
        static const std::string LIBOGREOFSPLUGIN_PATH;
        static const std::string OGSCENE_FORMAT_VERSION;
        static const std::string OGSCENE_FORMAT_EXTENSION;
    };
}
// On MSVC, restore warnings state
#ifdef _MSC_VER
    #pragma warning(pop)
#endif
