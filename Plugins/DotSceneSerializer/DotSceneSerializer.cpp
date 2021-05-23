///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include "DotSceneSerializer.h"
#include "ofs.h"
#include "OgreTerrain.h"

using namespace Ogitors;

//----------------------------------------------------------------------------
OgitorsPropertyValue parseVector3(TiXmlElement *XMLNode)
{
    OgitorsPropertyValue value;
    Ogre::Vector3 val = Ogre::Vector3(0,0,0);
    
    value.propType = PROP_VECTOR3;

    if(XMLNode->Attribute("x"))
        val.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("x"));

    if(XMLNode->Attribute("y"))
        val.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("y"));

    if(XMLNode->Attribute("z"))
        val.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("z"));

    value.val = Ogre::Any(val);
    
    return value;
}
//----------------------------------------------------------------------------
CDotSceneSerializer::CDotSceneSerializer() : CBaseSerializer("DotScene Serializer", CAN_EXPORT | CAN_IMPORT) 
{
}

static void connectElements(Ogre::SceneNode* node, CBaseEditor* editorNode)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
    propValue.propType = PROP_STRING;

    auto eroot = Ogitors::OgitorsRoot::getSingletonPtr();
    for(auto mo : node->getAttachedObjects())
    {
        propValue.val = mo->getName();
        if(mo->getName().empty() || Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(mo->getName()))
            propValue.val = mo->getMovableType() + eroot->CreateUniqueID(mo->getMovableType(),"");
        params["name"] = propValue;

        if(mo->getMovableType() == "Entity")
        {
            auto e = static_cast<CEntityEditor*>(eroot->CreateEditorObject(editorNode, "Entity", params, false, false));
            e->_setEntity(static_cast<Ogre::Entity*>(mo));
        }
        else if(mo->getMovableType() == "Light")
        {
            auto e = static_cast<CLightEditor*>(eroot->CreateEditorObject(editorNode, "Light", params, false, false));
            e->_setHandle(static_cast<Ogre::Light*>(mo));
        }
        else if(mo->getMovableType() == "Camera")
        {
            auto e = static_cast<CCameraEditor*>(eroot->CreateEditorObject(editorNode, "Camera", params, false, false));
            e->_setHandle(static_cast<Ogre::Camera*>(mo));
        }
    }

    for(auto n : node->getChildren())
    {
        propValue.val = n->getName();
        if(n->getName().empty() || Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(n->getName()))
            propValue.val = "Node" + eroot->CreateUniqueID("Node","");
        params["name"] = propValue;

        auto e = static_cast<CNodeEditor*>(eroot->CreateEditorObject(editorNode, "Node", params, false, false));
        e->_setHandle(static_cast<Ogre::SceneNode*>(n));
        connectElements(static_cast<Ogre::SceneNode*>(n), e);
    }
}

