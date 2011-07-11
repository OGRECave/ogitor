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

#include "OgitorsSingleton.h"


namespace Ogitors
{
    //! Ogitor System class
    /*!  
        A class that is responsible for handling UI dialogs, maintenance of tree view widget and extra utility functions
    */
    class OgitorExport OgitorsSystem: public Singleton<OgitorsSystem>, public Ogre::GeneralAllocatedObject
    {
    public:
        /**
        * Constructor
        */
        OgitorsSystem();
        /**
        * Destructor
        */
        virtual ~OgitorsSystem();
        /**
        * Checks if a file exists
        * @param filename source filename
        * @return true if file exists
        */
        virtual bool FileExists(const Ogre::String& filename) = 0;
        /**
        * Copies a file
        * @param source source filename
        * @param destination destination filename
        * @return true if successful
        */
        virtual bool CopyFile(Ogre::String source, Ogre::String destination) = 0;
        /**
        * Copies files from one directory into another
        * @param source source directory to copy files from
        * @param destination destination directory to copy files to
        * @return true 
        */
        virtual bool CopyFilesEx(Ogre::String source, Ogre::String destination) = 0;
        /**
        * Creates new directory at specified path
        * @param dirname path to make new directory at
        * @return true if directory was created successfully, otherwise false
        */
        virtual bool MakeDirectory(Ogre::String dirname) = 0;
        /**
        * Fetches full path to the Ogitor' Projects directory
        * @return full path to the Ogitor' Projects directory
        */
        virtual Ogre::String getProjectsDirectory() = 0;
        /**
        * Deletes specified file
        * @param file full path to the file, including file name
        */
        virtual void DeleteFile(const Ogre::String &file) = 0;
        /**
        * Changes file name
        * @param oldname original name of the file (full path)
        * @param newname new name of the file (full path)
        */
        virtual void RenameFile(const Ogre::String &oldname, const Ogre::String &newname) = 0;
        /**
        * Fetches a list of files in specified directory
        * @param path path which is to be enumerated with files
        * @param list a list to place file names into
        */
        virtual void GetFileList(Ogre::String path, Ogre::StringVector &list) = 0;
        /**
        * Fetches a list of directories in specified directory
        * @param path path which is to be enumerated with dirs
        * @param list a list to place directory names into
        */
        virtual void GetDirList(Ogre::String path, Ogre::StringVector &list) = 0;
        /**
        * Displays "Select Directory" dialog
        * @param title title of the dialog
        * @return name of selected directory
        */
        virtual Ogre::String DisplayDirectorySelector(Ogre::UTFString title) = 0;
        /**
        * Displays Progress dialog
        * @param title title of the dialog
        * @param min minimum value of progress bar
        * @param max maximum value of progress bar
        * @param value current value of progress bar
        */
        virtual void DisplayProgressDialog(Ogre::UTFString title, int min, int max, int value) = 0;
        /**
        * Hides Progress dialog
        */
        virtual void HideProgressDialog() = 0;
        /**
        * Updates Progress dialog
        * @param value current value of progress bar
        */
        virtual void UpdateProgressDialog(int value) = 0;
        /**
        * Displays "Open File" dialog
        * @param title title of the dialog
        * @param extensionlist the list of extensions to filter out
        * @return name of selected file
        */
        virtual Ogre::String DisplayOpenDialog(Ogre::UTFString title, UTFStringVector ExtensionList) = 0;
        /**
        * Displays "Save File" dialog
        * @param title title of the dialog
        * @param extensionlist the list of extensions to filter out
        * @return name of the file to be saved
        */
        virtual Ogre::String DisplaySaveDialog(Ogre::UTFString title, UTFStringVector ExtensionList) = 0;
        /**
        * Displays "Yes/No/Cancel" or "Okay/Cancel" dialog
        * @param msg message to convey to the user
        * @param dlgType type of the dialog
        * @return dialog result
        */
        virtual DIALOGRET    DisplayMessageDialog(Ogre::UTFString msg, DIALOGTYPE dlgType) = 0;
        /**
        * Displays terrain creation dialog
        * @param params additional terrain parameters needed for its creation
        * @return true if dialog' result was accepted, otherwise false
        */
        virtual bool         DisplayTerrainDialog(Ogre::NameValuePairList &params) = 0;
        /**
        * Displays heightmap import dialog
        * @param params additional heightmap parameters needed for its import
        * @return true if dialog' result was accepted, otherwise false
        */
        virtual bool         DisplayImportHeightMapDialog(Ogre::NameValuePairList &params) = 0;
        /**
        * Displays blendmap calculation dialog
        * @param params additional blendmap parameters needed
        * @return true if dialog' result was accepted, otherwise false
        */
        virtual bool         DisplayCalculateBlendMapDialog(Ogre::NameValuePairList &params) = 0;
        /**
        * Supplies feedback about loading progress 
        * @param percentage The completion percentage
        * @param msg info about the loading stage
        */
        virtual void         UpdateLoadProgress(float percentage, Ogre::UTFString msg) = 0;
        /**
        * Translates specified string into OGRE-based UTF string
        * @param str string to be translated
        * @return 
        */
        virtual Ogre::UTFString Translate(Ogre::String& str) {return str;};
        /**
        * Translates specified string into OGRE-based UTF string
        * @param str pointer to character array to be translated (wrapped)
        * @return 
        */
        virtual Ogre::UTFString Translate(const char * str) {return Ogre::String(str);};
        /**
        * Tests if the system has treeview widget associated with it
        * @return true if it has treeview widget associated with it, otherwise false
        */
        virtual bool         HasTreeView() = 0;
        /**
        * Tests if the system has properties widget associated with it
        * @return true if it has properties widget associated with it, otherwise false
        */
        virtual bool         HasPropertiesView() = 0;
        /**
        * Presents listed properties into treeview widget
        */
        virtual void         PresentPropertiesView(CBaseEditor* object) = 0;
        /**
        * Selects specified tree node item
        * @param handle a tree node item that can is obtained through 
        * @see OgitorsSystem::InsertTreeItem
        * @return
        */
        virtual void         SelectTreeItem(CBaseEditor *object) = 0;
        /**
        * Inserts new tree node item into specified parent node
        * @param parent parent handle
        * @param object object handle 
        * @param iconid ID for new tree node item icon 
        * @param colour text color of new tree node item text
        * @return handle of new tree node item
        */
        virtual void         InsertTreeItem(CBaseEditor *parent, CBaseEditor *object, int iconid, unsigned int colour) = 0;
        /**
        * Moves tree item to new parent node
        * @param newparent new parent node handle
        * @param object objects tree handle
        * @return handle of new tree node item
        */
        virtual void *       MoveTreeItem(void *newparent, void *object) = 0;
        /**
        * Moves tree item to new parent node
        * @param newparent new parent layer id
        * @param object objects tree handle
        */
        virtual void         MoveLayerTreeItem(int newparent, CBaseEditor *object) = 0;
        /**
        * Sets tree node item text
        * @param object handle to the object
        * @param newname new node item text
        */
        virtual void         SetTreeItemText(CBaseEditor *object, Ogre::String newname) = 0;
        /**
        * Deletes tree node item with specified text
        * @param object handle to the object
        */
        virtual void         DeleteTreeItem(CBaseEditor *object) = 0;
        /**
        * Creates tree item root node
        * @param name name of the root node
        * @return handle of the root node
        */
        virtual void *       CreateTreeRoot(Ogre::String name) = 0;
        /**
        * Sets tree node' item text color
        * @param object handle to the object
        * @param colour new colour of tree node item
        */
        virtual void         SetTreeItemColour(CBaseEditor *object, unsigned int colour) = 0;
        /**
        * Clears tree node(s)
        */
        virtual void         ClearTreeItems() = 0;
        /**
        * Sets mouse cursor icon
        * @param cursor cursor type 
        * @see OgitorsMouseCursorType
        */
        virtual void         SetMouseCursor(unsigned int cursor) {};
        /**
        * Sets mouse cursor position
        * @param new position of mouse cursor
        */
        virtual void         SetMousePosition(Ogre::Vector2 position) {};
        /**
        * Shows/hides mouse cursor
        * @param bShow flag to show/hide mouse cursor
        */
        virtual void         ShowMouseCursor(bool bShow) {};
    };

