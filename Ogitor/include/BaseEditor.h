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
///////////////////////////////////////////////////////////////////////////////////*/

/*///////////////////////////////////////////////////////////////////////////////////
/// This Header defines the CBaseEditor class which is the Base Class of All Editor
/// Objects in the Library.
/// This class has both general purpose base functions and also virtual functions
/// for each Editor Object to override in order to create a common framework
/// for different types of editable objects in the scene. It also helps to use the same
/// command for any object regardless of the actual commands needed for the underlying
/// OGRE objects.
///////////////////////////////////////////////////////////////////////////////////*/
#pragma once

#define PROPERTY(name, type, value, tag, setter)\
{\
    Ogitors::OgitorsPropertyDefMap::iterator defi = mFactory->mPropertyDefs.find(name);\
    if (defi != mFactory->mPropertyDefs.end())\
    {\
    mProperties.addProperty(OGRE_NEW Ogitors::OgitorsProperty<type>(&(defi->second), value, tag, setter));\
    }\
}\

#define PROPERTY_PTR(ptr, name, type, value, tag, setter)\
{\
    Ogitors::OgitorsPropertyDefMap::iterator defi = mFactory->mPropertyDefs.find(name);\
    ptr = 0;\
    if (defi != mFactory->mPropertyDefs.end())\
    {\
    ptr = OGRE_NEW Ogitors::OgitorsProperty<type>(&(defi->second), value, tag, setter);\
    mProperties.addProperty( ptr );\
    }\
    assert(ptr != 0);\
}\

#define SETTER(type, classname, func) (Ogitors::PropertySetterFunction<type>::from_method<classname, &classname::func>(this))
#define GETTER(type, classname, func) (Ogitors::PropertyGetterFunction<type>::from_method<classname, &classname::func>(this))

#define CONNECT_PROPERTY_MEMFN(object, propertyname, classname, function, connection)\
    object->getProperties()->getProperty(propertyname)->connect(Ogitors::OgitorsSignalFunction::from_method<classname, &classname::function>(this), connection);\

namespace Ogitors
{
    class OgitorExport CBaseEditor: public Ogre::GeneralAllocatedObject
    {
        friend class CBaseEditorFactory;
    public:
        /**
        * Fetches the factory associated with this object
        * @return factory associated with this object that can be overridden
        */
        inline CBaseEditorFactory *getFactoryDynamic() { return mFactory; }
        /** Static initialization */
        static void _initStatic(OgitorsRoot *root);

        inline void setModified(bool value) { mModified->set(value); }
        inline void setSelected(bool value) { mSelected->set(value); }
        inline void setHighlighted(bool value) { mHighlighted->set(value); }
        inline void setLoaded(bool value) { mLoaded->set(value); }
        inline void setLocked(bool value) { mLocked->set(value); }
        inline void setName(const Ogre::String& value) { mName->set(value); }
        inline void setParent(CBaseEditor *value) { mParentEditor->set((unsigned long)value); }
        inline void setLayer(int value) { mLayer->set(value); }
        inline void setUpdateScript(const Ogre::String& value) { mUpdateScript->set(value); }
        inline void setShowHelper(bool value) { mShowHelper->set(value); }

