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
#include "OgitorsRoot.h"
#include "VisualHelper.h"
#include "LightVisualHelper.h"
#include "LightEditor.h"
#include "tinyxml.h"

using namespace Ogitors;

PropertyOptionsVector CLightEditorFactory::mLightTypes;

//-----------------------------------------------------------------------------------------
CLightEditor::CLightEditor(CBaseEditorFactory *factory) : CBaseEditor(factory)
{
    mHandle = 0;
    mHelper = 0;
}
//-----------------------------------------------------------------------------------------
CLightEditor::~CLightEditor()
{
}
//-----------------------------------------------------------------------------------------
void CLightEditor::setParentImpl(CBaseEditor *oldparent, CBaseEditor *newparent)
{
    if(!mHandle)
        return;

    if(oldparent) 
        mHandle->detachFromParent();
    
    if(newparent) 
        newparent->getNode()->attachObject(mHandle);
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::setLayerImpl(unsigned int newlayer)
{
    if(mHelper)
        mHelper->setVisiblityFlags(1 << newlayer);

    if(mHandle)
        mHandle->setVisibilityFlags(1 << newlayer);

    return true;
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CLightEditor::getAABB() 
{
    if(mHelper) 
        return mHelper->getAABB(); 
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
Ogre::AxisAlignedBox CLightEditor::getWorldAABB() 
{
    if(mHelper) 
        return mHelper->getWorldAABB(); 
    else 
        return Ogre::AxisAlignedBox::BOX_NULL;
}
//-----------------------------------------------------------------------------------------
void CLightEditor::setDerivedPosition(Ogre::Vector3 val)
{
    if(getParent())
    {
        Ogre::Quaternion qParent = getParent()->getDerivedOrientation().Inverse();
        Ogre::Vector3 vParent = getParent()->getDerivedPosition();
        Ogre::Vector3 newPos = (val - vParent);
        val = qParent * newPos;
    }
    mPosition->set(val);
}
//-----------------------------------------------------------------------------------------
Ogre::Quaternion CLightEditor::getDerivedOrientation()
{
    if(getParent())
        return (getParent()->getDerivedOrientation() * mOrientation->get());
    else
        return mOrientation->get();
}
//-----------------------------------------------------------------------------------------
Ogre::Vector3 CLightEditor::getDerivedPosition()
{
    if(mHandle)
        return mHandle->getDerivedPosition();
    else
        return getParent()->getDerivedPosition() + (getParent()->getDerivedOrientation() * mPosition->get());
}
//-----------------------------------------------------------------------------------------
void CLightEditor::_calculateOrientation()
{
    Ogre::Vector3 direction = mHandle->getDerivedDirection();
    Ogre::Vector3 normal = Ogre::Vector3::UNIT_Z;
    mOrientation->initAndSignal( normal.getRotationTo(direction) );
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setPosition(OgitorsPropertyBase* property, const Ogre::Vector3& position)
{
    if(mHandle)
    {
        mHandle->setPosition(position);
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setOrientation(OgitorsPropertyBase* property, const Ogre::Quaternion& orientation)
{
    Ogre::Vector3 direction(0,0,1);
    direction = orientation * direction;
 
    if(mHandle)
    {
        mHandle->setDirection(direction);
    }    
    
    mDirection->initAndSignal(direction);
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setDirection(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(value == Ogre::Vector3::ZERO)
        mDirection->init(mDirection->getOld());

    if(mHandle)
    {
        mHandle->setDirection(value.normalisedCopy());
        _calculateOrientation();
    }
    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setLightType(OgitorsPropertyBase* property, const int& value)
{
    Ogre::Light::LightTypes ltype = (Ogre::Light::LightTypes)value;
    if(mHandle)
    {
        mHandle->setType(ltype);
        if(ltype == Ogre::Light::LT_SPOTLIGHT)
            _setRange(0, mRange->get());
        
        adjustBoundingBox();
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setDiffuse(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mHandle)
    {
        mHandle->setDiffuseColour(value);
    }

    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setSpecular(OgitorsPropertyBase* property, const Ogre::ColourValue& value)
{
    if(mHandle)
    {
        mHandle->setSpecularColour(value);
    }
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setRange(OgitorsPropertyBase* property, const Ogre::Vector3& value)
{
    if(mHandle && mHandle->getType() == Ogre::Light::LT_SPOTLIGHT)
    {
        mHandle->setSpotlightRange(Ogre::Degree(value.x),Ogre::Degree(value.y),value.z);
    }
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setAttenuation(OgitorsPropertyBase* property, const Ogre::Vector4& value)
{
    if(mHandle)
    {
        mHandle->setAttenuation(value.x,value.y,value.z,value.w);
    } 

    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setPower(OgitorsPropertyBase* property, const Ogre::Real& value)
{
    if(mHandle)
    {
        mHandle->setPowerScale(value);
    } 

    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::_setCastShadows(OgitorsPropertyBase* property, const bool& value)
{
    if(mHandle)
    {
        mHandle->setCastShadows(value);
    } 
    
    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::load(bool async)
{
    if(mLoaded->get())
        return true;

    if(!getParent()->load())
        return false;

    mHandle = mOgitorsRoot->GetSceneManager()->createLight(mName->get());
    getParent()->getNode()->attachObject(mHandle);
    
    mHandle->setPosition(mPosition->get());
    mHandle->setDirection(mDirection->get());
    mHandle->setDiffuseColour(mDiffuse->get());
    mHandle->setSpecularColour(mSpecular->get());
    mHandle->setType((Ogre::Light::LightTypes)mLightType->get());
    mHandle->setQueryFlags(0);
    
    if(mLightType->get() == Ogre::Light::LT_SPOTLIGHT)
        mHandle->setSpotlightRange(Ogre::Degree(mRange->get().x), Ogre::Degree(mRange->get().y), mRange->get().z);

    mHandle->setAttenuation(mAttenuation->get().x, mAttenuation->get().y, mAttenuation->get().z, mAttenuation->get().w);
    mHandle->setCastShadows(mCastShadows->get());
    mHandle->setPowerScale(mPower->get());

    CLightVisualHelper *helper = OGRE_NEW CLightVisualHelper(this);
    helper->setVisiblityFlags(1 << mLayer->get());
    helper->Show(mOgitorsRoot->GetLightVisiblity());

    setHelper(helper);

    mLoaded->set(true);

    registerForPostSceneUpdates();

    return true;
}
//-----------------------------------------------------------------------------------------
bool CLightEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

    unLoadAllChildren();

    destroyBoundingBox();

    if(mHandle)
    {
        mHandle->detachFromParent();
        mHandle->_getManager()->destroyLight(mHandle);
        mHandle = 0;
    }

    if(mHelper)
    {
        mHelper->Destroy();
        OGRE_DELETE mHelper;
        mHelper = 0;
    }
    
    mLoaded->set(false);
    return true;
}
//-----------------------------------------------------------------------------------------
void CLightEditor::createProperties(OgitorsPropertyValueMap &params)
{
    PROPERTY_PTR(mPosition   ,"position"   ,Ogre::Vector3    ,Ogre::Vector3::ZERO       , 0, SETTER(Ogre::Vector3, CLightEditor, _setPosition));
    PROPERTY_PTR(mOrientation,"orientation",Ogre::Quaternion ,Ogre::Quaternion::IDENTITY, 0, SETTER(Ogre::Quaternion, CLightEditor, _setOrientation));
    PROPERTY_PTR(mDirection  ,"direction"  ,Ogre::Vector3    ,Ogre::Vector3::UNIT_Z     , 0, SETTER(Ogre::Vector3, CLightEditor, _setDirection));
    PROPERTY_PTR(mLightType  ,"lighttype"  ,int              ,Ogre::Light::LT_POINT     , 0, SETTER(int, CLightEditor, _setLightType));
    PROPERTY_PTR(mDiffuse    ,"diffuse"    ,Ogre::ColourValue,Ogre::ColourValue(1,1,1,1), 0, SETTER(Ogre::ColourValue, CLightEditor, _setDiffuse));
    PROPERTY_PTR(mSpecular   ,"specular"   ,Ogre::ColourValue,Ogre::ColourValue(0,0,0,1), 0, SETTER(Ogre::ColourValue, CLightEditor, _setSpecular));
    PROPERTY_PTR(mRange      ,"lightrange" ,Ogre::Vector3    ,Ogre::Vector3(15,30,1.0f) , 0, SETTER(Ogre::Vector3, CLightEditor, _setRange));
    PROPERTY_PTR(mAttenuation,"attenuation",Ogre::Vector4    ,Ogre::Vector4(100,0.5,0.1f,0), 0, SETTER(Ogre::Vector4, CLightEditor, _setAttenuation));
    PROPERTY_PTR(mPower      ,"power"      ,Ogre::Real       ,1.0f                      , 0, SETTER(Ogre::Real, CLightEditor, _setPower));
    PROPERTY_PTR(mCastShadows,"castshadows",bool             ,true                      , 0, SETTER(bool, CLightEditor, _setCastShadows));

    mProperties.initValueMap(params);
}
//-----------------------------------------------------------------------------------------
TiXmlElement* CLightEditor::exportDotScene(TiXmlElement *pParent)
{
    Ogre::String ltype = "point";

    if(mLightType->get() == Ogre::Light::LT_DIRECTIONAL)
        ltype = "directional";
    else if(mLightType->get() == Ogre::Light::LT_SPOTLIGHT)
        ltype = "spot";

    // light
    TiXmlElement *pLight = pParent->InsertEndChild(TiXmlElement("light"))->ToElement();
    pLight->SetAttribute("name", mName->get().c_str());
    pLight->SetAttribute("id", Ogre::StringConverter::toString(mObjectID->get()).c_str());
    pLight->SetAttribute("type", ltype.c_str());
    pLight->SetAttribute("castShadows", Ogre::StringConverter::toString(mCastShadows->get()).c_str());
    // light position
    TiXmlElement *pLightPosition = pLight->InsertEndChild(TiXmlElement("position"))->ToElement();
    pLightPosition->SetAttribute("x", Ogre::StringConverter::toString(mPosition->get().x).c_str());
    pLightPosition->SetAttribute("y", Ogre::StringConverter::toString(mPosition->get().y).c_str());
    pLightPosition->SetAttribute("z", Ogre::StringConverter::toString(mPosition->get().z).c_str());
    // direction vector
    if(mLightType->get() != Ogre::Light::LT_POINT)
    {
        TiXmlElement *pDirectionVector = pLight->InsertEndChild(TiXmlElement("directionVector"))->ToElement();
        pDirectionVector->SetAttribute("x", Ogre::StringConverter::toString(mDirection->get().x).c_str());
        pDirectionVector->SetAttribute("y", Ogre::StringConverter::toString(mDirection->get().y).c_str());
        pDirectionVector->SetAttribute("z", Ogre::StringConverter::toString(mDirection->get().z).c_str());
    }
    // diffuse
    TiXmlElement *pColourDiffuse = pLight->InsertEndChild(TiXmlElement("colourDiffuse"))->ToElement();
    pColourDiffuse->SetAttribute("r", Ogre::StringConverter::toString(mDiffuse->get().r).c_str());
    pColourDiffuse->SetAttribute("g", Ogre::StringConverter::toString(mDiffuse->get().g).c_str());
    pColourDiffuse->SetAttribute("b", Ogre::StringConverter::toString(mDiffuse->get().b).c_str());
    pColourDiffuse->SetAttribute("a", Ogre::StringConverter::toString(mDiffuse->get().a).c_str());
    // specular
    TiXmlElement *pColourSpecular = pLight->InsertEndChild(TiXmlElement("colourSpecular"))->ToElement();
    pColourSpecular->SetAttribute("r", Ogre::StringConverter::toString(mSpecular->get().r).c_str());
    pColourSpecular->SetAttribute("g", Ogre::StringConverter::toString(mSpecular->get().g).c_str());
    pColourSpecular->SetAttribute("b", Ogre::StringConverter::toString(mSpecular->get().b).c_str());
    pColourSpecular->SetAttribute("a", Ogre::StringConverter::toString(mSpecular->get().a).c_str());
    // light range
    if(mLightType->get() == Ogre::Light::LT_SPOTLIGHT)
    {
        TiXmlElement *pLightRange = pLight->InsertEndChild(TiXmlElement("lightRange"))->ToElement();
        pLightRange->SetAttribute("inner", Ogre::StringConverter::toString(Ogre::Degree(mRange->get().x)).c_str());
        pLightRange->SetAttribute("outer", Ogre::StringConverter::toString(Ogre::Degree(mRange->get().y)).c_str());
        pLightRange->SetAttribute("falloff", Ogre::StringConverter::toString(mRange->get().z).c_str());
    }
    // attenuation
    if(mLightType->get() != Ogre::Light::LT_DIRECTIONAL)
    {
        TiXmlElement *pLightAttenuation = pLight->InsertEndChild(TiXmlElement("lightAttenuation"))->ToElement();
        pLightAttenuation->SetAttribute("range", Ogre::StringConverter::toString(mAttenuation->get().x).c_str());
        pLightAttenuation->SetAttribute("constant", Ogre::StringConverter::toString(mAttenuation->get().y).c_str());
        pLightAttenuation->SetAttribute("linear", Ogre::StringConverter::toString(mAttenuation->get().z).c_str());
        pLightAttenuation->SetAttribute("quadratic", Ogre::StringConverter::toString(mAttenuation->get().w).c_str());
    }

   return pLight;
}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//---------CLIGHTEDITORFACTORY-------------------------------------------------------------
//-----------------------------------------------------------------------------------------
CLightEditorFactory::CLightEditorFactory(OgitorsView *view) : CBaseEditorFactory(view)
{
    mTypeName = "Light";
    mEditorType = ETYPE_LIGHT;
    mAddToObjectList = true;
    mRequirePlacement = true;
    mIcon = "light.svg";
    mCapabilities = CAN_MOVE | CAN_ROTATE | CAN_CLONE | CAN_DELETE  | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_ACCEPTCOPY;
    mUsesGizmos = true;
    mUsesHelper = true;

    mLightTypes.clear();
    mLightTypes.push_back(PropertyOption("LT_POINT",Ogre::Any((int)Ogre::Light::LT_POINT)));
    mLightTypes.push_back(PropertyOption("LT_DIRECTIONAL",Ogre::Any((int)Ogre::Light::LT_DIRECTIONAL)));
    mLightTypes.push_back(PropertyOption("LT_SPOTLIGHT",Ogre::Any((int)Ogre::Light::LT_SPOTLIGHT)));

    OgitorsPropertyDef *definition;

    AddPropertyDefinition("position", "Position", "The position of the object.",PROP_VECTOR3);
    AddPropertyDefinition("orientation","Orientation", "The orientation of the object.",PROP_QUATERNION, true, false);
    AddPropertyDefinition("direction", "Direction", "The direction of the object.",PROP_VECTOR3);
    
    definition = AddPropertyDefinition("lighttype", "Light Type", "The light object's type.",PROP_INT);
    definition->setOptions(&mLightTypes);
    AddPropertyDefinition("diffuse", "Diffuse", "The light object's diffuse colour.",PROP_COLOUR);
    AddPropertyDefinition("specular", "Specular", "The light object's specular colour.",PROP_COLOUR);

    definition = AddPropertyDefinition("lightrange", "Range", "The light object's range.",PROP_VECTOR3);
    definition->setFieldNames("InnerAngle", "OuterAngle", "Falloff");

    definition = AddPropertyDefinition("attenuation", "Attenuation", "The light object's attenuation.",PROP_VECTOR4);
    definition->setFieldNames("Distance", "Constant Mult.", "Linear Mult.", "Quadratic Mult.");

    AddPropertyDefinition("power", "Power", "The light object's power.",PROP_REAL);
    AddPropertyDefinition("castshadows", "Cast Shadows", "Does the object cast shadows?",PROP_BOOL);

    OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("updatescript");
    it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CLightEditorFactory::duplicate(OgitorsView *view)
{
    CBaseEditorFactory *ret = OGRE_NEW CLightEditorFactory(view);
    ret->mTypeID = mTypeID;

    return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CLightEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
    CLightEditor *object = OGRE_NEW CLightEditor(this);
    
    OgitorsPropertyValueMap::iterator ni;

    if ((ni = params.find("init")) != params.end())
    {
        OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
        value.val = Ogre::Any(CreateUniqueID("Light"));
        params["name"] = value;
        params.erase(ni);
    }

    object->createProperties(params);
    object->mParentEditor->init(*parent);
    object->load();

    mInstanceCount++;
    return object;
}
//-----------------------------------------------------------------------------------------
