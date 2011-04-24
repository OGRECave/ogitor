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

#ifndef OGITORS_ROOT_H
#define OGITORS_ROOT_H

#include "OgitorsSingleton.h"

namespace OFS
{
    class OfsPtr;
};

namespace Ogitors
{
    class Selection2D;

    class OgitorsScriptConsole;
    class OgitorsScriptInterpreter;

    /** Dock Widget Parent Type: Determines where the widget will be inserted */
    enum DockingParentType
    {
        DOCK_MAIN = 0,           /** Add as a docking widget to main window */
        DOCK_PROPERTIES = 1,     /** Add as a ToolBox Item to Properties View */
        DOCK_RESOURCES = 2       /** Add as a ToolBox Item to Resources View */
    };

    struct DockWidgetData
    {
        Ogre::String       mCaption;
        void              *mHandle;
        Ogre::String       mIcon;
        DockingParentType  mParent;
    };

    struct TabWidgetData
    {
        Ogre::String  mCaption;
        void         *mHandle;
    };

    /*!  
    A structure that is responsible for registering a widget ("preference editor")
    that will later be used in the preferences dialog of Ogitor 
    */
    class PreferencesEditor;
    struct PreferencesEditorData
    {
        Ogre::String        mCaption;
        Ogre::String        mIcon;
        Ogre::String        mSectionName;
        PreferencesEditor  *mHandle;
    };

    /*!  
    A class that is responsible for handling plugin values exposed to the preferences editor
    */
    class PreferencesEditor
    {
    public:
        /**
        * Fetches preferences list
        * @param preferences place to fill preference values into
        */
        virtual void            getPreferences(Ogre::NameValuePairList& preferences) = 0;
        /**
        * Fetches a QWidget pointer to put preferences into (internal use only)
        * @return A QWidget type pointer
        * @see QWidget
        */
        virtual void            *getPreferencesWidget() = 0;
        /**
        * Applies preferences to the Ogitor
        * @return true if preferences were applied successfully, otherwise false
        */
        virtual bool            applyPreferences() = 0;
        /**
        * Sets the preferences section name associated with the preferences editor
        */
        virtual void            setPrefsSectionName(Ogre::String prefsSectionName) {mPrefsSectionName = prefsSectionName;};
        /**
        * Returns the section name associated with that preferences editor
        * @return preferences section name
        */
        virtual Ogre::String    getPrefsSectionName() {return mPrefsSectionName;};

    protected:
        Ogre::String mPrefsSectionName;
    };

    // A Hashed by Name Storage Definition for All Serializers
    typedef OgitorExport HashMap<Ogre::String,CBaseSerializer*> SerializerMap;

    typedef HashMap<Ogre::String, OgitorsScriptInterpreter*> ScriptInterpreterMap;

    typedef Ogre::map<unsigned int, CBaseEditor*>::type IDObjectPairList;
    
    typedef Ogre::vector<DockWidgetData>::type DockWidgetDataList;
   
    typedef Ogre::vector<TabWidgetData>::type TabWidgetDataList;

    typedef OgitorExport Ogre::vector<PreferencesEditorData>::type PreferencesEditorDataList;

