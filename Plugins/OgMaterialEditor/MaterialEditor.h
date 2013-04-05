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

#ifndef MATERIAL_EDITOR_H
#define MATERIAL_EDITOR_H

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

    class PluginExport CMaterialEditor : public CBaseEditor
    {
        friend class CMaterialEditorFactory;
    public:

        virtual void     createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        virtual void     showBoundingBox(bool bShow);
        virtual void     setPropertiesLevel(int level, int id)
        {
            mPropertyLevel = level;
            mPropertyID = id;
        };

        /// This function is called when user right clicks a property to get that property specific popup menu
        /// returns false if no menu present (Disabled Menu Items has a "D" prefix where Enabled Menu Items have "E" prefix
        virtual bool                 getObjectContextMenu(UTFStringVector &menuitems);
        /// This function is called when user selects a menuitem from ContextMenu
        virtual void                 onObjectContextMenu(int menuresult);
        /// Gets the Handle to encapsulated object
        inline virtual void         *getHandle() {return static_cast<void*>(mHandle.get());};
        virtual Ogre::AxisAlignedBox getAABB() {return Ogre::AxisAlignedBox::BOX_NULL;};
        virtual Ogre::SceneNode     *getNode() {return 0;};
        inline virtual int           getPropertyLevel() {return mPropertyLevel;};
        inline virtual int           getPropertyID() {return mPropertyID;};

        inline virtual void             setHandle(Ogre::MaterialPtr handle){mHandle = handle;}
        inline virtual Ogre::String     getMaterialPath(){return mMaterialPath->get();};
        inline virtual void             setMaterialPath(const Ogre::String& path) {mMaterialPath->set(path);};

    protected:
        Ogre::MaterialPtr  mHandle;
        OgitorsProperty<Ogre::String>       *mMaterialPath;
        Ogre::StringVector mCompareFunctions;
        Ogre::StringVector mCullingModes;
        Ogre::StringVector mShadingModes;
        Ogre::StringVector mPolygonModes;
        int                mPropertyLevel;
        int                mPropertyID;

        bool _setMaterialPath(OgitorsPropertyBase* property, const Ogre::String& value);

        CMaterialEditor(CBaseEditorFactory *factory);
        virtual     ~CMaterialEditor();

        void _displayTechnique(int id);
        void _displayPass(int techid, int id);
        void _displayTextureUnits(Ogre::Pass *pass, unsigned int BINDING); 
    };

    class PluginExport CMaterialEditorFactory: public CBaseEditorFactory
    {
    public:
        CMaterialEditorFactory(OgitorsView *view = 0);
        virtual ~CMaterialEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
    };
}

#endif