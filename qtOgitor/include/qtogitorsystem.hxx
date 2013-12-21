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
#ifndef __qtogitorsystem_h__431D421B_ADFE_44b2_B788_1E94A4A1A418
#define __qtogitorsystem_h__431D421B_ADFE_44b2_B788_1E94A4A1A418

#include "OgitorsPrerequisites.h"
#include "OgitorsSystem.h"

#include <QtCore/QStringList>
#include <QtCore/QSettings>

#include <QtWidgets/QProgressDialog>

class SceneViewWidget;
class LayerViewWidget;
class OgreWidget;
class GeneralPropertiesViewWidget;
class CustomPropertiesViewWidget;
class CalculateBlendMapDialog;
class QProgressDialog;

class QtOgitorSystem : public Ogitors::OgitorsSystem
{
public:
    QtOgitorSystem(void);
    ~QtOgitorSystem(void);

    void SetWindows(OgreWidget  *renderViewWidget, SceneViewWidget *sceneTreeWidget, LayerViewWidget *layerTreeWidget, GeneralPropertiesViewWidget *propsWidgetGeneral, CustomPropertiesViewWidget *propsWidgetCustom)
    {
        mSceneTreeWidget = sceneTreeWidget;
        mLayerTreeWidget = layerTreeWidget;
        mRenderViewWidget = renderViewWidget;
        mGeneralPropsWidget = propsWidgetGeneral;
        mCustomPropsWidget = propsWidgetCustom;
    };

    void DummyTranslationFunction();

    void InitTreeIcons();

    Ogre::String GetProjectsDirectory();
    Ogre::UTFString GetSetting(Ogre::UTFString group, Ogre::UTFString name, Ogre::UTFString defaultValue);
    bool SetSetting(Ogre::UTFString group, Ogre::UTFString name, Ogre::UTFString value);
    bool FileExists(const Ogre::String& filename);
    bool CopyFile(Ogre::String source, Ogre::String destination);
    bool CopyFilesEx(Ogre::String source, Ogre::String destination);
    bool MakeDirectory(Ogre::String dirname);
    void DeleteFile(const Ogre::String &file);
    void RenameFile(const Ogre::String &oldname, const Ogre::String &newname);
    void GetFileList(Ogre::String path, Ogre::StringVector &list);
    void GetDirList(Ogre::String path, Ogre::StringVector &list);
    void DisplayProgressDialog(Ogre::UTFString title, int min, int max, int value);
    void HideProgressDialog();
    void UpdateProgressDialog(int value);

    Ogre::String DisplayDirectorySelector(Ogre::UTFString title, Ogre::UTFString defaultPath = "");
    Ogre::String DisplayOpenDialog(Ogre::UTFString title, Ogitors::UTFStringVector ExtensionList, Ogre::UTFString defaultPath = "");
    Ogre::String DisplaySaveDialog(Ogre::UTFString title, Ogitors::UTFStringVector ExtensionList, Ogre::UTFString defaultPath = "");

    Ogitors::DIALOGRET DisplayMessageDialog(Ogre::UTFString msg, Ogitors::DIALOGTYPE dlgType);
    void DisplayTerrainDialog();
    bool DisplayImportHeightMapDialog(Ogre::NameValuePairList &params);
    bool DisplayEuclidDialog(Ogre::NameValuePairList &params);
    bool DisplayCalculateBlendMapDialog(Ogre::NameValuePairList &params);
    void UpdateLoadProgress(float percentage, Ogre::UTFString msg);
    Ogre::UTFString Translate(Ogre::String& str);
    Ogre::UTFString Translate(const char * str);

    bool HasTreeView();
    bool HasPropertiesView();

    void UnLoadPropertySheets();
    void LoadPropertySheet(int idx, Ogre::String filename);
    void ClearProperty(void *caller, int idx);
    void EnableProperty(void *caller, int idx, bool enable);
    Ogre::String GetStringProperty(void *caller, int idx);
    int GetIntProperty(void *caller, int idx);
    float GetFloatProperty(void *caller, int idx);
    bool GetBoolProperty(void *caller, int idx);
    Ogre::ColourValue GetColourProperty(void *caller, int idx);

    void PresentPropertiesView(Ogitors::CBaseEditor* object);

    void SelectTreeItem(Ogitors::CBaseEditor *object);
    void InsertTreeItem(Ogitors::CBaseEditor *parent, Ogitors::CBaseEditor *object,int iconid, unsigned int colour, bool order = false);
    void *MoveTreeItem(void *newparent, void *object);
    void MoveLayerTreeItem(int newparent, Ogitors::CBaseEditor *object);
    void *CreateTreeRoot(Ogre::String name);
    void SetTreeItemText(Ogitors::CBaseEditor *object, Ogre::String newname);
    void DeleteTreeItem(Ogitors::CBaseEditor *object);
    void SetTreeItemColour(Ogitors::CBaseEditor *object, unsigned int colour );
    void ClearTreeItems();

    void SetMouseCursor(unsigned int cursor);
    void SetMousePosition(Ogre::Vector2 position);
    void ShowMouseCursor(bool bShow);

private:
    GeneralPropertiesViewWidget *mGeneralPropsWidget;
    CustomPropertiesViewWidget  *mCustomPropsWidget;
    SceneViewWidget             *mSceneTreeWidget;
    LayerViewWidget             *mLayerTreeWidget;
    OgreWidget                  *mRenderViewWidget;
    std::map<unsigned int, QString> mIconList;
    QString                      mProjectsDirectory;
    CalculateBlendMapDialog     *mCalcBlendmapDlg;
    QProgressDialog             *mProgressDialog;
};

#endif // __qtogitorsystem_h__431D421B_ADFE_44b2_B788_1E94A4A1A418