        /**
        * Fetches the unique editor type
        * @return unique editor type
        */
        EDITORTYPE                  getEditorType();
        /**
        * Fetches the unique editor type ID
        * @return unique editor type ID
        */
        unsigned int                getTypeID();
        /**
        * Fetches the corresponding string value of the editor type
        * @return string value of the editor type name
        */
        Ogre::String                getTypeName();
        /**
        * Fetches a handle to encapsulated object
        * All editor objects represent an underlying object, this function returns that
        * underlying object pointer casted as void *
        * Ex: CCameraEditor returns Ogre::Camera*, CEntityEditor returns Ogre::Entity*
        * @return handle to encapsulated object
        */
        inline virtual void        *getHandle() {return 0;};
        /**
        * Fetches a flag for the bit mask to decide if an editor object has the queried editing option capability
        * @param flags flags to test editor flags against
        * @return true if object has that capability, false otherwise
        */
        bool                        supports(unsigned int flags);
        /**
        * Fetches the default world section to be used during paging
        * @return the default section id
        */
        OgitorWorldSectionId        getDefaultWorldSection();
        /**
        * Tests if editor object was modified 
        * @return true if editor object was modified, otherwise false
        */
        inline bool                 isModified() {return mModified->get();};
        /**
        * Tests if editor object uses gizmos
        * @return true if editor object uses gizmos, otherwise false
        */
        bool                        usesGizmos();
        /**
        * Tests if editor object uses visual helper object
        * @return true if editor object uses visual helper object, otherwise false
        */
        bool                        usesHelper();
        /**
        * Fetches handle to visual helper object
        * @return handle to visual helper object
        */
        inline CVisualHelper       *getHelper() {return mHelper;};
        /**
        * Tests if editor object has been loaded
        * @return true if editor object has been loaded, otherwise false
        */
        inline bool                 isLoaded() {return mLoaded->get();};
        /**
        * Tests if oriented bounding box was created for editor object
        * @return true if oriented bounding box was created for editor object, otherwise false
        */
        inline bool                 isBBoxCreated() {return (mOBBoxRenderable != 0);};
        /**
        * Fetches a handle to the parent of editor object
        * @return a handle to the parent of editor object
        */
        inline CBaseEditor*         getParent() {return mParentEditor->get();};
        /**
        * Fetches editor object name
        * @return editor object name
        */
        inline Ogre::String         getName() {return mName->get();};
        /**
        * Fetches editor object update script name
        * @return editor object update script name
        */
        Ogre::String                getUpdateScript()  {return mUpdateScript->get();};
        /**
        * Fetches editor object script resource handle
        * @return editor object script resource handle
        */
        unsigned int                getScriptResourceHandle() {return mScriptResourceHandle;};
        /**
        * Sets editor object script resource handle
        * @param handle new editor object script resource handle
        */
        void                        setScriptResourceHandle(unsigned int handle) {mScriptResourceHandle = handle;};
        /**
        * Fetches editor object layer
        * @return editor object layer
        */
        inline int                  getLayer() {return mLayer->get();};
        /**
        * Fetches editor object unique id
        * @return editor object unique id
        */
        inline unsigned int         getObjectID() {return mObjectID->get();}
        /**
        * Sets editor object unique id
        * @param id editor object unique id
        * @remarks NEVER USE IF YOU DONT KNOW WHAT YOU ARE DOING!!!
        */
        void                        setObjectID(unsigned int id);
        /**
        * Fetches locking condition for editor object
        * @return locking condition for editor object
        */
        inline bool                 getLocked() {return mLocked->get();};
        /**
        * Fetches whether editor renders its visual helper currently
        * @return whether editor renders its visual helper currently
        */
        inline bool                 getShowHelper() {return mShowHelper->get();};
        /**
        * Fetches editor object properties set
        * @return editor object properties set
        */
        inline OgitorsPropertySet  *getProperties() {return &mProperties;};
        /**
        * Fetches editor object property
        * @return editor object property
        */
        OgitorsPropertyBase        *getProperty(const Ogre::String& propname);
        /**
        * Fetches editor object property
        * @return editor object property
        */
        bool                        hasProperty(const Ogre::String& propname);
        /**
        * Fetches editor object property values
        * @param map a Property Value Map to be filled with the properties
        */
        void                        getPropertyMap(OgitorsPropertyValueMap& map);
        /**
        * Editor Specific Property Map Modifications during Property Map Fetch
        * @param map a Property Value Map filled by CBaseEditor to be modified by descendants
        */
        virtual void                getPropertyMapImpl(OgitorsPropertyValueMap& map) {};
        /**
        * Destroys editor object and cleans up used memory
        * @param informparent flag to inform the parent of this object' destruction
        */
        void                        destroy(bool informparent = false);
        /**
        * Called when destruction of an editor is requested. Can be used to notify the user that this editor
        * cannot be destroyed right now (although it can in general as indicated by the factory settings).
        * @return only if 'true' is returned, while the destruction process actually be executed.
        */
        virtual bool				onBeforeDestroy() {return true;};
        /**
        * Called when destruction of an editor is in process, more precisely: At this stage,
        * the user has no further chance of canceling the process anymore, but the editor is still present
        * and has not been cleaned up / destroyed by its factory. This is the last chance to do some
        * custom steps with the soon-to-be-vanished editor object.
        */
        virtual void				onDuringDestroy() {};
        /**
        * Returns a list with object's name and all of its children names recursively
        * @param list the list to be filled with names
        */
        void                        getNameList(Ogre::StringVector& list);
        /**
        * Adds a new child object to editor object
        * @param child new child object
        */
        void                       _addChild(CBaseEditor *child);
        /**
        * Searches for an object amongst parent-child tree with specified name
        * @param name name of the child to search for
        * @param searchchildren if true, search is recursive
        * @return child object handle if found, otherwise 0
        */
        CBaseEditor*                findChild(Ogre::String name, bool searchchildren);
        /**
        * Destroys all children of an object (recursive)
        */
        void                        destroyAllChildren();
        /**
        * Loads all children object(s) (recursive)
        * @param dontLoadPaged if TRUE do not load paged objects
        */
        void                        loadAllChildren(bool dontLoadPaged);
        /**
        * Unloads all children object(s)
        */
        void                        unLoadAllChildren();
        /**
        * Removes child from children list of editor object (non-recursive)
        * @param name name of child to be removed
        */
        void                        _removeChild(Ogre::String name);
        /**
        * Fetches a hash map of all children of editor object
        * @return hash map of all children of editor object
        */
        inline NameObjectPairList&  getChildren() {return mChildren;};
        /**
        * Creates an ordered node object list starting with parents first, children last
        * @param list objects list
        */
        void                        getNodeList(ObjectVector& list);

