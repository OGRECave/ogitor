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
#include "BaseEditor.h"
#include "ViewportEditor.h"
#include "BaseSerializer.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "OFSSceneSerializer.h"

#include "ofs.h"

using namespace Ogitors;

int COFSSceneSerializer::Import(Ogre::String importfile)
{
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    OgitorsSystem *mSystem = OgitorsSystem::getSingletonPtr();
    OFS::OfsPtr& mFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

    if(importfile == "")
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("Ogitor File System File"));
        extlist.push_back("*.ofs");
        extlist.push_back(OTR("Ogitor Scene File"));
        extlist.push_back("*.ogscene");

        importfile = mSystem->GetSetting("system", "oldOpenPath", "");
        importfile = mSystem->DisplayOpenDialog(OTR("Open"), extlist, importfile);
        if(importfile == "") 
            return SCF_CANCEL;

        mSystem->SetSetting("system", "oldOpenPath", importfile);
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
        mSystem->DisplayMessageDialog("The path is read-only. Ogitor can not work with read-only project paths!", DLGTYPE_OK);
        return SCF_CANCEL;
    }

    Ogre::UTFString loadmsg = "";

    int typepos = importfile.find_last_of(".");
    if(typepos != -1 && (importfile.substr(typepos, 4) != ".ofs"))
        importfile = filePath;
    

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
    typepos = fileName.find_last_of(".");
    if(typepos != -1)
        fileName.erase(typepos, fileName.length() - typepos);
    pOpt->ProjectName = fileName;

    fileName += ".ogscene";

    OFS::ofs64 file_size = 0;

    if(mFile->getFileSize(fileName.c_str(), file_size) != OFS::OFS_OK)
        return SCF_ERRFILE;

    char *file_data = new char[(unsigned int)file_size + 1];

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

    TiXmlNode* ogitorSceneNode = 0;
    TiXmlNode* projectNode;
    TiXmlElement* element = 0;
    bool upgradeExecuted = false;
    ogitorSceneNode = docImport.FirstChild("OGITORSCENE");

    if(!ogitorSceneNode)
        return SCF_ERRPARSE;

    element = ogitorSceneNode->ToElement();

    // Old OGSCENE version check and attempt to fix/update
    int version = Ogre::StringConverter::parseInt(ValidAttr(element->Attribute("version"), "0"));    
    if(Ogre::StringConverter::toString(version) < Globals::OGSCENE_FORMAT_VERSION)
    {
        mSystem->DisplayMessageDialog(mSystem->Translate("Old OGSCENE file version detected. Ogitor will now attempt to upgrade the format and will also create a backup version of your OFS file."), DLGTYPE_OK);

        loadmsg = mSystem->Translate("Upgrading OGSCENE file.");
        mSystem->UpdateLoadProgress(10, loadmsg);

        if(version == 0)
        {
            mSystem->DisplayMessageDialog(mSystem->Translate("OGSCENE files contains no version number set and therefore cannot be loaded."), DLGTYPE_OK);
            return SCF_ERRPARSE;
        }
        else if(version == 1)
        {
            mSystem->DisplayMessageDialog(mSystem->Translate("OGSCENE files with version 1 cannot be upgraded automatically. Please contact the Ogitor team for further details."), DLGTYPE_OK);
            return SCF_ERRPARSE;
        }
        if(version > 1)
            if(!mSystem->CopyFile(importfile, importfile + ".backup"))
                mSystem->DisplayMessageDialog(mSystem->Translate("Error while trying to create backup file."), DLGTYPE_OK);
        
        switch(version)
        {
         case 2:
            _upgradeOgsceneFileFrom2To3(ogitorSceneNode);
            break;
        }

        upgradeExecuted = true;
    }  

    projectNode = ogitorSceneNode->FirstChild("PROJECT");

    if(projectNode)
    {
        loadmsg = mSystem->Translate("Parsing project options");
        mSystem->UpdateLoadProgress(5, loadmsg);
        ogRoot->LoadProjectOptions(projectNode->ToElement());
        ogRoot->PrepareProjectResources();
    }

    element = ogitorSceneNode->FirstChildElement();

    loadmsg = mSystem->Translate("Creating scene objects");
    mSystem->UpdateLoadProgress(10, loadmsg);

    unsigned int obj_count = 0;
    Ogre::String objecttype;
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue tmpPropVal;
    Ogre::String objAttValue;
    Ogre::String elementName;
    TiXmlElement* properties = 0;
    Ogre::String attID;
    Ogre::String attValue;
    CBaseEditor* result = 0;
    TiXmlElement* customprop = 0;
    Ogre::StringVector invalidEditorTypes;

    do
    {
        // Make sure its NON-ZERO
        if(pOpt->ObjectCount)
        {
            ++obj_count;
            mSystem->UpdateLoadProgress(10 + ((obj_count * 70) / pOpt->ObjectCount), loadmsg);
        }

        params.clear();       

        objAttValue = ValidAttr(element->Attribute("object_id"), "");
        if(objAttValue != "")
        {
            tmpPropVal.propType = PROP_UNSIGNED_INT;
            tmpPropVal.val = Ogre::Any(Ogre::StringConverter::parseUnsignedInt(objAttValue));
            params.insert(OgitorsPropertyValueMap::value_type("object_id", tmpPropVal));
        }

        objAttValue = ValidAttr(element->Attribute("parentnode"), "");
        if(objAttValue != "")
        {
            tmpPropVal.propType = PROP_STRING;
            tmpPropVal.val = Ogre::Any(objAttValue);
            params.insert(OgitorsPropertyValueMap::value_type("parentnode", tmpPropVal));
        }

        objAttValue = ValidAttr(element->Attribute("name"), "");
        if(objAttValue != "")
        {
            tmpPropVal.propType = PROP_STRING;
            tmpPropVal.val = Ogre::Any(objAttValue);
            params.insert(OgitorsPropertyValueMap::value_type("name", tmpPropVal));
        }
        else
            continue;

        objAttValue = ValidAttr(element->Attribute("typename"), "");
        if(objAttValue != "")
        {
            tmpPropVal.propType = PROP_STRING;
            tmpPropVal.val = Ogre::Any(objAttValue);
            params.insert(OgitorsPropertyValueMap::value_type("typename", tmpPropVal));
        }
        else
            continue;

        properties = element->FirstChildElement();
        if(properties)
        {            
            do
            {
                elementName = properties->Value();
                if(elementName != "PROPERTY")
                    continue;

                attID = ValidAttr(properties->Attribute("id"), "");
                int attType = Ogre::StringConverter::parseInt(ValidAttr(properties->Attribute("type"), ""));
                attValue = ValidAttr(properties->Attribute("value"), "");

                params.insert(OgitorsPropertyValueMap::value_type(attID, OgitorsPropertyValue::createFromString((OgitorsPropertyType)attType, attValue)));
            } while(properties = properties->NextSiblingElement());
        }

        objecttype = Ogre::any_cast<Ogre::String>(params["typename"].val);
        result = ogRoot->CreateEditorObject(0, objecttype, params, false, false);
        if(result)
        {
            customprop = element->FirstChildElement("CUSTOMPROPERTIES");
            if(customprop) 
            {
                OgitorsUtils::ReadCustomPropertySet(customprop, result->getCustomProperties());
            }
        }
        else
            invalidEditorTypes.push_back(objecttype);

    } while(element = element->NextSiblingElement());

    // Print out invalid/unsupported editor types (= types where no factory could be found)
    if(invalidEditorTypes.size() > 0)
    {
        std::sort(invalidEditorTypes.begin(), invalidEditorTypes.end());
        invalidEditorTypes.erase(std::unique(invalidEditorTypes.begin(), invalidEditorTypes.end()), invalidEditorTypes.end());
        Ogre::String invalidTypesResultString;
        for(unsigned int i = 0; i < invalidEditorTypes.size(); i++)
        {
            invalidTypesResultString += invalidEditorTypes.at(i) + "\n";
        }
        mSystem->DisplayMessageDialog(mSystem->Translate("Could not create objects of types:\n" + invalidTypesResultString), DLGTYPE_OK);
    }

    // Save directly after upgrade
    if(upgradeExecuted)
        Export(false, importfile);

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

    bool forceSave = false;
    Ogre::String fileLocation = ogRoot->GetProjectFile()->getFileSystemName();
    Ogre::String fileName = "";

    if (!exportfile.empty())
    {
        // Save location was passed, so use this filename
        fileLocation = exportfile;
    }

    if (SaveAs)
    {
        // Saving at a different location
        UTFStringVector extlist;

        if( mFile->getFileSystemType() == OFS::OFS_PACKED )
        {
            extlist.push_back(OTR("Ogitor File System File"));
            extlist.push_back("*.ofs");
        }
        else
        {
            extlist.push_back(OTR("Ogitor Scene File"));
            extlist.push_back("*.ogscene");
        }

        Ogre::String newfileLocation = mSystem->DisplaySaveDialog(OTR("Save As"), extlist, fileLocation);
        if(newfileLocation == "") 
            return SCF_CANCEL;

        mSystem->SetSetting("system", "oldOpenPath", newfileLocation);

        if(Ogre::StringUtil::match(newfileLocation, fileLocation, false))
        {
            SaveAs = false;
        } 
        else 
        {
            forceSave = true;
            fileLocation = newfileLocation;
        }
    }

    Ogre::String filePath = OgitorsUtils::ExtractFilePath(fileLocation);
    fileName = OgitorsUtils::ExtractFileName(fileLocation);

    // Change the project directory to the new path
    pOpt->ProjectDir = filePath;

    if(fileName.substr(fileName.size() - 4, 4) != ".ofs")
        fileLocation = filePath;

    int dotpos = fileName.find_last_of(".");
    if (dotpos > 0)
    {
        fileName.erase(dotpos, fileName.length() - dotpos);
    }

    if (SaveAs && mFile->moveFileSystemTo(fileLocation.c_str()) != OFS::OFS_OK)
    {
        return SCF_ERRFILE;
    }

    if (SaveAs)
    {
        mFile->deleteFile((pOpt->ProjectName+".ogscene").c_str());
        pOpt->ProjectName = fileName;
    }

    if (_writeFile(fileName+".ogscene", forceSave) != SCF_OK)
    {
        return SCF_ERRFILE;
    }

    return SCF_OK;
}
//-----------------------------------------------------------------------------
int COFSSceneSerializer::_writeFile(Ogre::String exportfile, const bool forceSave)
{
    if (exportfile.empty())
        return SCF_ERRUNKNOWN;

    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    // Open a stream to output our XML Content and write the general headercopyFile
    std::stringstream outfile;

    outfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    outfile << "<OGITORSCENE version=\"" << Globals::OGSCENE_FORMAT_VERSION << "\">\n";

    PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();
    pOpt->CameraSpeed = ogRoot->GetViewport()->GetCameraSpeed();

    ogRoot->WriteProjectOptions(outfile, pOpt);

    ObjectVector ObjectList;
    OgitorsPropertyValueMap theList;
    OgitorsPropertyValueMap::iterator ni;

    // Start from 1, since 0 means all objects
    for(unsigned int i = 1; i < LAST_EDITOR; i++)
    {
        ogRoot->GetObjectList(i, ObjectList);
        for(unsigned int ob = 0; ob < ObjectList.size(); ob++)
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

    if (OgitorsUtils::SaveStreamOfs(outfile, exportfile)) {
        return SCF_OK;
    }

    return SCF_ERRFILE;
}
//-----------------------------------------------------------------------------
void Ogitors::COFSSceneSerializer::_upgradeOgsceneFileFrom2To3(TiXmlNode* ogsceneRootNode)
{
    TiXmlElement* element = ogsceneRootNode->FirstChildElement();   

    unsigned int obj_count = 0;
    Ogre::String objecttype;
    OgitorsPropertyValueMap params;
    OgitorsPropertyValue tmpPropVal;
    Ogre::String objAttValue;
    size_t offset = 0;

    do
    {
        objAttValue = ValidAttr(element->Attribute("typename"), "");
        if(objAttValue != "")
        {
            if((offset = objAttValue.find(" Object")) != Ogre::String::npos)
            {
                objAttValue = objAttValue.substr(0, offset);
                element->SetAttribute("typename", objAttValue.c_str());
            }
        }
        else
            continue;

    } while(element = element->NextSiblingElement());
}
//-----------------------------------------------------------------------------