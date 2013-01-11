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

#ifndef SCREEN_EFFECT_EDITOR_H
#define SCREEN_EFFECT_EDITOR_H

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

    class PluginExport CScreenEffectEditor : public CBaseEditor
    {
        friend class CScreenEffectEditorFactory;

    public:
        virtual void                    createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool                    load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool                    unLoad();
        virtual void                    showBoundingBox(bool bShow){};

        /// This function is called when user right clicks a property to get that property specific popup menu
        /// returns false if no menu present (Disabled Menu Items has a "D" prefix where Enabled Menu Items have "E" prefix
        virtual bool                    getObjectContextMenu(UTFStringVector &menuitems);
        /// This function is called when user selects a menuitem from ContextMenu
        virtual void                    onObjectContextMenu(int menuresult);
        /// Gets the Handle to encapsulated object
        inline virtual void*            getHandle() {return static_cast<void*>(mHandle);};
        inline virtual void             setHandle(Ogre::Viewport* handle){mHandle = handle;}
        inline virtual Ogre::String     getCompositorName(){return mCompositorName->get();};
        inline virtual void             setCompositorName(const Ogre::String& path) {mCompositorName->set(path);};

        TiXmlElement*                   exportDotScene(TiXmlElement *pParent); 

        /**
        * Adds new compositor
        * @param name compositor name
        * @param position place at which to insert new compositor
        */
        void                            addCompositor(const Ogre::String& name, int position = -1);
        /**
        * Removes compositor from the list
        * @param name compositor name
        */
        void                            removeCompositor(const Ogre::String& name);        
        /**
        * Enables a compositor
        * @param name compositor name
        * @param value boolean whether to enable or disable the compositor
        */
        void                            enableCompositor(const Ogre::String& name, const bool& value);            
              
protected:
        /**
        * Property setter for displaying compositor(s) flag (internal)
        * @param property Handle to property responsible for displaying compositor(s) flag
        * @param value new compositor(s) display flag
        * @return true if property handle is valid 
        */
        bool                            _setCompositorEnabled(OgitorsPropertyBase* property, const bool& value);
        /**
        * Property setter for compositor(s) name (internal)
        * @param property Handle to property responsible for compositor(s) name
        * @param value new compositor(s) name
        * @return true if property handle is valid 
        */
        bool                            _setCompositorName(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Restores compositor(s) in previous glory (undoes the operation(s))
        */
        void                            _restoreCompositors();

        bool                            _setCompositorNamedConstant(OgitorsPropertyBase* property, const float& value);
        bool                            _setCompositorNamedConstantColourValue(OgitorsPropertyBase* property, const Ogre::ColourValue& value);

        Ogre::Viewport*                     mHandle;
        OgitorsProperty<Ogre::String>*      mCompositorName;
        OgitorsProperty<bool>*              mCompositorEnabled;

        CScreenEffectEditor(Ogre::Viewport* viewport, CBaseEditorFactory *factory);
        virtual     ~CScreenEffectEditor();
    };

    class PluginExport CScreenEffectEditorFactory: public CBaseEditorFactory
    {
    public:
        CScreenEffectEditorFactory(OgitorsView *view = 0);
        virtual ~CScreenEffectEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
    };
}

#endif