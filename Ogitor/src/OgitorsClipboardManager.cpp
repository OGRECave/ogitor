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

#include "OgitorsPrerequisites.h"
#include "OgitorsClipboardManager.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BaseEditor.h"
#include "MultiSelEditor.h"

#include "ofs.h"

using namespace Ogitors;

template<> OgitorsClipboardManager* Ogitors::Singleton<OgitorsClipboardManager>::ms_Singleton = 0;
unsigned int OgitorsClipboardManager::mClipboardMaxSize = 10;

//----------------------------------------------------------------------------------
OgitorsClipboardManager::OgitorsClipboardManager()
{
}
//----------------------------------------------------------------------------------  
OgitorsClipboardManager::~OgitorsClipboardManager()
{
    unsigned int i;

    for(i = 0;i < mBuffer.size();i++)
        OGRE_DELETE mBuffer[i];

    mBuffer.clear();

    ObjectTemplateMap::iterator it = mGeneralTemplates.begin();
    while(it != mGeneralTemplates.end())
    {
        for(i = 0;i < it->second.size();i++)
        {
            OGRE_DELETE it->second[i].mCustomProperties;
        }
        it++;
    }

    mGeneralTemplates.clear();

    it = mProjectTemplates.begin();
    while(it != mProjectTemplates.end())
    {
        for(i = 0;i < it->second.size();i++)
        {
            OGRE_DELETE it->second[i].mCustomProperties;
        }
        it++;
    }

    mProjectTemplates.clear();
}
//----------------------------------------------------------------------------------  
void OgitorsClipboardManager::copy(CBaseEditor* object)
{
    assert(object != 0 && "The object to copy must not be null");
    
    if((mBuffer.size() + 1) > mClipboardMaxSize)
    {
        int count = (mBuffer.size() + 1) - mClipboardMaxSize;
        for(int i = 0;i < count;i++)
            OGRE_DELETE mBuffer[i];

        mBuffer.erase(mBuffer.begin(), mBuffer.begin() + (mBuffer.size() - mClipboardMaxSize + 1));
    }

    ObjectCopyData *data = OGRE_NEW ObjectCopyData;

    _copyRecursive( data, object );

    mBuffer.push_back(data);
}
//---------------------------------------------------------------------------------- 
void OgitorsClipboardManager::_copyRecursive(ObjectCopyData* data, CBaseEditor* object)
{
    data->mObjectName = object->getName();
    data->mObjectTypeName = object->getTypeName();
    object->getPropertyMap(data->mProperties);
    data->mProperties.erase(data->mProperties.find("object_id"));
    object->getCustomProperties()->cloneSet(data->mCustomProperties);

    NameObjectPairList& children = object->getChildren();

    for(NameObjectPairList::iterator it = children.begin(); it != children.end(); it++)
    {
        ObjectCopyData child_data;

        _copyRecursive( &child_data, it->second );

        data->mChildren.push_back( child_data );
    }
}
//---------------------------------------------------------------------------------- 
CBaseEditor *OgitorsClipboardManager::paste(CBaseEditor *parent, int index)
{
    ObjectCopyData *data = 0;

    assert(index < (int)mBuffer.size());

    if(index == -1)
        data = mBuffer[mBuffer.size() - 1];
    else
        data = mBuffer[index];

    CBaseEditor *object = _pasteRecursive( data, parent );
    
    OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(object);

    return object;
}
//----------------------------------------------------------------------------------
CBaseEditor *OgitorsClipboardManager::_pasteRecursive(ObjectCopyData* data, CBaseEditor *parent)
{
    CBaseEditor* find_object = OgitorsRoot::getSingletonPtr()->FindObject(data->mObjectName);
    
    OgitorsPropertyValueMap properties = data->mProperties;
    
    if(find_object)
    {
        Ogre::String newname = data->mObjectName + "Copy";
        newname += OgitorsRoot::getSingletonPtr()->CreateUniqueID(newname, "", 0);
        properties["name"].val = Ogre::Any(newname);
    }

    CBaseEditor* object = OgitorsRoot::getSingletonPtr()->CreateEditorObject(parent, data->mObjectTypeName, properties, true, false);
    
    if(!object)
        return 0;

    object->getCustomProperties()->initFromSet(data->mCustomProperties);
    
    object->load();

    for( unsigned int i = 0; i < data->mChildren.size(); i++ )
    {
        _pasteRecursive( &(data->mChildren[i]), object );
    }

    return object;
}
//---------------------------------------------------------------------------------- 
bool OgitorsClipboardManager::copyToTemplate(CBaseEditor *object, const Ogre::String& templatename, bool isGeneralScope)
{
    if(templatename.empty())
        return false;
    
    Ogre::String filename;

    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();
    
    if(isGeneralScope)
    {
        filename = OgitorsSystem::getSingletonPtr()->GetProjectsDirectory() + "/Templates";
        filename = OgitorsUtils::QualifyPath(filename);
        OgitorsSystem::getSingletonPtr()->MakeDirectory(filename);
    }
    else
    {
        filename = "/Templates";
        mFile->createDirectory(filename.c_str());
    }
    
    
    filename += "/" + templatename + ".otl";

    std::stringstream outfile;
    outfile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    outfile << "<TEMPLATES>\n";
    outfile << "  <OBJECTTEMPLATE name=\"";
    outfile << templatename.c_str();
    outfile << "\">\n";
    outfile << OgitorsUtils::GetObjectSaveStringV2(object, 4, false, false).c_str();
    outfile << "  </OBJECTTEMPLATE>\n";
    outfile << "</TEMPLATES>\n";

    if(isGeneralScope)
    {
        std::ofstream out_general(filename.c_str());
        out_general << outfile.str();
        out_general.close();
    }
    else
    {
        OgitorsUtils::SaveStreamOfs(outfile, filename);
    }

    ObjectTemplateMap::iterator rit;
    if(isGeneralScope)
        rit = mGeneralTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate())).first;
    else
        rit = mProjectTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate())).first;

    rit->second.push_back(ObjectTemplateData());
    object->getPropertyMap(rit->second[0].mObjectProperties);
    rit->second[0].mObjectProperties.erase(rit->second[0].mObjectProperties.find("object_id"));
    OgitorsCustomPropertySet *tmpset = OGRE_NEW OgitorsCustomPropertySet();
    object->getCustomProperties()->cloneSet(*tmpset);
    rit->second[0].mCustomProperties = tmpset;
    rit->second[0].mTypeName = object->getTypeName();

    return true;
}
//----------------------------------------------------------------------------------
bool OgitorsClipboardManager::copyToTemplateWithChildren(CBaseEditor *object, const Ogre::String& templatename, bool isGeneralScope)
{
    if(templatename.empty())
        return false;

    Ogre::String filename;
    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();
    
    if(isGeneralScope)
    {
        filename = OgitorsSystem::getSingletonPtr()->GetProjectsDirectory() + "/Templates";
        filename = OgitorsUtils::QualifyPath(filename);
        OgitorsSystem::getSingletonPtr()->MakeDirectory(filename);
    }
    else
    {
        filename = "/Templates";
        mFile->createDirectory(filename.c_str());
    }
    
    
    filename += "/" + templatename + ".otl";

    std::stringstream outfile;
    outfile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    outfile << "<TEMPLATES>\n";
    outfile << "  <OBJECTTEMPLATE name=\"";
    outfile << templatename.c_str();
    outfile << "\">\n";
    Ogre::StringVector list;
    object->getNameList(list);
    outfile << OgitorsUtils::GetObjectSaveStringV2(object, 4, false, false).c_str();

    ObjectTemplateMap::iterator rit;
    if(isGeneralScope)
        rit = mGeneralTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate())).first;
    else
        rit = mProjectTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate())).first;

    rit->second.push_back(ObjectTemplateData());
    object->getPropertyMap(rit->second[0].mObjectProperties);
    rit->second[0].mObjectProperties.erase(rit->second[0].mObjectProperties.find("object_id"));
    OgitorsCustomPropertySet *tmpset = OGRE_NEW OgitorsCustomPropertySet();
    object->getCustomProperties()->cloneSet(*tmpset);
    rit->second[0].mCustomProperties = tmpset;
    rit->second[0].mTypeName = object->getTypeName();

    for(unsigned int i = 1;i < list.size();i++)
    {
        CBaseEditor *item = OgitorsRoot::getSingletonPtr()->FindObject(list[i]);
        if(item != 0)
        {
            outfile << OgitorsUtils::GetObjectSaveStringV2(item, 4, false, true).c_str();

            rit->second.push_back(ObjectTemplateData());
            int itempos = rit->second.size() - 1;
            item->getPropertyMap(rit->second[itempos].mObjectProperties);
            rit->second[itempos].mObjectProperties.erase(rit->second[itempos].mObjectProperties.find("object_id"));
            OgitorsPropertyValue parentnodevalue;
            parentnodevalue.propType = PROP_STRING;
            parentnodevalue.val = Ogre::Any(item->getParent()->getName());
            rit->second[itempos].mObjectProperties.insert(OgitorsPropertyValueMap::value_type("parentnode", parentnodevalue));
            OgitorsCustomPropertySet *tmpset = OGRE_NEW OgitorsCustomPropertySet();
            item->getCustomProperties()->cloneSet(*tmpset);
            rit->second[itempos].mCustomProperties = tmpset;
            rit->second[itempos].mTypeName = item->getTypeName();
        }
    }
    
    outfile << "  </OBJECTTEMPLATE>\n";
    outfile << "</TEMPLATES>\n";

    if(isGeneralScope)
    {
        std::ofstream out_general(filename.c_str());
        out_general << outfile.str();
        out_general.close();
    }
    else
    {
        OgitorsUtils::SaveStreamOfs(outfile, filename);
    }

    return true;
}
//----------------------------------------------------------------------------------
bool OgitorsClipboardManager::copyToTemplateMulti(CMultiSelEditor *object, const Ogre::String& templatename, bool isGeneralScope)
{
    if(templatename.empty())
        return false;

    NameObjectPairList list = object->getSelection();
    NameObjectPairList::iterator oit = list.begin();

    if(oit == list.end())
        return false;

    Ogre::String filename;

    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(isGeneralScope)
    {
        filename = OgitorsSystem::getSingletonPtr()->GetProjectsDirectory() + "/Templates";
        filename = OgitorsUtils::QualifyPath(filename) + "/";
        OgitorsSystem::getSingletonPtr()->MakeDirectory(filename);
    }
    else
    {
        filename = "/Templates";
        mFile->createDirectory(filename.c_str());
    }
    
    
    filename += "/" + templatename + ".otl";

    std::stringstream outfile;
    outfile << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    outfile << "<TEMPLATES>\n";
    outfile << "  <OBJECTTEMPLATE name=\"";
    outfile << templatename.c_str();
    outfile << "\">\n";

    ObjectTemplateMap::iterator rit;
    if(isGeneralScope)
        rit = mGeneralTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate())).first;
    else
        rit = mProjectTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate())).first;

    std::vector<CBaseEditor*> objList;

    while(oit != list.end())
    {
        CBaseEditor *item = oit->second;
        if(!item->supports(CAN_ACCEPTCOPY))
        {
            list.erase(oit);
            oit = list.begin();
        }
        else
        {
            CBaseEditor *itemParent = item->getParent();
            Ogre::String parentName = "";
            if(itemParent)
                parentName = itemParent->getName();

            if(list.find(parentName) == list.end())
            {
                list.erase(oit);
                oit = list.begin();
                objList.push_back(item);
            }
            else
                oit++;
        }
    }

    for(unsigned int objn = 0;objn < objList.size();objn++)
    {
        CBaseEditor *item = objList[objn];

        outfile << OgitorsUtils::GetObjectSaveStringV2(item, 2, false, true).c_str();

        rit->second.push_back(ObjectTemplateData());
        int itempos = rit->second.size() - 1;
        item->getPropertyMap(rit->second[itempos].mObjectProperties);
        rit->second[itempos].mObjectProperties.erase(rit->second[itempos].mObjectProperties.find("object_id"));
        OgitorsPropertyValue parentnodevalue;
        parentnodevalue.propType = PROP_STRING;
        parentnodevalue.val = Ogre::Any(item->getParent()->getName());
        rit->second[itempos].mObjectProperties.insert(OgitorsPropertyValueMap::value_type("parentnode", parentnodevalue));
        OgitorsCustomPropertySet *tmpset = OGRE_NEW OgitorsCustomPropertySet();
        item->getCustomProperties()->cloneSet(*tmpset);
        rit->second[itempos].mCustomProperties = tmpset;
        rit->second[itempos].mTypeName = item->getTypeName();
    }

    outfile << "  </OBJECTTEMPLATE>\n";
    outfile << "</TEMPLATES>\n";

    if(isGeneralScope)
    {
        std::ofstream out_general(filename.c_str());
        out_general << outfile.str();
        out_general.close();
    }
    else
    {
        OgitorsUtils::SaveStreamOfs(outfile, filename);
    }

    return true;
}
//----------------------------------------------------------------------------------
CBaseEditor *OgitorsClipboardManager::instantiateTemplate(const Ogre::String& templatename)
{
    CBaseEditor *retObject = OgitorsRoot::getSingletonPtr()->GetSelection();

    CBaseEditor *item = 0;
    ObjectTemplateMap::const_iterator it;

    it = mGeneralTemplates.find(templatename);
    if(it == mGeneralTemplates.end())
    {
        it = mProjectTemplates.find(templatename);
        if(it == mProjectTemplates.end())
            return 0;
    }

    ObjectTemplate objTemplate = it->second;
    OgitorsPropertyValueMap::iterator pit;
    NameObjectPairList list;
    NameObjectPairList objlist;
    std::vector<CBaseEditor*> objlist2;
    NameObjectPairList::iterator nit;

    Ogre::String parentname;

    int numParentObjects = 0;

    for(unsigned int i = 0;i < objTemplate.size();i++)
    {
        OgitorsPropertyValueMap objMap = objTemplate[i].mObjectProperties;
        if((pit = objMap.find("parentnode")) != objMap.end())
        {
            parentname = Ogre::any_cast<Ogre::String>(pit->second.val);
            if((nit = list.find(parentname)) != list.end())
            {
                pit->second.val = Ogre::Any(nit->second->getName());
            }
            else
            {
                objMap.erase(pit);
                ++numParentObjects;
            }
        }
        else
            ++numParentObjects;

        parentname = Ogre::any_cast<Ogre::String>(objMap["name"].val);
        objMap["name"].val = Ogre::Any(parentname + OgitorsRoot::getSingletonPtr()->CreateUniqueID(parentname,"",0));

        item = OgitorsRoot::getSingletonPtr()->CreateEditorObject(0, objTemplate[i].mTypeName, objMap, true, false);

        if(item)
        {
            item->load();
            item->getCustomProperties()->initFromSet(*(objTemplate[i].mCustomProperties));
            list.insert(NameObjectPairList::value_type(parentname, item));
            objlist.insert(NameObjectPairList::value_type(item->getName(), item));
            objlist2.push_back(item);
        }
    }

    Ogre::Vector3 pos(999999, 999999, 999999);

    if(numParentObjects == 0)
        return 0;
    else if(numParentObjects == 1)
    {
        retObject = objlist2[0];
        OgitorsRoot::getSingletonPtr()->GetSelection()->setSelection(retObject);
    }
    else
        static_cast<CMultiSelEditor*>(retObject)->setSelection(objlist);


    if(retObject->getProperties()->hasProperty("position"))
        retObject->getProperties()->setValue("position", pos);

    return retObject;
}
//---------------------------------------------------------------------------------
bool OgitorsClipboardManager::isTemplateInstantiable(const Ogre::String& templatename)
{
    ObjectTemplateMap::const_iterator it;

    it = mGeneralTemplates.find(templatename);
    if(it == mGeneralTemplates.end())
    {
        it = mProjectTemplates.find(templatename);
        if(it == mProjectTemplates.end())
            return false;
    }

    if(it->second.size() == 0)
        return false;

    CBaseEditorFactory *factory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory(it->second[0].mTypeName);
    if(!factory)
        return false;

    return factory->CanInstantiate();
}
//---------------------------------------------------------------------------------
bool OgitorsClipboardManager::doesTemplateRequirePlacement(const Ogre::String& templatename)
{
    ObjectTemplateMap::const_iterator it;

    it = mGeneralTemplates.find(templatename);
    if(it == mGeneralTemplates.end())
    {
        it = mProjectTemplates.find(templatename);
        if(it == mProjectTemplates.end())
            return false;
    }

    if(it->second.size() == 0)
        return false;

    CBaseEditorFactory *factory = OgitorsRoot::getSingletonPtr()->GetEditorObjectFactory(it->second[0].mTypeName);
    if(!factory)
        return false;

    return factory->mRequirePlacement;
}
//---------------------------------------------------------------------------------
void OgitorsClipboardManager::addTemplatesFromFiles(Ogre::StringVector filenames, bool isGeneralScope)
{
    Ogre::String fname;
    
    for(unsigned int i = 0;i < filenames.size();i++)
    {
        if(isGeneralScope)
            fname = OgitorsUtils::QualifyPath(filenames[i]);
        else
            fname = filenames[i];

        addTemplateFromFile(fname, isGeneralScope);
    }
}
//---------------------------------------------------------------------------------
void OgitorsClipboardManager::addTemplateFromFile(Ogre::String filename, bool isGeneralScope)
{
    TiXmlDocument docImport;
    
    if(isGeneralScope)
    {
        if(!docImport.LoadFile(filename.c_str())) 
            return;
    }
    else
    {
        OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

        OFS::ofs64 file_size = 0;

        if(mFile->getFileSize(filename.c_str(), file_size) != OFS::OFS_OK)
            return;

        char *file_data = new char[(unsigned int)file_size + 1];

        OFS::OFSHANDLE fileHandle;

        if(mFile->openFile(fileHandle, filename.c_str(), OFS::OFS_READ) != OFS::OFS_OK)
        {
            delete [] file_data;
            return;
        }

        mFile->read(fileHandle, file_data, (unsigned int)file_size);
        mFile->closeFile(fileHandle);

        if(!docImport.LoadFromMemory(file_data, (unsigned int)file_size))
        {
            delete [] file_data;
            return;
        }

        delete [] file_data;
    }

    TiXmlElement* templates = 0;
    templates = docImport.FirstChildElement("TEMPLATES");
    if(!templates)
        return;

    templates = templates->FirstChildElement("OBJECTTEMPLATE");
    if(!templates)
        return;


    Ogre::String objecttype;
    Ogre::String templatename;
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue tmpPropVal;

    do
    {
        templatename = ValidAttr(templates->Attribute("name"));
        if(templatename == "")
            continue;

        TiXmlElement *element = templates->FirstChildElement("OBJECT");
        if(!element)
            continue;

        ObjectTemplateMap::iterator rit;
        if(isGeneralScope)
        {
            std::pair<ObjectTemplateMap::iterator, bool> retPair = mGeneralTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate()));
            if(!retPair.second)
                continue;
            rit = retPair.first;
        }
        else
        {
            std::pair<ObjectTemplateMap::iterator, bool> retPair = mProjectTemplates.insert(ObjectTemplateMap::value_type(templatename, ObjectTemplate()));
            if(!retPair.second)
                continue;
            rit = retPair.first;
        }

        do
        {
            params.clear();

            Ogre::String objAttValue;

            objAttValue = ValidAttr(element->Attribute("parentnode"),"");
            if(objAttValue != "")
            {
                tmpPropVal.propType = PROP_STRING;
                tmpPropVal.val = Ogre::Any(objAttValue);
                params.insert(OgitorsPropertyValueMap::value_type("parentnode", tmpPropVal));
            }

            objAttValue = ValidAttr(element->Attribute("name"),"");
            if(objAttValue != "")
            {
                tmpPropVal.propType = PROP_STRING;
                tmpPropVal.val = Ogre::Any(objAttValue);
                params.insert(OgitorsPropertyValueMap::value_type("name", tmpPropVal));
            }
            else
                continue;

            objAttValue = ValidAttr(element->Attribute("typename"),"");
            if(objAttValue != "")
            {
                tmpPropVal.propType = PROP_STRING;
                tmpPropVal.val = Ogre::Any(objAttValue);
                params.insert(OgitorsPropertyValueMap::value_type("typename", tmpPropVal));
            }
            else
                continue;

            TiXmlElement *properties = element->FirstChildElement();
            if(properties)
            {
                Ogre::String elementName;
                do
                {
                    elementName = properties->Value();
                    if(elementName != "PROPERTY")
                        continue;

                    Ogre::String attID = ValidAttr(properties->Attribute("id"),"");
                    int attType = Ogre::StringConverter::parseInt(ValidAttr(properties->Attribute("type"),""));
                    Ogre::String attValue = ValidAttr(properties->Attribute("value"),"");

                    params.insert(OgitorsPropertyValueMap::value_type(attID, OgitorsPropertyValue::createFromString((OgitorsPropertyType)attType, attValue)));
                } while(properties = properties->NextSiblingElement());
            }

            objecttype = Ogre::any_cast<Ogre::String>(params["typename"].val);

            rit->second.push_back(ObjectTemplateData());
            rit->second[rit->second.size() - 1].mObjectProperties = params;
            rit->second[rit->second.size() - 1].mCustomProperties = OGRE_NEW OgitorsCustomPropertySet();
            rit->second[rit->second.size() - 1].mTypeName = objecttype;

            TiXmlElement *customprop = element->FirstChildElement("CUSTOMPROPERTIES");
            if(customprop) 
            {
                OgitorsUtils::ReadCustomPropertySet(customprop, rit->second[rit->second.size() - 1].mCustomProperties);
            }
        } while (element = element->NextSiblingElement());
        if(rit->second.size() == 0)
        {
            if(isGeneralScope)
                mGeneralTemplates.erase(rit);
            else
                mProjectTemplates.erase(rit);
        }
    } while(templates = templates->NextSiblingElement());
}
//---------------------------------------------------------------------------------
