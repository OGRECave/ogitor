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

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "NodeEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "BillboardSetEditor.h"

using namespace Ogitors;

PropertyOptionsVector CBillboardSetEditorFactory::mBillboardTypes;
PropertyOptionsVector CBillboardSetEditorFactory::mBillboardOriginTypes;
PropertyOptionsVector CBillboardSetEditorFactory::mBillboardRotationTypes;

//-----------------------------------------------------------------------------------------
CBillboardSetEditor::CBillboardSetEditor(CBaseEditorFactory *factory) : CNodeEditor(factory),
mBillboardSetHandle(0)
{
    mHelper = 0;
    mUsesGizmos = true;
    mUsesHelper = false;
}
//-----------------------------------------------------------------------------------------
CBillboardSetEditor::~CBillboardSetEditor()
{
}
//-----------------------------------------------------------------------------------------
void CBillboardSetEditor::onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname)
{
    mProperties.setValue("material", materialname);
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::setLayerImpl(unsigned int newlayer)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setVisibilityFlags(1 << newlayer);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setBillboardType(OgitorsPropertyBase* property, const int& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setBillboardType((Ogre::BillboardType)value);

    adjustBoundingBox();
    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setSorting(OgitorsPropertyBase* property, const bool& bSorting)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setSortingEnabled(bSorting);
    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setOrigin(OgitorsPropertyBase* property, const int& origin)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setBillboardOrigin((Ogre::BillboardOrigin)origin);
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setRotation(OgitorsPropertyBase* property, const int& rotation)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setBillboardRotationType((Ogre::BillboardRotationType)rotation);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setWidth(OgitorsPropertyBase* property, const Ogre::Real& width)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setDefaultWidth(width);
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setHeight(OgitorsPropertyBase* property, const Ogre::Real& height)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setDefaultHeight(height);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setMaterial(OgitorsPropertyBase* property, const Ogre::String& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setMaterialName(value);
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setPointRendering(OgitorsPropertyBase* property, const bool& point)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setPointRenderingEnabled(point);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setRenderingDistance(OgitorsPropertyBase* property, const Ogre::Real& distance)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->setRenderingDistance(distance);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setBillboardPosition(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->getBillboard(property->getTag())->setPosition(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setBillboardColour(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->getBillboard(property->getTag())->setColour(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setBillboardRotation(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->getBillboard(property->getTag())->setRotation(Ogre::Degree(value));

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setBillboardDimensions(OgitorsPropertyBase* property, const Ogre::Vector2& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->getBillboard(property->getTag())->setDimensions(value.x, value.y);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setBillboardTexCoordIndex(OgitorsPropertyBase* property, const int& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->getBillboard(property->getTag())->setTexcoordIndex(value);

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::_setBillboardTexRect(OgitorsPropertyBase* property, const Ogre::Vector4& value)
{
    if(mBillboardSetHandle)
        mBillboardSetHandle->getBillboard(property->getTag())->setTexcoordRect(value.x, value.y, value.z, value.w);

    return true;
}
//-----------------------------------------------------------------------------------------
void CBillboardSetEditor::_removeBillboard(int id)
{
    int count = mBillboardCount->get();

    Ogre::String propName = "billboard" + Ogre::StringConverter::toString(id);
    Ogre::String propName2;
    Ogre::Vector2 v2val;
    Ogre::Vector3 v3val;
    Ogre::Vector4 v4val;
    Ogre::ColourValue cval;
    Ogre::Real rval;
    int ival;

    for(int c = (id + 1);c < count;c++)
    {
        propName = "billboard" + Ogre::StringConverter::toString(c -1);
        propName2 = "billboard" + Ogre::StringConverter::toString(c);
        mProperties.getValue(propName2 + "::position", v3val);
        mProperties.getValue(propName2 + "::colour", cval);
        mProperties.getValue(propName2 + "::dimensions", v2val);
        mProperties.getValue(propName2 + "::rotation", rval);
        mProperties.getValue(propName2 + "::texcoordindex", ival);
        mProperties.getValue(propName2 + "::texrect", v4val);

        static_cast<OgitorsProperty<Ogre::Vector3>*>(mProperties.getProperty(propName + "::position"))->initAndSignal(v3val);
        static_cast<OgitorsProperty<Ogre::ColourValue>*>(mProperties.getProperty(propName + "::colour"))->initAndSignal(cval);
        static_cast<OgitorsProperty<Ogre::Vector2>*>(mProperties.getProperty(propName + "::dimensions"))->initAndSignal(v2val);
        static_cast<OgitorsProperty<Ogre::Real>*>(mProperties.getProperty(propName + "::rotation"))->initAndSignal(rval);
        static_cast<OgitorsProperty<int>*>(mProperties.getProperty(propName + "::texcoordindex"))->initAndSignal(ival);
        static_cast<OgitorsProperty<Ogre::Vector4>*>(mProperties.getProperty(propName + "::texrect"))->initAndSignal(v4val);
    }
    
    propName = "billboard" + Ogre::StringConverter::toString(count - 1);
    mProperties.removeProperty(propName + "::position");
    mProperties.removeProperty(propName + "::colour");
    mProperties.removeProperty(propName + "::dimensions");
    mProperties.removeProperty(propName + "::rotation");
    mProperties.removeProperty(propName + "::texcoordindex");
    mProperties.removeProperty(propName + "::texrect");
    mBillboardCount->set(mBillboardCount->get() - 1);

    if(mBillboardSetHandle)
        mBillboardSetHandle->removeBillboard(id);
}
//-----------------------------------------------------------------------------------------
void CBillboardSetEditor::_addBillboard()
{
    Ogre::String propName;
    Ogre::String propName2;
    OgitorsPropertyDef *definition;

    int count = mBillboardCount->get();

    Ogre::Billboard *pBillboard = 0;
    if(mBillboardSetHandle)
        pBillboard = mBillboardSetHandle->createBillboard(Ogre::Vector3::ZERO);

    Ogre::String sCount1 = "Billboards::Billboard" + Ogre::StringConverter::toString(count);
    Ogre::String sCount2 = "billboard" + Ogre::StringConverter::toString(count);

    mFactory->AddPropertyDefinition(sCount2 + "::position", sCount1 + "::Position", "Billboard position.", PROP_VECTOR3);
    mFactory->AddPropertyDefinition(sCount2 + "::colour", sCount1 + "::Colour", "Billboard colour.", PROP_COLOUR);
    mFactory->AddPropertyDefinition(sCount2 + "::rotation", sCount1 + "::Rotation", "Billboard rotation.", PROP_REAL);
    definition = mFactory->AddPropertyDefinition(sCount2 + "::dimensions", sCount1 + "::Dimensions", "Billboard dimensions.", PROP_VECTOR2);
    definition->setFieldNames("Width","Height");
    mFactory->AddPropertyDefinition(sCount2 + "::texcoordindex", sCount1 + "::TexCoordIndex", "Billboard texture coordination index.", PROP_INT);
    definition = mFactory->AddPropertyDefinition(sCount2 + "::texrect", sCount1 + "::TexRect", "Billboard texture mapping rect.", PROP_VECTOR4);
    definition->setFieldNames("U1","V1","U2","V2");
    
    PROPERTY(sCount2 + "::position", Ogre::Vector3  , Ogre::Vector3::ZERO, count, SETTER(Ogre::Vector3, CBillboardSetEditor, _setBillboardPosition)); 
    PROPERTY(sCount2 + "::dimensions", Ogre::Vector2, Ogre::Vector2(mDefaultWidth->get(),mDefaultHeight->get()) , count, SETTER(Ogre::Vector2, CBillboardSetEditor, _setBillboardDimensions)); 
    PROPERTY(sCount2 + "::colour", Ogre::ColourValue, Ogre::ColourValue(1,1,1), count, SETTER(Ogre::ColourValue, CBillboardSetEditor, _setBillboardColour)); 
    PROPERTY(sCount2 + "::rotation", Ogre::Real     , 0                  , count, SETTER(Ogre::Real, CBillboardSetEditor, _setBillboardRotation)); 
    PROPERTY(sCount2 + "::texcoordindex", int       , 0                  , count, SETTER(int, CBillboardSetEditor, _setBillboardTexCoordIndex)); 
    PROPERTY(sCount2 + "::texrect", Ogre::Vector4   , Ogre::Vector4(0,0,1,1), count, SETTER(Ogre::Vector4, CBillboardSetEditor, _setBillboardTexRect));

    mBillboardCount->set(mBillboardCount->get() + 1);
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::load(bool async)
{
    if(mLoaded->get())
        return true;
    
    if(CNodeEditor::load())
    {
        mBillboardSetHandle = mOgitorsRoot->GetSceneManager()->createBillboardSet(mName->get());
        mHandle->attachObject(mBillboardSetHandle);
        
        mBillboardSetHandle->setBillboardType((Ogre::BillboardType)mBillboardType->get());
        mBillboardSetHandle->setSortingEnabled(mSorting->get());
        mBillboardSetHandle->setBillboardOrigin((Ogre::BillboardOrigin)mOrigin->get());
        mBillboardSetHandle->setBillboardRotationType((Ogre::BillboardRotationType)mRotation->get());
        mBillboardSetHandle->setDefaultWidth(mDefaultWidth->get());
        mBillboardSetHandle->setDefaultHeight(mDefaultHeight->get());
        mBillboardSetHandle->setVisibilityFlags(1 << mLayer->get());
        if(mMaterial->get() != "")
            mBillboardSetHandle->setMaterialName(mMaterial->get());
        
        mBillboardSetHandle->setPointRenderingEnabled(mPointRendering->get());
        mBillboardSetHandle->setRenderingDistance(mRenderDistance->get());

        Ogre::Vector2 v2val;
        Ogre::Vector3 v3val;
        Ogre::Vector4 v4val;
        Ogre::ColourValue cval;
        Ogre::Real rval;
        int ival;
        Ogre::String propname;

        int count = mBillboardCount->get();
        for(int ix = 0;ix < count;ix++)
        {
            propname = "billboard" + Ogre::StringConverter::toString(ix);

            Ogre::Billboard *pBillboard = mBillboardSetHandle->createBillboard(Ogre::Vector3::ZERO);
            mProperties.getValue(propname + "::position", v3val);
            pBillboard->setPosition(v3val);
            mProperties.getValue(propname + "::colour", cval);
            pBillboard->setColour(cval);
            mProperties.getValue(propname + "::dimensions", v2val);
            pBillboard->setDimensions(v2val.x, v2val.y);
            mProperties.getValue(propname + "::rotation", rval);
            pBillboard->setRotation(Ogre::Degree(rval));
            mProperties.getValue(propname + "::texcoordindex", ival);
            pBillboard->setTexcoordIndex(ival);
            mProperties.getValue(propname + "::texrect", v4val);
            pBillboard->setTexcoordRect(v4val.x, v4val.y, v4val.z, v4val.w);
        }

    }
    else
        return false;

    return true;
}
//-----------------------------------------------------------------------------------------
bool CBillboardSetEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    if(mBillboardSetHandle)
    {
        mBillboardSetHandle->detachFromParent();
        mBillboardSetHandle->_getManager()->destroyBillboardSet(mBillboardSetHandle);
        mBillboardSetHandle = 0;
    }
    
    return CNodeEditor::unLoad();
}
//-----------------------------------------------------------------------------------------
void CBillboardSetEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mBillboardType , "billboardtype"    , int         , Ogre::BBT_ORIENTED_COMMON, 0, SETTER(int, CBillboardSetEditor, _setBillboardType));
    PROPERTY_PTR(mSorting       , "sorting"          , bool        , true                     , 0, SETTER(bool, CBillboardSetEditor, _setSorting));
    PROPERTY_PTR(mOrigin        , "billboardorigin"  , int         , Ogre::BBO_CENTER         , 0, SETTER(int, CBillboardSetEditor, _setOrigin));
    PROPERTY_PTR(mRotation      , "billboardrotation", int         , Ogre::BBR_VERTEX         , 0, SETTER(int, CBillboardSetEditor, _setRotation));
    PROPERTY_PTR(mDefaultWidth  , "defaultwidth"     , Ogre::Real  , 1.0f                     , 0, SETTER(Ogre::Real, CBillboardSetEditor, _setWidth));
    PROPERTY_PTR(mDefaultHeight , "defaultheight"    , Ogre::Real  , 1.0f                     , 0, SETTER(Ogre::Real, CBillboardSetEditor, _setHeight));
    PROPERTY_PTR(mMaterial      , "material"         , Ogre::String, ""                       , 0, SETTER(Ogre::String, CBillboardSetEditor, _setMaterial));
    PROPERTY_PTR(mPointRendering, "pointrendering"   , bool        , false                    , 0, SETTER(bool, CBillboardSetEditor, _setPointRendering));
    PROPERTY_PTR(mRenderDistance, "renderdistance"   , Ogre::Real  , 0.0f                     , 0, SETTER(Ogre::Real, CBillboardSetEditor, _setRenderingDistance));
    PROPERTY_PTR(mBillboardCount, "billboardcount"   , int         , 1                        , 0, 0);

    int count = 1;
    OgitorsPropertyValueMap::const_iterator it = params.find("billboardcount");
    if(it != params.end())
        count = Ogre::any_cast<int>(it->second.val);

    OgitorsPropertyDef *definition;
    for(int ix = 0;ix < count;ix++)
    {
        Ogre::String sCount1 = "Billboards::Billboard" + Ogre::StringConverter::toString(ix);
        Ogre::String sCount2 = "billboard" + Ogre::StringConverter::toString(ix);

        mFactory->AddPropertyDefinition(sCount2 + "::position", sCount1 + "::Position", "Billboard position.", PROP_VECTOR3);
        mFactory->AddPropertyDefinition(sCount2 + "::colour", sCount1 + "::Colour", "Billboard colour.", PROP_COLOUR);
        mFactory->AddPropertyDefinition(sCount2 + "::rotation", sCount1 + "::Rotation", "Billboard rotation.", PROP_REAL);
        definition = mFactory->AddPropertyDefinition(sCount2 + "::dimensions", sCount1 + "::Dimensions", "Billboard dimensions.", PROP_VECTOR2);
        definition->setFieldNames("Width","Height");
        mFactory->AddPropertyDefinition(sCount2 + "::texcoordindex", sCount1 + "::TexCoordIndex", "Billboard texture coordination index.", PROP_INT);
        definition = mFactory->AddPropertyDefinition(sCount2 + "::texrect", sCount1 + "::TexRect", "Billboard texture mapping rect.", PROP_VECTOR4);
        definition->setFieldNames("U1","V1","U2","V2");

        PROPERTY(sCount2 + "::position", Ogre::Vector3  , Ogre::Vector3::ZERO, ix, SETTER(Ogre::Vector3, CBillboardSetEditor, _setBillboardPosition)); 
        PROPERTY(sCount2 + "::dimensions", Ogre::Vector2, Ogre::Vector2(mDefaultWidth->get(),mDefaultHeight->get()) , ix, SETTER(Ogre::Vector2, CBillboardSetEditor, _setBillboardDimensions)); 
        PROPERTY(sCount2 + "::colour", Ogre::ColourValue, Ogre::ColourValue(1,1,1), ix, SETTER(Ogre::ColourValue, CBillboardSetEditor, _setBillboardColour)); 
        PROPERTY(sCount2 + "::rotation", Ogre::Real     , 0                  , ix, SETTER(Ogre::Real, CBillboardSetEditor, _setBillboardRotation)); 
        PROPERTY(sCount2 + "::texcoordindex", int       , 0                  , ix, SETTER(int, CBillboardSetEditor, _setBillboardTexCoordIndex)); 
        PROPERTY(sCount2 + "::texrect", Ogre::Vector4   , Ogre::Vector4(0,0,1,1), ix, SETTER(Ogre::Vector4, CBillboardSetEditor, _setBillboardTexRect)); 
    }
    
    mProperties.initValueMap(params);
}
//-------------------------------------------------------------------------------
bool CBillboardSetEditor::getPropertyContextMenu(Ogre::String propertyName, UTFStringVector &menuitems)
{
    menuitems.clear();

    if((propertyName.find("billboard") != -1) || ((propertyName.find("Billboard") != -1) && (propertyName != "Billboards")))
    {
        menuitems.push_back(OTR("Add Billboard") + ";:/icons/additional.svg");
        menuitems.push_back(OTR("Remove Billboard") + ";:/icons/trash.svg");
        return true;
    }
    else
    {
        menuitems.push_back(OTR("Add Billboard") + ";:/icons/additional.svg");
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------------
void CBillboardSetEditor::onPropertyContextMenu(Ogre::String propertyName, int menuresult)
{
    switch(menuresult)
    {
    case 0:_addBillboard();
           break;
    case 1:
        {
            //Do not let the last billboard get deleted
            if(mBillboardCount->get() < 2)
                return;

            propertyName.erase(0,9);
            int pos = propertyName.find("::");
            if(pos == -1)
                pos = 5;

            Ogre::String idstr;
            idstr = propertyName.substr(0, pos);
            _removeBillboard(Ogre::StringConverter::parseInt(idstr));
            break;
        }
    }
}
//-------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//---------CBILLBOARDSETEDITORFACTORY-------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CBillboardSetEditorFactory::CBillboardSetEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
    mTypeName = "BillboardSet Object";
    mEditorType = ETYPE_BILLBOARDSET;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "billboardset.svg";
    mCapabilities = CAN_PAGE | CAN_MOVE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_USEMATERIAL | CAN_ACCEPTCOPY;

    OgitorsPropertyDef *definition;
    
    mBillboardTypes.clear();
    mBillboardTypes.push_back(PropertyOption("BBT_ORIENTED_COMMON", Ogre::Any((int)Ogre::BBT_ORIENTED_COMMON)));
    mBillboardTypes.push_back(PropertyOption("BBT_ORIENTED_SELF", Ogre::Any((int)Ogre::BBT_ORIENTED_SELF)));
    mBillboardTypes.push_back(PropertyOption("BBT_PERPENDICULAR_COMMON", Ogre::Any((int)Ogre::BBT_PERPENDICULAR_COMMON)));
    mBillboardTypes.push_back(PropertyOption("BBT_PERPENDICULAR_SELF", Ogre::Any((int)Ogre::BBT_PERPENDICULAR_SELF)));
    mBillboardTypes.push_back(PropertyOption("BBT_POINT", Ogre::Any((int)Ogre::BBT_POINT)));

    mBillboardOriginTypes.clear();
    mBillboardOriginTypes.push_back(PropertyOption("BBO_BOTTOM_CENTER", Ogre::Any((int)Ogre::BBO_BOTTOM_CENTER)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_BOTTOM_LEFT", Ogre::Any((int)Ogre::BBO_BOTTOM_LEFT)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_BOTTOM_RIGHT", Ogre::Any((int)Ogre::BBO_BOTTOM_RIGHT)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_CENTER", Ogre::Any((int)Ogre::BBO_CENTER)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_CENTER_LEFT", Ogre::Any((int)Ogre::BBO_CENTER_LEFT)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_CENTER_RIGHT", Ogre::Any((int)Ogre::BBO_CENTER_RIGHT)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_TOP_CENTER", Ogre::Any((int)Ogre::BBO_TOP_CENTER)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_TOP_LEFT", Ogre::Any((int)Ogre::BBO_TOP_LEFT)));
    mBillboardOriginTypes.push_back(PropertyOption("BBO_TOP_RIGHT", Ogre::Any((int)Ogre::BBO_TOP_RIGHT)));

    mBillboardRotationTypes.clear();
    mBillboardRotationTypes.push_back(PropertyOption("BBR_TEXCOORD", Ogre::Any((int)Ogre::BBR_TEXCOORD)));
    mBillboardRotationTypes.push_back(PropertyOption("BBR_VERTEX", Ogre::Any((int)Ogre::BBR_VERTEX)));

    definition = AddPropertyDefinition("billboardtype", "Billboard Type", "Type of the billboard set",PROP_INT, true, true);
    definition->setOptions(&mBillboardTypes);
    AddPropertyDefinition("sorting", "Sorting", "Enable/Disable sorting",PROP_BOOL, true, true);
    definition = AddPropertyDefinition("billboardorigin", "Billboard Origin", "Origin of billboards in the set",PROP_INT, true, true);
    definition->setOptions(&mBillboardOriginTypes);
    definition = AddPropertyDefinition("billboardrotation", "Billboard Rotation", "Rotation of billboards",PROP_INT, true, true);
    definition->setOptions(&mBillboardRotationTypes);
    AddPropertyDefinition("defaultwidth", "Default Width", "Default width of billboard",PROP_REAL, true, true);
    AddPropertyDefinition("defaultheight", "Default Height", "Default height of billboard",PROP_REAL, true, true);
    definition = AddPropertyDefinition("material", "Material", "Material used by the object.",PROP_STRING, true, true);
    definition->setOptions(OgitorsRoot::GetMaterialNames());
    AddPropertyDefinition("pointrendering", "Point Rendering", "Render billboards as points?",PROP_BOOL, true, true);
    AddPropertyDefinition("renderdistance", "Render Distance", "Maximum render distance",PROP_REAL, true, true);
    AddPropertyDefinition("billboardcount", "", "Number of Billboards",PROP_INT, false, false);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CBillboardSetEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CBillboardSetEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CBillboardSetEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CBillboardSetEditor *object =  OGRE_NEW CBillboardSetEditor(this);
    
    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("BillboardSet"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