    //! This is the Root Class in Charge of All Editors
    /*!  

    */
    class OgitorExport OgitorsRoot : public Singleton<OgitorsRoot>, public Ogre::GeneralAllocatedObject
    {
        friend class CBaseEditor;
    public:
        /**
        * Constructor, initializes with platform dependent system classes
        */ 
        OgitorsRoot(Ogre::StringVector* pDisabledPluginPaths);
        /**
        * Destructor
        */
        ~OgitorsRoot();
        inline bool                IsSceneLoaded() { return (mLoadState == LS_LOADED); }
        inline LoadState           GetLoadState() { return mLoadState; }
        void                       SetRunState(RunState state);
        inline RunState            GetRunState() { return mRunState; }
        void                       SetEditorTool(unsigned int tool);
        inline unsigned int        GetEditorTool() { return mEditorTool; }
        /**
        * Registers a new editor object factory
        * @param pluginIdentifier the id given to the plugin when dllStartPlugin is called
        * @param factory pointer to the new factory to be registered
        */
        void            RegisterEditorFactory(void *pluginIdentifier, CBaseEditorFactory *factory);
        /**
        * Registers a new serializer
        * @param serializer additional serializer
        */
        void            RegisterSerializer(void *pluginIdentifier, CBaseSerializer *serializer);
        /**
        * Registers a new script interpreter
        * @param pluginIdentifier the id given to the plugin when dllStartPlugin is called
        * @param interpreter pointer to the new interpreter to be registered
        */
        void            RegisterScriptInterpreter(void *pluginIdentifier, OgitorsScriptInterpreter *interpreter);
        /**
        * Registers the object instance and supplies it a unique object identifier
        * @param object to register
        */
        inline unsigned int GetUniqueObjectID(CBaseEditor *object)
        {
            unsigned int ID = (Ogre::Math::UnitRandom() * 1000000000.0f) + 1;
            while(mIDList.find(ID) != mIDList.end())
                ID = (Ogre::Math::UnitRandom() * 1000000000.0f) + 1;

            mIDList.insert(IDObjectPairList::value_type(ID, object));
            return ID;
        }
        /**
        * UnRegisters the object instance associated with unique object identifier
        * @param id the unique identifier of object
        */
        inline void     RemoveObjectID(unsigned int id)
        {
            if(id > 0)
            {
                IDObjectPairList::iterator it = mIDList.find(id);
                
                assert(it != mIDList.end());

                if(it != mIDList.end())
                    mIDList.erase(it);
            }
        }
        /**
        * Registers the object instance associated with unique object identifier
        * @param id the unique identifier of object
        * @param object the object to be registered for the id
        */
        inline void     AddObjectID(unsigned int id, CBaseEditor *object)
        {
            assert(mIDList[id] == 0);
            mIDList[id] = object;
        }
        /**
        * @return the global listener
        */
        OgitorsPropertySetListener* GetGlobalPropertyListener();
        /**
        * Fetches a list filled with registered serializer type names
        * @param list a list to place serializer type names into
        */
        void            GetSerializerList(Ogre::StringVector &list);
        /**
        * Fetches a list filled with registered serializer type names capable of export
        * @param list a list to place serializer type names into
        */
        void            GetExportSerializerList(Ogre::StringVector &list);
        /**
        * Returns a list filled with registered serializer type names capable of import
        * @param list a list to place serializer type names into
        */
        void            GetImportSerializerList(Ogre::StringVector &list);
        /**
        * Triggers an import serializer
        * @param name name of the serializer
        */
        bool            TriggerImportSerializer(Ogre::String name);
        /**
        * Triggers an export serializer
        * @param name name of the serializer
        */
        bool            TriggerExportSerializer(Ogre::String name);
        /**
        * Fetches registration data for an editor object type
        * @param typeName type of an object
        * @return the factory with associated the typename, otherwise 0
        */
        CBaseEditorFactory *GetEditorObjectFactory(const Ogre::String& typeName);
        /**
        * Returns a list of all Registered Editor Object Factories
        */
        EditorObjectFactoryMap GetEditorObjectFactories() {return mEditorObjectFactories;};
        /**
        * Fetches type ID of a factory associated with the type name
        * @param typeName type name of editor object factory
        * @return type ID if found, otherwise 0
        */
        unsigned int    GetTypeID(const Ogre::String& typeName);
        /**
        * Registers an object name for the editor
        * @param name object name
        * @param obj editor associated with an object
        */
        void            RegisterObjectName(Ogre::String name,CBaseEditor *obj);
        /**
        * Unregisters an object (called upon deletion of object)
        * @param name object name
        * @param obj editor associated with an object
        */
        void            UnRegisterObjectName(Ogre::String name,CBaseEditor *obj);
        /**
        * A delegate function between Terrain and Hydrax that is used when ETL material changes
        * @param terrainobject terrain editor for which material has been changed
        */
        void            OnTerrainMaterialChange(CBaseEditor *terrainobject);
        /**
        * Registers object for updates if it needs to update every frame
        * @param object editor object
        */
        void            RegisterForUpdates(CBaseEditor *object);
        /**
        * Unregisters object from receiving updates
        * @param object editor object
        */
        void            UnRegisterForUpdates(CBaseEditor *object);
        /**
        * Registers object for script updates if it has an update script
        * @param object editor object
        */
        void            RegisterForScriptUpdates(CBaseEditor *object);
        /**
        * Unregisters object from receiving script updates
        * @param object editor object
        */
        void            UnRegisterForScriptUpdates(CBaseEditor *object);
        /**
        * This function should be called every frame in Frame Started
        * @param
        * @return
        */
        bool            Update(float timePassed);
        /**
        * An object registers itself for updates if it needs to update when scene is loaded
        * @param object an object to receive post-update event
        */
        void            RegisterForPostSceneUpdates(CBaseEditor *object);
        /**
        * Locates child by a name or ID
        * @param name name of an object to locate
        * @param type type of object to locate
        * @return an object pointer if found, otherwise 0
        */
        CBaseEditor    *FindObject(Ogre::String name, unsigned int type = 0);
        /**
        * Locates child by its Object ID
        * @param id ID of an object to locate
        * @return an object pointer if found, otherwise 0
        */
        inline CBaseEditor *FindObject(unsigned int id) { return mIDList[id]; }
        /**
        * Fetches a list key-value pairs of objects with specified type
        * @param type type of an Object
        * @return a list of key-value pairs of objects that have specified type
        */
        const NameObjectPairList GetObjectsByType(unsigned int type);
        /**
        * Fetches a list key-value pairs of objects with specified type name
        * @param typeName type name of objects to be fetched
        * @return a list of key-value pairs of objects that have specified type name
        */
        const NameObjectPairList GetObjectsByTypeName(const Ogre::String& typeName);
        /**
        * Fetches all objects of a specified type (Used while saving scene since it also orders the items)
        * @param type type of an object
        * @param list a list to place objects with specified type into
        */
        void            GetObjectList(unsigned int type, ObjectVector& list);
        /**
        * Fetches all objects of a specified type (Used while saving scene since it also orders the items)
        * @param typeName typename of an object
        * @param list a list to place objects with specified type into
        */
        void            GetObjectList(const Ogre::String& typeName, ObjectVector& list);
        /**
        * Fetches all objects of a specified type (Used while saving scene since it also orders the items)
        * @param type type of an object
        * @param nameregexp RegExp for name
        * @param inverse if set to TRUE, it includes the objects that doesnt match
        * @param list a list to place objects with specified type into
        */
        void            GetObjectListByName(unsigned int type, const Ogre::String& nameregexp, bool inverse, ObjectVector& list);
        /**
        * Fetches all objects of a specified type (Used while saving scene since it also orders the items)
        * @param type type of an object
        * @param nameregexp RegExp for name
        * @param inverse if set to TRUE, it includes the objects that doesnt match
        * @param list a list to place objects with specified type into
        */
        void            GetObjectListByProperty(unsigned int type, const Ogre::String& nameregexp, bool inverse, ObjectVector& list);
        /**
        * Fetches all objects of a specified type (Used while saving scene since it also orders the items)
        * @param type type of an object
        * @param nameregexp RegExp for name
        * @param inverse if set to TRUE, it includes the objects that doesnt match
        * @param list a list to place objects with specified type into
        */
        void            GetObjectListByCustomProperty(unsigned int type, const Ogre::String& nameregexp, bool inverse, ObjectVector& list);
        /**
        * Refines a given list by RegExp Matching Names
        * @param nameregexp RegExp for name
        * @param inverse if set to TRUE, it includes the objects that doesnt match
        * @param list_in a list to search in
        * @param list_out a list to place objects with specified type into
        */
        void            RegExpByName(const Ogre::String& nameregexp, bool inverse, const ObjectVector& list_in, ObjectVector& list_out);
        /**
        * Refines a given list by RegExp Matching Property Names
        * @param nameregexp RegExp for name
        * @param inverse if set to TRUE, it includes the objects that doesnt match
        * @param list_in a list to search in
        * @param list_out a list to place objects with specified type into
        */
        void            RegExpByProperty(const Ogre::String& nameregexp, bool inverse, const ObjectVector& list_in, ObjectVector& list_out);
        /**
        * Refines a given list by RegExp Matching Custom Property Names
        * @param nameregexp RegExp for name
        * @param inverse if set to TRUE, it includes the objects that doesnt match
        * @param list_in a list to search in
        * @param list_out a list to place objects with specified type into
        */
        void            RegExpByCustomProperty(const Ogre::String& nameregexp, bool inverse, const ObjectVector& list_in, ObjectVector& list_out);
        /**
        * Fetches top level editor object
        * @return root editor
        */
        CBaseEditor    *GetRootEditor() {return mRootEditor;};
        /**
        * Clears project options
        */
        void            ClearProjectOptions();
        /**
        * Fetches project options
        * @return a pointer to project options
        */
        PROJECTOPTIONS *GetProjectOptions() {return &mProjectOptions;}
        /**
        * Fetches project file system
        * @return project file system
        */
        OFS::OfsPtr& GetProjectFile() {return *mProjectFile;}
        /**
        * Loads a scene from specified file
        * @param Filename file name for the scene to be loaded
        * @return result of loading (or attempting) a scene
        * @see SCENEFILERESULT
        */
        int             LoadScene(Ogre::String Filename);
        /**
        * Saves current scene into a file
        * @param SaveAs flag to save scene at different location as specified by user in SaveAs dialog 
        * @return true if scene was saved successfully, otherwise false 
        * @see SCENEFILERESULT for return results
        */
        bool            SaveScene(bool SaveAs = false, Ogre::String exportfile = "");
        /**
        * Terminates current scene
        * @return true if scene was terminated (and saved after being modified) or not modified at all, otherwise false
        */
        bool            TerminateScene();
        /**
        * Peforms necessary actions after scene was loaded successfully
        * @return unconditional true
        * @todo See if return type should be changed to void
        */
        bool            AfterLoadScene();
        /**
        * Loads project options from an XML element
        * @param optRoot XML element to load project options from
        * @return true if options were loaded successfully, otherwise false
        */
        bool            LoadProjectOptions(TiXmlElement *optRoot);
        /**
        * Loads camera positions from an XML element
        * @param parent top XML element containing camera position(s) XML elements
        * @return true if loaded successfully, otherwise false
        */
        bool            OptionsReadCameraPositions(TiXmlElement *parent);
        /**
        * Loads directory list from an XML element
        * @param parent top XML element containing directories XML elements 
        * @param pDir string list to place directories into
        * @return true if options were read successfully, otherwise false
        */
        bool            OptionsReadDirectories(TiXmlElement *parent, Ogre::StringVector &pDir);
        /**
        * Loads layer list from an XML element
        * @param parent top XML element containing layers XML elements 
        * @return true if options were read successfully, otherwise false
        */
        bool            OptionsReadLayers(TiXmlElement *parent);
        /**
        * Prepares project resource group(s)
        */
        void            PrepareProjectResources();
        /**
        * Refreshes materials of the objects in scene using the given material
        */
        void            UpdateMaterialInScene(Ogre::String materialName);
        /**
        * Refreshes all materials of the objects in scene
        */
        void            UpdateMaterialsInScene();
        /**
        * Reloads project resources
        */
        void            ReloadUserResources();
        /**
        * Converts relative project resource paths according to new project directory
        * @param oldpath previous path to project' scene file
        */
        void            AdjustUserResourceDirectories(Ogre::String oldpath);
        /**
        * Creates an editor object using registered factory
        * @param parent parent editor object 
        * @param objecttypestring type of an object to be created
        * @param params additional list of parameters
        * @param addtotreelist to insert new object into scene if true, not otherwise
        * @return newly created object
        */
        CBaseEditor    *CreateEditorObject(CBaseEditor *parent, const Ogre::String objecttypestring, OgitorsPropertyValueMap &params,bool addtotreelist = false, bool display = false);
        /**
        * Clones an editor object using registered factory
        * @param object object to be cloned
        * @param addtotreelist to insert new object into scene tree view widget if true, not otherwise
        * @param display display newly cloned object in the scene
        * @return newly cloned object
        */
        CBaseEditor    *CloneEditorObject(CBaseEditor *object,bool addtotreelist = false, bool display = false);
        /**
        * Clones an editor object using registered factory, makes the object an instance of the original
        * The clone will be affected by the property changes of the original
        * @param object object to be cloned
        * @param addtotreelist to insert new object into scene tree view widget if true, not otherwise
        * @param display display newly cloned object in the scene
        * @return newly cloned object
        */
        CBaseEditor    *InstanceCloneEditorObject(CBaseEditor *object,bool addtotreelist = false, bool display = false);
        /**
        * Destroys an editor object using registered factory
        * @param object object to be destroyed
        * @param removefromtreelist to remove new object from the scene tree view widget if true, not otherwise
        * @param display display destroyed object in the scene
        * @return newly cloned object
        */
        void            DestroyEditorObject(CBaseEditor *object, bool removefromtreelist = false, bool display = false);
        /**
        * Tests if certain object can be instantiated (created)
        * @param typeName type of an object
        * @return true if object can be instantiated, otherwise false
        */
        bool            CanInstantiateObject(const Ogre::String& typeName);

