/*/////////////////////////////////////////////////////////////////////////////////
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
////////////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "OgitorsSingleton.h"

namespace Ogitors
{
    class OgitorsUndoBase: public Ogre::GeneralAllocatedObject
    {
    public:
        OgitorsUndoBase() { mDescription = ""; }
        virtual ~OgitorsUndoBase() {};

        virtual void init() {};
        inline Ogre::String getDescription() { return mDescription; }
        virtual bool apply() = 0;
    protected:
        Ogre::String mDescription;
    };

    typedef Ogre::vector<OgitorsUndoBase*>::type OgitorsUndoVector;
    
    class PropertyUndo : public OgitorsUndoBase
    {
    public:
        PropertyUndo(OgitorsPropertySet *set, OgitorsPropertyBase *property);
        virtual ~PropertyUndo() {};

        virtual bool apply();
    protected:
        unsigned int         mObjectID;
        PropertySetType      mSetType;
        Ogre::String         mPropertyName;
        OgitorsPropertyValue mValue;
    };

    class ObjectCreationUndo : public OgitorsUndoBase
    {
    public:
        ObjectCreationUndo(CBaseEditor *object);
        virtual ~ObjectCreationUndo() {};

        virtual bool apply();
    protected:
        unsigned int              mObjectID;
    };

    class ObjectDeletionUndo : public OgitorsUndoBase
    {
    public:
        ObjectDeletionUndo(CBaseEditor *object);
        virtual ~ObjectDeletionUndo();

        virtual bool apply();
    protected:
        unsigned int              mParentObjectID;

        Ogre::String              mTypeName;

        OgitorsPropertyValueMap   mObjectProperties;
        OgitorsCustomPropertySet *mCustomProperties;
    };

    class CustomSetRebuiltUndo : public OgitorsUndoBase
    {
    public:
        CustomSetRebuiltUndo(OgitorsCustomPropertySet *set);
        virtual ~CustomSetRebuiltUndo();

        virtual bool apply();
    protected:
        unsigned int              mObjectID;
        OgitorsCustomPropertySet *mCustomProperties;
    };

    class UndoCollection : public OgitorsUndoBase
    {
    public:
        UndoCollection(const Ogre::String& desc);
        virtual ~UndoCollection();

        virtual bool apply();
        virtual void addUndo(OgitorsUndoBase *undo);
        bool isEmpty() { return (mBuffer.size() == 0); }

    protected:
        OgitorsUndoVector mBuffer;
    };

    typedef Ogre::vector<UndoCollection*>::type UndoCollectionVector;

    //! Undo manager class
    /*!  
        A class that handles undo/redo operations
    */
    class OgitorExport OgitorsUndoManager: public Singleton<OgitorsUndoManager>, public OgitorsPropertySetListener
    {
    public:
        /**
        * Constructor
        */
        OgitorsUndoManager();
        /**
        * Destructor
        */
        virtual ~OgitorsUndoManager();
        /**
        * Clears Undo Manager
        */
        void Clear();
        /**
        * Starts an undo collector object
        */
        void BeginCollection(const Ogre::String& desc);
        /**
        * Starts an undo collector object
        */
        void BeginCollection(UndoCollection *collection = NULL);
        /**
        * Ends a collection and returns it
        */
        UndoCollection *EndCollection(bool add = false, bool ignore = false);
        /**
        * Adds an undo to the queue
        */
        void AddUndo(OgitorsUndoBase *undo);
        /**
        * Reverts last operation
        */
        void Undo();
        /**
        * Re-applies next change in the queue
        */
        void Redo();
        /**
        * String'ifies last undo structure information
        * @return string'ified last undo structure information
        */
        inline Ogre::String GetUndoString()
        {
            if(CanUndo() && mBuffer[mCurrentIndex - 1])
                return mBuffer[mCurrentIndex - 1]->getDescription();
            else
                return "";
        };
        /**
        * String'ifies last redo structure information
        * @return string'ified last redo structure information
        */
        inline Ogre::String GetRedoString()
        {
            if(CanRedo() && mBuffer[mCurrentIndex])
                return mBuffer[mCurrentIndex]->getDescription();
            else
                return "";
        };
        /**
        * Tests if an undo operation can be performed
        */
        inline bool CanUndo() 
        {
            return (mCurrentIndex > 0);
        };
        /**
        * Tests if a redo operation can be performed
        */
        inline bool CanRedo()
        {
            return (mCurrentIndex < mBuffer.size());
        };
    private:
        OgitorsUndoVector    mBuffer;
        UndoCollectionVector mCollections;
        unsigned int         mCurrentIndex;
        bool                 mListeningActive;

        void OnPropertyRemoved(OgitorsPropertySet* set, OgitorsPropertyBase* property); 
        void OnPropertyAdded(OgitorsPropertySet* set, OgitorsPropertyBase* property);
        void OnPropertyChanged(OgitorsPropertySet* set, OgitorsPropertyBase* property); 
        void OnPropertySetRebuilt(OgitorsPropertySet* set);
    };
};
