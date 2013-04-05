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
void extractOFS(Ogre::String path)
{
    unsigned int MAX_BUFFER_SIZE = 16 * 1024 * 1024;

    char *tmp_buffer = new char[MAX_BUFFER_SIZE];

    OFS::OfsPtr& ofsFile = OgitorsRoot::getSingletonPtr()->GetProjectFile();

    OFS::FileList list;

    ofsFile->listFilesRecursive("/", list);

    std::sort(list.begin(), list.end(), OFS::FileEntry::Compare);

    std::ofstream out_handle;
    OFS::OFSHANDLE in_handle;

    for(unsigned int i = 0;i < list.size();i++)
    {
        std::string file_path = path + list[i].name;

        if(list[i].flags & OFS::OFS_DIR)
        {
            OgitorsSystem::getSingletonPtr()->MakeDirectory(file_path);
        }
        else
        {
            std::string file_ofs_path = list[i].name;

            out_handle.open(file_path.c_str(), std::ofstream::out | std::ofstream::binary);

            if(out_handle.is_open())
            {
                try
                {
                    OFS::OfsResult ret = ofsFile->openFile(in_handle, file_ofs_path.c_str());
                    if(ret != OFS::OFS_OK)
                    {
                        out_handle.close();
                        continue;
                    }

                    unsigned int total = list[i].file_size;

                    while(total > 0)
                    {
                        if(total < MAX_BUFFER_SIZE)
                        {
                            ofsFile->read(in_handle, tmp_buffer, total);
                            out_handle.write(tmp_buffer, total);
                            total = 0;
                        }
                        else
                        {
                            ofsFile->read(in_handle, tmp_buffer, MAX_BUFFER_SIZE);
                            out_handle.write(tmp_buffer, MAX_BUFFER_SIZE);
                            total -= MAX_BUFFER_SIZE;
                        }
                    }
                }
                catch(OFS::Exception&)
                {
                }

                out_handle.close();
                ofsFile->closeFile(in_handle);
            }
        }
    }

    delete [] tmp_buffer;
}
//----------------------------------------------------------------------------
void saveUserData(OgitorsCustomPropertySet *set, TiXmlElement *pParent)
{
    if(!set || !pParent)
        return;

    OgitorsPropertyVector vec = set->getPropertyVector();

    if(vec.size() < 1)
        return;

    TiXmlElement *pNode = pParent->InsertEndChild(TiXmlElement("userData"))->ToElement();

    for(unsigned int i = 0;i < vec.size();i++)
    {
        OgitorsPropertyBase *property = vec[i];
        const OgitorsPropertyDef *def = property->getDefinition();
        OgitorsPropertyValue value;
        value.propType = property->getType();
        value.val = property->getValue();

        TiXmlElement *pProp = pNode->InsertEndChild(TiXmlElement("property"))->ToElement();
        pProp->SetAttribute("type", Ogre::StringConverter::toString(value.propType).c_str());
        pProp->SetAttribute("name", property->getName().c_str());
        pProp->SetAttribute("data", OgitorsUtils::GetValueString(value).c_str());
    }
}
//----------------------------------------------------------------------------------------
int CDotSceneSerializer::Export(bool SaveAs, Ogre::String exportfile)
{
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    OgitorsSystem *mSystem = OgitorsSystem::getSingletonPtr();

    PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();
    Ogre::String fileName = pOpt->ProjectName;

    UTFStringVector extlist;
    extlist.push_back(OTR("DotScene File"));
    extlist.push_back("*.scene");
    extlist.push_back(OTR("DotScene File"));
    extlist.push_back("*.xml");
    fileName = mSystem->DisplaySaveDialog(OTR("Export DotScene File"), extlist, "");
    if(fileName == "") 
        return SCF_CANCEL;

    mSystem->SetSetting("system", "oldOpenPath", OgitorsUtils::ExtractFilePath(fileName));

    Ogre::String newDir = OgitorsUtils::ExtractFilePath(fileName);

    extractOFS(newDir);
    mSystem->DeleteFile(newDir + "/" + pOpt->ProjectName + ".ogscene");

    TiXmlDocument *pXMLDoc = new TiXmlDocument();
    pXMLDoc->InsertEndChild(TiXmlDeclaration("1.0", "UTF-8", ""));
    pXMLDoc->InsertEndChild(TiXmlElement("scene"));

    // export basic info
    TiXmlElement *pRoot = pXMLDoc->RootElement();
    pRoot->SetAttribute("formatVersion", "1.0.0");
    pRoot->SetAttribute("generator", Ogre::String(Ogre::String("Ogitor SceneBuilder ") + Ogre::String(Globals::OGITOR_VERSION)).c_str());

    // export resource locations
    TiXmlElement *pResourceLocations = pRoot->InsertEndChild(TiXmlElement("resourceLocations"))->ToElement();
    
    for(unsigned int r = 0;r < pOpt->ResourceDirectories.size();r++)
    {
        TiXmlElement *pResourceLocation = pResourceLocations->InsertEndChild(TiXmlElement("resourceLocation"))->ToElement();
        Ogre::String loc = pOpt->ResourceDirectories[r];

        pResourceLocation->SetAttribute("type", "FileSystem");

        std::replace(loc.begin(),loc.end(),'\\','/');

        if(loc[0] != '.')
            loc = "./" + loc;

        pResourceLocation->SetAttribute("name", loc.c_str());
    }

    //TODO: do we need all those object id's ?

    TiXmlElement *pEnvironment = pRoot->InsertEndChild(TiXmlElement("environment"))->ToElement();

    // export octree scenemanagers
    NameObjectPairList smList = ogRoot->GetObjectsByTypeName("OctreeSceneManager");
    NameObjectPairList::const_iterator smIt = smList.begin();
    while(smIt != smList.end())
    {
        TiXmlElement *result = smIt->second->exportDotScene(pEnvironment);
        saveUserData(smIt->second->getCustomProperties(), result);
        smIt++;        
    }

    // export viewports
    NameObjectPairList vpList = ogRoot->GetObjectsByTypeName("Viewport");
    NameObjectPairList::const_iterator vpIt = vpList.begin();
    while(vpIt != vpList.end())
    {
        TiXmlElement *result = vpIt->second->exportDotScene(pEnvironment);
        saveUserData(vpIt->second->getCustomProperties(), result);
        vpIt++;
    }

    // export terrains
    NameObjectPairList terrainList = ogRoot->GetObjectsByType(ETYPE_TERRAIN_MANAGER);
    NameObjectPairList::const_iterator tlIt = terrainList.begin();
    while(tlIt != terrainList.end())
    {
        TiXmlElement *result = tlIt->second->exportDotScene(pRoot);
        saveUserData(tlIt->second->getCustomProperties(), result);
        tlIt++;
    }

    NameObjectPairList items = ogRoot->GetSceneManagerEditor()->getChildren();
   
    // export lights
    NameObjectPairList::const_iterator nodeIt = items.begin();

    while(nodeIt != items.end())
    {
        if(nodeIt->second->getEditorType() == ETYPE_LIGHT)
        {
            TiXmlElement *result = nodeIt->second->exportDotScene(pRoot);
            saveUserData(nodeIt->second->getCustomProperties(), result);
        }
        nodeIt++;
    }

    // export cameras
    nodeIt = items.begin();

    while(nodeIt != items.end())
    {
        if(nodeIt->second->getEditorType() == ETYPE_CAMERA)
        {
            TiXmlElement *result = nodeIt->second->exportDotScene(pRoot);
            saveUserData(nodeIt->second->getCustomProperties(), result);
        }
        nodeIt++;
    }

    // export nodes
    TiXmlElement *pNodes = pRoot->InsertEndChild(TiXmlElement("nodes"))->ToElement();
    nodeIt = items.begin();

    while(nodeIt != items.end())
    {
        if( nodeIt->second->getEditorType() != ETYPE_TERRAIN_MANAGER &&
            nodeIt->second->getEditorType() != ETYPE_LIGHT &&
            nodeIt->second->getEditorType() != ETYPE_CAMERA )
        {
            TiXmlElement *result = nodeIt->second->exportDotScene(pNodes);
            saveUserData(nodeIt->second->getCustomProperties(), result);
        }
        nodeIt++;
    }

    if (pXMLDoc->SaveFile(fileName.c_str()))
    {
        OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(OTR("Scene has been exported succesfully"), DLGTYPE_OK);
        delete pXMLDoc;
    }
    else
        OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(OTR("An error occured during export.. :("), DLGTYPE_OK);

    return SCF_OK;
}
//----------------------------------------------------------------------------