    //! Dummy class
    /*!  
        Dummy class with empty implementations
        @deprecated
    */
    class OgitorsDummySystem: public OgitorsSystem
    {
    public:
        /**
        * @copydoc OgitorsSystem::OgitorsSystem()
        */
        OgitorsDummySystem() {};
        /**
        * @copydoc OgitorsSystem::~OgitorsSystem()
        */
        virtual ~OgitorsDummySystem() {};
        /**
        * @copydoc OgitorsSystem::FileExists(const Ogre::String&)
        */
        virtual bool FileExists(const Ogre::String& filename) {return false;};
        /**
        * @copydoc OgitorsSystem::CopyFile(Ogre::String, Ogre::String)
        */
        virtual bool CopyFile(Ogre::String source, Ogre::String destination) {return false;};
        /**
        * @copydoc OgitorsSystem::CopyFilesEx(Ogre::String, Ogre::String)
        */
        virtual bool CopyFilesEx(Ogre::String source, Ogre::String destination) {return false;};
        /**
        * @copydoc OgitorsSystem::MakeDirectory(Ogre::String)
        */
        virtual bool MakeDirectory(Ogre::String dirname) {return false;};
        /**
        * @copydoc OgitorsSystem::GetExePath()
        */
        virtual Ogre::String GetExePath() {return "./";};
        /**
        * @copydoc OgitorsSystem::getProjectsDirectory()
        */
        virtual Ogre::String getProjectsDirectory() {return "./";};
        /**
        * @copydoc OgitorsSystem::DeleteFile(const Ogre::String&)
        */
        virtual void DeleteFile(const Ogre::String &file) {};
        /**
        * @copydoc OgitorsSystem::RenameFile(const Ogre::String&, const Ogre::String&)
        */
        virtual void RenameFile(const Ogre::String &oldname, const Ogre::String &newname) {};
        /**
        * @copydoc OgitorsSystem::GetFileList(Ogre::String, Ogre::StringVector&)
        */
        virtual void GetFileList(Ogre::String path, Ogre::StringVector &list) {list.clear();};
        /**
        * @copydoc OgitorsSystem::GetDirList(Ogre::String, Ogre::StringVector&)
        */
        virtual void GetDirList(Ogre::String path, Ogre::StringVector &list) {list.clear();};
        /**
        * @copydoc OgitorsSystem::DisplayDirectorySelector(Ogre::UTFString)
        */
        virtual Ogre::String DisplayDirectorySelector(Ogre::UTFString title) {return "";};
        /**
        * @copydoc OgitorsSystem::DisplayProgressDialog(Ogre::UTFString, int, int, int)
        */
        virtual void DisplayProgressDialog(Ogre::UTFString title, int min, int max, int value) {};
        /**
        * @copydoc OgitorsSystem::HideProgressDialog()
        */
        virtual void HideProgressDialog() {};
        /**
        * @copydoc OgitorsSystem::UpdateProgressDialog(int)
        */
        virtual void UpdateProgressDialog(int value) {};
        /**
        * @copydoc OgitorsSystem::DisplayOpenDialog(Ogre::UTFString, Ogre::UTFStringVector&)
        */
        virtual Ogre::String DisplayOpenDialog(Ogre::UTFString title, UTFStringVector ExtensionList) {return "";};
        /**
        * @copydoc OgitorsSystem::DisplaySaveDialog(Ogre::UTFString, Ogre::UTFStringVector&)
        */
        virtual Ogre::String DisplaySaveDialog(Ogre::UTFString title, UTFStringVector ExtensionList) {return "";};
        /**
        * @copydoc OgitorsSystem::DisplayMessageDialog(Ogre::UTFString, DIALOGTYPE)
        */
        virtual DIALOGRET    DisplayMessageDialog(Ogre::UTFString msg, DIALOGTYPE dlgType) {return DLGRET_CANCEL;};
        /**
        * @copydoc OgitorsSystem::DisplayTerrainDialog(Ogre::NameValuePairList&)
        */
        virtual bool         DisplayTerrainDialog(Ogre::NameValuePairList &params) {return false;};
        /**
        * @copydoc OgitorsSystem::DisplayImportHeightMapDialog(Ogre::NameValuePairList)
        */
        virtual bool         DisplayImportHeightMapDialog(Ogre::NameValuePairList &params) {return false;};
        /**
        * @copydoc OgitorsSystem::DisplayCalculateBlendMapDialog(Ogre::NameValuePairList)
        */
        virtual bool         DisplayCalculateBlendMapDialog(Ogre::NameValuePairList &params) {return false;};
        /**
        * @copydoc OgitorsSystem::UpdateLoadProgress(float percentage, Ogre::UTFString msg)
        */
        virtual void         UpdateLoadProgress(float percentage, Ogre::UTFString msg) {};
        /**
        * @copydoc OgitorsSystem::HasTreeView()
        */
        virtual bool         HasTreeView() {return false;};
        /**
        * @copydoc OgitorsSystem::HasPropertiesView()
        */
        virtual bool         HasPropertiesView() {return false;};
        /**
        * @copydoc OgitorsSystem::PresentPropertiesView()
        */
        virtual void         PresentPropertiesView(CBaseEditor *object) {};
        /**
        * @copydoc OgitorsSystem::SelectTreeItem(void *)
        */
        virtual void         SelectTreeItem(CBaseEditor *object) {};
        /**
        * @copydoc OgitorsSystem::InsertTreeItem(CBaseEditor *, CBaseEditor *, int, unsigned int)
        */
        virtual void         InsertTreeItem(CBaseEditor *parent, CBaseEditor *object, int iconid, unsigned int colour) {};
        /**
        * @copydoc OgitorsSystem::MoveTreeItem(void *, void *)
        */
        virtual void *       MoveTreeItem(void *newparent, void *object) { return 0; }
        /**
        * @copydoc OgitorsSystem::MoveTreeItem(int, CBaseEditor *)
        */
        virtual void         MoveLayerTreeItem(int newparent, CBaseEditor *object) {};
        /**
        * @copydoc OgitorsSystem::CreateTreeRoot(Ogre::String)
        */
        virtual void *       CreateTreeRoot(Ogre::String name) {return 0;};
        /**
        * @copydoc OgitorsSystem::SetTreeItemText(CBaseEditor *, Ogre::String)
        */
        virtual void         SetTreeItemText(CBaseEditor *object, Ogre::String newname) {};
        /**
        * @copydoc OgitorsSystem::SetTreeItemColour(CBaseEditor *, unsigned int)
        */
        virtual void         SetTreeItemColour(CBaseEditor *object, unsigned int colour) {};
        /**
        * @copydoc OgitorsSystem::DeleteTreeItem(CBaseEditor *)
        */
        virtual void         DeleteTreeItem(CBaseEditor *object) {};
        /**
        * @copydoc OgitorsSystem::ClearTreeItems()
        */
        virtual void         ClearTreeItems() {};
    };
}
