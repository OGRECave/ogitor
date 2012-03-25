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
#include "MaterialEditor.h"
#include "OgreMaterialManager.h"

namespace Ogitors
{

//-----------------------------------------------------------------------------------------
CMaterialEditor::CMaterialEditor(CBaseEditorFactory *factory) : CBaseEditor(factory), mPropertyLevel(0),
mPropertyID(0)
{
    mHandle.setNull();
    mHelper = 0;
    mUsesGizmos = false;
    mUsesHelper = false;

    mCompareFunctions.push_back("ALWAYS_FAIL");     
    mCompareFunctions.push_back("ALWAYS_PASS");     
    mCompareFunctions.push_back("LESS");     
    mCompareFunctions.push_back("LESS_EQUAL");     
    mCompareFunctions.push_back("EQUAL");     
    mCompareFunctions.push_back("NOT_EQUAL");     
    mCompareFunctions.push_back("GREATER_EQUAL");     
    mCompareFunctions.push_back("GREATER"); 

    mCullingModes.push_back("");
    mCullingModes.push_back("NONE");
    mCullingModes.push_back("CLOCKWISE");
    mCullingModes.push_back("ANTICLOCKWISE"); 

    mShadingModes.push_back("FLAT");
    mShadingModes.push_back("GOURAUD");
    mShadingModes.push_back("PHONG"); 

    mPolygonModes.push_back("");
    mPolygonModes.push_back("POINTS");
    mPolygonModes.push_back("WIREFRAME");
    mPolygonModes.push_back("SOLID"); 
}
//-----------------------------------------------------------------------------------------
CMaterialEditor::~CMaterialEditor()
{

}
//-----------------------------------------------------------------------------------------
void CMaterialEditor::showBoundingBox(bool bShow) 
{
}
//-----------------------------------------------------------------------------------------
bool CMaterialEditor::getObjectContextMenu(UTFStringVector &menuitems) 
{
    menuitems.clear();
    if(getParent() && getParent()->getLocked())
        return false;

    if(mPropertyLevel == 0)
    {
        menuitems.push_back(OTR("Add Technique") + ";:/icons/technique.svg");
        return true;
    }
    else if(mPropertyLevel == 1)
    {
        if(mHandle->getNumTechniques() > 1)
            menuitems.push_back(OTR("Delete Technique") + ";:/icons/trash.svg");
        else
            menuitems.push_back("");
        menuitems.push_back(OTR("Add Pass") + ";:/icons/pass.svg");
        return true;
    }
    else if(mPropertyLevel == 2)
    {
        if(mHandle->getTechnique(mPropertyID >> 8)->getNumPasses() > 1)
        {
            menuitems.push_back(OTR("Delete Pass") + ";:/icons/trash.svg");
            return true;
        }
    }

    return false;
}
//-------------------------------------------------------------------------------
void CMaterialEditor::onObjectContextMenu(int menuresult) 
{
    if(mHandle.isNull())
        return;

    Ogre::Pass      *pass;
    Ogre::Technique *tech;

    if(mPropertyLevel == 0)
    {
        if(menuresult == 0)
        {
            tech = mHandle->createTechnique();
        }
    }
    else if(mPropertyLevel == 1)
    {
        switch(menuresult)
        {
        case 0:mHandle->removeTechnique(mPropertyID >> 8);break;
        case 1:pass = mHandle->getTechnique(mPropertyID >> 8)->createPass();break;
        };
    }
    else if(mPropertyLevel == 2)
    {
        if(menuresult == 0)
            mHandle->getTechnique(mPropertyID >> 8)->removePass(mPropertyID & 0xFF);
    }
}
//-----------------------------------------------------------------------------------------
void CMaterialEditor::_displayTechnique(int id)
{
    //PROPERTY_PTR(id, "Technique", int, 999, 0, 0);

    /*unsigned int BINDING = CUSTOM_BINDING_START;
    Ogre::Technique *tech = mHandle->getTechnique(id);

    m_System->CreateStringProperty(this, BIND_MAIN, BINDING, "Material", m_Name);
    m_System->EnableProperty(this, BINDING, false);

    BINDING++;

    m_System->CreateGroupProperty(this, BIND_MAIN, BINDING, "Technique " + Ogre::StringConverter::toString(id));
    m_System->CreateStringProperty(this, BINDING, BINDING + 1, "Name", tech->getName());
    m_System->CreateStringProperty(this, BINDING, BINDING + 2, "Scheme", tech->getSchemeName());
    m_System->CreateIntProperty(this, BINDING, BINDING + 3, "Lod Index", tech->getLodIndex());
    m_System->CreateEnumProperty(this, BINDING, BINDING + 4, "Shadow Rec. Mat.", m_MaterialNames);
    if(tech->getShadowReceiverMaterial().isNull())
    m_System->SetProperty(this, BINDING + 4, "None");
    else
    m_System->SetProperty(this, BINDING + 4, tech->getShadowReceiverMaterial()->getName());

    m_System->CreateEnumProperty(this, BINDING, BINDING + 5, "Shadow Cast. Mat.", m_MaterialNames);
    if(tech->getShadowCasterMaterial().isNull())
    m_System->SetProperty(this, BINDING + 5, "None");
    else
    m_System->SetProperty(this, BINDING + 5, tech->getShadowCasterMaterial()->getName());

    for(unsigned int i = 0;i < 6;i++)
    m_System->EnableProperty(this, BINDING + i, false);*/
}
//-----------------------------------------------------------------------------------------
void CMaterialEditor::_displayPass(int techid, int id)
{
    /*unsigned int PASSBINDING = CUSTOM_BINDING_START;
    Ogre::Pass *pass = mHandle->getTechnique(techid)->getPass(id);
    Ogre::String techname = "Technique " + Ogre::StringConverter::toString(techid) + ": (" + mHandle->getTechnique(techid)->getName() + ")";

    m_System->CreateStringProperty(this, BIND_MAIN, PASSBINDING, "Material", m_Name);
    m_System->EnableProperty(this, PASSBINDING, false);
    m_System->CreateStringProperty(this, BIND_MAIN, PASSBINDING + 1, "Technique", techname);
    m_System->EnableProperty(this, PASSBINDING + 1, false);

    PASSBINDING += 2;

    m_System->CreateGroupProperty(this, BIND_MAIN, PASSBINDING, "Pass " + Ogre::StringConverter::toString(id));
    m_System->CreateStringProperty(this, PASSBINDING, PASSBINDING + 1, "Name", pass->getName());
    m_System->CreateStringProperty(this, PASSBINDING, PASSBINDING + 2, "Name", pass->getName());
    m_System->CreateColourProperty(this, PASSBINDING, PASSBINDING + 3, "Ambient", pass->getAmbient());
    m_System->CreateColourProperty(this, PASSBINDING, PASSBINDING + 3, "Diffuse", pass->getDiffuse());
    m_System->CreateColourProperty(this, PASSBINDING, PASSBINDING + 4, "Specular", pass->getSpecular());
    m_System->CreateColourProperty(this, PASSBINDING, PASSBINDING + 5, "Emissive", pass->getSelfIllumination());
    m_System->CreateFloatProperty(this, PASSBINDING, PASSBINDING + 6, "Shininess", (float)pass->getShininess());

    m_System->CreateGroupProperty(this, PASSBINDING, PASSBINDING + 7, "Point Size");
    m_System->CreateFloatProperty(this, PASSBINDING + 7, PASSBINDING + 8, "Default", (float)pass->getPointSize());
    m_System->CreateFloatProperty(this, PASSBINDING + 7, PASSBINDING + 9, "Minimum", (float)pass->getPointMinSize());
    m_System->CreateFloatProperty(this, PASSBINDING + 7, PASSBINDING + 10, "Maximum", (float)pass->getPointMaxSize());

    m_System->CreateGroupProperty(this, PASSBINDING, PASSBINDING + 11, "Point Attenuation");
    m_System->CreateBoolProperty(this, PASSBINDING + 11, PASSBINDING + 12, "Enabled", pass->isPointAttenuationEnabled());
    m_System->CreateFloatProperty(this, PASSBINDING + 11, PASSBINDING + 13, "Constant", (float)pass->getPointAttenuationConstant());
    m_System->CreateFloatProperty(this, PASSBINDING + 11, PASSBINDING + 14, "Linear", (float)pass->getPointAttenuationLinear());
    m_System->CreateFloatProperty(this, PASSBINDING + 11, PASSBINDING + 15, "Quadratic", (float)pass->getPointAttenuationQuadratic());

    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 16, "Depth Check", pass->getDepthCheckEnabled());
    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 17, "Depth Write", pass->getDepthWriteEnabled());
    m_System->CreateEnumProperty(this, PASSBINDING, PASSBINDING + 18, "Depth Func", m_CompareFunctions);
    m_System->SetProperty(this, PASSBINDING + 18, m_CompareFunctions[pass->getDepthFunction()]);
    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 19, "Colour Write", pass->getColourWriteEnabled());
    m_System->CreateEnumProperty(this, PASSBINDING, PASSBINDING + 20, "Culling Mode", m_CullingModes);
    m_System->SetProperty(this, PASSBINDING + 20, m_CullingModes[pass->getCullingMode()]);
    m_System->CreateGroupProperty(this, PASSBINDING, PASSBINDING + 21, "Lighting");
    m_System->CreateBoolProperty(this, PASSBINDING + 21, PASSBINDING + 22, "Enabled", pass->getLightingEnabled());
    m_System->CreateIntProperty(this, PASSBINDING + 21, PASSBINDING + 23, "Max Lights", pass->getMaxSimultaneousLights());
    m_System->CreateIntProperty(this, PASSBINDING + 21, PASSBINDING + 24, "Start Light", pass->getStartLight());
    m_System->CreateEnumProperty(this, PASSBINDING, PASSBINDING + 25, "Shading Mode", m_ShadingModes);
    m_System->SetProperty(this, PASSBINDING + 25, m_ShadingModes[pass->getShadingMode()]);
    m_System->CreateEnumProperty(this, PASSBINDING, PASSBINDING + 26, "Polygon Mode", m_PolygonModes);
    m_System->SetProperty(this, PASSBINDING + 26, m_PolygonModes[pass->getPolygonMode()]);
    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 27, "PolyMode Overrideable", pass->getPolygonModeOverrideable());
    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 28, "Fog Override", pass->getFogOverride());
    m_System->CreateGroupProperty(this, PASSBINDING, PASSBINDING + 29, "Depth Bias");
    m_System->CreateFloatProperty(this, PASSBINDING + 29, PASSBINDING + 30, "Constant", pass->getDepthBiasConstant());
    m_System->CreateFloatProperty(this, PASSBINDING + 29, PASSBINDING + 31, "Slope Scale", pass->getDepthBiasSlopeScale());
    m_System->CreateFloatProperty(this, PASSBINDING + 29, PASSBINDING + 32, "Iteration Bias", pass->getIterationDepthBias());
    m_System->CreateGroupProperty(this, PASSBINDING, PASSBINDING + 33, "Alpha Reject");
    m_System->CreateEnumProperty(this, PASSBINDING + 33, PASSBINDING + 34, "Function", m_CompareFunctions);
    m_System->SetProperty(this, PASSBINDING + 34, m_CompareFunctions[pass->getAlphaRejectFunction()]);
    m_System->CreateIntProperty(this, PASSBINDING + 33, PASSBINDING + 35, "Value", pass->getAlphaRejectValue());
    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 36, "AlphaToCoverage", pass->isAlphaToCoverageEnabled());
    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 37, "TransparentSorting", pass->getTransparentSortingEnabled());
    m_System->CreateBoolProperty(this, PASSBINDING, PASSBINDING + 38, "NormaliseNormals", pass->getNormaliseNormals());
    m_System->CreateGroupProperty(this, PASSBINDING, PASSBINDING + 39, "Programs");
    m_System->CreateStringProperty(this, PASSBINDING + 39, PASSBINDING + 40, "Vertex", pass->getVertexProgramName());
    m_System->CreateStringProperty(this, PASSBINDING + 39, PASSBINDING + 41, "Fragment", pass->getFragmentProgramName());
    m_System->CreateStringProperty(this, PASSBINDING + 39, PASSBINDING + 42, "Shdw.Cast.Vertex", pass->getShadowCasterVertexProgramName());
    m_System->CreateStringProperty(this, PASSBINDING + 39, PASSBINDING + 43, "Shdw.Recv.Vertex", pass->getShadowReceiverVertexProgramName());
    m_System->CreateStringProperty(this, PASSBINDING + 39, PASSBINDING + 44, "Shdw.Recv.Fragment", pass->getShadowReceiverFragmentProgramName());

    m_System->CreateGroupProperty(this, PASSBINDING, PASSBINDING + 59, "Texture Units");

    _displayTextureUnits(pass, PASSBINDING + 59);

    for(unsigned int i = 0;i < 62;i++)
    m_System->EnableProperty(this, PASSBINDING + i - 2, false);*/
}
//-----------------------------------------------------------------------------------------
void CMaterialEditor::_displayTextureUnits(Ogre::Pass *pass, unsigned int BINDING)
{
    /*unsigned int count = pass->getNumTextureUnitStates();
    for(unsigned int i = 0;i < count;i++)
    {
    unsigned int UNITBINDING = (i * 15) + BINDING + 1;
    Ogre::TextureUnitState *unit = pass->getTextureUnitState(i);

    m_System->CreateGroupProperty(this, BINDING, UNITBINDING, "Unit " + Ogre::StringConverter::toString(i));
    m_System->CreateStringProperty(this, UNITBINDING, UNITBINDING + 1, "Name", unit->getName());
    m_System->CreateStringProperty(this, UNITBINDING, UNITBINDING + 2, "Texture", unit->getTextureName());
    for(unsigned int i = 0;i < 3;i++)
    m_System->EnableProperty(this, UNITBINDING + i, false);
    }*/
}
//-----------------------------------------------------------------------------------------
bool CMaterialEditor::_setMaterialPath(OgitorsPropertyBase* property, const Ogre::String& value)
{
    mMaterialPath->set(OgitorsUtils::QualifyPath(value));
    return true;
}
//-----------------------------------------------------------------------------------------
bool CMaterialEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    mHandle = Ogre::MaterialManager::getSingletonPtr()->getByName(mName->get());

    mLoaded->set(true);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CMaterialEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    mHandle.setNull();

    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CMaterialEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mMaterialPath, "material_path", Ogre::String, "", 0, SETTER(Ogre::String, CMaterialEditor, _setMaterialPath)); 

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//------CMATERIALEDITORFACTORY-----------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CMaterialEditorFactory::CMaterialEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Material Object";
    mEditorType = ETYPE_GENERALPURPOSE;
    mAddToObjectList = false;
    mRequirePlacement = true;
    mIcon = "material.svg";
    mCapabilities = CAN_UNDO;

    AddPropertyDefinition("material_path", "Material Path", "The path to the material file", Ogitors::PROP_STRING);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CMaterialEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = new CMaterialEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CMaterialEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CMaterialEditor *object = new CMaterialEditor(this);
    OgitorsPropertyValueMap::iterator ni;

    if((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Material"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
}
