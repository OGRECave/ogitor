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
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "CameraEditor.h"
#include "ViewportEditor.h"
#include "PGInstanceManager.h"
#include "PGInstanceEditor.h"
#include "OgitorsUndoManager.h"
#include "tinyxml.h"
#include "ofs.h"

#include "PagedGeometry.h"
#include "BatchPage.h"
#include "ImpostorPage.h"
#include "TreeLoader3D.h"

using namespace Ogitors;
using namespace Forests;

//-----------------------------------------------------------------------------------------
namespace Ogitors
{
    class AddInstanceUndo : public OgitorsUndoBase
    {
    public:
        AddInstanceUndo(unsigned int objectID, int index) : mObjectID(objectID), mIndex(index) {};
        virtual bool apply();

    protected:
        unsigned int  mObjectID;
        int           mIndex;
    };
    //-----------------------------------------------------------------------------------------
    class RemoveInstanceUndo : public OgitorsUndoBase
    {
    public:
        RemoveInstanceUndo(unsigned int objectID, Ogre::Vector3 pos, Ogre::Real scale, Ogre::Real yaw) : mObjectID(objectID), mPos(pos), mScale(scale), mYaw(yaw) {};
        virtual bool apply();

    protected:
        unsigned int  mObjectID;
        Ogre::Vector3 mPos;
        Ogre::Real    mScale;
        Ogre::Real    mYaw;
    };
}
//-----------------------------------------------------------------------------------------
bool AddInstanceUndo::apply()
{
    CPGInstanceManager *man = static_cast<CPGInstanceManager*>(OgitorsRoot::getSingletonPtr()->FindObject(mObjectID));
    if(man)
    {
        PGInstanceInfo info = man->getInstanceInfo(mIndex);

        man->_deleteChildEditor(mIndex);
        man->removeInstance(mIndex);

        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW RemoveInstanceUndo(mObjectID, info.pos, info.scale, info.yaw));
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool RemoveInstanceUndo::apply()
{
    CPGInstanceManager *man = static_cast<CPGInstanceManager *>(OgitorsRoot::getSingletonPtr()->FindObject(mObjectID));
    if(man)
    {
        int index = man->addInstance(mPos, mScale, mYaw);

        man->_createChildEditor(index , mPos, mScale, mYaw);

        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW AddInstanceUndo(mObjectID, index));
    }

    return true;
}
//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CPGInstanceManager::CPGInstanceManager(CBaseEditorFactory *factory) : CBaseEditor(factory),
mHandle(0), mPGHandle(0), mEntityHandle(0), mPlacementMode(false), mNextInstanceIndex(0)
{
    mHelper = 0;
    mUsesGizmos = false;
    mUsesHelper = false;
    mHideChildrenInProgress = false;
    mLastFileName = "";
    mUsingPlaceHolderMesh = false;
    mTempFileName = "";
    mShowChildren = false;
}
//-----------------------------------------------------------------------------------------
CPGInstanceManager::~CPGInstanceManager()
{

}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CPGInstanceManager::getAABB()
{
    if(mEntityHandle)
        return mEntityHandle->getBoundingBox();
    else
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::update(float timePassed)
{
    if(mPGHandle)
        mPGHandle->update();

    return false;
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::showBoundingBox(bool bShow)
{
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::setLayerImpl(unsigned int newlayer)
{
    if(mEntityHandle)
        mEntityHandle->setVisibilityFlags(1 << newlayer);

    if(mLoaded->get())
    {
        unLoad();
        load();
    }

    return true;
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::setSelectedImpl(bool bSelected)
{
    if(!bSelected)
    {
        mPlacementMode = false;
        mOgitorsRoot->ReleaseMouse();
    }
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::_save(Ogre::String filename)
{
    std::stringstream stream;

    PGInstanceList::iterator it = mInstanceList.begin();

    while(it != mInstanceList.end())
    {
        stream << Ogre::StringConverter::toString(it->first).c_str();
        stream << ";";
        stream << Ogre::StringConverter::toString(it->second.pos).c_str();
        stream << ";";
        stream << Ogre::StringConverter::toString(it->second.scale).c_str();
        stream << ";";
        stream << Ogre::StringConverter::toString(it->second.yaw).c_str();
        stream << "\n";

        it++;
    }

    OgitorsUtils::SaveStreamOfs(stream, filename);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::onSave(bool forced)
{
    Ogre::String dir = "/PGInstances/";
    OFS::OfsPtr& mFile = mOgitorsRoot->GetProjectFile();
    mFile->createDirectory(dir.c_str());

    Ogre::String name = mName->get();
    std::replace(name.begin(), name.end(), '<', ' ');
    std::replace(name.begin(), name.end(), '>', ' ');
    std::replace(name.begin(), name.end(), '#', ' ');

    if(!mLastFileName.empty())
        mFile->deleteFile(mLastFileName.c_str());

    if(!mTempFileName.empty())
        mFile->deleteFile(mTempFileName.c_str());

    Ogre::String filename = dir + Ogre::StringConverter::toString(mObjectID->get()) + "_" + name + ".instance";

    mLastFileName = filename;

    _save(filename);

    mTempModified->set(false);
    mTempFileName = "";
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::_onLoad()
{
    Ogre::String filename;

    if(mTempModified->get())
    {
        if(mTempFileName.empty())
            mTempFileName = "/Temp/tmp" + Ogre::StringConverter::toString(mObjectID->get()) + ".instance";

        filename = mTempFileName;
    }
    else
    {
        Ogre::String name = mName->get();
        std::replace(name.begin(), name.end(), '<', ' ');
        std::replace(name.begin(), name.end(), '>', ' ');
        std::replace(name.begin(), name.end(), '#', ' ');

        filename = "/PGInstances/" + Ogre::StringConverter::toString(mObjectID->get()) + "_" + name + ".instance";
    }

    OFS::OFSHANDLE handle;

    OFS::OfsPtr& mFile = mOgitorsRoot->GetProjectFile();

    OFS::OfsResult ret = mFile->openFile(handle, filename.c_str());

    if(ret != OFS::OFS_OK)
        return;

    unsigned int file_size = 0;

    mFile->getFileSize(handle, file_size);

    if(file_size == 0)
    {
        mFile->closeFile(handle);
        return;
    }

    char *buffer = new char[file_size];
    mFile->read(handle, buffer, file_size);

    std::stringstream stream;
    stream << buffer;

    delete [] buffer;

    if(!mTempModified->get())
        mLastFileName = filename;

    Ogre::StringVector list;

    char res[128];

    while(!stream.eof())
    {
        stream.getline(res, 128);
        Ogre::String resStr(res);
        OgitorsUtils::ParseStringVector(resStr, list);

        if(list.size() == 3)
        {
            PGInstanceInfo info;

            info.pos = Ogre::StringConverter::parseVector3(list[0]);
            info.scale = Ogre::StringConverter::parseReal(list[1]);
            info.yaw = Ogre::StringConverter::parseReal(list[2]);

            mInstanceList[mNextInstanceIndex++] = info;
        }
        else if(list.size() == 4)
        {
            PGInstanceInfo info;

            int index = Ogre::StringConverter::parseInt(list[0]);
            info.pos = Ogre::StringConverter::parseVector3(list[1]);
            info.scale = Ogre::StringConverter::parseReal(list[2]);
            info.yaw = Ogre::StringConverter::parseReal(list[3]);
            info.instance = 0;

            mInstanceList[index] = info;

            if(index >= mNextInstanceIndex)
                mNextInstanceIndex = index + 1;
        }
    }
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::getObjectContextMenu(UTFStringVector &menuitems)
{
    menuitems.clear();

    if(!mEntityHandle)
        return false;

    if(mPlacementMode)
        menuitems.push_back(OTR("Stop Placement"));
    else
        menuitems.push_back(OTR("Start Placement"));

    if(mInstanceList.size())
    {
        if(mShowChildren)
            menuitems.push_back(OTR("Hide Children"));
        else
            menuitems.push_back(OTR("Show Children"));
    }

    return true;
}
//-------------------------------------------------------------------------------
void CPGInstanceManager::onObjectContextMenu(int menuresult)
{
    if(menuresult == 0)
    {
        mPlacementMode = !mPlacementMode;

        if(mPlacementMode)
            mOgitorsRoot->CaptureMouse(this);
        else
            mOgitorsRoot->ReleaseMouse();
    }
    else if(menuresult == 1)
    {
        if(mShowChildren)
        {
            mHideChildrenInProgress = true;

            NameObjectPairList::iterator i, iend;
            iend = mChildren.end();

            for (i = mChildren.begin(); i != iend; ++i)
            {
                mSystem->DeleteTreeItem(i->second);
                i->second->destroy();
            }

            mChildren.clear();

            mHideChildrenInProgress = false;
            mShowChildren = false;
        }
        else
        {
            mShowChildren = true;
            PGInstanceList::iterator it = mInstanceList.begin();

            while(it != mInstanceList.end())
            {
                _createChildEditor(it->first, it->second.pos, it->second.scale, it->second.yaw);
                it++;
            }
        }
    }
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mModel           , "model"           , Ogre::String , "" , 0, SETTER(Ogre::String, CPGInstanceManager, _setModel));
    PROPERTY_PTR(mPageSize        , "pagesize"        , int          , 75 , 0, SETTER(int, CPGInstanceManager, _setPageSize));
    PROPERTY_PTR(mBatchDistance   , "batchdistance"   , int          , 75 , 0, SETTER(int, CPGInstanceManager, _setBatchDistance));
    PROPERTY_PTR(mImpostorDistance, "impostordistance", int          , 1000, 0, SETTER(int, CPGInstanceManager, _setImpostorDistance));
    PROPERTY_PTR(mBounds          , "bounds"          , Ogre::Vector4, Ogre::Vector4(-10000,-10000,10000,10000), 0, SETTER(Ogre::Vector4, CPGInstanceManager, _setBounds));
    PROPERTY_PTR(mCastShadows     , "castshadows"     , bool         ,false,0, SETTER(bool, CPGInstanceManager, _setCastShadows));
    PROPERTY_PTR(mTempModified    , "tempmodified"    , bool         ,false,0, 0);
    PROPERTY_PTR(mMinScale        , "randomizer::minscale", Ogre::Real,1.0f,0, 0);
    PROPERTY_PTR(mMaxScale        , "randomizer::maxscale", Ogre::Real,1.0f,0, 0);
    PROPERTY_PTR(mMinYaw          , "randomizer::minyaw", Ogre::Real , 0.0f,0, 0);
    PROPERTY_PTR(mMaxYaw          , "randomizer::maxyaw", Ogre::Real , 0.0f,0, 0);

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CPGInstanceManager::exportDotScene(TiXmlElement *pParent)
{
    Ogre::String name = mName->get();
    std::replace(name.begin(), name.end(), '<', ' ');
    std::replace(name.begin(), name.end(), '>', ' ');
    std::replace(name.begin(), name.end(), '#', ' ');

    name = Ogre::StringConverter::toString(mObjectID->get()) + "_" + name;

    Ogre::String filename = "PGInstances/" + name + ".instance";

    TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("node"))->ToElement();

    // node properties
    pNode->SetAttribute("name", mName->get().c_str());
    pNode->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    // position
    TiXmlElement *pPosition = pNode->InsertEndChild(TiXmlElement("position"))->ToElement();
    pPosition->SetAttribute("x", "0");
    pPosition->SetAttribute("y", "0");
    pPosition->SetAttribute("z", "0");
    // rotation
    TiXmlElement *pRotation = pNode->InsertEndChild(TiXmlElement("rotation"))->ToElement();
    pRotation->SetAttribute("qw", "1");
    pRotation->SetAttribute("qx", "0");
    pRotation->SetAttribute("qy", "0");
    pRotation->SetAttribute("qz", "0");
    // scale
    TiXmlElement *pScale = pNode->InsertEndChild(TiXmlElement("scale"))->ToElement();
    pScale->SetAttribute("x", "1");
    pScale->SetAttribute("y", "1");
    pScale->SetAttribute("z", "1");
    
    
    TiXmlElement *pPG = pNode->InsertEndChild(TiXmlElement("pagedgeometry"))->ToElement();
    pPG->SetAttribute("fileName", filename.c_str());
    pPG->SetAttribute("model", mModel->get().c_str());
    pPG->SetAttribute("pageSize", Ogre::StringConverter::toString(mPageSize->get()).c_str());
    pPG->SetAttribute("batchDistance", Ogre::StringConverter::toString(mBatchDistance->get()).c_str());
    pPG->SetAttribute("impostorDistance", Ogre::StringConverter::toString(mImpostorDistance->get()).c_str());
    pPG->SetAttribute("bounds", Ogre::StringConverter::toString(mBounds->get()).c_str());
    pPG->SetAttribute("castShadows", Ogre::StringConverter::toString(mCastShadows->get()).c_str());

    return pPG;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::load(bool async)
{
    if(mLoaded->get())
        return true;

    Ogre::String tmpDir = OgitorsUtils::QualifyPath(mOgitorsRoot->GetProjectOptions()->ProjectDir + "/Temp") + "/";

    mPGHandle = new PagedGeometry();
    mPGHandle->setCamera(mOgitorsRoot->GetViewport()->getCameraEditor()->getCamera());
    mPGHandle->setPageSize(mPageSize->get());
    mPGHandle->setInfinite();
    mPGHandle->setTempDir(tmpDir);

    mPGHandle->addDetailLevel<BatchPage>(mBatchDistance->get(),0);
    mPGHandle->addDetailLevel<ImpostorPage>(mImpostorDistance->get(),0);

    Ogre::Vector4 bounds = mBounds->get();
    mHandle = new Forests::TreeLoader3D(mPGHandle, Forests::TBounds(bounds.x, bounds.y, bounds.z, bounds.w));
    mPGHandle->setPageLoader(mHandle);

    if(mInstanceList.size() == 0)
        _onLoad();

    if(mModel->get() != "")
    {
        try
        {
            mEntityHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), mModel->get() + ".mesh", PROJECT_RESOURCE_GROUP);
            mUsingPlaceHolderMesh = false;
        }
        catch(...)
        {
            mUsingPlaceHolderMesh = true;
            mEntityHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), "missing_mesh.mesh");
            mEntityHandle->setMaterialName("MAT_GIZMO_X_L");
        }

        mEntityHandle->setQueryFlags(0);
        mEntityHandle->setVisibilityFlags(1 << mLayer->get());
        mEntityHandle->setCastShadows(mCastShadows->get());

        PGInstanceList::iterator it = mInstanceList.begin();

        while(it != mInstanceList.end())
        {
            mHandle->addTree(mEntityHandle, it->second.pos, Ogre::Degree(it->second.yaw), it->second.scale);

            it++;
        }
    }

    registerForUpdates();

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::unLoad()
{
    if(!mLoaded->get())
        return true;

    if(mOgitorsRoot->GetLoadState() != LS_UNLOADED)
    {
        if(mTempFileName.empty())
        {
            mTempFileName = "/Temp/tmp" + Ogre::StringConverter::toString(mObjectID->get()) + ".instance";
        }

        _save(mTempFileName);
        mTempModified->set(true);
    }

    unRegisterForUpdates();

    if(mHandle)
        delete mHandle;

    if(mPGHandle)
        delete mPGHandle;

    if(mEntityHandle)
        mOgitorsRoot->GetSceneManager()->destroyEntity(mEntityHandle);

    mHandle = 0;
    mPGHandle = 0;
    mEntityHandle = 0;

    if(mPlacementMode)
        mOgitorsRoot->ReleaseMouse();

    mPlacementMode = false;

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
PGInstanceInfo CPGInstanceManager::getInstanceInfo(int index)
{
    PGInstanceList::iterator it = mInstanceList.find(index);

    if(it != mInstanceList.end())
    {
        return it->second;
    }

    return PGInstanceInfo();
}
//-----------------------------------------------------------------------------------------
int CPGInstanceManager::addInstance(const Ogre::Vector3& pos, const Ogre::Real& scale, const Ogre::Real& yaw)
{
    if(!mEntityHandle || !mHandle)
        return -1;

    mHandle->addTree(mEntityHandle, pos, Ogre::Degree(yaw), scale);

    PGInstanceInfo instance;
    instance.pos = pos;
    instance.scale = scale;
    instance.yaw = yaw;
    instance.instance = 0;

    int result = mNextInstanceIndex++;
    mInstanceList.insert(PGInstanceList::value_type(result, instance));

    return result;
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::removeInstance(int index)
{
    if(!mLoaded->get() || index == -1)
        return;

    PGInstanceList::iterator it = mInstanceList.find(index);

    if(it != mInstanceList.end())
    {
        it->second.instance = 0;
        if(!mHideChildrenInProgress)
        {
            mHandle->deleteTrees(it->second.pos, 0.01f, mEntityHandle);
            mInstanceList.erase(it);
        }
    }
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::modifyInstancePosition(int index, const Ogre::Vector3& pos)
{
    if(!mHandle || index == -1)
        return;

    PGInstanceList::iterator it = mInstanceList.find(index);

    if(it != mInstanceList.end())
    {
        if(mEntityHandle)
        {
            mHandle->deleteTrees(it->second.pos, 0.01f, mEntityHandle);
            mHandle->addTree(mEntityHandle, pos, Ogre::Degree(it->second.yaw), it->second.scale);
        }

        it->second.pos = pos;
    }
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::modifyInstanceScale(int index, Ogre::Real scale)
{
    if(!mHandle || index == -1)
        return;

    PGInstanceList::iterator it = mInstanceList.find(index);

    if(it != mInstanceList.end())
    {
        it->second.scale = scale;
        if(mEntityHandle)
        {
            mHandle->deleteTrees(it->second.pos, 0.01f, mEntityHandle);
            mHandle->addTree(mEntityHandle, it->second.pos, Ogre::Degree(it->second.yaw), it->second.scale);
        }
    }
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::modifyInstanceYaw(int index, Ogre::Real yaw)
{
    if(!mHandle || index == -1)
        return;

    PGInstanceList::iterator it = mInstanceList.find(index);

    if(it != mInstanceList.end())
    {
        it->second.yaw = yaw;
        if(mEntityHandle)
        {
            mHandle->deleteTrees(it->second.pos, 0.01f, mEntityHandle);
            mHandle->addTree(mEntityHandle, it->second.pos, Ogre::Degree(it->second.yaw), it->second.scale);
        }
    }
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::_setModel(OgitorsPropertyBase* property, const Ogre::String& value)
{
    Ogre::String newname = "PGInstance<" + value + ">";
    newname += mOgitorsRoot->CreateUniqueID(newname,"");

    if(mPlacementMode)
    {
        mPlacementMode = false;
        mOgitorsRoot->ReleaseMouse();
    }

    mModel->init(value);
    mName->set(newname);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::_setPageSize(OgitorsPropertyBase* property, const int& value)
{
    if(value < 10)
        return false;

    if(mLoaded->get())
    {
        unLoad();
        load();
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::_setBatchDistance(OgitorsPropertyBase* property, const int& value)
{
    if(value < 50)
        return false;

    if(mLoaded->get())
    {
        unLoad();
        load();
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::_setImpostorDistance(OgitorsPropertyBase* property, const int& value)
{
    if(value < (mBatchDistance->get() + 50))
        return false;

    if(mLoaded->get())
    {
        unLoad();
        load();
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::_setBounds(OgitorsPropertyBase* property, const Ogre::Vector4& value)
{
    if(mLoaded->get())
    {
        unLoad();
        load();
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CPGInstanceManager::_setCastShadows(OgitorsPropertyBase* property, const bool& value)
{
    if(mEntityHandle)
    {
        mEntityHandle->setCastShadows(value);
    }

    if(mPGHandle)
        mPGHandle->reloadGeometry();

    return true;
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::_createChildEditor(int index, Ogre::Vector3 pos, Ogre::Real scale, Ogre::Real yaw)
{
    if(index == -1 || !mShowChildren)
        return;

    //We do not want an UNDO to be created for creation of children
    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Eat Creation");

    OgitorsPropertyValueMap params;
    params["init"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any(Ogre::String("")));
    params["position"] = OgitorsPropertyValue(PROP_VECTOR3, Ogre::Any(pos));
    params["index"] = OgitorsPropertyValue(PROP_INT, Ogre::Any(index));
    params["scale"] = OgitorsPropertyValue(PROP_VECTOR3, Ogre::Any(Ogre::Vector3(scale, scale, scale)));
    params["uniformscale"] = OgitorsPropertyValue(PROP_REAL, Ogre::Any(scale));
    params["yaw"] = OgitorsPropertyValue(PROP_REAL, Ogre::Any(yaw));
    Ogre::Quaternion q1;
    q1.FromAngleAxis(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);
    params["orientation"] = OgitorsPropertyValue(PROP_QUATERNION, Ogre::Any(q1));

    mInstanceList[index].instance = static_cast<CPGInstanceEditor*>(mOgitorsRoot->CreateEditorObject(this, "PGInstance Object", params, true, false));

    OgitorsUndoManager::getSingletonPtr()->EndCollection(false, true);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::_deleteChildEditor(int index)
{
    if(index == -1 || !mShowChildren)
        return;

    //We do not want an UNDO to be created for deletion of children
    OgitorsUndoManager::getSingletonPtr()->BeginCollection("Eat Deletion");

    PGInstanceList::iterator it = mInstanceList.find(index);

    if(it != mInstanceList.end())
    {
        mHideChildrenInProgress = true;
        if(it->second.instance)
        {
            mSystem->DeleteTreeItem(it->second.instance);
            it->second.instance->destroy(true);
            it->second.instance = 0;
        }
        mHideChildrenInProgress = false;
    }

    OgitorsUndoManager::getSingletonPtr()->EndCollection(false, true);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseLeftUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseLeftDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    Ogre::Camera *cam = viewport->getCameraEditor()->getCamera();
    Ogre::Viewport *vp = static_cast<Ogre::Viewport*>(viewport->getHandle());

    float width = vp->getActualWidth();
    float height = vp->getActualHeight();
    Ogre::Ray mRay = cam->getCameraToViewportRay(point.x / width, point.y / height);

    Ogre::Vector3 vPos;
    if(viewport->GetHitPosition(mRay, vPos))
    {
        float yaw = (mMaxYaw->get() - mMinYaw->get()) * Ogre::Math::UnitRandom() + mMinYaw->get();
        float scale = (mMaxScale->get() - mMinScale->get()) * Ogre::Math::UnitRandom() + mMinScale->get();
        
        int index = addInstance(vPos, scale, yaw);

        OgitorsUndoManager::getSingletonPtr()->BeginCollection("Add Instance");

        _createChildEditor(index , vPos, scale, yaw);

        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW AddInstanceUndo(mObjectID->get(), index));
        OgitorsUndoManager::getSingletonPtr()->EndCollection(true);
    }
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseMove (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    viewport->OnMouseMove(point, buttons & ~OMB_LEFT);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseLeave (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    viewport->OnMouseLeave(point, buttons);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseRightDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    viewport->OnMouseRightDown(point, buttons);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseRightUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    viewport->OnMouseRightUp(point, buttons);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseMiddleDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    viewport->OnMouseMiddleDown(point, buttons);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseMiddleUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons)
{
    viewport->OnMouseMiddleUp(point, buttons);
}
//-----------------------------------------------------------------------------------------
void CPGInstanceManager::OnMouseWheel (CViewportEditor *viewport, Ogre::Vector2 point, float delta, unsigned int buttons)
{
    viewport->OnMouseWheel(point, delta, buttons);
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//------CMATERIALEDITORFACTORY-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CPGInstanceManagerFactory::CPGInstanceManagerFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "PGInstance Manager Object";
    mEditorType = ETYPE_CUSTOM_MANAGER;
    mAddToObjectList = true;
    mRequirePlacement = false;
    mIcon = "Icons/pagedgeometry.svg";
    mCapabilities = CAN_UNDO | CAN_DELETE;

    OgitorsPropertyDef *definition = AddPropertyDefinition("model", "Model", "The model to be used.", PROP_STRING);
    definition->setOptions(OgitorsRoot::getSingletonPtr()->GetModelNames());
    AddPropertyDefinition("pagesize", "Page Size", "Size of batch pages.",PROP_INT);
    AddPropertyDefinition("batchdistance", "Batch Distance", "Batch Distance.",PROP_INT);
    AddPropertyDefinition("impostordistance", "Impostor Distance", "Impostor Distance.",PROP_INT);
    definition = AddPropertyDefinition("bounds", "Bounds", "The dimensions of the bounding area.",PROP_VECTOR4);
    definition->setFieldNames("X1","Z1","X2","Z2");
    AddPropertyDefinition("castshadows","Cast Shadows","Do the instances cast shadows?",PROP_BOOL);
    AddPropertyDefinition("tempmodified", "", "Is it temporarily modified.", PROP_BOOL);

    AddPropertyDefinition("randomizer::minscale", "Randomizer::Min. Scale", "Minimum Scale of new objects.",PROP_REAL);
    AddPropertyDefinition("randomizer::maxscale", "Randomizer::Max. Scale", "Maximum Scale of new objects.",PROP_REAL);
    AddPropertyDefinition("randomizer::minyaw", "Randomizer::Min. Yaw", "Minimum Yaw of new objects.",PROP_REAL);
    AddPropertyDefinition("randomizer::maxyaw", "Randomizer::Max. Yaw", "Maximum Yaw of new objects.",PROP_REAL);


    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("name");
    it->second.setAccess(true, false);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CPGInstanceManagerFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = new CPGInstanceManagerFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CPGInstanceManagerFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CPGInstanceManager *object = new CPGInstanceManager(this);
    OgitorsPropertyValueMap::iterator ni;

    if((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("PGInstance<>"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