        // UTILITY FUNCTIONS
        /**
        * Creates a unique identifier string
        * @param n1 head of a string
        * @param n2 tail of the string
        * @param seed additional randomization seed
        * @return string with unique object identifier
        */
        Ogre::String    CreateUniqueID(Ogre::String n1, Ogre::String n2, int seed = -1);
        /**
        * Writes camera position(s) into specified output stream
        * @param outstream output stream to write camera position(s) into
        * @param addcurrent if true, writes down current viewport' camera position(s)
        */
        void            WriteCameraPositions(std::ostream &outstream, bool addcurrent);
        /**
        * Writes project options into specified output stream
        * @param outstream output stream to write project options into
        * @param newproject used for sub-call to COgitorsRoot::WriteCameraPositions
        */
        void            WriteProjectOptions(std::ostream &outstream, bool newproject);
        /**
        * Attempts to pick X, Y or Z axis gizmo(s) presented when object is moved, scaled etc
        * @param ray ray cast from the mouse  
        * @see COgitorsRoot::GetMouseRay
        * @param Axis axis of the selected gizmo
        * @return true if intersected with any of gizmo(s), otherwise false
        */
        bool          PickGizmos(Ogre::Ray &ray, int &Axis);
        /**
        * Calculates gizmo translation plane on the X, Y or Z axis
        * @param pickRay ray to pick gizmo with
        * @param TranslationAxis axis flags to use when finding the plane
        * @return a plane on which all axis reside on as specified in 2nd parameter  
        * @see AXISTYPE
        */
        Ogre::Plane   FindGizmoTranslationPlane(Ogre::Ray &pickRay, int TranslationAxis);
        /**
        * Calculates the new position delta of the object according to mouse position and desired axis
        * @param pickRay a ray from camera position to mouse position in 3D space
        * @param planetouse a plane to be used for intersection tests with the pickRay
        * @param TranslationAxis the axis on which movement will be calculated
        * @param vLastPosition the last position from which the delta will be calculated
        * @return returns the delta from vLastPosition, will be calculated according to pickRay, Plane and Axis
        */
        Ogre::Vector3 GetGizmoIntersect(Ogre::Ray &pickRay, Ogre::Plane &planetouse, int TranslationAxis, Ogre::Vector3 &vLastPosition);
        /**
        * Calculates the new position delta of the object according to mouse position
        * @param object the object whose new position is being calculated
        * @param pickRay a ray from camera position to mouse position in 3D space
        * @return returns the new position of the object, calculated from the pickRay's intersection with camera's back plane
        */
        Ogre::Vector3 GetGizmoIntersectCameraPlane(CBaseEditor *object, Ogre::Ray &pickRay);
        /**
        * Calculates the new position according to mouse position and current position using camera plane
        * @param pos the reference position which new position is calculated from
        * @param orient the orientation of the position
        * @param pickRay a ray from camera position to mouse position in 3D space
        * @return returns the new position, calculated from the pickRay's intersection with camera's back plane
        */
        Ogre::Vector3 GetGizmoIntersectCameraPlane(Ogre::Vector3& pos, Ogre::Quaternion& orient, Ogre::Ray &pickRay);
        /**
        * Selects objects names that are within volume
        * @param cam handle to camera
        * @param left top left ray boundary
        * @param right top right ray boundary
        * @param top top left ray boundary
        * @param bottom bottom right ray boundary
        * @param result names of objects within volume
        */
        void          VolumeSelect(Ogre::Camera *cam, Ogre::Real left, Ogre::Real right, Ogre::Real top, Ogre::Real bottom, NameObjectPairList &result);
        /**
        * Fills resource group with resource name & path from specified location
        * @param mngr group resource manager that will load resource(s)
        * @param list list of resource names to add to the group
        * @param path directory in which resources are located
        * @param group name of the group to add resource names to
        */
        void          FillResourceGroup(Ogre::ResourceGroupManager *mngr,Ogre::StringVector &list,Ogre::String path,Ogre::String group);