//----------------------------------------------------------------------------
int CDotSceneSerializer::Import(Ogre::String importfile)
{
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    OgitorsSystem *mSystem = OgitorsSystem::getSingletonPtr();

    if(importfile == "")
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("DotScene File"));
        extlist.push_back("*.scene");
        extlist.push_back(OTR("DotScene File"));
        extlist.push_back("*.xml");

        importfile = mSystem->DisplayOpenDialog(OTR("Import DotScene File"), extlist, "");
        if(importfile == "") return SCF_CANCEL;

        mSystem->SetSetting("system", "oldOpenPath", OgitorsUtils::ExtractFilePath(importfile));
    }

    ogRoot->ClearProjectOptions();

    Ogre::String filePath = OgitorsUtils::ExtractFilePath(importfile);
    Ogre::String fileName = OgitorsUtils::ExtractFileName(importfile);

    PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();
    pOpt->CreatedIn = "";

    if(filePath.find(".") == 0)
    {
        filePath = OgitorsUtils::GetExePath() + filePath;
        filePath = OgitorsUtils::QualifyPath(filePath);
    }

    pOpt->ProjectDir = filePath;
    int typepos = fileName.find_last_of(".");
    pOpt->ProjectName = fileName;
    if(typepos != -1)
        pOpt->ProjectName.erase(typepos,pOpt->ProjectName.length() - typepos);
    
    pOpt->SceneManagerName = "OctreeSceneManager";
    pOpt->ResourceDirectories.push_back("/");
    pOpt->ResourceDirectories.push_back(filePath);
    // compatibility with ogitor export structure
    pOpt->ResourceDirectories.push_back(filePath+"Models/");
    pOpt->ResourceDirectories.push_back(filePath+"Materials/");
    pOpt->TerrainDirectory = "Terrain";
    pOpt->ResourceDirectories.push_back(filePath+"Terrain/terrain/");
    pOpt->ResourceDirectories.push_back(filePath+"Terrain/textures/diffusespecular/");
    pOpt->ResourceDirectories.push_back(filePath+"Terrain/textures/normalheight/");

    pOpt->CameraPositions[0] = Ogre::Vector3(0,10,0);
    pOpt->CameraOrientations[0] = Ogre::Quaternion::IDENTITY;
    pOpt->CameraPositions[1] = Ogre::Vector3(0,10,0);
    pOpt->CameraOrientations[1] = Ogre::Quaternion::IDENTITY;
    pOpt->CameraSaveCount = 1;

    OFS::OfsPtr& ofsFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

    Ogre::String ofs_file_name = OgitorsUtils::QualifyPath(filePath + "/" + pOpt->ProjectName + ".ofs");
    
    if(ofsFile.mount(ofs_file_name.c_str(), OFS::OFS_MOUNT_CREATE) != OFS::OFS_OK)
        return SCF_ERRFILE;

    OgitorsUtils::CopyDirOfs(filePath, "/");

    ofsFile->deleteFile(fileName.c_str());

    ofs_file_name = OgitorsUtils::ExtractFileName(ofs_file_name);

    ofsFile->deleteFile(ofs_file_name.c_str());
    
    ogRoot->PrepareProjectResources();

    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
    
    propValue.propType = PROP_STRING;
    propValue.val = Ogre::Any(pOpt->SceneManagerConfigFile);
    params["configfile"] = propValue;
        
    Ogre::Vector2 vClipping(1,1000);

    propValue.propType = PROP_STRING;
    propValue.val = Ogre::Any(Ogre::String("SceneManager1"));
    params["name"] = propValue;
    CSceneManagerEditor *mngred = static_cast<CSceneManagerEditor*>(ogRoot->CreateEditorObject(0, pOpt->SceneManagerName,params,false,false));

    // required to load terrain. TODO connect with CTerrainGroupEditor
    new Ogre::TerrainGlobalOptions();

    Ogre::ResourceGroupManager::getSingleton().setWorldResourceGroupName(PROJECT_RESOURCE_GROUP);
    try
    {
        mngred->getSceneManager()->getRootSceneNode()->loadChildren(importfile);
    }
    catch(const Ogre::Exception& e)
    {
        Ogre::LogManager::getSingleton().logError(e.getDescription());
        return SCF_ERROGRE;
    }

    connectElements(mngred->getSceneManager()->getRootSceneNode(), mngred);

    ogRoot->AfterLoadScene();
    ogRoot->GetViewport()->getCameraEditor()->setClipDistance(vClipping);
    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::ReadParticle(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
 
    propValue.propType = PROP_STRING;
    Ogre::String name = ValidAttr(element->Attribute("name"));
    if(Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(name))
        propValue.val = Ogre::Any(Ogre::String("Particle") + Ogitors::OgitorsRoot::getSingletonPtr()->CreateUniqueID("Particle",""));
    else
        propValue.val = Ogre::Any(name);

    params["name"] = propValue;

    propValue.val = Ogre::String(ValidAttr(element->Attribute("file")));
    params["particle"] = propValue;
    
    *ret = Ogitors::OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, "Particle", params, false, false);
    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::ReadPlane(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
 
    propValue.propType = PROP_STRING;
    Ogre::String name = ValidAttr(element->Attribute("name"));
    if(Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(name))
        propValue.val = Ogre::Any(Ogre::String("Plane") + Ogitors::OgitorsRoot::getSingletonPtr()->CreateUniqueID("Plane",""));
    else
        propValue.val = Ogre::Any(name);

    params["name"] = propValue;
    propValue.val = Ogre::String(ValidAttr(element->Attribute("material")));
    params["material"] = propValue;

    propValue.propType = PROP_REAL;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("distance"))));
    params["distance"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("width"))));
    params["width"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("height"))));
    params["height"] = propValue;

    propValue.propType = PROP_INT;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("xSegments"), "1")));
    params["xsegments"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("ySegments"), "1")));
    params["ysegments"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("numTexCoordSets"), "1")));
    params["numcoordsets"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("uTile"), "1")));
    params["utile"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("vTile"), "1")));
    params["vtile"] = propValue;
    propValue.propType = PROP_BOOL;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseBool(ValidAttr(element->Attribute("normals"), "true")));
    params["hasnormals"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseBool(ValidAttr(element->Attribute("castShadows"), "true")));
    params["castshadows"] = propValue;
    
    TiXmlElement *normal = element->FirstChildElement("normal");
    if(normal)
    {
        params["normal"] = parseVector3(normal);
    }

    *ret = Ogitors::OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, "Plane", params, false, false);
    return SCF_OK;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
CDotSceneSerializer *DotSceneSerializer = 0;

bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Dot Scene Serializer Plugin";
    DotSceneSerializer = OGRE_NEW CDotSceneSerializer();
    OgitorsRoot::getSingletonPtr()->RegisterSerializer(identifier, DotSceneSerializer);
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "Dot Scene Serializer Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    OGRE_DELETE DotSceneSerializer;
    DotSceneSerializer = 0;
    return true;
}
//----------------------------------------------------------------------------