        /**
        * Fetches custom property set
        * @return set of custom properties
        */
        OgitorsCustomPropertySet   *getCustomProperties() { return &mCustomProperties; }
        /**
        VIRTUAL FUNCTIONS: Overridden By Editor Objects to suit their needs
        **/

        /**
        * Processes parameters list, initializing the internal properties according to values in the given list
        * @param params parameters list
        */
        virtual void                createProperties(OgitorsPropertyValueMap &params);
        /**
        * General purpose update function used by OgitorsRoot @see OgitorsRoot::Update
        * @param timePassed delta of time
        * @return true if update modified the scene, otherwise false
        */
        virtual bool                update(float timePassed) {return false;};
        /**
        * General purpose update function used by OgitorsRoot
        * @param SceneMngr scene manager handle
        * @param Camera camera handle
        * @param RenderWindow render window handle
        * @return must always return false
        * @remarks the return value may be used in the future
        */
        virtual bool                postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow) {return false;};
        /**
        * Creates editor object
        * @return true if this object is successfully, otherwise false (also forces object's parent to load too)
        */
        virtual bool				load(bool async = true) {mLoaded->set(true);return true;};
        /**
        * Unloads editor object and all of its children
        * @return false if editor was not loaded properly, otherwise true
        */
        virtual bool				unLoad() {unLoadAllChildren();mLoaded->set(false);return true;};
        /**
        * Writes custom files during an export procedure
        * @param forced Force to save even if data is not changed
        */
        virtual void				onSave(bool forced = false) {};
        /**
        * Shows or hides editor' bounding box
        * @param bShow flag to signify whether to show or hide bounding box
        */
        virtual void				showBoundingBox(bool bShow);
        /**
        * Assigns a visual helper for a objects without mass (Light or camera, for example)
        * @param objHelper helper object
        */
        virtual void				setHelper(CVisualHelper *objHelper);
        /**
        * Shows or hides helper object
        * @param show flag to signify whether to hide or show helper object
        */
        virtual void				showHelper(bool show);
        /**
        * Tests if any object is selected
        * @return true if this object is selected, otherwise false
        */
        inline bool					getSelected() { return mSelected->get(); }
        /**
        * Called before displaying an object properties in properties view 
        */
        virtual void				prepareBeforePresentProperties() {};
        /**
        * Fetches context-based properties menu for specified property type 
        * @param propertyName name of the property for which a menu is requested
        * @param menuitems menu entries for the property
        * @return true if any properties were returned, otherwise false
        */
        virtual bool				getPropertyContextMenu(Ogre::String propertyName, UTFStringVector &menuitems) {menuitems.clear();return false;};
        /**
        * Delegate function that is called when menu entry for context-based properties menu is selected
        * @param propertyName name of the property for which a menu is requested
        * @param menuresult the index of selected menu item
        */
        virtual void				onPropertyContextMenu(Ogre::String propertyName, int menuresult) {};
        /**
        * Fetches a context-based object menu (right-click on the scene)
        * @param menu item menu items the menu contains
        * @return true if menu has entries, otherwise false
        */
        virtual bool				getObjectContextMenu(UTFStringVector &menuitems) {menuitems.clear();return false;};
        /**
        * Delegate function that is called when an object is selected in objects context menu
        * @param menuresult result of menu selection
        */
        virtual void				onObjectContextMenu(int menuresult) {};
        /**
        * Delegate function that is called when material is dropped onto the scene
        * @param position position of where it is dropped on
        * @param materialname name of the material being dropped
        */
        virtual void				onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname) {};
        /**
        * Tests if editor object is serializable
        * @return true if editor object is serializable, otherwise false
        * @remarks Some objects may be temporary and should not be stored in project file
        */
        virtual bool				isSerializable() {return true;};
        /**
        * Tests if editor object is of node type (Overridden by editor class if it has node capabilities)
        * @return true if editor object has node capabilities, otherwise false
        */
        virtual bool				isNodeType() {return false;};
        /**
        * Tests if editor object can be auto tracked
        * @return true if editor object can be auto tracked
        */
        virtual bool				isAutoTrackTarget() {return false;};
        /**
        * Tests if editor object is of terrain type (Overridden by editor class if it has terrain capabilities)
        * @return true if editor object has terrain capabilities, otherwise false
        */
        virtual bool				isTerrainType() {return false;};
        /**
        * Fetches terrain editor associated with editor object
        * @return terrain editor associated with editor object
        */
        virtual ITerrainEditor*		getTerrainEditor() {return 0;};
        /**
        * Fetches paging editor associated with editor object
        * @return paging editor associated with editor object
        */
        virtual IPagingEditor*		getPagingEditor() {return 0;};
        /**
        * Delegate function that is called when terrain editor associated with editor object is destroyed
        * @return true when destruction procedure is completed successfully, otherwise false
        */
        virtual bool				onTerrainDestroyed() {return false;};

        /// VARIOUS GETXXX FUNCTIONS OVERRIDEN BY EDITOR OBJECTS TO STANDARDIZE
        /// PROPERTY RETRIEVAL FROM UNDERLYING OGRE OBJECTS

        /**
        * Fetches node handle associated with editor object
        * @return node handle associated with editor object, if none returns 0
        * @remarks the default behavior is to fetch object's parent's node if object has a parent
        */
        virtual Ogre::SceneNode     *getNode() {if(mParentEditor->get()) return mParentEditor->get()->getNode(); else return 0;};
        /**
        * Fetches scene manager handle associated with editor object
        * @return scene manager handle associated with editor object, if none returns 0
        */
        virtual Ogre::SceneManager  *getSceneManager() {if(mParentEditor->get()) return mParentEditor->get()->getSceneManager(); else return 0;};
        /**
        * Fetches editor object' DERIVED position
        * @return editor object' DERIVED position
        */
        virtual Ogre::Vector3         getDerivedPosition() {return Ogre::Vector3(0,0,0);}
        /**
        * Fetches editor object' DERIVED orientation
        * @return editor object' DERIVED orientation
        */
        virtual Ogre::Quaternion     getDerivedOrientation() {return Ogre::Quaternion::IDENTITY;};
        /**
        * Fetches editor object' DERIVED scale
        * @return editor object' DERIVED scale
        */
        virtual Ogre::Vector3        getDerivedScale() {return Ogre::Vector3(1,1,1);};
        /**
        * Fetches axis aligned bounding box associated with editor object
        * @return axis aligned bounding box associated with editor object
        */
        virtual Ogre::AxisAlignedBox getAABB() {return Ogre::AxisAlignedBox::BOX_NULL;};
        /**
        * Fetches world axis aligned bounding box associated with editor object
        * @return world axis aligned bounding box associated with editor object
        */
        virtual Ogre::AxisAlignedBox getWorldAABB();
        /**
        * Sets editor object' DERIVED position
        * @param val new editor object' DERIVED position
        */    
        virtual void                 setDerivedPosition(Ogre::Vector3 val);
        /**
        * Sets editor object' DERIVED orientation
        * @param val new editor object' DERIVED orientation
        */    
        virtual void                 setDerivedOrientation(Ogre::Quaternion val);
        /**
        * Sets editor object' DERIVED scale
        * @param val new editor object' DERIVED scale
        */    
        virtual void                 setDerivedScale(Ogre::Vector3 val);
        /**
        * Fetches void-based scene treeview item handle @see OgitorsSystem::InsertTreeItem
        * @return treeview item handle
        */
        inline void                 *getSceneTreeItemHandle() {return mSceneTreeItemHandle;};
        /**
        * Fetches void-based layer treeview item handle @see OgitorsSystem::InsertTreeItem
        * @return treeview item handle
        */
        inline void                 *getLayerTreeItemHandle() {return mLayerTreeItemHandle;};
        inline unsigned int          getTextColourInt() 
        {
            if(mLoaded->get())
                return (mLocked->get()?INTFROMRGB(255,0,0):0);
            else
                return (mLocked->get()?INTFROMRGB(255,128,128):INTFROMRGB(128,128,128));
        };
        /**
        * Sets scene treeview item handle to specified value
        * @param handle handle for treeview item
        */
        inline void                  setSceneTreeItemHandle(void * handle) {mSceneTreeItemHandle = handle;};
        /**
        * Sets layer treeview item handle to specified value
        * @param handle handle for treeview item
        */
        inline void                  setLayerTreeItemHandle(void * handle) {mLayerTreeItemHandle = handle;};
        /**
        * Exports the parameters to a dotscene file
        * @param output the stream to output
        * @param indent the indentation at the beginning
        */    
        virtual TiXmlElement*        exportDotScene(TiXmlElement *pParent) { return NULL; };
        /**
        * Notifies Paging Manager about all of its children
        */    
        void                         _refreshPaging();
        /**
        * Notifies Paging Manager about changes to it's position and all of its children's positions
        */    
        void                         _updatePaging();

        inline OgitorsPropertyBase* operator [] (const Ogre::String& index)
        {
            return mProperties.getProperty(index);
        };

        /**
        * This function supplies a widget that displays object specific tools
        * @return Handle to any custom tools widget that object wants to show
        */
        void						*getCustomToolsWindow();
        /**
        * This function supplies a widget which extends the current properties view
        * @return Handle to any custom "property widget" that object wants to show
        */
        void						*getPropertyEditorToolWindow();

        /** Script related reference counting ++ref */
        void						_addRef();
        /** Script related reference counting --ref */
        void						_release();

    protected:
        static Ogre::String      mOBBMaterials[3];
        static OgitorsRoot      *mOgitorsRoot;              /** Global OgitorsRoot handle */ 
        static OgitorsSystem    *mSystem;                   /** Global OgitorsSystem handle */ 
        static OgitorsPhysics   *mPhysics;                  /** Global OgitorsPhysics handle */ 

        CBaseEditorFactory      *mFactory;                  /** The factory that created the object */
        OgitorsView             *mView;                     /** The View instantiating the object, NULL for OgitorsRoot */

        NameObjectPairList       mChildren;                 /** Hash map of children object(s)*/
        CVisualHelper           *mHelper;                   /** Visual helper object handle */
        Ogre::SceneNode         *mBoxParentNode;            /** Custom bounding box' parent node */
        Ogre::SceneNode         *mBBoxNode;                 /** Custom bounding Box node */
        OBBoxRenderable*         mOBBoxRenderable;          /** Custom bounding box handle */
        Ogre::AxisAlignedBox     mOBBoxData;                /** A copy of previous AABB size */
        OgitorsCustomPropertySet mCustomProperties;         /** Editor custom properties list */
        OgitorsPropertySet       mProperties;               /** Editor properties list */
        void                    *mSceneTreeItemHandle;      /** Treeview item handle */
        void                    *mLayerTreeItemHandle;      /** Treeview item handle */
        int                      mRefCount;                 /** Used for Scripting */
        unsigned int             mScriptResourceHandle;     /** Handle for Object's resources at the Script Interpreter side */

        OgitorsParentProperty           *mParentEditor;     /** Parent handle */
        OgitorsProperty<unsigned int>   *mObjectID;         /** Unique Object ID */
        OgitorsProperty<Ogre::String>   *mName;             /** A copy of editor object's name to register/unregister when object destroyed by scene manager and we cant rely on GetName() */
        OgitorsProperty<unsigned int>   *mLayer;            /** The layer this object belongs to */
        OgitorsProperty<bool>           *mLocked;           /** Flag signifying if this object is locked */
        OgitorsProperty<bool>           *mLoaded;           /** Flag signifying if editor object was successfully created and loaded */ 
        OgitorsProperty<bool>           *mSelected;         /** Flag signifying if editor object is selected*/
        OgitorsProperty<bool>           *mHighlighted;      /** Flag signifying that editor object is highlighted */
        OgitorsProperty<bool>           *mModified;         /** Flag signifying if editor object was modified */
        OgitorsProperty<Ogre::String>   *mUpdateScript;     /** The script to be called during update */
        OgitorsProperty<bool>           *mShowHelper;       /** Flag signifying if helper should be rendered */

        /**
        * Constructor
        */
        CBaseEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual          ~CBaseEditor();

        /** Used by setParent */
        virtual void     setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent);

        /**
        * Sets new parent for editor object
        * @param value new parent
        */
        bool             _setParent(OgitorsPropertyBase* property, const unsigned long& value);
        /**
        * Sets name for editor object
        * @param value new editor object name
        */
        bool             _setName(OgitorsPropertyBase* property, const Ogre::String& value);
        /**
        * Behavior implementation  when name is changed
        * @param name new editor name
        */
        virtual bool     setNameImpl(Ogre::String name);
        /**
        * Sets layer for editor object
        * @param value new editor object name
        */
        bool _setLayer(OgitorsPropertyBase* property, const unsigned int& value);
        /**
        * Behavior implementation  when layer is changed
        * @param name layer id
        */
        virtual bool setLayerImpl(unsigned int newlayer);
        /**
        * Sets modified flag for editor object
        * @param bModified new modified flag for editor object
        */
        bool             _setModified(OgitorsPropertyBase* property, const bool& bModified);
        /**
        * Sets update script for editor object
        * @param scriptfile the script file to be used
        */
        bool             _setUpdateScript(OgitorsPropertyBase* property, const Ogre::String& scriptfile);
        /**
        * Sets locking condition for editor object
        * @param locked lock flag
        */
        bool             _setLocked(OgitorsPropertyBase* property, const bool& locked);
        /**
        * Highlights editor object
        * @param highlight flag to signify whether to highlight selected object or not
        */
        bool             _setHighlighted(OgitorsPropertyBase* property, const bool& highlight );
        /**
        * Implementation for selecting an object
        * @param bSelected flag to signify if object is selected or not
        */
        virtual bool     setHighlightedImpl(bool bSelected);
        /**
        * Sets an object selection flag
        * @param bSelected flag to signify if object is selected or not
        */
        bool             _setSelected(OgitorsPropertyBase* property, const bool& bSelected);
        /**
        * Implementation for selecting an object
        * @param bSelected flag to signify if object is selected or not
        */
        virtual void     setSelectedImpl(bool bSelected);
        /**
        * Sets object's loaded flag
        * @param bLoaded flag to signify if object is loaded or not
        */
        bool             _setLoaded(OgitorsPropertyBase* property, const bool& bLoaded);
        /**
        * Sets object's "show helper" flag
        * @param bShowHelper flag to signify if editor should render the visual helper
        */
        bool             _setShowHelper(OgitorsPropertyBase* property, const bool& bShowHelper);
        /**
        * Registers editor object name in the system
        */
        void             registerObjectName();
        /**
        * Unregisters editor object name in the system
        */
        void             unRegisterObjectName();
        /**
        * Registers editor object for update notifications
        */
        void             registerForUpdates();
        /**
        * Unregisters editor object from update notifications
        */
        void             unRegisterForUpdates();
        /**
        * Registers editor object for script update notifications
        */
        void             registerForScriptUpdates();
        /**
        * Unregisters editor object from script update notifications
        */
        void             unRegisterForScriptUpdates();
        /**
        * Registers editor object for a single update after the scene is loaded
        */
        void             registerForPostSceneUpdates();
        /**
        * Creates a bounding box for editor object
        */
        void             createBoundingBox();
        /**
        * Adjusts bounding box after transformation has occurred (rotation, translation, scale)
        * @return
        */
        void             adjustBoundingBox();
        /**
        * Destroys bounding box
        */
        void             destroyBoundingBox();
    };

    //! Base editor factory class
    /*!  
    A class that is responsible for instantiating base editor class(es)
    */
    class OgitorExport CBaseEditorFactory: public Ogre::GeneralAllocatedObject
    {
    public:
        OgitorsView             *mView;                     /** The View instantiating the factory, NULL for OgitorsRoot */
        unsigned int            mTypeID;                    /** Unique type identifier (Internal Use) */
        Ogre::String            mTypeName;                  /** Unique type name. Set in call to OgitorsRoot::RegisterEditorObjectFactory (Internal Use) */
        EDITORTYPE              mEditorType;                /** Type of editor objects created by this factory */
        bool                    mAddToObjectList;           /** If set to true, editor objects created by this factory will be displayed in Add Objects Menu/Toolbar */
        bool                    mRequirePlacement;          /** If true, will allow placement of editor objects created by this factory onto the scene with left mouse click */
        Ogre::String            mIcon;                      /** Path to SVG Icon for editor objects created by this factory */
        unsigned int            mCapabilities;              /** The capabilities supported by the editor objects created by this factory */
        int                     mInstanceCount;             /** Number of base editor objects instantiated */
        OgitorWorldSectionId    mDefaultWorldSection;       /** The Default World Section to be used during paging */
        bool                    mUsesHelper;                 /** A flag signifying that editor uses visual helper object */
        bool                    mUsesGizmos;                 /** A flag signifying  that editor object uses gizmos when selected*/

        OgitorsPropertyDefMap   mPropertyDefs;              /** Class holding all static property definition data */

        void                    *mToolsWindow;
        void                    *mPropertyEditorToolWindow;

        /**
        * Constructor
        */
        CBaseEditorFactory(OgitorsView *view = 0);
        /**
        * Destructor
        */
        virtual ~CBaseEditorFactory() {};
        /**
        * Initialization procedure
        */
        virtual void Init() {};
        /**
        * Returns a new instance of this factory
        */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
        /**
        * Returns the type name of this factory
        */
        Ogre::String getTypeName() {return mTypeName;};
        /**
        * Creates editor object
        * @param parent parent for editor object
        * @param params additional instantiation parameters
        * @return newly created editor object on success, otherwise 0
        * @remarks the object may modify the parent parameter (Some objects may prefer to be parented by some other fixed object)
        */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /**
        * Clones editor object
        * @param parent parent for editor object
        * @param object editor object that is to be cloned
        * @return cloned editor object on success, otherwise 0
        */
        virtual CBaseEditor *CloneObject(CBaseEditor **parent, CBaseEditor *object);
        /**
        * Destroys editor object
        * @param object editor object to be destroyed
        */
        virtual void DestroyObject(CBaseEditor *object);
        /**
        * Tests if another editor object instance can be created
        * @return true if another editor object instance can be created, otherwise false
        */
        virtual bool CanInstantiate() {return true;};
        /**
        * Fetches placeholder Mesh Name, to visualize the object during a 3D Drag'n'Drop
        * @return name of placeholder
        */
        virtual Ogre::String GetPlaceHolderName() {return "";};
        /**
        * Adds a new property definition
        * @param name Name of the property to be added
        * @param displayname Name of the property that will be displayed in properties view
        * @param desc Description of the property to be added
        * @param pType Type ID of the property
        * @param read can this property's value be read? (if not, it wont be displayed in properties view)
        * @param write can this property's value be changed? (if not, it becomes read-only in properties view)
        * @param trackchanges should this property be tracked by its set for value changes?
        * @return Returns a pointer to the new property definition
        */
        OgitorsPropertyDef *AddPropertyDefinition(const Ogre::String& name, const Ogre::String& displayname, const Ogre::String& desc, OgitorsPropertyType pType, bool read = true, bool write = true, bool trackchanges = true);
        /**
        * Retrieves an existing property definition
        * @param name Name of the property to be retrieved
        * @return Returns a pointer to the property definition
        */
        OgitorsPropertyDef *GetPropertyDefinition(const Ogre::String& name);
        /**
        * Creates a unique object name
        * @param name head of a string
        * @return a unique object name
        */
        Ogre::String CreateUniqueID(const Ogre::String& name);
        /**
        * This function supplies a widget that displays object specific tools
        * @param window Handle to any custom tools widget that objects created by this factory want to show
        */
        void setCustomToolsWindow(void *window) { mToolsWindow = window; };
        /**
        * This function supplies a widget which extends the current properties view
        * @param window Handle to any custom "property widget" that objects created by this factory want to show
        */
        void setPropertyEditorToolWindow(void * window) { mPropertyEditorToolWindow = window; };
    };
}