        /**
        * Fetches if WorldSpaceGizmoOrientation usage state
        * @return WorldSpaceGizmoOrientation state
        */
        inline bool GetWorldSpaceGizmoOrientation() { return mWorldSpaceGizmoOrientation; };
        /**
        * Sets WorldSpaceGizmoOrientation state
        * @param state new WorldSpaceGizmoOrientation state
        */
        inline void SetWorldSpaceGizmoOrientation(bool state) 
        {
            mWorldSpaceGizmoOrientation = state;
        };
        /**
        * Fetches render window handle
        * @return render window handle
        */
        inline Ogre::RenderWindow *     GetRenderWindow() {return mRenderWindow;};
        /**
        * Sets current render window handle
        * @param window new rendering window handle
        */
        inline void                     SetRenderWindow(Ogre::RenderWindow *window) {mRenderWindow = window;};
        /**
        * Fetches currently active viewport
        * @return currently active viewport
        */
        inline CViewportEditor *        GetViewport() {return mActiveViewport;};
        /**
        * Sets current active viewport
        * @param viewport currently active viewport
        */
        inline void                     SetActiveViewport(CViewportEditor *viewport) {mActiveViewport = viewport;};
        /**
        * Fetches current scene manager
        * @return current scene manager; if not found, returns the first Scene Manager in the list
        */
        inline Ogre::SceneManager *     GetSceneManager()
        {
            if(!mSceneManager)
                mSceneManager = GetFirstSceneManager();
            return mSceneManager;
        };
        /**
        * Fetches current scene manager editor
        * @return current scene manager editor; if not found, returns the first Scene Manager in the list
        */
        inline CSceneManagerEditor * GetSceneManagerEditor()
        {
            if(!mSceneManagerEditor)
                mSceneManagerEditor = FindObject(GetFirstSceneManager()->getName(), ETYPE_SCENEMANAGER);
            return (CSceneManagerEditor*)mSceneManagerEditor;
        };
        /**
        * Fetches a list of names for objects that are of node type
        * @param list list to place object names
        */
        void                                GetAutoTrackTargets(Ogre::StringVector &list);
        /**
        * Fetches terrain editor
        * @return terrain editor
        */
        inline ITerrainEditor *             GetTerrainEditor() {return mTerrainEditor;};
        /**
        * Fetches terrain editor object (actual terrain)
        * @return terrain editor object (actual terrain)
        */
        inline CBaseEditor *                GetTerrainEditorObject() {return mTerrainEditorObject;};
        /**
        * Fetches paging editor
        * @return paging editor
        */
        inline IPagingEditor *              GetPagingEditor() {return mPagingEditor;};
        /**
        * Fetches paging editor object
        * @return paging editor object
        */
        inline CBaseEditor *                GetPagingEditorObject() {return mPagingEditorObject;};
        /**
        * Fetches first scene manager
        * @return first scene manager
        */
        Ogre::SceneManager *                GetFirstSceneManager();
        /**
        * Tests if the scene is modified
        * @return true if scene is modified, otherwise false
        */
        inline bool                         IsSceneModified() {return mIsSceneModified;};
        /**
        * Sets modified flag when scene has been modified
        * @param modified true if scene was modified, otherwise false
        */
        void                                SetSceneModified(bool modified);
        /**
        * Sets modified flag only if the value is true (Additive)
        * @param state new state
        */
        void                                ChangeSceneModified(bool state);
        /**
        * Hides the selection rectangle (destroys it)
        */
        void                                HideSelectionRect();
        /**
        * Creates and shows selection rectangle
        * @param size rectangle' boundaries (x - left, y - top, z - right, w - bottom)
        */
        void                                ShowSelectionRect(Ogre::Vector4 size);
        /**
        * Highlights gizmo(s)
        * @param ID gizmo axis(i) flag(s) to highlight 
        * @see AXISTYPE
        */
        void                                HighlightGizmo(int ID);
        /**
        * Sets new gizmo mode 
        * @param mode new gizmo mode flag(s) 
        * @see EDITORTOOLS
        */
        void                                SetGizmoMode(int mode);
        /**
        * Fetches light visibility
        * @return true if light(s) are visible, otherwise false
        */
        inline bool                         GetLightVisiblity() {return mGlobalLightVisiblity;};
        /**
        * Sets light visibility
        * @param visibility new light visibility
        */
        void                                SetLightVisiblity(bool visibility); 
        /**
        * Fetches camera visibility state
        * @return true if camera(s) is visible, otherwise false
        */
        inline bool                         GetCameraVisiblity() {return mGlobalCameraVisiblity;};
        /**
        * Sets camera visibility
        * @param visibility new camera visibility
        */
        void                                SetCameraVisiblity(bool visibility);
        /**
        * Fetches multi-selection editor handle
        * @return multi-selection editor handle
        */
        CMultiSelEditor *                   GetSelection();
        /**
        * Fetches active script interpreter handle
        * @return active script interpreter handle
        */
        OgitorsScriptInterpreter *          GetScriptInterpreter() { return mScriptInterpreter; };
        /**
        * Fetches a named script interpreter handle
        * @param typeName the type name of the interpreter
        * @return a named script interpreter handle
        */
        OgitorsScriptInterpreter *          GetScriptInterpreter(const Ogre::String& typeName) { return mScriptInterpreterList[typeName]; };
        /**
        * Recursively fills a treeview with object names and icons
        * @param pEditor an editor from which to retrieve objects
        */
        void RecurseFillTreeView(CBaseEditor *pEditor);
        /**
        * Renders viewport windows after they were resized
        */
        void                                RenderWindowResized();
        /**
        * Sets background clearing flag
        * @param bClear clearing flag
        */
        inline void                         ClearScreenBackground(bool bClear) {mClearScreenBackground = bClear;};
        /**
        * Fetches background clearing flag
        * @return background clearing flag
        */
        inline bool                         IsClearScreenNeeded() {return mClearScreenBackground;};
        /**
        * Registers a new drag&drop handler for render window
        * @param source pointer to the drag&drop source to be handled
        * @param handler pointer to the handler class for the drag&drop events originating from source
        */
        void                                RegisterDragDropHandler(void *source, DragDropHandler *handler);
        /**
        * UnRegisters a drag&drop handler for render window
        * @param source pointer to the drag&drop source to be handled
        * @param handler pointer to the handler class for the drag&drop events originating from source
        */
        void                                UnRegisterDragDropHandler(void *source, DragDropHandler *handler);
        /**
        * Registers QWidget-type toolbar
        * @param toolbar QWidget-type toolbar
        */
        void                                RegisterToolBar(void *pluginIdentifier, void *toolbar);
        /**
        * Fetches a list of toolbars
        * @return list of toolbar handles
        */
        std::vector<void*>                  GetToolBars() {return mToolBars;};
        /**
        * Registers QDockWidget-type dockwidget
        * @param dockwidget QDockWidget-type dockwidget
        */
        void                                RegisterDockWidget(void *pluginIdentifier, const DockWidgetData& data);
        /**
        * Registers QWidget-type Tab Window
        * @param widget QWidget-type Tab Window
        */
        void                                RegisterTabWidget(void *pluginIdentifier, const TabWidgetData& data);
        /**
        * Registers new preferences for the editor
        * @param QWidget-type data
        */
        void                                RegisterPreferenceEditor(void *pluginIdentifier, PreferencesEditorData& data);
        /**
        * Fetches a list of dockwidgets
        * @return list of dockwidget info
        */
        DockWidgetDataList                  GetDockWidgets() {return mDockWidgets;};
        /**
        * Fetches a list of tabwidgets
        * @return list of tabwidget info
        */
        TabWidgetDataList                   GetTabWidgets() {return mTabWidgets;};
        /**
        * Fetches a list of editor preferences
        * @return list of editor preferences
        */
        PreferencesEditorDataList           GetPreferencesEditorList() {return mPrefEditors;};
        /**
        * Initializes recent files list with given entries
        * @param list a vector to be used as the current recent files list
        */
        void                                InitRecentFiles(UTFStringVector& list);
        /**
        * Adds an entry to recent files list, moving it to top
        * @param entry the string to be added to the list
        */
        void                                AddToRecentFiles(const Ogre::UTFString& entry);
        /**
        * Fetches the list of recent files, the order is: The last used is first in the list
        * @param list a vector to be flled with ordered recent files list
        */
        void                                GetRecentFiles(UTFStringVector& list);
        /**
        * Redirects all mouse events to the registered listener class
        * @param listener class to redirect mouse events to
        */
        inline void                         CaptureMouse(MouseListener *listener) { mMouseListener = listener; };
        /**
        * Restores redirection of mouse events to the viewports
        */
        inline void                         ReleaseMouse() { mMouseListener = 0; };
        /**
        * Redirects all keyboard events to the registered listener class
        * @param listener class to redirect keyboard events to
        */
        inline void                         CaptureKeyboard(KeyboardListener *listener) { mKeyboardListener = listener; };

