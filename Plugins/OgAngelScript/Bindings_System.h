/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2012 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

/*////////////////////////////////////////////////////////////////////////////////
// The main script binding functions
////////////////////////////////////////////////////////////////////////////////*/

#ifndef BINDINGS_SYSTEM_H
#define BINDINGS_SYSTEM_H

class asIScriptEngine;

namespace Ogitors
{
        //virtual bool         DisplayTerrainDialog(Ogre::NameValuePairList &params) {return false;};
        //virtual bool         DisplayImportHeightMapDialog(Ogre::NameValuePairList &params) {return false;};
        //virtual bool         DisplayCalculateBlendMapDialog(Ogre::NameValuePairList &params) {return false;};

    void RegisterSystemBindings(asIScriptEngine *engine)
    {
        int r;
        r = engine->RegisterObjectMethod("OgitorsSystem", "bool fileExists(const string &in)", asMETHOD(OgitorsSystem, FileExists), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "bool copyFile(string, string)", asMETHOD(OgitorsSystem, CopyFile), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "bool copyFilesEx(string, string)", asMETHOD(OgitorsSystem, CopyFilesEx), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "bool makeDirectory(string)", asMETHOD(OgitorsSystem, MakeDirectory), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "string getProjectsDirectory()", asMETHOD(OgitorsSystem, GetProjectsDirectory), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void deleteFile(const string &in)", asMETHOD(OgitorsSystem, DeleteFile), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void renameFile(const string &in, const string &in)", asMETHOD(OgitorsSystem, RenameFile), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void getFileList(string, StringVector &out)", asMETHOD(OgitorsSystem, GetFileList), asCALL_THISCALL);assert(r >= 0);

        r = engine->RegisterObjectMethod("OgitorsSystem", "string displayDirectorySelector(UTFString)", asMETHOD(OgitorsSystem, DisplayDirectorySelector), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "string displayOpenDialog(UTFString, UTFStringVector)", asMETHOD(OgitorsSystem, DisplayOpenDialog), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "string displaySaveDialog(UTFString, UTFStringVector)", asMETHOD(OgitorsSystem, DisplaySaveDialog), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "DialogReturnType displayMessageDialog(UTFString, DialogType)", asMETHOD(OgitorsSystem, DisplayMessageDialog), asCALL_THISCALL);assert(r >= 0);

        r = engine->RegisterObjectMethod("OgitorsSystem", "void presentPropertiesView(BaseEditor@)", asMETHOD(OgitorsSystem, PresentPropertiesView), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void selectTreeItem(BaseEditor@)", asMETHOD(OgitorsSystem, SelectTreeItem), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void insertTreeItem(BaseEditor@, BaseEditor@, int, uint)", asMETHOD(OgitorsSystem, InsertTreeItem), asCALL_THISCALL);assert(r >= 0);
        
        if(sizeof(size_t) == 4)
        {
            r = engine->RegisterObjectMethod("OgitorsSystem", "uint64 moveTreeItem(uint64, uint64)", asMETHOD(OgitorsSystem, MoveTreeItem), asCALL_THISCALL);assert(r >= 0);
        }
        else
        {
            r = engine->RegisterObjectMethod("OgitorsSystem", "uint moveTreeItem(uint, uint)", asMETHOD(OgitorsSystem, MoveTreeItem), asCALL_THISCALL);assert(r >= 0);
        }

        r = engine->RegisterObjectMethod("OgitorsSystem", "void moveLayerTreeItem(int, BaseEditor@)", asMETHOD(OgitorsSystem, MoveLayerTreeItem), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void setTreeItemText(BaseEditor@, string)", asMETHOD(OgitorsSystem, SetTreeItemText), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void setTreeItemColour(BaseEditor@, uint)", asMETHOD(OgitorsSystem, SetTreeItemColour), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void deleteTreeItem(BaseEditor@)", asMETHOD(OgitorsSystem, DeleteTreeItem), asCALL_THISCALL);assert(r >= 0);
        r = engine->RegisterObjectMethod("OgitorsSystem", "void clearTreeItems()", asMETHOD(OgitorsSystem, ClearTreeItems), asCALL_THISCALL);assert(r >= 0);
    }

};

#endif