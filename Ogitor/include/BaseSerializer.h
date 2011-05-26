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
    /** Serialization capability enumeration */
    enum SERIALIZERCAPABILITY
    {
        CAN_EXPORT = 1,         /** Serializer can export */
        CAN_IMPORT = 2          /** Serializer can import */
    };

    /** Scene loading result enumeration */
    enum SCENEFILERESULT
    {
        SCF_OK = 1,             /** Scene was loaded successfully */
        SCF_CANCEL = 0,         /** Scene was not loaded */
        SCF_ERRUNKNOWN = -1,    /** Unknown error has occured during loading of the scene */
        SCF_ERRFILE = -2,       /** Invalid import file or parsing error on structure level */
        SCF_ERRPARSE = -3,      /** Parsing error has occured during scene loading */
        SCF_ERROGRE = -4,       /** Ogre error has been raised during scene loading */
        SCF_ERRVERSION = -5     /** Incorrect version error has been raised during scene loading */
    };

    //! Base serializer class
    /*!  
        A class that is responsible for serialization/deserialization of scene file
    */
    class OgitorExport CBaseSerializer: public Ogre::GeneralAllocatedObject
    {
    public:
        /**
        * Constructor
        * @param typestring type name of the serializer
        * @param capability capabilities of the serializer (@see SERIALIZERCAPABILITY)
        */
        CBaseSerializer(Ogre::String typestring = "Base Serializer", unsigned int capability = 0) : mTypeName(typestring), mTypeID(0), mCapability(capability) {};
        /**
        * Destructor
        */
        virtual ~CBaseSerializer() {};

        /**
        * Fetches type name of the serializer
        * @return type name of the serializer
        */
        inline Ogre::String GetTypeString() {return mTypeName;};
        /**
        * Fetches error message as string
        * @return errno the error code
        */
        static Ogre::String GetErrorString(SCENEFILERESULT err)
        {
            switch(err)
            {
            case SCF_CANCEL: return "Operation Cancelled";
            case SCF_ERRUNKNOWN: return "Unknown Error";
            case SCF_ERRFILE: return "File I/O Error";
            case SCF_ERRPARSE: return "Error Parsing File";
            case SCF_ERROGRE: return "OGRE Related Error";
            case SCF_ERRVERSION: return "This File Version is not Supported";
            };
            return "Success";
        }
        /**
        * Fetches type ID of the serializer
        * @return type ID of the serializer
        */
        inline unsigned int GetTypeID() {return mTypeID;};
        /**
        * Sets type ID of the serializer
        * @param ID sets type ID the serializer
        */
        inline void         SetTypeID(unsigned int ID) {mTypeID = ID;};
        /**
        * Tests if serializer object can export
        * @return true if serializer object can export, otherwise false
        */
        inline bool         CanExport() {return ((mCapability & CAN_EXPORT) != 0);};
        /**
        * Tests if serializer object can import
        * @return true if serializer object can import, otherwise false
        */
        inline bool         CanImport() {return ((mCapability & CAN_IMPORT) != 0);};

        /**
        * Tests if serializer object requires scene to be terminated before importing new scene
        * @return true if serializer object requires scene to be terminated before importing new scene, otherwise false
        */
        virtual bool RequiresTerminateScene() {return false;};
        /**
        * Exports scene data
        * @param SaveAs flag signifying that a "Save As" dialog is invoked to save scene file into different place
        * @param exportfile optional parameter to pass the location where the scene is to be saved to
        * @return @see SCENEFILERESULT
        */
        virtual int  Export(bool SaveAs = false, Ogre::String exportfile = "") {return SCF_OK;};
        /**
        * Imports scene data
        * @param importfile file name of imported scene
        * @return @see SCENEFILERESULT
        */
        virtual int  Import(Ogre::String importfile = "") {return false;};

    protected:
        Ogre::String mTypeName;            /** Serializer type name */
        unsigned int mTypeID;              /** Serializer type ID */
        unsigned int mCapability;          /** Serializer import/export/other capabilities */
    };
};