        /**
        * Restores redirection of keyboard events to the viewports
        */
        inline void                         ReleaseKeyboard() { mKeyboardListener = 0; };

        /**
        * Fetches names of models in user assets folders
        * @return names of models in user assets folders
        */
        static PropertyOptionsVector        *GetModelNames() {return &mModelNames;};

        /**
        * Fetches names of materials in user assets folders
        * @return names of materials in user assets folders
        */
        static PropertyOptionsVector        *GetMaterialNames() {return &mMaterialNames;};
 
        /**
        * Fetches mapping of models to their default materials
        * @return mapping of models to their default materials
        */
        static Ogre::NameValuePairList      *GetModelMaterialMap() {return &mModelMaterialMap;}

        /**
        * Fetches a list of properties containing skybox materials
        * @return a list of properties containing skybox material
        */
        static PropertyOptionsVector        *GetSkyboxMaterials() { return &mSkyboxMaterials; }

        /**
        * Fetches a list of properties containing particle templates
        * @return a list of properties containing particle templates
        */
        static PropertyOptionsVector        *GetParticleTemplateNames() { return &mParticleTemplateNames; }

        /**
        * Fetches names of auto track targets in the scene
        * @return names of auto track targets in the scene
        */
        static PropertyOptionsVector        *GetAutoTrackTargets() {return &mAutoTrackTargets;};

