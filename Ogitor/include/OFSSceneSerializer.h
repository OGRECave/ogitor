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

#pragma once

namespace Ogitors
{
    //! Ogitor scene serializer class
    /*!  
    A class that is responsible for serializing entire Ogitor-based scene
    */
    class OgitorExport COFSSceneSerializer: public CBaseSerializer
    {
    public:
        /**
        * Constructor (empty)
        */
        COFSSceneSerializer() : CBaseSerializer("OFS Scene Serializer", CAN_EXPORT | CAN_IMPORT) {};
        /**
        * Destructor (empty)
        */
        virtual ~COFSSceneSerializer() {};
        /**
        * Tests if serializer needs to terminate the scene before import
        * @return
        */
        virtual bool RequiresTerminateScene() {return true;};
        /**
        * Exports (serializes) scene data
        * @param SaveAs flag to open "Save As" dialog to allow user to save scene into different location
        * @return result of the scene export  
        * @see SCENEFILERESULT
        */
        virtual int  Export(bool SaveAs = false, Ogre::String exportfile = "");
        /**
        * Imports (de-serializes) scene data
        * @param importfile a scene file to import
        * @return result of the scene import  
        * @see SCENEFILERESULT
        */
        virtual int  Import(Ogre::String importfile = "");
    };
};

