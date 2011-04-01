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

    class OgitorExport CFolderEditor : public CBaseEditor
    {
        friend class CFolderEditorFactory;
    public:

        virtual void     createProperties(OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool     load(bool async = true);
        /** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        /** @copydoc CBaseEditor::isNodeType() */
        virtual bool     isNodeType() {return true;};

        /// This function is called when user right clicks a property to get that property specific popup menu
        /// returns false if no menu present (Disabled Menu Items has a "D" prefix where Enabled Menu Items have "E" prefix
        virtual bool                 getObjectContextMenu(UTFStringVector &menuitems);
        /// This function is called when user selects a menuitem from ContextMenu
        virtual void                 onObjectContextMenu(int menuresult);
        virtual Ogre::SceneNode     *getNode();
        virtual Ogre::SceneManager  *getSceneManager();
        /** @copydoc CBaseEditor::setNameImpl(Ogre::String) */
        virtual bool                 setNameImpl(Ogre::String name);


    protected:
        
        CFolderEditor(CBaseEditorFactory *factory);
        virtual     ~CFolderEditor();

    };

    class OgitorExport CFolderEditorFactory: public CBaseEditorFactory
    {
    public:
        CFolderEditorFactory(OgitorsView *view = 0);
        virtual ~CFolderEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
    };
}