        /**
        * Fetches names of layers in the scene
        * @return names of layers in the scene
        */
        static PropertyOptionsVector        *GetLayerNames() {return &mLayerNames;};

        /**
        * Fetches diffuse texture name(s) property(ies)
        * @return diffuse texture name(s) property(ies)
        */
        static PropertyOptionsVector        *GetTerrainDiffuseTextureNames() { return &mTerrainDiffuseTextureNames; }

        /**
        * Fetches normal texture name(s) property(ies)
        * @return normal texture name(s) property(ies)
        */
        static PropertyOptionsVector        *GetTerrainNormalTextureNames() { return &mTerrainNormalTextureNames; }

        /**
        * Fetches plant material name(s) property(ies)
        * @return plant material name(s) property(ies)
        */
        static PropertyOptionsVector        *GetTerrainPlantMaterialNames() { return &mTerrainPlantMaterialNames; }

        /**
        * Fetches names of scripts in user project folders
        * @return names of script in user project folders
        */
        static PropertyOptionsVector        *GetScriptNames() {return &mScriptNames;};

        /**
        * Sets Gizmo scaling factor
        * @param names of auto track targets in the scene
        */
        void                                SetGizmoScale(Ogre::Real value);

        /**
        * Fetches gizmo scaling factor
        * @return gizmo's current scaling factor
        */
        Ogre::Real                          GetGizmoScale() {return mGizmoScale;};

        void                                ToggleLayerVisibility(int index);

        void                                SetWalkAroundMode(bool walk) { mWalkAroundMode = walk; };
        bool                                GetWalkAroundMode() { return mWalkAroundMode; };

        const PluginEntryMap*               GetPluginMap() {return &mPlugins; };

        /**
        * Unloads one specific editor plugin
        */
        void                                UnLoadPlugin(void *identifier);

        void                                DestroyResourceGroup(const Ogre::String& resGrpName);

