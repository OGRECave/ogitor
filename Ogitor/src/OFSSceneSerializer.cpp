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
#include "BaseSerializer.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OFSSceneSerializer.h"

#include "tinyxml.h"
#include "ofs.h"

using namespace Ogitors;

int COFSSceneSerializer::Import(Ogre::String importfile)
{
    LOG4CPLUS_TRACE(log4cplus::Logger::getInstance("ogitor"), __PRETTY_FUNCTION__ << " : " << __FILE__ << ", line " << __LINE__);
  
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    OgitorsSystem *mSystem = OgitorsSystem::getSingletonPtr();
    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(importfile == "")
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("Ogitor Scene File"));
        extlist.push_back("*.ofs");
        importfile = mSystem->DisplayOpenDialog(OTR("Open"),extlist);
        if(importfile == "") 
            return SCF_CANCEL;
    }

    importfile = OgitorsUtils::QualifyPath(importfile);

    Ogre::String filePath = OgitorsUtils::ExtractFilePath(importfile);
    Ogre::String fileName = OgitorsUtils::ExtractFileName(importfile);

    bool testpassed = false;
    try
    {
        std::ofstream test((filePath + "test.dat").c_str());
        if(test.is_open())
            testpassed = true;
        test.close();
        mSystem->DeleteFile(filePath + "test.dat");
    }
    catch(...)
    {
        testpassed = false;
    }

    if(!testpassed)
    {
        mSystem->DisplayMessageDialog("The path is Read-Only. Ogitor can not work with Read-Only Project Paths!", DLGTYPE_OK);
        return SCF_CANCEL;
    }

    Ogre::UTFString loadmsg = "";

    OFS::OfsResult oRet;
    if((oRet = mFile.mount(importfile.c_str(), OFS::OFS_MOUNT_OPEN | OFS::OFS_MOUNT_RECOVER)) != OFS::OFS_OK)
    {
        if(oRet == OFS::OFS_PREVIOUS_VERSION)
        {
            mSystem->DisplayMessageDialog("The OFS file is a previous version, please use qtOFS to upgrade it to new file version.", DLGTYPE_OK);
        }
        
        loadmsg = mSystem->Translate("Please load a Scene File...");
        mSystem->UpdateLoadProgress(-1, loadmsg);
        return SCF_ERRPARSE;
    }

    OFS::FileSystemStats fsStats;

    mFile->getFileSystemStats(fsStats);

    PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();
    pOpt->CreatedIn = "";

    pOpt->ProjectDir = filePath;
    int typepos = fileName.find_last_of(".");
    if(typepos != -1)
        fileName.erase(typepos, fileName.length() - typepos);
    pOpt->ProjectName = fileName;
    
    fileName += ".ogscene";

    unsigned int file_size = 0;

    if(mFile->getFileSize(fileName.c_str(), file_size) != OFS::OFS_OK)
        return SCF_ERRFILE;

    char *file_data = new char[file_size + 1];

    OFS::OFSHANDLE projHandle;

    if(mFile->openFile(projHandle, fileName.c_str(), OFS::OFS_READ) != OFS::OFS_OK)
    {
        delete [] file_data;
        return SCF_ERRFILE;
    }

    mFile->read(projHandle, file_data, file_size);
    mFile->closeFile(projHandle);

    TiXmlDocument docImport;

    if(!docImport.LoadFromMemory(file_data, file_size))
    {
        delete [] file_data;
        return SCF_ERRFILE;
    }

    delete [] file_data;

    loadmsg = mSystem->Translate("Parsing Scene File");
    mSystem->UpdateLoadProgress(1, loadmsg);

    TiXmlNode* node = 0;
    TiXmlElement* element = 0;
    node = docImport.FirstChild("OGITORSCENE");
    
    if(!node)
        return SCF_ERRPARSE;

    element = node->ToElement();
    int version = Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("version"),"1"));
    if(version == 1)
        return SCF_ERRPARSE;

    node = node->FirstChild("PROJECT");

    if(node)
    {
        loadmsg = mSystem->Translate("Parsing project options");
        mSystem->UpdateLoadProgress(5, loadmsg);
        ogRoot->LoadProjectOptions(node->ToElement());
        ogRoot->PrepareProjectResources();
    }

    node = docImport.FirstChild("OGITORSCENE");
    if(!node) 
        return SCF_ERRPARSE;
    element = node->FirstChildElement();

    loadmsg = mSystem->Translate("Creating scene objects");
    mSystem->UpdateLoadProgress(10, loadmsg);

    unsigned int obj_count = 0;
    Ogre::String objecttype;
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue tmpPropVal;
    do
    {
        // Make sure its NON-ZERO
        if(pOpt->ObjectCount)
        {
            ++obj_count;
            mSystem->UpdateLoadProgress(10 + ((obj_count * 70) / pOpt->ObjectCount), loadmsg);
        }
        
        params.clear();

        Ogre::String objAttValue;

        objAttValue = ValidAttr(element->Attribute("object_id"), "");
        if(objAttValue != "")
        {
            tmpPropVal.propType = PROP_UNSIGNED_INT;
            tmpPropVal.val = Ogre::Any(Ogre::StringConverter::parseUnsignedInt(objAttValue));
            params.insert(OgitorsPropertyValueMap::value_type("object_id", tmpPropVal));
        }

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
        CBaseEditor *result = ogRoot->CreateEditorObject(0, objecttype, params, false, false);
        if(result)
        {
            TiXmlElement *customprop = element->FirstChildElement("CUSTOMPROPERTIES");
            if(customprop) 
            {
                OgitorsUtils::ReadCustomPropertySet(customprop, result->getCustomProperties());
            }
        }
    } while(element = element->NextSiblingElement());

    ogRoot->AfterLoadScene();

    return SCF_OK;
}
//-----------------------------------------------------------------------------
int COFSSceneSerializer::Export(bool SaveAs, Ogre::String exportfile)
{
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    OgitorsSystem *mSystem = OgitorsSystem::getSingletonPtr();
    OFS::OfsPtr& mFile = ogRoot->GetProjectFile();

    PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();
    Ogre::String fileName = ogRoot->GetProjectFile()->getFileSystemName();

    bool forceSave = false;

    Ogre::String restoreFileSystemPath = "";
    // If SaveAs is TRUE, use the OgitorsSystem Functions to retrieve
    // a FileName and also copy the contents of current scene to the new location
    if(SaveAs)
    {
        Ogre::String oldFileName = fileName;

        UTFStringVector extlist;
        extlist.push_back(OTR("Ogitor Scene File"));
        extlist.push_back("*.ofs");
        fileName = mSystem->DisplaySaveDialog(OTR("Save As"),extlist);
        if(fileName == "") 
            return SCF_CANCEL;

        Ogre::String oldProjDir = pOpt->ProjectDir;
        Ogre::String oldProjName = pOpt->ProjectName + ".ogscene";

        pOpt->ProjectName = OgitorsUtils::ExtractFileName(fileName);
        int typepos = pOpt->ProjectName.find_last_of(".");
        if(typepos != -1)
            pOpt->ProjectName.erase(typepos,pOpt->ProjectName.length() - typepos);

        pOpt->ProjectDir = OgitorsUtils::ExtractFilePath(fileName);

        mFile->moveFileSystemTo(fileName.c_str());

        mFile->deleteFile(oldProjName.c_str());

        forceSave = true;
    }
    else
    {
        // Save location was passed, so use that one
        if(!exportfile.empty())
        {
            fileName = exportfile;
            
            if(exportfile.substr(exportfile.length() - 4, 4) != ".ofs")
                fileName += ".ofs";

            fileName = OgitorsUtils::QualifyPath(fileName);

            restoreFileSystemPath = mFile->getFileSystemName();
            
            mFile->moveFileSystemTo(fileName.c_str());
        }
    }

    fileName = OgitorsUtils::ExtractFileName(fileName);
    int dotpos = fileName.find_last_of(".");
    fileName.erase(dotpos, fileName.length() - dotpos);
    fileName += ".ogscene";

    if(!SaveAs)
        fileName += ".tmp";

    // Open a stream to output our XML Content and write the general header
    std::stringstream outfile;

    outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    outfile << "<OGITORSCENE version=\"2\">\n";
    
    ogRoot->WriteProjectOptions(outfile, false);

    ObjectVector ObjectList;
    OgitorsPropertyValueMap theList;
    OgitorsPropertyValueMap::iterator ni;

    // Start from 1, since 0 means all objects
    for(unsigned int i = 1;i < LAST_EDITOR;i++)
    {
        ogRoot->GetObjectList(i, ObjectList);
        for(unsigned int ob = 0;ob < ObjectList.size();ob++)
        {
            /// If Object does not have a parent, then it is not part of the scene
            if(ObjectList[ob]->getParent())
            {
                ObjectList[ob]->onSave(forceSave);
                if(ObjectList[ob]->isSerializable())
                {
                    outfile << OgitorsUtils::GetObjectSaveStringV2(ObjectList[ob], 2, true, true).c_str();
                    outfile << "\n";
                }
            }
        }
    }
    outfile << "</OGITORSCENE>\n";

    OgitorsUtils::SaveStreamOfs(outfile, fileName);
    
    ogRoot->SetSceneModified(false);

    if(SaveAs)
    {
        ogRoot->TerminateScene();
        ogRoot->LoadScene(fileName);
    }
    else
    {
        Ogre::String delfile = fileName.substr(0, fileName.length() - 4);
        mFile->deleteFile(delfile.c_str());
        mFile->renameFile(fileName.c_str(), delfile.c_str());

        if(!restoreFileSystemPath.empty())
            mFile->switchFileSystemTo(restoreFileSystemPath.c_str());
    }

    return SCF_OK;
}
//-----------------------------------------------------------------------------
