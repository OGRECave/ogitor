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

#pragma once

#include "OgitorsSingleton.h"

namespace Ogitors
{
    struct ObjectCopyData: public Ogre::GeneralAllocatedObject
    {
        Ogre::String             mObjectName;
        Ogre::String             mObjectTypeName;
        OgitorsPropertyValueMap  mProperties;
        OgitorsCustomPropertySet mCustomProperties;
    };

    typedef Ogre::vector<ObjectCopyData*>::type ClipboardBuffer;

    typedef Ogre::map<Ogre::String, UndoCollection*>::type TemplateMap;
    
    struct ObjectTemplateData
    {
        Ogre::String              mTypeName;
        OgitorsPropertyValueMap   mObjectProperties;
        OgitorsCustomPropertySet *mCustomProperties;
    };

    typedef Ogre::vector<ObjectTemplateData>::type ObjectTemplate;

    typedef Ogre::map<Ogre::String, ObjectTemplate>::type ObjectTemplateMap;

    //! Clipboard manager class
    /*!  
        A class that handles clipboard operations
    */
    class OgitorExport OgitorsClipboardManager: public Singleton<OgitorsClipboardManager>, public Ogre::GeneralAllocatedObject
    {
    public:
        /**
        * Constructor
        */
        OgitorsClipboardManager();
        
        /**
        * Destructor
        */
        virtual ~OgitorsClipboardManager();
        
        /**
        * Copies given object
        */
        void copy(CBaseEditor* object);
        
        /**
        * Pastes an object from clipboard as a child of given parent object
        * Index specifies which object from the clipboardbuffer will be pasted
        */
        CBaseEditor *paste(CBaseEditor *parent, int index = -1);
        /**
        * Fetch the names of objects in the clipboard buffer
        * @return stringvector of available objects in clipboard buffer
        */
        inline Ogre::StringVector getPasteNames()
        {
            Ogre::StringVector names;
            
            for(unsigned int i = 0;i < mBuffer.size();i++)
                names.push_back(mBuffer[i]->mObjectName);
            
            return names;
        };
        
        /**
        * Tests if a paste operation can be performed
        */
        inline bool canPaste() 
        {
            return (mBuffer.size() > 0);
        };

        /**
        * Copies the given object to a template file
        * @param object the object to copy
        * @param templatename the name of the template file
        * @param isGeneralScope is the template available to all projects?
        */
        bool copyToTemplate(CBaseEditor *object, const Ogre::String& templatename, bool isGeneralScope);
        /**
        * Copies the given multi selection object to a template file
        * @param object the object to copy
        * @param templatename the name of the template file
        * @param isGeneralScope is the template available to all projects?
        */
        bool copyToTemplateMulti(CMultiSelEditor *object, const Ogre::String& templatename, bool isGeneralScope);
        /**
        * Copies the given object and all its children to a template file
        * @param object the object to copy
        * @param templatename the name of the template file
        * @param isGeneralScope is the template available to all projects?
        */
        bool copyToTemplateWithChildren(CBaseEditor *object, const Ogre::String& templatename, bool isGeneralScope);

        /**
        * Returns a map of available general scope templates
        * @return a const reference to internal General Templates Map
        */
        const ObjectTemplateMap& getGeneralTemplates() const { return mGeneralTemplates; }

        /**
        * Returns a map of available project scope templates
        * @return a const reference to internal Project Templates Map
        */
        const ObjectTemplateMap& getProjectTemplates() const { return mProjectTemplates; }

        /**
        * Checks if template can be instantiated
        * @param templatename name of the template to instantiate
        * @return true if the template can be instantiated
        */
        bool isTemplateInstantiable(const Ogre::String& templatename);

        /**
        * Checks if template requires placement
        * @param templatename name of the template to instantiate
        * @return true if the template needs placement
        */
        bool doesTemplateRequirePlacement(const Ogre::String& templatename);

        /**
        * Creates objects using a template and returns a pointer to top level object
        * @param templatename name of the template to instantiate
        */
        CBaseEditor *instantiateTemplate(const Ogre::String& templatename);

        /**
        * Sets the Current Clipboard Buffer's Max Size to desired value
        * @param newsize the new max size of Clipboard Buffer
        */
        static void setClipboardMaxSize(unsigned int newsize)
        {
            if(newsize < 1)
                newsize = 1;
            mClipboardMaxSize = newsize;
        }
        /**
        * Returns the current Clipboard Buffer's Max Size
        * @return max size of clipboard buffer
        */
        static unsigned int getClipboardMaxSize()
        {
            return mClipboardMaxSize;
        }

        /**
        * Reads a file and adds templates from it
        * @param filename the name of the file to read
        * @param isGeneralScope will the templates placed to general templates?
        */
        void addTemplateFromFile(Ogre::String filename, bool isGeneralScope);

        /**
        * Reads a list of files and adds templates from them
        * @param filenames the list of file names to read from
        * @param isGeneralScope will the templates placed to general templates?
        */
        void addTemplatesFromFiles(Ogre::StringVector filenames, bool isGeneralScope);

    protected:
        static unsigned int mClipboardMaxSize;          /** The number of items the clipboard will store */
        ClipboardBuffer     mBuffer;                    /** Clipboard's internal buffer to hold multiple copy operations */
        ObjectTemplateMap   mGeneralTemplates;          /** General Scope Templates */
        ObjectTemplateMap   mProjectTemplates;          /** Project Scope Templates */
    private:
        /** Private function for recursively pasting an object */
        CBaseEditor *pasteRecursive(CBaseEditor* copyParent, CBaseEditor* newParent);
    };
};
