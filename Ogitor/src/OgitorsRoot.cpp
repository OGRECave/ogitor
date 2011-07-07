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

#include "OgitorsPrerequisites.h"
#include "tinyxml.h"
#include "OgreDynLib.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OgitorsPhysics.h"
#include "BaseSerializer.h"
#include "BaseEditor.h"
#include "OFSSceneSerializer.h"
#include "SceneManagerEditor.h"
#include "ViewportEditor.h"
#include "FolderEditor.h"
#include "NodeEditor.h"
#include "EntityEditor.h"
#include "LightEditor.h"
#include "CameraEditor.h"
#include "MarkerEditor.h"
#include "PlaneEditor.h"
#include "BillboardSetEditor.h"
#include "ParticleEditor.h"
#include "MultiSelEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "TerrainGroupEditor.h"
#include "PagingEditor.h"
#include "EditableMeshEditor.h"
#include "OgitorsPaging.h"
#include "OgitorsUndoManager.h"
#include "OgitorsClipboardManager.h"
#include "Selection2D.h"
#include "PGInstanceManager.h"
#include "PGInstanceEditor.h"
#include "OgitorsScriptInterpreter.h"
#include "OgitorsScriptConsole.h"
#include "Event.h"
#include "DefaultEvents.h"
#include "EventManager.h"

#include "ofs.h"


using namespace Ogre;

namespace Ogitors
{
PropertyOptionsVector OgitorsRoot::mModelNames;
PropertyOptionsVector OgitorsRoot::mMaterialNames;
Ogre::NameValuePairList OgitorsRoot::mModelMaterialMap;
PropertyOptionsVector OgitorsRoot::mSkyboxMaterials;
PropertyOptionsVector OgitorsRoot::mAutoTrackTargets;
PropertyOptionsVector OgitorsRoot::mLayerNames;
PropertyOptionsVector OgitorsRoot::mParticleTemplateNames;
PropertyOptionsVector OgitorsRoot::mTerrainDiffuseTextureNames;
PropertyOptionsVector OgitorsRoot::mTerrainNormalTextureNames;
PropertyOptionsVector OgitorsRoot::mTerrainPlantMaterialNames;
PropertyOptionsVector OgitorsRoot::mScriptNames;

template<> OgitorsRoot* Singleton<OgitorsRoot>::ms_Singleton = 0;
static OgitorsDummySystem *dummySystem = 0;
static OgitorsDummyPhysics *dummyPhysics = 0;
static OgitorsDummyScriptInterpreter *dummyInterpreter = 0;

//-----------------------------------------------------------------------------------------
class OgitorsRootPropertySetListener: public OgitorsPropertySetListener
{
public:
    OgitorsRootPropertySetListener() {};
    ~OgitorsRootPropertySetListener() {};

