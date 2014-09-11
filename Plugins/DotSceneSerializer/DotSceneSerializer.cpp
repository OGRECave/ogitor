///////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////

#include "DotSceneSerializer.h"
#include "ofs.h"

using namespace Ogitors;

//----------------------------------------------------------------------------
OgitorsPropertyValue parseQuaternion(TiXmlElement *XMLNode)
{
    OgitorsPropertyValue value;
    Ogre::Quaternion orientation;
    
    value.propType = PROP_QUATERNION;

    if(XMLNode->Attribute("qx"))
    {
        orientation.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("qx"));
        orientation.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("qy"));
        orientation.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("qz"));
        orientation.w = Ogre::StringConverter::parseReal(XMLNode->Attribute("qw"));
    }
    else if(XMLNode->Attribute("axisX"))
    {
        Ogre::Vector3 axis;
        axis.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisX"));
        axis.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisY"));
        axis.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("axisZ"));
        Ogre::Real angle = Ogre::StringConverter::parseReal(XMLNode->Attribute("angle"));;
        orientation.FromAngleAxis(Ogre::Angle(angle), axis);
    }
    else if(XMLNode->Attribute("angleX"))
    {
        Ogre::Vector3 axis;
        axis.x = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleX"));
        axis.y = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleY"));
        axis.z = Ogre::StringConverter::parseReal(XMLNode->Attribute("angleZ"));
        
        Ogre::Quaternion q1(Ogre::Radian(axis.x),Ogre::Vector3(1,0,0));
        Ogre::Quaternion q2(Ogre::Radian(axis.y),Ogre::Vector3(0,1,0));
        Ogre::Quaternion q3(Ogre::Radian(axis.z),Ogre::Vector3(0,0,1));
        orientation = q1 * q2 * q3;
    }

    value.val = Ogre::Any(orientation);
    
    return value;
}
//----------------------------------------------------------------------------
OgitorsPropertyValue parseColourValue(TiXmlElement *XMLNode)
{
    OgitorsPropertyValue value;
    Ogre::ColourValue val = Ogre::ColourValue(0,0,0,1);
    
    value.propType = PROP_COLOUR;

    if(XMLNode->Attribute("r"))
        val.r = Ogre::StringConverter::parseReal(XMLNode->Attribute("r"));

    if(XMLNode->Attribute("g"))
        val.g = Ogre::StringConverter::parseReal(XMLNode->Attribute("g"));

    if(XMLNode->Attribute("b"))
        val.b = Ogre::StringConverter::parseReal(XMLNode->Attribute("b"));

    if(XMLNode->Attribute("a"))
        val.a = Ogre::StringConverter::parseReal(XMLNode->Attribute("a"));

    value.val = Ogre::Any(val);
    
    return value;
}
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

    TiXmlDocument docImport((filePath + fileName).c_str());

    if(!docImport.LoadFile()) return SCF_ERRFILE;

    TiXmlElement* element = 0;
    element = docImport.FirstChildElement("scene");
    if(!element)
        return SCF_ERRFILE;

    float version = Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("formatVersion")));
    if(version != 1.0f)
    {
        mSystem->DisplayMessageDialog(OTR("Only File Version 1.0 is supported!"),DLGTYPE_OK);
        return SCF_ERRFILE;
    }
    
    pOpt->SceneManagerName = "OctreeSceneManager";

    pOpt->ResourceDirectories.push_back("/");
    TiXmlElement* resLoc = element->FirstChildElement("resourceLocations");
    if(resLoc)
    {
        resLoc = resLoc->FirstChildElement();
        while(resLoc)
        {
            Ogre::String resType = ValidAttr(resLoc->Attribute("type"));
            Ogre::String resName = ValidAttr(resLoc->Attribute("name"));
            if(resType == "FileSystem")
            {
                OgitorsUtils::CleanPath(resName);
                
                if(resName[0] == '.')
                    resName.erase(0, 1);

                pOpt->ResourceDirectories.push_back(resName);
            }

            resLoc = resLoc->NextSiblingElement();
        }
    }

    TiXmlElement* configData = element->FirstChildElement("terrain");
    if(configData)
    {
        pOpt->SceneManagerConfigFile = ValidAttr(configData->Attribute("dataFile"));
    }

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
    TiXmlElement* environment = element->FirstChildElement("environment");
    if(environment)
    {
        TiXmlElement* current = environment->FirstChildElement("clipping");
        if(current)
        {
            vClipping.x = Ogre::StringConverter::parseReal(ValidAttr(current->Attribute("near"),"1"));
            vClipping.y = Ogre::StringConverter::parseReal(ValidAttr(current->Attribute("far"),"1000"));
        }

        current = environment->FirstChildElement("colourAmbient");
        if(current)
        {
            params["ambient"] = parseColourValue(current);
        }
        
        current = environment->FirstChildElement("skyBox");
        if(current)
        {
            propValue.propType = PROP_BOOL;
            propValue.val = Ogre::Any(Ogre::StringConverter::parseBool(ValidAttr(current->Attribute("enable"),"0")));
            params["skybox::active"] = propValue;
            propValue.propType = PROP_STRING;
            propValue.val = Ogre::Any(Ogre::String(ValidAttr(current->Attribute("material"))));
            params["skybox::material"] = propValue;
            propValue.propType = PROP_REAL;
            propValue.val = Ogre::Any(Ogre::StringConverter::parseReal(ValidAttr(current->Attribute("distance"),"0")));
            params["skybox::distance"] = propValue;
        }

        current = environment->FirstChildElement("skyDome");
        if(current)
        {
            propValue.propType = PROP_BOOL;
            propValue.val = Ogre::Any(Ogre::StringConverter::parseBool(ValidAttr(current->Attribute("enable"),"0")));
            params["skydome::active"] = propValue;
            propValue.propType = PROP_STRING;
            propValue.val = Ogre::Any(Ogre::String(ValidAttr(current->Attribute("material"))));
            params["skydome::material"] = propValue;
        }

        current = environment->FirstChildElement("fog");
        if(current)
        {
            propValue.propType = PROP_INT;
            Ogre::String fogmode = ValidAttr(current->Attribute("mode"),"None");
            if(fogmode == "Linear" || fogmode == "linear" )
                propValue.val = Ogre::Any((int)Ogre::FOG_LINEAR);
            else if(fogmode == "Exp" || fogmode == "exp" )
                propValue.val = Ogre::Any((int)Ogre::FOG_EXP);
            else if(fogmode == "Exp2" || fogmode == "exp2" )
                propValue.val = Ogre::Any((int)Ogre::FOG_EXP2);
            else
                propValue.val = Ogre::Any(Ogre::StringConverter::parseInt(fogmode));

            params["fog::mode"] = propValue;
            Ogre::Real start = Ogre::StringConverter::parseReal(ValidAttr(current->Attribute("linearStart"),"0"));
            Ogre::Real end = Ogre::StringConverter::parseReal(ValidAttr(current->Attribute("linearEnd"),"1"));

            propValue.propType = PROP_REAL;
            propValue.val = Ogre::Any((Ogre::Real)(start * (vClipping.y - vClipping.x)));
            params["fog::start"] = propValue;
            propValue.val = Ogre::Any((Ogre::Real)(end * (vClipping.y - vClipping.x)));
            params["fog::end"] = propValue;
            propValue.val = Ogre::Any(Ogre::StringConverter::parseReal(ValidAttr(current->Attribute("expDensity"),"0")));
            params["fog::density"] = propValue;
            current = current->FirstChildElement("colourDiffuse");
            if(current)
            {
                params["fogcolour"] = parseColourValue(current);
            }
        }
    }

    propValue.propType = PROP_STRING;
    propValue.val = Ogre::Any(Ogre::String("SceneManager1"));
    params["name"] = propValue;
    CSceneManagerEditor *mngred = static_cast<CSceneManagerEditor*>(ogRoot->CreateEditorObject(0, pOpt->SceneManagerName,params,false,false));

    // read cameras placed outside nodes
    TiXmlElement* otherElems = element->FirstChildElement("camera");
    Ogitors::CBaseEditor* cbeTemp;
    while(otherElems){
        ReadCamera(otherElems, mngred, &cbeTemp);
        otherElems = otherElems->NextSiblingElement("camera");
    }

    // read lights placed outside nodes
    otherElems = element->FirstChildElement("light");
    while(otherElems){
        ReadLight(otherElems, mngred, &cbeTemp);
        otherElems = otherElems->NextSiblingElement("light");
    }

    element = element->FirstChildElement("nodes");

    RecurseReadObjects(element, mngred);
    ogRoot->AfterLoadScene();
    ogRoot->GetViewport()->getCameraEditor()->setClipDistance(vClipping);
    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::RecurseReadObjects(TiXmlElement *parentelement,CBaseEditor* parentobject)
{
    CBaseEditor *newobj;
    TiXmlElement* element = 0;
    Ogre::String eType;
    element = parentelement->FirstChildElement();
    if(!element) return SCF_OK;
    do
    {
        newobj = 0;
        eType = element->Value();
        if(eType == "node") 
        {
            ReadSceneNode(element, parentobject, &newobj);
            int ret = RecurseReadObjects(element, newobj);
            
            if(ret != SCF_OK) 
                return ret;

            NameObjectPairList& childlist = newobj->getChildren();
            if(childlist.size() == 1)
            {
                CBaseEditor *childobj = childlist.begin()->second;
                childobj->setParent(newobj->getParent());
                OgitorsPropertyValueMap vmapn = newobj->getProperties()->getValueMap();
                OgitorsPropertyValueMap vmapo;

                OgitorsPropertyValueMap::iterator vit;

                vit = vmapn.find("autotracktarget");
                vmapo.insert(OgitorsPropertyValueMap::value_type(vit->first, vit->second));
                vit = vmapn.find("position");
                vmapo.insert(OgitorsPropertyValueMap::value_type(vit->first, vit->second));
                vit = vmapn.find("scale");
                vmapo.insert(OgitorsPropertyValueMap::value_type(vit->first, vit->second));
                vit = vmapn.find("orientation");
                vmapo.insert(OgitorsPropertyValueMap::value_type(vit->first, vit->second));

                OgitorsRoot::getSingletonPtr()->DestroyEditorObject(newobj);
                newobj = childobj;
                childobj->getProperties()->setValueMap(vmapo);
            }
        }
        else if(eType == "entity") ReadEntity(element, parentobject, &newobj);
        else if(eType == "subentities") ReadSubEntity(element, parentobject, &newobj);
        else if(eType == "light") ReadLight(element, parentobject, &newobj);
        else if(eType == "camera") ReadCamera(element, parentobject, &newobj);
        else if(eType == "particle") ReadParticle(element, parentobject, &newobj);
        else if(eType == "plane") ReadPlane(element, parentobject, &newobj);
        else 
            continue;
    } while(element = element->NextSiblingElement());
    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::ReadSceneNode(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
 
    propValue.propType = PROP_STRING;
    Ogre::String name = ValidAttr(element->Attribute("name"));
    if(Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(name))
        propValue.val = Ogre::Any(Ogre::String("Node") + Ogitors::OgitorsRoot::getSingletonPtr()->CreateUniqueID("Node",""));
    else
        propValue.val = Ogre::Any(name);

    params["name"] = propValue;

    TiXmlElement* subs = 0;
    Ogre::String eType;
    subs = element->FirstChildElement();
    if(!subs) return SCF_OK;
    do
    {
        eType = subs->Value();
        if(eType == "scale")
            params["scale"] = parseVector3(subs);
        else if(eType == "position")
            params["position"] = parseVector3(subs);
        else if(eType == "rotation")
            params["orientation"] = parseQuaternion(subs);
    } while(subs = subs->NextSiblingElement());
    
    *ret = Ogitors::OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, "Node", params, false, false);
    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::ReadEntity(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
 
    propValue.propType = PROP_STRING;
    Ogre::String name = ValidAttr(element->Attribute("name"));
    if(Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(name))
        propValue.val = Ogre::Any(Ogre::String("Entity") + Ogitors::OgitorsRoot::getSingletonPtr()->CreateUniqueID("Entity",""));
    else
        propValue.val = Ogre::Any(name);

    params["name"] = propValue;

    propValue.val = Ogre::String(ValidAttr(element->Attribute("meshFile")));
    params["meshfile"] = propValue;
    propValue.val = Ogre::String(ValidAttr(element->Attribute("materialFile")));
    params["material"] = propValue;
    propValue.propType = PROP_BOOL;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseBool(ValidAttr(element->Attribute("castShadows"))));
    params["castshadows"] = propValue;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseBool(ValidAttr(element->Attribute("receiveShadows"))));
    params["receiveshadows"] = propValue;
    
    *ret = Ogitors::OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, "Entity", params, false, false);
    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::ReadSubEntity(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
 
    propValue.propType = PROP_STRING;

    TiXmlElement* subs = 0;
    Ogre::String eType;
    subs = element->FirstChildElement();
    if(!subs) return SCF_OK;
    do
    {
        eType = subs->Value();
        if(eType == "subentity")
        {
            Ogre::String idx = ValidAttr(subs->Attribute("index"),"0");
            Ogre::String matname = ValidAttr(subs->Attribute("materialName"));
            propValue.val = Ogre::Any(matname);
            params["subentity" + idx + "::material"] = propValue;
        }
    } while(subs = subs->NextSiblingElement());

    parent->getProperties()->initValueMap(params);

    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::ReadLight(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
 
    propValue.propType = PROP_STRING;
    Ogre::String name = ValidAttr(element->Attribute("name"));
    if(Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(name))
        propValue.val = Ogre::Any(Ogre::String("Light") + Ogitors::OgitorsRoot::getSingletonPtr()->CreateUniqueID("Light",""));
    else
        propValue.val = Ogre::Any(name);

    params["name"] = propValue;
 
    int ltype = Ogre::Light::LT_POINT;
    Ogre::String lighttype = ValidAttr(element->Attribute("type"),"point");
    
    if(lighttype == "directional")
        ltype = Ogre::Light::LT_DIRECTIONAL;
    else if(lighttype == "spot")
        ltype = Ogre::Light::LT_SPOTLIGHT;

    propValue.propType = PROP_INT;
    propValue.val = Ogre::Any(ltype);
    params["lighttype"] = propValue;

    propValue.propType = PROP_BOOL;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseBool(ValidAttr(element->Attribute("castShadows"))));
    params["castshadows"] = propValue;
    propValue.propType = PROP_REAL;
    propValue.val = Ogre::Any(Ogre::StringConverter::parseReal(ValidAttr(element->Attribute("power"), "1")));
    params["power"] = propValue;

    TiXmlElement* subs = 0;
    Ogre::String eType;
    subs = element->FirstChildElement();
    if(!subs) return SCF_OK;
    do
    {
        eType = subs->Value();
        if(eType == "colourDiffuse")
        {
            params["diffuse"] = parseColourValue(subs);
        }
        else if(eType == "colourSpecular")
        {
            params["specular"] = parseColourValue(subs);
        }
        else if(eType == "lightAttenuation")
        {
            Ogre::Vector4 att;
            att.x = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("range")));
            att.y = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("constant")));
            att.z = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("linear")));
            att.w = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("quadric")));
            propValue.propType = PROP_VECTOR4;
            propValue.val = Ogre::Any(att);
            params["attenuation"] = propValue;
        }
        else if(eType == "lightRange")
        {
            Ogre::Vector3 range;
            range.x = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("inner"),"15"));
            range.y = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("outer"),"30"));
            range.z = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("falloff"),"1"));
            propValue.propType = PROP_VECTOR3;
            propValue.val = Ogre::Any(range);
            params["lightrange"] = propValue;
        }
        else if(eType == "position")
        {
            params["position"] = parseVector3(subs);
        }
        else if(eType == "directionVector")
        {
            params["direction"] = parseVector3(subs);
            /* Orientation is not stored for lights in the .scene files.
              If we don't set orientation for the spotlights/directional
              lights they will face to the default direction             */
            Ogre::Vector3 normal = Ogre::Vector3::UNIT_Z;
            Ogitors::OgitorsPropertyValue pOrient;

            pOrient.propType = PROP_QUATERNION;
            pOrient.val = normal.getRotationTo(Ogre::any_cast<Ogre::Vector3>(parseVector3(subs).val));
            params["orientation"] = pOrient;
        }
    } while(subs = subs->NextSiblingElement());
    
    *ret = Ogitors::OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, "Light", params, false, false);
    return SCF_OK;
}
//----------------------------------------------------------------------------
int CDotSceneSerializer::ReadCamera(TiXmlElement *element, CBaseEditor *parent, CBaseEditor **ret)
{
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue propValue;
 
    propValue.propType = PROP_STRING;
    Ogre::String name = ValidAttr(element->Attribute("name"));
    if(Ogitors::OgitorsRoot::getSingletonPtr()->FindObject(name))
        propValue.val = Ogre::Any(Ogre::String("Camera") + Ogitors::OgitorsRoot::getSingletonPtr()->CreateUniqueID("Camera",""));
    else
        propValue.val = Ogre::Any(name);

    params["name"] = propValue;

    TiXmlElement* subs = 0;
    Ogre::String eType;
    subs = element->FirstChildElement();
    if(!subs) return SCF_OK;
    do
    {
        eType = subs->Value();
        if(eType == "clipping")
        {
            Ogre::Vector2 clip;
            clip.x = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("near"),"0"));
            clip.y = Ogre::StringConverter::parseReal(ValidAttr(subs->Attribute("far"),"0"));
            propValue.propType = PROP_VECTOR2;
            propValue.val = Ogre::Any(clip);
            params["clipdistance"] = propValue;
        }
        else if(eType == "position")
        {
            params["position"] = parseVector3(subs);
        }
        else if(eType == "rotation")
        {
            params["orientation"] = parseQuaternion(subs);
        }
    } while(subs = subs->NextSiblingElement());
    
    *ret = Ogitors::OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, "Camera", params, false, false);
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