        ///EVENTS
        /**
        * Delegate function that is called when material has been dragged onto a scene (viewport)
        * @param x x mouse cursor coordinate
        * @param y y mouse cursor coordinate
        * @return true if material has been assigned to an entity, otherwise false
        */
        bool OnDragEnter (void * source);
        /**
        * Delegate function that is called when object is being dragged around the scene (after being dragged on)
        * @param source pointer to the source of event
        * @param modifier keyboard modifier
        * @param x x mouse cursor coordinate
        * @param y y mouse cursor coordinate
        * @return return true if the move event is accepted
        */
        bool OnDragMove (void *source, unsigned int modifier, int x, int y);
        /**
        * Delegate function that is called when dragging has concluded
        * @param source pointer to the source of event
        */
        void OnDragLeave ();
        /**
        * Delegate function that is called when object has been dropped onto a scene
        * @param source pointer to the source of event
        * @param x x mouse cursor coordinate
        * @param y y mouse cursor coordinate
        */
        void OnDragDropped (void *source, int x, int y);
        /** 
        * Delegate function that is called when keyboard key is held down
        * @param key code of the keyboard key that is held down
        */
        void OnKeyDown (unsigned int key);
        /** 
        * Delegate function that is called when keyboard key has been released
        * @param key keyboard key code that has been released
        */
        void OnKeyUp (unsigned int key);
        /** 
        * Delegate function that is called when mouse cursor is moved
        * @param point mouse cursor coordinate
        * @param buttons mouse button(s) flag
        */
        void OnMouseMove (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when mouse cursor has left certain area
        * @param point mouse cursor coordinate
        * @param buttons mouse button(s) flag
        */
        void OnMouseLeave (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when left mouse button is held down
        * @param point mouse cursor coordinate at the time when left mouse button had been triggered
        * @param buttons mouse button(s) flag
        */
        void OnMouseLeftDown (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when left mouse button is released
        * @param point mouse cursor coordinate at the time when left mouse button had been triggered
        * @param buttons mouse button(s) flag
        */
        void OnMouseLeftUp (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when right mouse button is held down
        * @param point mouse cursor coordinate at the time when right mouse button had been triggered
        * @param buttons mouse button(s) flag
        */
        void OnMouseRightDown (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when right mouse button is released
        * @param point mouse cursor coordinate at the time when right mouse button had been triggered
        * @param buttons mouse button(s) flag
        */
        void OnMouseRightUp (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when middle mouse button is held down
        * @param point mouse cursor coordinate at the time when middle mouse button had been triggered
        * @param buttons mouse button(s) flag
        */
        void OnMouseMiddleDown (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when middle mouse button is released
        * @param point mouse cursor coordinate at the time when middle mouse button had been triggered
        * @param buttons mouse button(s) flag
        */
        void OnMouseMiddleUp (Ogre::Vector2 point, unsigned int buttons);
        /** 
        * Delegate function that is called when mouse wheel is used
        * @param point mouse cursor coordinate at the time when mouse wheel is used
        * @param delta an amount of how much mouse wheel has been moved
        * @param buttons mouse button(s) flag
        */
        void OnMouseWheel (Ogre::Vector2 point, float delta, unsigned int buttons);

    private:
        OFS::OfsPtr *                mProjectFile;
        LoadState                    mLoadState;
        RunState                     mRunState;
        unsigned int                 mEditorTool;

        OgitorsUndoManager *mUndoManager;                               /** Undo manager handle */
        OgitorsClipboardManager *mClipboardManager;                     /** Clipboard manager handle */
        OgitorsSystem      *mSystem;                                    /** The platform-dependent system handle */
        OgitorsPhysics     *mPhysics;                                   /** The platform-dependent physics handle */
        OgitorsScriptConsole *mScriptConsole;                           /** Script Console handle */
        OgitorsScriptInterpreter *mScriptInterpreter;                   /** Script Interpreter handle */
        PROJECTOPTIONS      mProjectOptions;                            /** A handle to hold all project options for the current scene */
        Ogre::SceneManager *mSceneManager;                              /** Current scene manager handle */
        CBaseEditor        *mSceneManagerEditor;                        /** Current scene manager editor handle */
        Ogre::RenderWindow *mRenderWindow;                              /** Current render window handle */
        CViewportEditor    *mActiveViewport;                            /** The Current Viewport Pointer */
        CBaseEditor        *mRootEditor;                                /** Handle of the root editor (Actual root of all editors in a scene) */
        CMultiSelEditor    *mMultiSelection;                            /** Multi-selection editor handle */
        NameObjectPairList  mNameList;                                  /** Hash map of all objects (sorted by name) */
        IDObjectPairList    mIDList;                                    /** Hash map of all objects (sorted by id) */
        NameObjectPairList  mNamesByType[LAST_EDITOR];                  /** Type-specific hash maps */
        NameObjectPairList  mNamesByTypeID[OGITOR_MAX_OBJECT_TYPE];     /** TypeID-specific hash maps */
        NameObjectPairList  mUpdateList;                                /** Update queue */
        NameObjectPairList  mUpdateScriptList;                          /** Update queue for scripts */
        ObjectVector        mPostSceneUpdateList;                       /** Update list that is called after scene is loaded */
        SerializerMap       mSerializerList;                            /** Hash map of all registered serializers */
        PluginEntryMap      mPlugins;                                   /** List of all loaded plugins */
        ScriptInterpreterMap mScriptInterpreterList;                    /** List of all loaded script interpreters */
        EditorObjectFactoryMap mEditorObjectFactories;                  /** List of all Editor Object Factories */
        UTFStringVector     mRecentFiles;                               /** List of recently opened files */
        Ogre::Vector3       mLastTranslationDelta;                      /** Last object translation change (used with gizmos) */
        ITerrainEditor     *mTerrainEditor;                             /** Terrain editor handle */
        IPagingEditor      *mPagingEditor;                              /** Paging editor handle */
        CBaseEditor        *mTerrainEditorObject;                       /** Terrain editor object handle (actual terrain object) */
        CBaseEditor        *mPagingEditorObject;                        /** Terrain editor object handle (actual terrain object) */
        bool                mIsSceneModified;                           /** Flag signifying if scene was modified since last save */
        bool                mGlobalLightVisiblity;                      /** Flag determining light helper visibility */
        bool                mGlobalCameraVisiblity;                     /** Flag determining camera helper visibility */
        Selection2D        *mSelRect;                                   /** Selection rectangle (used for volume object selection) */
        Ogre::SceneNode    *mSelectionNode;                             /** Scene node to hold selection rectangle */
        MouseListener      *mMouseListener;                             /** Mouse listener handle (used to receive and redirect mouse events) */
        KeyboardListener   *mKeyboardListener;                          /** keyboard listener handle (used to receive and redirect keyboard events) */

        Ogre::Real          mGizmoScale;                                /** Gizmo scaling factor */
        Ogre::SceneNode    *mGizmoNode;                                 /** Gizmo widget handle */
        Ogre::SceneNode    *mGizmoX;                                    /** X axis widget node handle */
        Ogre::SceneNode    *mGizmoY;                                    /** Y axis widget node handle */
        Ogre::SceneNode    *mGizmoZ;                                    /** Z axis widget node handle */
        Ogre::Entity       *mGizmoEntities[6];                          /** Gizmo handles */
        bool                mWorldSpaceGizmoOrientation;                /** Is the gizmo Orientation in World Space? */ 
        int                 mOldGizmoMode;                              /** Previous gizmo mode (@see EDITORTOOLS) */
        int                 mOldGizmoAxis;                              /** Previous gizmo axis along which transformation has occured */
        std::vector<int>    mObjectDisplayOrder;                        /** Scene object(s) rendering order list */
        bool                mClearScreenBackground;                     /** Flag signifying if it is time to clear screen background (used in paintEvent) */
        bool                mWalkAroundMode;                            /** Is the camera movement state WalkAround? */

        std::vector<void*>          mToolBars;                          /** Additional Qt toolbars to display in Ogitor */
        DockWidgetDataList          mDockWidgets;                       /** Additional Qt dock widgets to display in Ogitor */
        TabWidgetDataList           mTabWidgets;                        /** Additional Qt tab widgets to display in Ogitor */
        PreferencesEditorDataList   mPrefEditors;                       /** Additional Qt widgets to display in Ogitor preferences*/

        Ogre::StringVector          mDisabledPluginPaths;               /** Paths to plugins that are disabled via the preferences */
        
        DragDropHandlerMap          mDragDropHandlers;                  /** Drag Drop Handlers list */
        void                       *mActiveDragSource;                  /** Current Drag Source */

        static PropertyOptionsVector mModelNames;                       /** List of model names in the current user assets directories */
        static PropertyOptionsVector mMaterialNames;                    /** List of material names in the current user assets directories */
        static Ogre::NameValuePairList mModelMaterialMap;               /** Mapping between a model and its default material */
        static PropertyOptionsVector mAutoTrackTargets;                 /** List of auto track targets in the scene */
        static PropertyOptionsVector mSkyboxMaterials;                  /** Skybox Material property list */
        static PropertyOptionsVector mLayerNames;                       /** List of layer names in the current user project */
        static PropertyOptionsVector mParticleTemplateNames;            /** List of particle template names in the current user assets directories */
        static PropertyOptionsVector mTerrainDiffuseTextureNames;       /** Diffuse texture name(s) property(ies) */
        static PropertyOptionsVector mTerrainNormalTextureNames;        /** Normal texture name(s) property(ies) */
        static PropertyOptionsVector mTerrainPlantMaterialNames;        /** Plant Material name(s) property(ies) */
        static PropertyOptionsVector mScriptNames;                      /** Script name(s) property(ies) */

        /**
        * Cleans up editor between scene loads
        */
        void            ClearEditors();
        /**
        * Sets the plugin paths of the plugins that are disabled
        * @param vector with the plugin paths
        */
        void            SetDisabledPluginPaths(Ogre::StringVector disabledPluginPaths) 
                        {mDisabledPluginPaths = disabledPluginPaths;};
        /**
        * Loads binary plugin file
        * @param pluginPath the path to the plugin library file
        */
        void            LoadPlugin(Ogre::String pluginPath, bool noRegistration = false);
        /**
        * Unloads all editor plugins
        */
        void            UnLoadPlugins();
        /**
        * Finds and registers all editor object types
        */
        void            RegisterAllEditorObjects(Ogre::StringVector* pDisabledPluginPaths);
        /**
        * Creates gizmo objects
        */
        void            CreateGizmo();
        /**
        * Destroys gizmo objects
        * @return
        */
        void            DestroyGizmo();
        /**
        * Updates position of gizmo objects
        * @return
        */
        void            UpdateGizmo();
        /**
        * Fills the scene tree view with objects in the scene
        */
        void            FillTreeView();
        /**
        * Registers a new editor object factory
        * @param factory pointer to new editor object factory to register
        */
        void            _RegisterEditorFactory(CBaseEditorFactory *factory);
        /**
        * Setter function for LoadState
        * @param state new state
        */
        void             setLoadState(LoadState state);

    };

    /*!
        Class to handle all resource loading conflicts by unloading the existing resource
        and replacing it with the newly attempted to be loaded one
    */
    class OgitorExport ResourceLoadingListener : public Ogre::ResourceLoadingListener
    {
    public:
        bool resourceCollision(Ogre::Resource *resource, Ogre::ResourceManager *resourceManager);

        Ogre::DataStreamPtr resourceLoading(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource) {return Ogre::DataStreamPtr();};

        void resourceStreamOpened(const Ogre::String &name, const Ogre::String &group, Ogre::Resource *resource, Ogre::DataStreamPtr& dataStream){};
    };
}

#endif