    void OnPropertyRemoved(OgitorsPropertySet* set, OgitorsPropertyBase* property)
    {
        OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
    }
    void OnPropertyAdded(OgitorsPropertySet* set, OgitorsPropertyBase* property)
    {
        OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
    }
    void OnPropertyChanged(OgitorsPropertySet* set, OgitorsPropertyBase* property)
    {
        OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
    }
    void OnPropertySetRebuilt(OgitorsPropertySet* set)
    {
        OgitorsRoot::getSingletonPtr()->SetSceneModified(true);
    }
};

static OgitorsRootPropertySetListener GlobalOgitorsRootPropertySetListener;

//-----------------------------------------------------------------------------------------
OgitorsPropertySetListener* OgitorsRoot::GetGlobalPropertyListener()
{
    return &GlobalOgitorsRootPropertySetListener;
}
//-----------------------------------------------------------------------------------------
OgitorsRoot::OgitorsRoot(Ogre::StringVector* pDisabledPluginPaths) :
mUndoManager(0), mClipboardManager(0), mSceneManager(0), mSceneManagerEditor(0), mRenderWindow(0), mActiveViewport(0),
mRootEditor(0), mMultiSelection(0), mLastTranslationDelta(Vector3::ZERO),
mTerrainEditor(0), mTerrainEditorObject(0), mPagingEditor(0), mPagingEditorObject(0), mIsSceneModified(false),
mGlobalLightVisiblity(true), mGlobalCameraVisiblity(true), mSelRect(0), mSelectionNode(0),
mMouseListener(0), mKeyboardListener(0),
mGizmoScale(1.0f), mGizmoNode(0), mGizmoX(0), mGizmoY(0), mGizmoZ(0), mWorldSpaceGizmoOrientation(false),
mOldGizmoMode(256), mOldGizmoAxis(256), mWalkAroundMode(false), mActiveDragSource(0)
{
    unsigned int i;

    mProjectFile = new OFS::OfsPtr();

    mGizmoEntities[0] = mGizmoEntities[1] = mGizmoEntities[2] = 0;
    mGizmoEntities[3] = mGizmoEntities[4] = mGizmoEntities[5] = 0;

    mLoadState = LS_UNLOADED;
    mRunState = RS_STOPPED;
    mEditorTool = TOOL_SELECT;

    mSystem = OgitorsSystem::getSingletonPtr();
    if(!mSystem)
    {
        dummySystem = OGRE_NEW OgitorsDummySystem();
        mSystem = dummySystem;
    }

    mPhysics = OgitorsPhysics::getSingletonPtr();
    if(!mPhysics)
    {
        dummyPhysics = OGRE_NEW OgitorsDummyPhysics();
        mPhysics = dummyPhysics;
    }

    new EventManager();

    CBaseEditor::_initStatic(this);

    for(i = 0;i < LAST_EDITOR;i++)
    {
        mNamesByType[i].clear();
    }

    mLayerNames.clear();

    for(i = 0;i < 31;i++)
    {
        mLayerNames.push_back(PropertyOption("Layer " + Ogre::StringConverter::toString(i), Ogre::Any(i)));
    }

    mDragDropHandlers.clear();
    mUpdateList.clear();
    mUpdateScriptList.clear();
    mPlugins.clear();
    mSerializerList.clear();
    mToolBars.clear();
    mDockWidgets.clear();
    mEditorObjectFactories.clear();
    mRecentFiles.clear();

    RegisterAllEditorObjects(pDisabledPluginPaths);

    OgitorsPropertyValueMap params;
    unsigned int id = 2222222222;
    params["object_id"] = OgitorsPropertyValue(PROP_UNSIGNED_INT, Ogre::Any(id));
    CBaseEditorFactory *factory = GetEditorObjectFactory("");
    mRootEditor = factory->CreateObject(0, params);
    mIDList[id] = mRootEditor;

    mUndoManager = OGRE_NEW OgitorsUndoManager();

    mObjectDisplayOrder.clear();

    mObjectDisplayOrder.push_back(ETYPE_VIEWPORT);
    mObjectDisplayOrder.push_back(ETYPE_PAGINGMANAGER);
    mObjectDisplayOrder.push_back(ETYPE_SCENEMANAGER);
    mObjectDisplayOrder.push_back(ETYPE_SKY_MANAGER);
    mObjectDisplayOrder.push_back(ETYPE_WATER_MANAGER);
    mObjectDisplayOrder.push_back(ETYPE_TERRAIN_MANAGER);
    mObjectDisplayOrder.push_back(ETYPE_CUSTOM_MANAGER);
    mObjectDisplayOrder.push_back(ETYPE_FOLDER);
    mObjectDisplayOrder.push_back(ETYPE_LIGHT);
    mObjectDisplayOrder.push_back(ETYPE_CAMERA);

    for(int x = ETYPE_CAMERA + 1;x < LAST_EDITOR;x++)
    {
        if(x != ETYPE_MULTISEL)
            mObjectDisplayOrder.push_back(x);
    }
    mObjectDisplayOrder.push_back(ETYPE_NODE);

    mClearScreenBackground = false;

    mAutoTrackTargets.clear();
    mAutoTrackTargets.push_back(PropertyOption("None",Ogre::Any(Ogre::String("None"))));

    mScriptConsole = OGRE_NEW OgitorsScriptConsole(this, mScriptInterpreter);
}
//-----------------------------------------------------------------------------------------
OgitorsRoot::~OgitorsRoot()
{
    ClearEditors();

    for(unsigned int i = 0;i < LAST_EDITOR;i++)
    {
        mNamesByType[i].clear();
    }

    mObjectDisplayOrder.clear();

    mRootEditor->destroy();

    EditorObjectFactoryMap::iterator it = mEditorObjectFactories.begin();
    while(it != mEditorObjectFactories.end())
    {
        OGRE_DELETE it->second;
        it++;
    }

    mEditorObjectFactories.clear();

    UnLoadPlugins();

    OGRE_DELETE mScriptConsole;
    OGRE_DELETE dummyInterpreter;
    OGRE_DELETE mUndoManager;
    OGRE_DELETE dummySystem;
    OGRE_DELETE dummyPhysics;

    OgitorsUtils::FreeBuffers();

    if(EventManager::getSingletonPtr())
        delete EventManager::getSingletonPtr();

    delete mProjectFile;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::ClearEditors()
{
    if(mPagingEditorObject)
        mPagingEditorObject->unLoad();

    mTerrainEditor = 0;
    mTerrainEditorObject = 0;
    mPagingEditor = 0;
    mPagingEditorObject = 0;
    mActiveDragSource = 0;

    DestroyGizmo();

    // UnLoad editors in inverse load order
    // End at 1, since 0 means all objects
    for(unsigned int i = LAST_EDITOR - 1;i > 0;i--)
    {
        NameObjectPairList::iterator unldit = mNamesByType[i].begin();
        while(unldit != mNamesByType[i].end())
        {
            unldit->second->unLoad();
            unldit++;
        }
    }

    mRootEditor->destroy();
    mRootEditor = 0;

    mActiveViewport = 0;
    mSceneManager = 0;
    mSceneManagerEditor = 0;
    SetSceneModified(false);
    mNameList.clear();
    mIDList.clear();
    mUpdateList.clear();
    mUpdateScriptList.clear();
    mPostSceneUpdateList.clear();

    EditorObjectFactoryMap::iterator it = mEditorObjectFactories.begin();
    while (it != mEditorObjectFactories.end())
    {
#ifdef DEBUG
        if(it->second && it->second->mInstanceCount !=0)
        {
            Ogre::String errStr = "\"" + it->second->mTypeName + " Factory\" Instance Count = " + Ogre::StringConverter::toString(it->second->mInstanceCount);
            mSystem->DisplayMessageDialog(errStr,DLGTYPE_OK);
        }
#else
        if(it->second)
            it->second->mInstanceCount = 0;
#endif
        it++;
    }

    for(unsigned int i = 0;i < LAST_EDITOR;i++)
    {
        mNamesByType[i].clear();
    }

    for(unsigned int i = 0;i < OGITOR_MAX_OBJECT_TYPE;i++)
    {
        mNamesByTypeID[i].clear();
    }

    OgitorsPropertyValueMap params;
    unsigned int id = 2222222222;
    params["object_id"] = OgitorsPropertyValue(PROP_UNSIGNED_INT, Ogre::Any(id));
    CBaseEditorFactory *factory = GetEditorObjectFactory("");
    mRootEditor = factory->CreateObject(0, params);
    mIDList[id] = mRootEditor;

    mGizmoNode = 0;
    mGizmoX = 0;
    mGizmoY = 0;
    mGizmoZ = 0;
    mOldGizmoMode = 256;
    mOldGizmoAxis = 256;

    mWalkAroundMode = false;
    mWorldSpaceGizmoOrientation = false;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterSerializer(void *pluginIdentifier, CBaseSerializer *serializer)
{
    if(!serializer) return;
    SerializerMap::iterator i = mSerializerList.find(serializer->GetTypeString());
    if (i != mSerializerList.end())
        return;

    PluginEntryMap::iterator it = mPlugins.find(pluginIdentifier);
    if(it != mPlugins.end())
    {
        try
        {
            it->second.mSerializers.push_back(serializer->GetTypeString());
            it->second.mFeatures |= PLUGIN_FEATURE_SERIALIZER;
            serializer->SetTypeID(mSerializerList.size());
            mSerializerList.insert(SerializerMap::value_type(serializer->GetTypeString(), serializer));
        }
        catch(...)
        {
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterDragDropHandler(void *source, DragDropHandler *handler)
{
    DragDropHandlerMap::iterator it = mDragDropHandlers.find(source);
    if(it != mDragDropHandlers.end())
    {
        mDragDropHandlers.erase(it);
    }
    mDragDropHandlers.insert(DragDropHandlerMap::value_type(source, handler));
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UnRegisterDragDropHandler(void *source, DragDropHandler *handler)
{
    DragDropHandlerMap::iterator it = mDragDropHandlers.find(source);
    if(it != mDragDropHandlers.end())
    {
        mDragDropHandlers.erase(it);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetSerializerList(Ogre::StringVector &list)
{
    list.clear();
    SerializerMap::iterator i = mSerializerList.begin();
    while (i != mSerializerList.end())
    {
        list.push_back(i->first);
        i++;
    }
    std::sort(list.begin(),list.end());
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetExportSerializerList(Ogre::StringVector &list)
{
    list.clear();
    SerializerMap::iterator i = mSerializerList.begin();
    while (i != mSerializerList.end())
    {
        if(i->second->CanExport())
            list.push_back(i->first);
        i++;
    }
    std::sort(list.begin(),list.end());
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetImportSerializerList(Ogre::StringVector &list)
{
    list.clear();
    SerializerMap::iterator i = mSerializerList.begin();
    while (i != mSerializerList.end())
    {
        if(i->second->CanImport())
            list.push_back(i->first);
        i++;
    }
    std::sort(list.begin(),list.end());
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::TriggerImportSerializer(Ogre::String name)
{
    SerializerMap::iterator i = mSerializerList.find(name);
    if (i == mSerializerList.end())
        return false;

    if(i->second->CanImport())
    {
        if(i->second->RequiresTerminateScene())
        {
            if(!TerminateScene())
                return false;
        }

        setLoadState(LS_LOADING);

        try
        {
            if( i->second->Import() == SCF_OK )
                return true;
            else
                setLoadState(LS_UNLOADED);
        }
        catch(...)
        {
            SetSceneModified(false);
            TerminateScene();
            return false;
        }
    }

    return false;
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::TriggerExportSerializer(Ogre::String name)
{
    SerializerMap::iterator i = mSerializerList.find(name);
    if (i == mSerializerList.end())
        return false;

    if(i->second->CanExport())
    {
        if( i->second->Export() == SCF_OK)
            return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *OgitorsRoot::GetEditorObjectFactory(const Ogre::String& typeName)
{
    EditorObjectFactoryMap::iterator it = mEditorObjectFactories.find(typeName);
    if(it == mEditorObjectFactories.end())
        return 0;

    return it->second;
}
//-----------------------------------------------------------------------------------------
unsigned int OgitorsRoot::GetTypeID(const Ogre::String& typeName)
{
    EditorObjectFactoryMap::iterator it = mEditorObjectFactories.find(typeName);
    if(it == mEditorObjectFactories.end())
        return 0;

    return it->second->mTypeID;
}
//-----------------------------------------------------------------------------------------
int EditorObjectTypeIDCounter = 0;
void OgitorsRoot::RegisterAllEditorObjects(Ogre::StringVector* pDisabledPluginPaths)
{
    EditorObjectTypeIDCounter = 0;

    _RegisterEditorFactory(OGRE_NEW CBaseEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CSceneManagerEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CViewportEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CFolderEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CEntityEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CLightEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CCameraEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CNodeEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CPlaneEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CBillboardSetEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CMarkerEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CParticleEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CMultiSelEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CTerrainGroupEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CTerrainPageEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CPagingManagerFactory());
    _RegisterEditorFactory(OGRE_NEW CPGInstanceEditorFactory());
    _RegisterEditorFactory(OGRE_NEW CPGInstanceManagerFactory());
    _RegisterEditorFactory(OGRE_NEW CEditableMeshEditorFactory());
    Ogre::StringVector ScriptPluginList, PluginList;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifdef DEBUG
    mSystem->GetFileList("../Plugins/debug/*Script_d.dll",ScriptPluginList);
#else
    mSystem->GetFileList("../Plugins/*Script.dll",ScriptPluginList);
#endif
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#ifdef DEBUG
    mSystem->GetFileList("/usr/share/qtOgitor/Plugins/debug/*Script_d.so",ScriptPluginList);
#else
    mSystem->GetFileList("/usr/share/qtOgitor/Plugins/*Script.so",ScriptPluginList);
#endif
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#ifdef DEBUG
    mSystem->GetFileList(OgitorsUtils::GetMacBundlePath() + "/Contents/Plugins/Ogitor/*Script_d.dylib", ScriptPluginList);
#else
    mSystem->GetFileList(OgitorsUtils::GetMacBundlePath() + "/Contents/Plugins/Ogitor/*Script.dylib", ScriptPluginList);
#endif
#endif
    for(unsigned int i = 0;i < ScriptPluginList.size();i++)
    {
        // Is plugin disabled and therefore do not register it?
        if(std::find(pDisabledPluginPaths->begin(), pDisabledPluginPaths->end(), ScriptPluginList[i]) == pDisabledPluginPaths->end())
            LoadPlugin(ScriptPluginList[i]);
        else
            LoadPlugin(ScriptPluginList[i], true);
    }

    PluginList.clear();

    if(mScriptInterpreterList["AngelScript"] != 0)
    {
        mScriptInterpreter = mScriptInterpreterList["AngelScript"];
        mScriptInterpreter->Init();
    }
    else
    {
        dummyInterpreter = OGRE_NEW OgitorsDummyScriptInterpreter;
        mScriptInterpreter = dummyInterpreter;
    }

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifdef DEBUG
    mSystem->GetFileList("../Plugins/debug/*_d.dll",PluginList);
#else
    mSystem->GetFileList("../Plugins/*.dll",PluginList);
#endif
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#ifdef DEBUG
    mSystem->GetFileList("/usr/share/qtOgitor/Plugins/debug/*_d.so",PluginList);
#else
    mSystem->GetFileList("/usr/share/qtOgitor/Plugins/*.so",PluginList);
#endif
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#ifdef DEBUG
    mSystem->GetFileList(OgitorsUtils::GetMacBundlePath() + "/Contents/Plugins/Ogitor/*_d.dylib", PluginList);
#else
    mSystem->GetFileList(OgitorsUtils::GetMacBundlePath() + "/Contents/Plugins/Ogitor/*.dylib", PluginList);
#endif
#endif
    for(unsigned int i = 0; i < PluginList.size(); i++)
    {
        // Is plugin disabled and therefore do not register it?
        if(std::find(pDisabledPluginPaths->begin(), pDisabledPluginPaths->end(), PluginList[i]) == pDisabledPluginPaths->end())
            LoadPlugin(PluginList[i]);
        else
            LoadPlugin(PluginList[i], true);
    }

    EditorObjectFactoryMap::iterator it = mEditorObjectFactories.begin();

    while(it != mEditorObjectFactories.end())
    {
        if(it->second)
            it->second->Init();
        it++;
    }
}
//-----------------------------------------------------------------------------------------
typedef bool (*DLL_START_PLUGIN)(void*, Ogre::String&);
typedef bool (*DLL_GET_PLUGIN_NAME)(Ogre::String&);
typedef bool (*DLL_STOP_PLUGIN)(void);
//-----------------------------------------------------------------------------------------
void OgitorsRoot::LoadPlugin(Ogre::String pluginPath, bool noRegistration)
{
    PluginEntryMap::iterator it = mPlugins.begin();

    // Check if we already have loaded the plugin from this path
    while(it != mPlugins.end())
    {
        PLUGINENTRY ent = it->second;
        if(ent.mPluginPath == pluginPath)
            return;

        it++;
    }

    DynLib* lib;
    lib = OGRE_NEW DynLib(pluginPath);

    try
    {
        lib->load();
    }
    catch(...)
    {
        OGRE_DELETE lib;
        return;
    }

    Ogre::String pluginName = "";

    PLUGINENTRY entry;
    entry.mName = "";
    entry.mPluginPath = pluginPath;
    entry.mLibrary = lib;
    entry.mLoaded = false;
    entry.mLoadingError = false;
    entry.mFeatures = 0;
    entry.mEditorObjects.clear();
    entry.mSerializers.clear();
    mPlugins.insert(PluginEntryMap::value_type((void*)lib, entry));

    it = mPlugins.find(lib);

    // Either load the plugin and register all its components or just look up its name
    if(noRegistration)
    {
        DLL_GET_PLUGIN_NAME pFunc = (DLL_GET_PLUGIN_NAME)lib->getSymbol("dllGetPluginName");

        if(pFunc && pFunc(pluginName))
            it->second.mName = pluginName;
        else
            it->second.mName = pluginPath;

        // Always unload since we only wanted to get the name
        lib->unload();
    }
    else
    {
        DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->getSymbol("dllStartPlugin");

        if(pFunc && pFunc(lib, pluginName))
        {
            it->second.mName = pluginName;
            it->second.mLoaded = true;
        }
        else
        {
            it->second.mName = pluginPath;
            it->second.mLoadingError = true;
            lib->unload();
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UnLoadPlugin(void *identifier)
{
    PluginEntryMap::iterator it = mPlugins.begin();

    // Check if plugin is already unloaded
    while(it != mPlugins.end())
    {
        if(it->first == identifier && it->second.mLoaded == false)
            return;
        it++;
    }

    Ogre::DynLib *lib = static_cast<Ogre::DynLib*>(identifier);
    DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)lib->getSymbol("dllStopPlugin");
    // Call stop function
    if(pFunc)
        pFunc();

    it = mPlugins.find(identifier);
    it->second.mLoaded = false;

    lib->unload();
    OGRE_DELETE lib;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UnLoadPlugins()
{
    PluginEntryMap::iterator it = mPlugins.begin();

    while(it != mPlugins.end())
    {
        if(it->second.mLoaded)
        {
            Ogre::DynLib *lib = static_cast<Ogre::DynLib*>(it->first);
            DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)lib->getSymbol("dllStopPlugin");
            // Call stop function
            if(pFunc)
                pFunc();

            lib->unload();
            OGRE_DELETE lib;
        }
        it++;
    }

    mPlugins.clear();
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterObjectName(Ogre::String name,CBaseEditor *obj)
{
    mNameList.insert(NameObjectPairList::value_type(name, obj));
    mNamesByType[obj->getEditorType()].insert(NameObjectPairList::value_type(name, obj));
    mNamesByTypeID[obj->getTypeID()].insert(NameObjectPairList::value_type(name, obj));
    if(obj->isTerrainType())
    {
        mTerrainEditor = obj->getTerrainEditor();
        mTerrainEditorObject = obj;

        TerrainEditorChangeEvent evt(true, mTerrainEditor);
        EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
    }

    if(obj->getPagingEditor() != 0)
    {
        mPagingEditor = obj->getPagingEditor();
        mPagingEditorObject = obj;
    }

    if(obj->isAutoTrackTarget())
    {
        mAutoTrackTargets.push_back(PropertyOption(name, Ogre::Any(name)));
        std::sort(++mAutoTrackTargets.begin(), mAutoTrackTargets.end(), PropertyOption::comp_func);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UnRegisterObjectName(Ogre::String name, CBaseEditor *obj)
{
    NameObjectPairList::iterator i = mNameList.find(name);
    if (i != mNameList.end()) mNameList.erase(i);

    i = mNamesByType[obj->getEditorType()].find(name);
    if (i != mNamesByType[obj->getEditorType()].end()) mNamesByType[obj->getEditorType()].erase(i);

    i = mNamesByTypeID[obj->getTypeID()].find(name);
    if (i != mNamesByTypeID[obj->getTypeID()].end()) mNamesByTypeID[obj->getTypeID()].erase(i);

    if(obj->isTerrainType() && (obj == mTerrainEditorObject))
    {
        mTerrainEditor = 0;
        mTerrainEditorObject = 0;

        TerrainEditorChangeEvent evt(false, 0);
        EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
    }

    if(obj == mPagingEditorObject)
    {
        mPagingEditor = 0;
        mPagingEditorObject = 0;
    }

    if(obj->isAutoTrackTarget())
    {
        for(unsigned int i = 0;i < mAutoTrackTargets.size();i++)
        {
            if(mAutoTrackTargets[i].mKey == name)
            {
                mAutoTrackTargets.erase(mAutoTrackTargets.begin() + i);
                break;
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::OnTerrainMaterialChange(CBaseEditor *terrainobject)
{
    NameObjectPairList hydrax = GetObjectsByTypeName("Hydrax Object");
    NameObjectPairList::const_iterator i = hydrax.begin();
    if(i != hydrax.end())
    {
        CBaseEditor *hyed = i->second;
        if(mTerrainEditor)
        {
            StringVector matnames = mTerrainEditor->getMaterialNames();
            for(unsigned int i = 0;i < matnames.size();i++)
            {
                if(hyed->getProperties()->hasProperty("technique_add"))
                    hyed->getProperties()->setValue<Ogre::String>("technique_add",matnames[i]);
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterForUpdates(CBaseEditor *object)
{
    mUpdateList.insert(NameObjectPairList::value_type(object->getName(), object));
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UnRegisterForUpdates(CBaseEditor *object)
{
    NameObjectPairList::iterator i = mUpdateList.find(object->getName());
    if (i != mUpdateList.end())
        mUpdateList.erase(i);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterForScriptUpdates(CBaseEditor *object)
{
    mUpdateScriptList.insert(NameObjectPairList::value_type(object->getName(), object));
    unsigned int handle = object->getScriptResourceHandle();
    if(handle != 0)
    {
        object->setScriptResourceHandle(0);
        mScriptInterpreter->releaseHandle(handle);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::UnRegisterForScriptUpdates(CBaseEditor *object)
{
    NameObjectPairList::iterator i = mUpdateScriptList.find(object->getName());
    if (i != mUpdateScriptList.end())
        mUpdateScriptList.erase(i);
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::Update(float timePassed)
{
    OgitorsScriptInterpreter::setTimeSinceLastFrame(timePassed);

    UpdateFrameEvent evt(timePassed);
    EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);

    if(mScriptInterpreter != dummyInterpreter && mRunState == RS_RUNNING)
    {
        NameObjectPairList::const_iterator i = mUpdateScriptList.begin();
        while(i != mUpdateScriptList.end())
        {
            // We get the pointer first and increment since
            // Editor object may unregister itself during update
            CBaseEditor *editor = i->second;
            i++;
            Ogre::String module = Ogre::StringConverter::toString(editor->getObjectID());
            mScriptInterpreter->runUpdateFunction(module, editor, timePassed);
        }
    }

    bool Changed = false;
    NameObjectPairList::const_iterator i = mUpdateList.begin();
    while(i != mUpdateList.end())
    {
        // We get the pointer first and increment since
        // Editor object may unregister itself during update
        CBaseEditor *editor = i->second;
        i++;
        Changed |= editor->update(timePassed);
    }

    ChangeSceneModified(Changed);


    if(mGizmoNode)
    {
        if(!mMultiSelection->isEmpty())
            UpdateGizmo();
        else
            mGizmoNode->setVisible(false);
    }

    return Changed;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterForPostSceneUpdates(CBaseEditor *object)
{
    if(mLoadState == LS_LOADED)
        object->postSceneUpdate(GetSceneManager(),GetViewport()->getCameraEditor()->getCamera(),mRenderWindow);
    else
        mPostSceneUpdateList.push_back(object);
}
//-----------------------------------------------------------------------------------------
CBaseEditor * OgitorsRoot::FindObject(Ogre::String name, unsigned int type)
{
    NameObjectPairList::const_iterator i;
    if(type == 0)
    {
        i = mNameList.find(name);
        if (i != mNameList.end())
            return i->second;
    }
    else
    {
        i = mNamesByType[type].find(name);
        if (i != mNamesByType[type].end())
            return i->second;
    }
    return 0;
}
//-----------------------------------------------------------------------------------------
const NameObjectPairList OgitorsRoot::GetObjectsByType(unsigned int type)
{
    if(type < LAST_EDITOR)
        return mNamesByType[type];
    else
        return mNamesByType[0];
}
//-----------------------------------------------------------------------------------------
const NameObjectPairList OgitorsRoot::GetObjectsByTypeName(const Ogre::String& typeName)
{
    CBaseEditorFactory *factory = GetEditorObjectFactory(typeName);

    unsigned int ID;
    if(!factory)
        ID = 0;
    else
        ID = factory->mTypeID;

    return mNamesByTypeID[ID];
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetObjectList(unsigned int type, ObjectVector& list)
{
    list.clear();
    if(type == ETYPE_MULTISEL) return;

    if(type == 0)
    {
        NameObjectPairList::const_iterator it = mNameList.begin();
        while(it != mNameList.end())
        {
            list.push_back(it->second);
            it++;
        }
    }
    else if(type == ETYPE_NODE)
    {
        mRootEditor->getNodeList(list);
    }
    else
    {
        NameObjectPairList::const_iterator it = mNamesByType[type].begin();
        while(it != mNamesByType[type].end())
        {
            list.push_back(it->second);
            it++;
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetObjectList(const Ogre::String& typeName, ObjectVector& list)
{
    list.clear();
    CBaseEditorFactory *factory = GetEditorObjectFactory(typeName);

    unsigned int ID;
    if(!factory)
        ID = 0;
    else
        ID = factory->mTypeID;

    NameObjectPairList::const_iterator it = mNamesByTypeID[ID].begin();
    while(it != mNamesByTypeID[ID].end())
    {
        list.push_back(it->second);
        it++;
    }
}
//-----------------------------------------------------------------------------------------
Ogre::SceneManager *OgitorsRoot::GetFirstSceneManager()
{
    NameObjectPairList::const_iterator i = mNamesByType[ETYPE_SCENEMANAGER].begin();
    if(i != mNamesByType[ETYPE_SCENEMANAGER].end())
        return (static_cast<CSceneManagerEditor*>(i->second))->getSceneManager();
    else
        return 0;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *OgitorsRoot::CreateEditorObject(CBaseEditor *parent, const Ogre::String objecttypestring, OgitorsPropertyValueMap &params,bool addtotreelist, bool display)
{
    if(parent == 0)
    {
        OgitorsPropertyValueMap::const_iterator ni;

        if ((ni = params.find("parentnode")) != params.end())
        {
            parent = FindObject(Ogre::any_cast<Ogre::String>(ni->second.val));
        }

        if(!parent)
        {
            if(GetSceneManager() == 0)
                parent = mRootEditor;
            else
                parent = GetSceneManagerEditor();
        }
    }

    CBaseEditorFactory *factory = GetEditorObjectFactory(objecttypestring);
    if(!factory)
        return 0;

    CBaseEditor *object = factory->CreateObject(&parent, params);

    if(!object)
        return 0;

    if(object->getObjectID() == 0)
        object->setObjectID(GetUniqueObjectID(object));
    else
        AddObjectID(object->getObjectID(), object);

    RegisterObjectName(object->getName(), object);

    parent->_addChild(object);

    if(addtotreelist)
    {
        mSystem->InsertTreeItem(parent,object,object->getTypeID(),object->getTextColourInt());
    }

    if(parent->getLocked())
        object->setLocked(true);

    if(display)
    {
        object->load();
        mMultiSelection->setSelection(object);
    }

    object->getProperties()->addListener(&GlobalOgitorsRootPropertySetListener);
    object->getCustomProperties()->addListener(&GlobalOgitorsRootPropertySetListener);
    object->getProperties()->addListener(mUndoManager);
    object->getCustomProperties()->addListener(mUndoManager);

    if(mLoadState == LS_LOADED)
        mUndoManager->AddUndo(OGRE_NEW ObjectCreationUndo(object));

    SetSceneModified(true);

    if(mPagingEditor)
        mPagingEditor->addObject(object);

    return object;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *OgitorsRoot::CloneEditorObject(CBaseEditor *object,bool addtotreelist, bool display)
{
    CBaseEditor *clone;
    CBaseEditor *parent = object->getParent();

    if(parent == 0)
        return 0;

    clone = object->getFactoryDynamic()->CloneObject(&parent, object);

    if(!clone)
        return 0;

    clone->setObjectID(GetUniqueObjectID(clone));

    RegisterObjectName(clone->getName(), clone);
    parent->_addChild(clone);

    OgitorsCustomPropertySet customPropertyClone;
    object->getCustomProperties()->cloneSet(customPropertyClone);
    clone->getCustomProperties()->initFromSet(customPropertyClone);

    if(addtotreelist)
    {
        mSystem->InsertTreeItem(parent,clone,clone->getTypeID(),object->getTextColourInt());
    }

    if(display)
    {
        clone->load();
        clone->loadAllChildren(false);
        mMultiSelection->setSelection(clone);
    }

    clone->getProperties()->addListener(&GlobalOgitorsRootPropertySetListener);
    clone->getCustomProperties()->addListener(&GlobalOgitorsRootPropertySetListener);
    clone->getProperties()->addListener(mUndoManager);
    clone->getCustomProperties()->addListener(mUndoManager);

    if(mLoadState == LS_LOADED)
        mUndoManager->AddUndo(OGRE_NEW ObjectCreationUndo(clone));

    SetSceneModified(true);

    if(mPagingEditor)
        mPagingEditor->addObject(clone);
    return clone;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *OgitorsRoot::InstanceCloneEditorObject(CBaseEditor *object,bool addtotreelist, bool display)
{
    CBaseEditor *clone = CloneEditorObject(object, addtotreelist, display);
    if(clone)
    {
        OgitorsPropertyVector vOriginal = object->getProperties()->getPropertyVector();
        OgitorsPropertyVector vClone = clone->getProperties()->getPropertyVector();

        // Do not connect to any BASE property (currently there are 12 base properties)
        // Last one needs connecting to
        for(unsigned int i = 11;i < vOriginal.size();i++)
        {
            vClone[i]->connectTo(vOriginal[i]);
        }
    }
    return clone;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::DestroyEditorObject(CBaseEditor *object, bool removefromtreelist, bool display)
{
    // Index0 Viewport can not be deleted
    if(object->getEditorType() == ETYPE_VIEWPORT)
    {
        CViewportEditor *viewport = static_cast<CViewportEditor*>(object);

        if(viewport->getViewportIndex() == 1)
        {
            OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(OTR("Can not delete the main viewport!!"), DLGTYPE_OK);
            return;
        }

        if(mActiveViewport == viewport)
        {
            NameObjectPairList::const_iterator vt = mNamesByType[ETYPE_VIEWPORT].begin();
            while(vt != mNamesByType[ETYPE_VIEWPORT].end())
            {
                if(vt->second == object)
                {
                    vt++;
                    continue;
                }
                mActiveViewport = static_cast<CViewportEditor*>(vt->second);
                break;
            }
        }
    }

    if(mMultiSelection->contains(object))
    {
        object->setSelected(false);
        object->setHighlighted(false);
    }

    NameObjectPairList viewports = GetObjectsByType(ETYPE_VIEWPORT);
    NameObjectPairList::const_iterator vit = viewports.begin();

    while(vit != viewports.end())
    {
        static_cast<CViewportEditor*>(vit->second)->OnObjectDestroyed(object);

        vit++;
    }

    if(removefromtreelist)
    {
        mSystem->DeleteTreeItem(object);
    }

    if(mLoadState == LS_LOADING)
    {
        for(unsigned int p = 0;p < mPostSceneUpdateList.size();p++)
        {
            if(mPostSceneUpdateList[p] == object)
            {
                mPostSceneUpdateList.erase(mPostSceneUpdateList.begin() + p);
                break;
            }
        }
    }

    if(mLoadState == LS_LOADED)
        mUndoManager->BeginCollection("Delete " + object->getName());

    if(object->getTerrainEditor() == mTerrainEditor)
    {
        std::vector<CBaseEditor*> destroyList;
        NameObjectPairList::iterator it = mNameList.begin();
        while(it != mNameList.end())
        {
            if(it->second->onTerrainDestroyed())
                destroyList.push_back(it->second);
            it++;
        }

        for(unsigned int destroyIDX = 0;destroyIDX < destroyList.size();destroyIDX++)
            DestroyEditorObject(destroyList[destroyIDX],true,true);

        mTerrainEditor = 0;
    }

    if(mLoadState == LS_LOADED)
    {
        Ogre::StringVector list;
        object->getNameList(list);
        CBaseEditor *item = 0;
        for(int c = list.size() - 1;c >= 0;--c)
        {
            item = FindObject(list[c]);
            if(item)
            {
                item->unLoad();
                mUndoManager->AddUndo(OGRE_NEW ObjectDeletionUndo(item));
            }
        }

        mUndoManager->EndCollection(true);
    }

    if(mPagingEditor)
        mPagingEditor->removeObject(object);

    object->destroy(true);

    SetSceneModified(true);
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::CanInstantiateObject(const Ogre::String& typeName)
{
    CBaseEditorFactory *factory = GetEditorObjectFactory(typeName);
    if(!factory)
        return false;
    return factory->CanInstantiate();
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RecurseFillTreeView(CBaseEditor *pEditor)
{
    NameObjectPairList& children = pEditor->getChildren();
    NameObjectPairList::iterator i, iend;

    iend = children.end();

    std::map<Ogre::String,CBaseEditor*> mymap;
    mymap.clear();
    for (i = children.begin(); i != iend; ++i)
    {
        mymap.insert(std::map<Ogre::String,CBaseEditor*>::value_type(i->first,i->second));
    }

    std::map<Ogre::String,CBaseEditor*>::const_iterator i2, i2end;
    i2end = mymap.end();

    for(unsigned int z = 0;z < mObjectDisplayOrder.size();z++)
    {
      for (i2 = mymap.begin(); i2 != i2end; ++i2)
      {
          if(i2->second->getEditorType() == mObjectDisplayOrder[z])
          {
              mSystem->InsertTreeItem(pEditor,i2->second,i2->second->getTypeID(),i2->second->getTextColourInt());
              RecurseFillTreeView(i2->second);
          }
      }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::FillTreeView()
{
    mSystem->ClearTreeItems();
    void *rootitem = mSystem->CreateTreeRoot(mProjectOptions.ProjectName + ".ogscene");
    mRootEditor->setSceneTreeItemHandle(rootitem);
    RecurseFillTreeView(mRootEditor);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::ClearProjectOptions()
{
    mProjectOptions.IsNewProject = false;
    mProjectOptions.CreatedIn.clear();
    mProjectOptions.ProjectDir.clear();
    mProjectOptions.ProjectName.clear();
    mProjectOptions.SceneManagerName.clear();
    mProjectOptions.TerrainDirectory = "Terrain";
    mProjectOptions.HydraxDirectory = "Hydrax";
    mProjectOptions.CaelumDirectory = "Caelum";
    mProjectOptions.PagedGeometryDirectory = "PagedGeometry";
    mProjectOptions.SceneManagerConfigFile = "";
    mProjectOptions.ResourceDirectories.clear();
    mProjectOptions.CameraSaveCount = 0;
    for(int i = 0;i < 10;i++)
    {
        mProjectOptions.CameraPositions[i] = Vector3::ZERO;
        mProjectOptions.CameraOrientations[i] = Quaternion::IDENTITY;
    }

    for(unsigned int v = 0;v < 31;v++)
    {
        mProjectOptions.LayerNames[v] = "Layer " + Ogre::StringConverter::toString(v);
        mProjectOptions.LayerVisible[v] = true;
    }

    mProjectOptions.LayerCount = 1;
    mProjectOptions.SelectionRectColour = Ogre::ColourValue(0.5f, 0, 1);
    mProjectOptions.SelectionBBColour = Ogre::ColourValue(1, 1, 1);
    mProjectOptions.SelectHighlightBBColour = Ogre::ColourValue(0.19f, 0.91f, 0.19f);
    mProjectOptions.HighlightBBColour = Ogre::ColourValue(0.91f, 0.19f, 0.19f);
    mProjectOptions.GridColour = Ogre::ColourValue(0.7f, 0.7f, 0.7f);
    mProjectOptions.GridSpacing = 1.0f;
    mProjectOptions.SnapAngle = 5.0f;
    mProjectOptions.CameraSpeed = 1.0f;
    mProjectOptions.WalkAroundHeight = 2.0f;
    mProjectOptions.VolumeSelectionDepth = 20.0f;
    mProjectOptions.ObjectCount = 0;
    mProjectOptions.AutoBackupEnabled = false;
    mProjectOptions.AutoBackupPeriod = 30;
    mProjectOptions.AutoBackupPeriodType = 0;
    mProjectOptions.AutoBackupFolder = ".";
    mProjectOptions.AutoBackupNumber = 0;
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::OptionsReadCameraPositions(TiXmlElement *parent)
{
    TiXmlElement* element = 0;
    Ogre::String sPos,sOrient;
    element = parent->FirstChildElement();

    int pos = 0;
    mProjectOptions.CameraSaveCount = 0;
    if(!element)
        return false;
    do
    {
        sPos = ValidAttr(element->Attribute("position"));
        sOrient = ValidAttr(element->Attribute("orientation"));
        mProjectOptions.CameraPositions[pos] = Ogre::StringConverter::parseVector3(sPos);
        mProjectOptions.CameraOrientations[pos] = Ogre::StringConverter::parseQuaternion(sOrient);
        pos++;
    } while(element = element->NextSiblingElement());
    mProjectOptions.CameraSaveCount = pos;
    return true;
}
//----------------------------------------------------------------------------
bool OgitorsRoot::OptionsReadDirectories(TiXmlElement *parent, Ogre::StringVector &pDir)
{
    TiXmlElement* element = 0;
    Ogre::String value,stype;
    element = parent->FirstChildElement();
    if(!element) return false;
    do
    {
        value = ValidAttr(element->Attribute("value"));

        std::replace(value.begin(), value.end(), '\\', '/');

        if(value[0] == '.')
            value.erase(0,1);

        if(value[0] != '/')
            value = Ogre::String("/") + value;

        if(value[value.length() - 1] != '/')
            value = value + "/";

        pDir.push_back(value);
    } while(element = element->NextSiblingElement());
    return true;
}
//----------------------------------------------------------------------------
bool OgitorsRoot::OptionsReadLayers(TiXmlElement *parent)
{
    TiXmlElement* element = 0;
    Ogre::String id, name, visible;
    element = parent->FirstChildElement();
    if(!element) return false;
    do
    {
        id = ValidAttr(element->Attribute("id"));
        name = ValidAttr(element->Attribute("name"));
        visible = ValidAttr(element->Attribute("visible"));

        int idx = Ogre::StringConverter::parseInt(id);
        mProjectOptions.LayerNames[idx] = name;
        mProjectOptions.LayerVisible[idx] = Ogre::StringConverter::parseBool(visible);
    } while(element = element->NextSiblingElement());
    return true;
}
//----------------------------------------------------------------------------
bool OgitorsRoot::LoadProjectOptions(TiXmlElement *optRoot)
{
    TiXmlElement* element = 0;
    Ogre::String eType;
    element = optRoot->FirstChildElement();
    if(!element) return false;
    do
    {
        eType = element->Value();
        if(eType == "SCENEMANAGER") mProjectOptions.SceneManagerName = ValidAttr(element->Attribute("value"));
        else if(eType == "TERRAINDIR") mProjectOptions.TerrainDirectory = ValidAttr(element->Attribute("value"));
        else if(eType == "HYDRAXDIR") mProjectOptions.HydraxDirectory = ValidAttr(element->Attribute("value"));
        else if(eType == "CAELUMDIR") mProjectOptions.CaelumDirectory = ValidAttr(element->Attribute("value"));
        else if(eType == "PAGEDGEOMETRYDIR") mProjectOptions.PagedGeometryDirectory = ValidAttr(element->Attribute("value"));
        else if(eType == "MODELDIRECTORIES") OptionsReadDirectories(element,mProjectOptions.ResourceDirectories);
        else if(eType == "MATERIALDIRECTORIES") OptionsReadDirectories(element,mProjectOptions.ResourceDirectories);
        else if(eType == "RESOURCEDIRECTORIES") OptionsReadDirectories(element,mProjectOptions.ResourceDirectories);
        else if(eType == "CAMERASPEED") mProjectOptions.CameraSpeed = Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("value"), "10"));
        else if(eType == "CAMERAPOSITIONS") OptionsReadCameraPositions(element);
        else if(eType == "LAYERS") OptionsReadLayers(element);
        else if(eType == "LAYERCOUNT") mProjectOptions.LayerCount = Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("value"), "1"));
        else if(eType == "SELECTIONRECTCOLOUR") mProjectOptions.SelectionRectColour = Ogre::StringConverter::parseColourValue(ValidAttr(element->Attribute("value"), "0.5 0 1"));
        else if(eType == "SELECTIONCOLOUR") mProjectOptions.SelectionBBColour = Ogre::StringConverter::parseColourValue(ValidAttr(element->Attribute("value"), "1 1 1"));
        else if(eType == "HIGHLIGHTCOLOUR") mProjectOptions.HighlightBBColour = Ogre::StringConverter::parseColourValue(ValidAttr(element->Attribute("value"), "0.91 0.19 0.19"));
        else if(eType == "SELECTHIGHLIGHTCOLOUR") mProjectOptions.SelectHighlightBBColour = Ogre::StringConverter::parseColourValue(ValidAttr(element->Attribute("value"), "0.19 0.91 0.19"));
        else if(eType == "GRIDCOLOUR") mProjectOptions.GridColour = Ogre::StringConverter::parseColourValue(ValidAttr(element->Attribute("value"), "0.7 0.7 0.7"));
        else if(eType == "GRIDSPACING") mProjectOptions.GridSpacing = Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("value"), "1"));
        else if(eType == "SNAPANGLE") mProjectOptions.SnapAngle = Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("value"), "5"));
        else if(eType == "WALKAROUNDHEIGHT") mProjectOptions.WalkAroundHeight = Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("value"), "2"));
        else if(eType == "VOLUMESELECTIONDEPTH") mProjectOptions.VolumeSelectionDepth = Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("value"), "2"));
        else if(eType == "OBJECTCOUNT") mProjectOptions.ObjectCount = Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("value"), "2"));
        else if(eType == "AUTOBACKUP") mProjectOptions.AutoBackupEnabled = Ogre::StringConverter::parseBool(ValidAttr(element->Attribute("value"), "false"));
        else if(eType == "AUTOBACKUPPERIOD") mProjectOptions.AutoBackupPeriod = Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("value"), "1"));
        else if(eType == "AUTOBACKUPPERIODTYPE") mProjectOptions.AutoBackupPeriodType = Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("value"), "0"));
        else if(eType == "AUTOBACKUPFOLDER") mProjectOptions.AutoBackupFolder = ValidAttr(element->Attribute("value"), "/backup");
        else if(eType == "AUTOBACKUPNUMBER") mProjectOptions.AutoBackupNumber = Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("value"), "10"));
    } while(element = element->NextSiblingElement());
    return true;
}
//-----------------------------------------------------------------------------------------
int OgitorsRoot::LoadScene(Ogre::String filename)
{
    setLoadState(LS_LOADING);

    ClearProjectOptions();
    mPostSceneUpdateList.clear();

    Ogre::UTFString msg = mSystem->Translate("Load in progress...");
    mSystem->UpdateLoadProgress(-1, msg);

    COFSSceneSerializer *defaultserializer = OGRE_NEW COFSSceneSerializer();
    int ret = defaultserializer->Import(filename);

    if(ret != Ogitors::SCF_OK)
    {
        msg = mSystem->Translate("Please load a Scene File...");
        mSystem->UpdateLoadProgress(-1, msg);
        setLoadState(LS_UNLOADED);
        (*mProjectFile).unmount();
    }

    OGRE_DELETE defaultserializer;

    return ret;
}
//-----------------------------------------------------------------------------------------
CMultiSelEditor *OgitorsRoot::GetSelection()
{
    if(!GetSceneManager())
        return 0;

    if(!mMultiSelection && mLoadState != LS_UNLOADED)
    {
        OgitorsPropertyValueMap params;
        params.clear();
        unsigned int id = 2000000000;
        params["object_id"] = OgitorsPropertyValue(PROP_UNSIGNED_INT, Ogre::Any(id));
        mMultiSelection = static_cast<CMultiSelEditor*>(CreateEditorObject(0,"Multi Selection",params,false,false));
    }

    return mMultiSelection;
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::AfterLoadScene()
{
    OgitorsPropertyValueMap params;

    Ogre::UTFString entry = OgitorsUtils::QualifyPath((*mProjectFile)->getFileSystemName());
    AddToRecentFiles(entry);

    mProjectOptions.CameraSaveCount--;

    CreateGizmo();
    SetGizmoMode(0);
    mClipboardManager = OGRE_NEW OgitorsClipboardManager();
    CViewportEditor::ResetCommonValues();
    SetEditorTool(TOOL_SELECT);

    ViewportGrid::setGridSpacing(mProjectOptions.GridSpacing);
    ViewportGrid::setGridColour(mProjectOptions.GridColour);

    Ogre::StringVector templateList;

    Ogre::String generaltemplates = OgitorsUtils::QualifyPath(OgitorsSystem::getSingletonPtr()->getProjectsDirectory() + "/Templates/*.otl");
    mSystem->GetFileList(generaltemplates, templateList);
    mClipboardManager->addTemplatesFromFiles(templateList, true);
    templateList.clear();

    OFS::FileList templatenames = (*mProjectFile)->listFiles("/Templates", OFS::OFS_FILE);

    for(unsigned int i = 0;i < templatenames.size();i++)
    {
        Ogre::String tname = Ogre::String("/Templates/") + templatenames[i].name;
        Ogre::String extension = tname.substr(tname.length() - 4, 4);
        if(extension == Ogre::String(".otl"))
            templateList.push_back(tname);
    }

    mClipboardManager->addTemplatesFromFiles(templateList, false);
    templateList.clear();

    if(!mActiveViewport)
    {
        params.clear();
        OgitorsPropertyValue pvalue;
        pvalue.propType = PROP_STRING;
        pvalue.val = Ogre::Any(Ogre::String("Viewport1"));
        params["name"] = pvalue;
        pvalue.propType = PROP_COLOUR;
        pvalue.val = Ogre::Any(Ogre::ColourValue(0,0,0));
        params["colour"] = pvalue;
        pvalue.propType = PROP_INT;
        pvalue.val = Ogre::Any(int(1));
        params["index"] = pvalue;
        pvalue.propType = PROP_VECTOR3;
        pvalue.val = Ogre::Any(Ogre::Vector3(0,10,0));
        params["camera::position"] = pvalue;
        unsigned int id = 2000000002;
        params["object_id"] = OgitorsPropertyValue(PROP_UNSIGNED_INT, Ogre::Any(id));
        mActiveViewport = static_cast<CViewportEditor*>(CreateEditorObject(0,"Viewport Object",params,false,false));
    }

    unsigned int vismask = 0x80000000;
    for(unsigned v = 0;v < 31;v++)
    {
        unsigned int curmask = 1 << v;
        if(mProjectOptions.LayerVisible[v])
            vismask |= curmask;
    }

    GetSceneManager()->setVisibilityMask(vismask);

    Ogre::UTFString loadmsg = mSystem->Translate("Loading scene objects");
    mSystem->UpdateLoadProgress(80, loadmsg);

    mRootEditor->load();
    mRootEditor->loadAllChildren(mPagingEditor != 0);

    loadmsg = mSystem->Translate("Applying Post Load Updates");
    mSystem->UpdateLoadProgress(80, loadmsg);

    for(unsigned int i = 0;i < mPostSceneUpdateList.size();i++)
    {
        mPostSceneUpdateList[i]->postSceneUpdate(GetSceneManager(),GetViewport()->getCameraEditor()->getCamera(),mRenderWindow);
    }
    mPostSceneUpdateList.clear();

    FillTreeView();

    loadmsg = mSystem->Translate("Rendering...");
    mSystem->UpdateLoadProgress(100, loadmsg);

    // Attach background to the scene
    mSelectionNode = GetSceneManager()->getRootSceneNode()->createChildSceneNode("scbnSELECTION");
    mSelectionNode->setVisible(true);

    Ogre::ResourceManager::ResourceCreateOrRetrieveResult result = Ogre::MaterialManager::getSingleton().createOrRetrieve("SelectionBBMaterial", "General");
    if(!result.first.isNull())
    {
        Ogre::MaterialPtr matptrOBBoxManualMaterial = result.first;
        matptrOBBoxManualMaterial->setReceiveShadows(false);
        matptrOBBoxManualMaterial->getTechnique(0)->setLightingEnabled(true);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setDiffuse(mProjectOptions.SelectionBBColour);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setAmbient(mProjectOptions.SelectionBBColour);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(mProjectOptions.SelectionBBColour);
    }

    result = Ogre::MaterialManager::getSingleton().createOrRetrieve("HighlightBBMaterial", "General");
    if(!result.first.isNull())
    {
        Ogre::MaterialPtr matptrOBBoxManualMaterial = result.first;
        matptrOBBoxManualMaterial->setReceiveShadows(false);
        matptrOBBoxManualMaterial->getTechnique(0)->setLightingEnabled(true);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setDiffuse(mProjectOptions.HighlightBBColour);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setAmbient(mProjectOptions.HighlightBBColour);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(mProjectOptions.HighlightBBColour);
    }

    result = Ogre::MaterialManager::getSingleton().createOrRetrieve("SelectHighlightBBMaterial", "General");
    if(!result.first.isNull())
    {
        Ogre::MaterialPtr matptrOBBoxManualMaterial = result.first;
        matptrOBBoxManualMaterial->setReceiveShadows(false);
        matptrOBBoxManualMaterial->getTechnique(0)->setLightingEnabled(true);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setDiffuse(mProjectOptions.SelectHighlightBBColour);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setAmbient(mProjectOptions.SelectHighlightBBColour);
        matptrOBBoxManualMaterial->getTechnique(0)->getPass(0)->setSelfIllumination(mProjectOptions.SelectHighlightBBColour);
    }

    Ogre::MaterialPtr matPtr = Ogre::MaterialManager::getSingleton().getByName("mtSELECTION", "EditorResources");
    if(!matPtr.isNull())
    {
        matPtr->getTechnique(0)->getPass(0)->setDiffuse(mProjectOptions.SelectionRectColour);
        matPtr->getTechnique(0)->getPass(0)->setAmbient(mProjectOptions.SelectionRectColour);
        matPtr->getTechnique(0)->getPass(0)->setSelfIllumination(mProjectOptions.SelectionRectColour);
        matPtr.setNull();
    }

    if(mActiveViewport)
        mActiveViewport->SetCameraSpeed(mProjectOptions.CameraSpeed);

    mLayerNames.clear();

    for(unsigned int li = 0;li < 31;li++)
    {
        mLayerNames.push_back(PropertyOption(mProjectOptions.LayerNames[li], Ogre::Any(li)));
    }

    setLoadState(LS_LOADED);
    SetSceneModified(false);
    return true;
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::SaveScene(bool SaveAs, Ogre::String exportfile)
{
    COFSSceneSerializer *defaultserializer = OGRE_NEW COFSSceneSerializer();

    if(defaultserializer->Export(SaveAs, exportfile) == SCF_OK)
    {
        //mUndoManager->Clear();
        OGRE_DELETE defaultserializer;
        return true;
    }

    OGRE_DELETE defaultserializer;

    return false;
}
//-----------------------------------------------------------------------------------------
bool OgitorsRoot::TerminateScene()
{
    if(mLoadState == LS_UNLOADED)
        return true;

    if(mIsSceneModified)
    {
        DIALOGRET ret = mSystem->DisplayMessageDialog(OTR("Do you want to save your current project?"),DLGTYPE_YESNOCANCEL);
        switch(ret)
        {
        case DLGRET_YES:SaveScene(false);break;
        case DLGRET_CANCEL:return false;
        case DLGRET_NO:break;
        }
    }

    SetRunState(RS_STOPPED);
    SetEditorTool(TOOL_SELECT);

    if(mSelRect)
    {
        mSelRect->detachFromParent();
        OGRE_DELETE mSelRect;
        mSelRect = 0;
    }

    OGRE_DELETE mClipboardManager;
    mClipboardManager = 0;
    if(mMultiSelection)
        mMultiSelection->destroy(true);
    mMultiSelection = 0;
    mActiveViewport = 0;

    setLoadState(LS_UNLOADED);

    ClearEditors();

    mSystem->ClearTreeItems();
    mSystem->PresentPropertiesView(0);

    mUndoManager->Clear();

    mRenderWindow->removeAllViewports();

    Ogre::ResourceGroupManager * mngr = Ogre::ResourceGroupManager::getSingletonPtr();

    DestroyResourceGroup(PROJECT_RESOURCE_GROUP);
    DestroyResourceGroup(PROJECT_TEMP_RESOURCE_GROUP);

    mMaterialNames.clear();
    mAutoTrackTargets.clear();
    mAutoTrackTargets.push_back(PropertyOption("None",Ogre::Any(Ogre::String("None"))));

    (*mProjectFile).unmount();

    return true;
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::HideSelectionRect()
{
    if(mSelRect)
    {
        mSelRect->detachFromParent();
        OGRE_DELETE mSelRect;
        mSelRect = 0;
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::ShowSelectionRect(Ogre::Vector4 size)
{
    if(mSelRect)
    {
        mSelRect->detachFromParent();
        OGRE_DELETE mSelRect;
    }

    mSelRect = OGRE_NEW Selection2D(true);
    mSelRect->setCorners(size.x,size.y,size.z,size.w);
    mSelRect->setMaterial("mtSELECTION");
    mSelRect->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
    mSelRect->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);

    mSelectionNode->attachObject(mSelRect);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::SetLightVisiblity(bool visibility)
{
    if(mGlobalLightVisiblity == visibility)
        return;

    mGlobalLightVisiblity = visibility;

    NameObjectPairList& theList = mNamesByType[ETYPE_LIGHT];
    NameObjectPairList::const_iterator it = theList.begin();
    while(it != theList.end())
    {
        it->second->showHelper(visibility);
        it++;
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::SetCameraVisiblity(bool visibility)
{
    if(mGlobalCameraVisiblity == visibility)
        return;

    mGlobalCameraVisiblity = visibility;

    NameObjectPairList& theList = mNamesByType[ETYPE_CAMERA];
    NameObjectPairList::const_iterator it = theList.begin();
    while(it != theList.end())
    {
        it->second->showHelper(visibility);
        it++;
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetAutoTrackTargets(Ogre::StringVector &list)
{
    list.clear();
    list.push_back("None");
    NameObjectPairList::const_iterator it = mNameList.begin();
    while(it != mNameList.end())
    {
        if(it->second->isAutoTrackTarget())
            list.push_back(it->first);
        it++;
    }
    std::sort(list.begin() + 1,list.end());
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterToolBar(void *pluginIdentifier, void *toolbar)
{
    PluginEntryMap::iterator it = mPlugins.find(pluginIdentifier);
    if(it != mPlugins.end())
    {
        try
        {
            it->second.mToolbars.push_back(toolbar);
            it->second.mFeatures |= PLUGIN_FEATURE_TOOLBAR;
            mToolBars.push_back(toolbar);
        }
        catch(...)
        {
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterDockWidget(void *pluginIdentifier, const DockWidgetData& data)
{
    PluginEntryMap::iterator it = mPlugins.find(pluginIdentifier);
    if(it != mPlugins.end())
    {
        try
        {
            it->second.mDockWidgets.push_back(data.mHandle);
            it->second.mFeatures |= PLUGIN_FEATURE_DOCKWINDOW;
            mDockWidgets.push_back(data);
        }
        catch(...)
        {
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterTabWidget(void *pluginIdentifier, const TabWidgetData& data)
{
    PluginEntryMap::iterator it = mPlugins.find(pluginIdentifier);
    if(it != mPlugins.end())
    {
        try
        {
            it->second.mTabWidgets.push_back(data.mHandle);
            it->second.mFeatures |= PLUGIN_FEATURE_TABWINDOW;
            mTabWidgets.push_back(data);
        }
        catch(...)
        {
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterPreferenceEditor(void *pluginIdentifier, PreferencesEditorData& data)
{
    PluginEntryMap::iterator it = mPlugins.find(pluginIdentifier);
    if(it != mPlugins.end())
    {
        try
        {
            data.mHandle->setPrefsSectionName(data.mSectionName);
            it->second.mPrefWidgets.push_back(data.mHandle);
            it->second.mFeatures |= PLUGIN_FEATURE_PREFWINDOW;
            mPrefEditors.push_back(data);
        }
        catch(...)
        {
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterScriptInterpreter(void *pluginIdentifier, OgitorsScriptInterpreter *interpreter)
{
    if(!interpreter)
        return;

    PluginEntryMap::iterator it = mPlugins.find(pluginIdentifier);
    if(it != mPlugins.end())
    {
        try
        {
            it->second.mScriptInterpreters.push_back(interpreter->getTypeString());
            it->second.mFeatures |= PLUGIN_FEATURE_SCRIPTINTERPRETER;
            ScriptInterpreterMap::iterator it = mScriptInterpreterList.find(interpreter->getTypeString());
            if(it == mScriptInterpreterList.end())
            {
                mScriptInterpreterList.insert(ScriptInterpreterMap::value_type(interpreter->getTypeString(), interpreter));
            }
        }
        catch(...)
        {
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegisterEditorFactory(void *pluginIdentifier, CBaseEditorFactory *factory)
{
    if(!factory)
        return;

    PluginEntryMap::iterator it = mPlugins.find(pluginIdentifier);
    if(it != mPlugins.end())
    {
        try
        {
            it->second.mEditorObjects.push_back(factory->getTypeName());
            it->second.mFeatures |= PLUGIN_FEATURE_FACTORY;
            _RegisterEditorFactory(factory);
        }
        catch(...)
        {
        }
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::_RegisterEditorFactory(CBaseEditorFactory *factory)
{
    EditorObjectFactoryMap::iterator it = mEditorObjectFactories.find(factory->getTypeName());
    if(it == mEditorObjectFactories.end())
    {
        mEditorObjectFactories.insert(EditorObjectFactoryMap::value_type(factory->getTypeName(), factory));
        factory->mTypeID = EditorObjectTypeIDCounter++;
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::ToggleLayerVisibility(int index)
{
    mProjectOptions.LayerVisible[index] = !mProjectOptions.LayerVisible[index];
    Ogre::SceneManager *manager = GetSceneManager();
    if(manager)
    {
        unsigned int vismask = manager->getVisibilityMask();
        unsigned int curmask = 1 << index;
        if(mProjectOptions.LayerVisible[index])
            vismask |= curmask;
        else
            vismask &= ~curmask;

        manager->setVisibilityMask(vismask);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::SetRunState(RunState state)
{
    if(mLoadState != LS_LOADED)
        return;

    if(state == RS_STOPPED)
    {
        UndoCollection * coll = mUndoManager->EndCollection();
        if(coll)
        {
            mUndoManager->AddUndo(coll);
            mUndoManager->Undo();
        }

        mUpdateScriptList.clear();
    }
    else if(state == RS_RUNNING  && mRunState == RS_STOPPED)
    {
        mUndoManager->BeginCollection("Run Scene Scripts");

        mUpdateScriptList.clear();

        NameObjectPairList::iterator it = mNameList.begin();
        while(it != mNameList.end())
        {
            if(!it->second->getUpdateScript().empty())
                RegisterForScriptUpdates(it->second);

            it++;
        }
    }

    mRunState = state;

    RunStateChangeEvent evt(state);
    EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::setLoadState(LoadState state)
{
    if(mLoadState != state)
    {
        mLoadState = state;

        LoadStateChangeEvent evt(state);
        EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::SetEditorTool(unsigned int tool)
{
    if(mEditorTool != tool)
    {
        mEditorTool = tool;

        EditorToolChangeEvent evt(tool);
        EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::SetSceneModified(bool modified)
{
    if(mIsSceneModified != modified)
    {
        mIsSceneModified = modified;

        SceneModifiedChangeEvent evt(modified);
        EventManager::getSingletonPtr()->sendEvent(this, 0, &evt);
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::ChangeSceneModified(bool state)
{
    SetSceneModified(mIsSceneModified | state);
}

}

