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

#include "CustomSerializer.h"

using namespace Ogitors;

//----------------------------------------------------------------------------
CCustomSerializer::CCustomSerializer() : CBaseSerializer("Custom Serializer", CAN_EXPORT | CAN_IMPORT) 
{
}
//----------------------------------------------------------------------------
int CCustomSerializer::Import(Ogre::String importfile)
{
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    OgitorsSystem *mSystem = OgitorsSystem::getSingletonPtr();

    if(importfile == "")
    {
        UTFStringVector extlist;
        extlist.push_back(OTR("Custom Scene File"));
        extlist.push_back("*.scene");
        extlist.push_back(OTR("Custom Scene File"));
        extlist.push_back("*.xml");

        importfile = mSystem->DisplayOpenDialog(OTR("Import Custom File"), extlist, "");
        if(importfile == "") return SCF_CANCEL;

        mSystem->SetSetting("system", "oldOpenPath", OgitorsUtils::ExtractFilePath(importfile));
    }


    return SCF_OK;
}
//----------------------------------------------------------------------------
int CCustomSerializer::Export(bool SaveAs, Ogre::String exportfile)
{
    OgitorsRoot *ogRoot = OgitorsRoot::getSingletonPtr();
    OgitorsSystem *mSystem = OgitorsSystem::getSingletonPtr();

    PROJECTOPTIONS *pOpt = ogRoot->GetProjectOptions();
    Ogre::String fileName = pOpt->ProjectName;

    UTFStringVector extlist;
    extlist.push_back(OTR("Custom Scene File"));
    extlist.push_back("*.scene");
    extlist.push_back(OTR("Custom Scene File"));
    extlist.push_back("*.xml");
    fileName = mSystem->DisplaySaveDialog(OTR("Export Custom Scene File"), extlist, "");
    if(fileName == "") 
        return SCF_CANCEL;

    mSystem->SetSetting("system", "oldOpenPath", OgitorsUtils::ExtractFilePath(fileName));

    return SCF_OK;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
CCustomSerializer *CustomSerializer = 0;

bool dllStartPlugin(void *identifier, Ogre::String& name)
{
    name = "Custom Serializer Plugin";
    CustomSerializer = OGRE_NEW CCustomSerializer();
    
    // TODO: Uncomment the line below to register your serializer with Ogitor, 
    // its commented out so that it wont confuse Ogitor users by displaying an empty/identity serializer
    
    //OgitorsRoot::getSingletonPtr()->RegisterSerializer(identifier, CustomSerializer);
    
    return true;
}
//----------------------------------------------------------------------------
bool dllGetPluginName(Ogre::String& name)
{
    name = "Custom Serializer Plugin";
    return true;
}
//----------------------------------------------------------------------------
bool dllStopPlugin(void)
{
    OGRE_DELETE CustomSerializer;
    CustomSerializer = 0;
    return true;
}
//----------------------------------------------------------------------------
