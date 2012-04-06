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
//This File is modified version of the Original Implementation by OGITOR TEAM
/////////////////////////////////////////////////////////////////////////////

#include "OgitorsPrerequisites.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include <Ogre.h>
#include <istream>
#include "OgitorsExports.h"
#include "OgitorsProperty.h"
#include "OgitorsUndoManager.h"

using namespace Ogitors;

namespace Ogitors
{
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    const Ogre::String& OgitorsPropertyDef::getTypeName(OgitorsPropertyType theType)
    {
        static const Ogre::String sPropNames[] = {
            "short",
            "unsigned short",
            "int",
            "unsigned int",
            "long",
            "unsigned long",
            "Real",
            "String",
            "Vector2",
            "Vector3",
            "Vector4",
            "ColourValue",
            "bool",
            "Quaternion",
            "Matrix3",
            "Matrix4"
        };

        return sPropNames[(int)theType];
    }
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    OgitorsPropertySet::OgitorsPropertySet() : mType(PROPSET_OBJECT), mRefCount(1)
    {
 
    }
    //---------------------------------------------------------------------
    OgitorsPropertySet::~OgitorsPropertySet()
    {
        for (OgitorsPropertyMap::iterator i = mPropertyMap.begin(); i != mPropertyMap.end(); ++i)
        {
            OGRE_DELETE i->second;
        }
        mPropertyMap.clear();
        
        for (unsigned int c = 0; c < mPropertyConnections.size(); c++)
        {
            OGRE_DELETE mPropertyConnections[c];
        }
        mPropertyConnections.clear();

        mPropertyVector.clear();

        mListeners.clear();
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::addProperty(OgitorsPropertyBase* prop)
    {
        std::pair<OgitorsPropertyMap::iterator, bool> retPair = 
            mPropertyMap.insert(OgitorsPropertyMap::value_type(prop->getName(), prop));

        assert(retPair.second);

        mPropertyVector.push_back(prop);

        if(prop->getDefinition()->getTrackChanges())
        {
            OgitorsScopedConnection *connection = OGRE_NEW OgitorsScopedConnection();
            prop->connect(OgitorsSignalFunction::from_method<OgitorsPropertySet, &OgitorsPropertySet::propertyChangeTracker>(this), *connection);
            mPropertyConnections.push_back(connection);
        }
        for(unsigned int i = 0;i < mListeners.size();i++)
            mListeners[i]->OnPropertyAdded(this, prop);
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::removeProperty(OgitorsPropertyBase* prop)
    {
        removeProperty(prop->getName());
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::removeProperty(const Ogre::String& propname)
    {
        unsigned int i;
        OgitorsPropertyMap::iterator it = mPropertyMap.find(propname);

        if(it != mPropertyMap.end())
        {
            for(i = 0;i < mPropertyVector.size();i++)
            {
                if(mPropertyVector[i] == it->second)
                {
                    mPropertyVector.erase(mPropertyVector.begin() + i);
                    break;
                }
            }

            for(i = 0;i < mListeners.size();i++)
                mListeners[i]->OnPropertyRemoved(this, it->second);

            OGRE_DELETE it->second;

            mPropertyMap.erase(it);
        }
    }
    //---------------------------------------------------------------------
       void OgitorsPropertySet::propertyChangeTracker(const Ogitors::OgitorsPropertyBase* property, Ogre::Any value)
    {
        OgitorsPropertyBase *prop = const_cast<OgitorsPropertyBase*>(property);
        for(unsigned int i = 0;i < mListeners.size();i++)
            mListeners[i]->OnPropertyChanged(this, prop);
    }
    //---------------------------------------------------------------------
    bool OgitorsPropertySet::hasProperty(const Ogre::String& name) const
    {
        OgitorsPropertyMap::const_iterator i = mPropertyMap.find(name);
        return i != mPropertyMap.end();
    }
    //---------------------------------------------------------------------
    OgitorsPropertyBase* OgitorsPropertySet::getProperty(const Ogre::String& name) const
    {
        OgitorsPropertyMap::const_iterator i = mPropertyMap.find(name);
        assert(i != mPropertyMap.end());
        return i->second;
    }
    //---------------------------------------------------------------------
    OgitorsPropertySet::OgitorsPropertyIterator OgitorsPropertySet::getPropertyIterator()
    {
        return OgitorsPropertyIterator(mPropertyMap.begin(), mPropertyMap.end());
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::addListener(OgitorsPropertySetListener* listener)
    {
        int found = -1;
        for(unsigned int i = 0;i < mListeners.size();i++)
        {
            if(mListeners[i] == listener)
            {
                found = i;
                break;
            }
        }
        if(found == -1)
            mListeners.push_back(listener);
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::removeListener(OgitorsPropertySetListener* listener)
    {
        for(unsigned int i = 0;i < mListeners.size();i++)
        {
            if(mListeners[i] == listener)
            {
                mListeners.erase(mListeners.begin() + i);
                return;
            }
        }
    }
    //---------------------------------------------------------------------
    OgitorsPropertyValueMap OgitorsPropertySet::getValueMap() const
    {
        OgitorsPropertyValueMap ret;
        for (OgitorsPropertyMap::const_iterator i = mPropertyMap.begin(); i != mPropertyMap.end(); ++i)
        {
            OgitorsPropertyValue val;
            val.propType = i->second->getType();

            switch(val.propType)
            {
            case PROP_SHORT:
                val.val = Ogre::Any(static_cast<OgitorsProperty<short>*>(i->second)->get());
                break;
            case PROP_UNSIGNED_SHORT:
                val.val = Ogre::Any(static_cast<OgitorsProperty<unsigned short>*>(i->second)->get());
                break;
            case PROP_INT:
                val.val = Ogre::Any(static_cast<OgitorsProperty<int>*>(i->second)->get());
                break;
            case PROP_UNSIGNED_INT:
                val.val = Ogre::Any(static_cast<OgitorsProperty<unsigned int>*>(i->second)->get());
                break;
            case PROP_LONG:
                val.val = Ogre::Any(static_cast<OgitorsProperty<long>*>(i->second)->get());
                break;
            case PROP_UNSIGNED_LONG:
                val.val = Ogre::Any(static_cast<OgitorsProperty<unsigned long>*>(i->second)->get());
                break;
            case PROP_REAL:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::Real>*>(i->second)->get());
                break;
            case PROP_STRING:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::String>*>(i->second)->get());
                break;
            case PROP_VECTOR2:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::Vector2>*>(i->second)->get());
                break;
            case PROP_VECTOR3:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::Vector3>*>(i->second)->get());
                break;
            case PROP_VECTOR4:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::Vector4>*>(i->second)->get());
                break;
            case PROP_COLOUR:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::ColourValue>*>(i->second)->get());
                break;
            case PROP_BOOL:
                val.val = Ogre::Any(static_cast<OgitorsProperty<bool>*>(i->second)->get());
                break;
            case PROP_QUATERNION:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::Quaternion>*>(i->second)->get());
                break;
            case PROP_MATRIX3:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::Matrix3>*>(i->second)->get());
                break;
            case PROP_MATRIX4:
                val.val = Ogre::Any(static_cast<OgitorsProperty<Ogre::Matrix4>*>(i->second)->get());
                break;
            };
            ret[i->second->getName()] = val;
        }

        return ret;
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::setValueMap(const OgitorsPropertyValueMap& values)
    {
        for (OgitorsPropertyValueMap::const_iterator i = values.begin(); i != values.end(); ++i)
        {
            OgitorsPropertyMap::iterator j = mPropertyMap.find(i->first);
            if (j != mPropertyMap.end())
            {
                // matching properties
                // check type
                if (j->second->getType() != i->second.propType)
                {
                    Ogre::StringUtil::StrStreamType msg;
                    msg << "Property " << i->first << " mismatched type; incoming type: '"
                        << OgitorsPropertyDef::getTypeName(i->second.propType) << "', property type: '"
                        << OgitorsPropertyDef::getTypeName(j->second->getType()) << "'";
                    OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS, msg.str(), "PropertySet::setValueMap");
                }
                switch(i->second.propType)
                {
                case PROP_SHORT:
                    static_cast<OgitorsProperty<short>*>(j->second)->set(Ogre::any_cast<short>(i->second.val));
                    break;
                case PROP_UNSIGNED_SHORT:
                    static_cast<OgitorsProperty<short>*>(j->second)->set(Ogre::any_cast<unsigned short>(i->second.val));
                    break;
                case PROP_INT:
                    static_cast<OgitorsProperty<int>*>(j->second)->set(Ogre::any_cast<int>(i->second.val));
                    break;
                case PROP_UNSIGNED_INT:
                    static_cast<OgitorsProperty<int>*>(j->second)->set(Ogre::any_cast<unsigned int>(i->second.val));
                    break;
                case PROP_LONG:
                    static_cast<OgitorsProperty<long>*>(j->second)->set(Ogre::any_cast<long>(i->second.val));
                    break;
                case PROP_UNSIGNED_LONG:
                    static_cast<OgitorsProperty<long>*>(j->second)->set(Ogre::any_cast<unsigned long>(i->second.val));
                    break;
                case PROP_REAL:
                    static_cast<OgitorsProperty<Ogre::Real>*>(j->second)->set(Ogre::any_cast<Ogre::Real>(i->second.val));
                    break;
                case PROP_STRING:
                    static_cast<OgitorsProperty<Ogre::String>*>(j->second)->set(Ogre::any_cast<Ogre::String>(i->second.val));
                    break;
                case PROP_VECTOR2:
                    static_cast<OgitorsProperty<Ogre::Vector2>*>(j->second)->set(Ogre::any_cast<Ogre::Vector2>(i->second.val));
                    break;
                case PROP_VECTOR3:
                    static_cast<OgitorsProperty<Ogre::Vector3>*>(j->second)->set(Ogre::any_cast<Ogre::Vector3>(i->second.val));
                    break;
                case PROP_VECTOR4:
                    static_cast<OgitorsProperty<Ogre::Vector4>*>(j->second)->set(Ogre::any_cast<Ogre::Vector4>(i->second.val));
                    break;
                case PROP_COLOUR:
                    static_cast<OgitorsProperty<Ogre::ColourValue>*>(j->second)->set(Ogre::any_cast<Ogre::ColourValue>(i->second.val));
                    break;
                case PROP_BOOL:
                    static_cast<OgitorsProperty<bool>*>(j->second)->set(Ogre::any_cast<bool>(i->second.val));
                    break;
                case PROP_QUATERNION:
                    static_cast<OgitorsProperty<Ogre::Quaternion>*>(j->second)->set(Ogre::any_cast<Ogre::Quaternion>(i->second.val));
                    break;
                case PROP_MATRIX3:
                    static_cast<OgitorsProperty<Ogre::Matrix3>*>(j->second)->set(Ogre::any_cast<Ogre::Matrix3>(i->second.val));
                    break;
                case PROP_MATRIX4:
                    static_cast<OgitorsProperty<Ogre::Matrix4>*>(j->second)->set(Ogre::any_cast<Ogre::Matrix4>(i->second.val));
                    break; 
                };
            }
        }
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::initValueMap(OgitorsPropertyValueMap& values)
    {
        for (OgitorsPropertyValueMap::iterator i = values.begin(); i != values.end(); ++i)
        {
            OgitorsPropertyMap::iterator j = mPropertyMap.find(i->first);
            if (j != mPropertyMap.end())
            {
                // Check for property type mismatch and try to correct it
                Ogitors::OgitorsSystem* mSystem = Ogitors::OgitorsSystem::getSingletonPtr();
                if(j->second->getType() != i->second.propType)
                {
                    mSystem->DisplayMessageDialog(OTR(Ogre::String("The type of the property ").append(i->first) + " seems to have changed.\n\nOgitor will attempt to automatically correct the situation, but please check the values of the property afterwards."), Ogitors::DLGTYPE_OK);
                    changePropertyType(&i->second, j->second->getType());
                }

                switch(i->second.propType)
                {
                case PROP_SHORT:
                    static_cast<OgitorsProperty<short>*>(j->second)->init(Ogre::any_cast<short>(i->second.val));
                    break;
                case PROP_UNSIGNED_SHORT:
                    static_cast<OgitorsProperty<short>*>(j->second)->init(Ogre::any_cast<unsigned short>(i->second.val));
                    break;
                case PROP_INT:
                    static_cast<OgitorsProperty<int>*>(j->second)->init(Ogre::any_cast<int>(i->second.val));
                    break;
                case PROP_UNSIGNED_INT:
                    static_cast<OgitorsProperty<int>*>(j->second)->init(Ogre::any_cast<unsigned int>(i->second.val));
                    break;
                case PROP_LONG:
                    static_cast<OgitorsProperty<long>*>(j->second)->init(Ogre::any_cast<long>(i->second.val));
                    break;
                case PROP_UNSIGNED_LONG:
                    static_cast<OgitorsProperty<long>*>(j->second)->init(Ogre::any_cast<unsigned long>(i->second.val));
                    break;
                case PROP_REAL:
                    static_cast<OgitorsProperty<Ogre::Real>*>(j->second)->init(Ogre::any_cast<Ogre::Real>(i->second.val));
                    break;
                case PROP_STRING:
                    static_cast<OgitorsProperty<Ogre::String>*>(j->second)->init(Ogre::any_cast<Ogre::String>(i->second.val));
                    break;
                case PROP_VECTOR2:
                    static_cast<OgitorsProperty<Ogre::Vector2>*>(j->second)->init(Ogre::any_cast<Ogre::Vector2>(i->second.val));
                    break;
                case PROP_VECTOR3:
                    static_cast<OgitorsProperty<Ogre::Vector3>*>(j->second)->init(Ogre::any_cast<Ogre::Vector3>(i->second.val));
                    break;
                case PROP_VECTOR4:
                    static_cast<OgitorsProperty<Ogre::Vector4>*>(j->second)->init(Ogre::any_cast<Ogre::Vector4>(i->second.val));
                    break;
                case PROP_COLOUR:
                    static_cast<OgitorsProperty<Ogre::ColourValue>*>(j->second)->init(Ogre::any_cast<Ogre::ColourValue>(i->second.val));
                    break;
                case PROP_BOOL:
                    static_cast<OgitorsProperty<bool>*>(j->second)->init(Ogre::any_cast<bool>(i->second.val));
                    break;
                case PROP_QUATERNION:
                    static_cast<OgitorsProperty<Ogre::Quaternion>*>(j->second)->init(Ogre::any_cast<Ogre::Quaternion>(i->second.val));
                    break;
                case PROP_MATRIX3:
                    static_cast<OgitorsProperty<Ogre::Matrix3>*>(j->second)->init(Ogre::any_cast<Ogre::Matrix3>(i->second.val));
                    break;
                case PROP_MATRIX4:
                    static_cast<OgitorsProperty<Ogre::Matrix4>*>(j->second)->init(Ogre::any_cast<Ogre::Matrix4>(i->second.val));
                    break;
                };
            }
        }
    }
    //---------------------------------------------------------------------
    void OgitorsPropertySet::changePropertyType(OgitorsPropertyValue* currentValue, OgitorsPropertyType targetType)
    {
        *currentValue = OgitorsPropertyValue::createFromString(targetType, currentValue->origVal);
    }
    //---------------------------------------------------------------------
    OgitorsCustomPropertySet::OgitorsCustomPropertySet()
    {
        mType = PROPSET_CUSTOM;
    }
    //---------------------------------------------------------------------
    OgitorsCustomPropertySet::~OgitorsCustomPropertySet()
    {
        OgitorsPropertyDefMap::iterator it = mDefinitions.begin();

        while(it != mDefinitions.end())
        {
            if(it->second.getAutoOptionType() == AUTO_OPTIONS_NONE)
            {
                PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(it->second.getOptions());
                OGRE_DELETE options;
            }
            it++;
        }
        mDefinitions.clear();
    }
    //---------------------------------------------------------------------
    OgitorsPropertyDef *OgitorsCustomPropertySet::addProperty(const Ogre::String& name, OgitorsPropertyType pType)
    {
        OgitorsPropertyMap::iterator it = mPropertyMap.find(name);
        if(it != mPropertyMap.end())
            return 0;

        OgitorsPropertyDefMap::iterator defi;
        defi = mDefinitions.insert(OgitorsPropertyDefMap::value_type(name, OgitorsPropertyDef(name, name, name, pType))).first;
        
        OgitorsPropertyBase *prop = 0;

        switch(pType)
        {
        case PROP_INT:
            prop = OGRE_NEW OgitorsProperty<int>(&(defi->second), 0, 0, 0);
            break;
        case PROP_REAL:
            prop = OGRE_NEW OgitorsProperty<Ogre::Real>(&(defi->second), 0, 0, 0);
            break;
        case PROP_STRING:
            prop = OGRE_NEW OgitorsProperty<Ogre::String>(&(defi->second), "", 0, 0);
            break;
        case PROP_VECTOR2:
            prop = OGRE_NEW OgitorsProperty<Ogre::Vector2>(&(defi->second), Ogre::Vector2::ZERO, 0, 0);
            break;
        case PROP_VECTOR3:
            prop = OGRE_NEW OgitorsProperty<Ogre::Vector3>(&(defi->second), Ogre::Vector3::ZERO, 0, 0);
            break;
        case PROP_VECTOR4:
            prop = OGRE_NEW OgitorsProperty<Ogre::Vector4>(&(defi->second), Ogre::Vector4::ZERO, 0, 0);
            break;
        case PROP_COLOUR:
            prop = OGRE_NEW OgitorsProperty<Ogre::ColourValue>(&(defi->second), Ogre::ColourValue(0,0,0), 0, 0);
            break;
        case PROP_BOOL:
            prop = OGRE_NEW OgitorsProperty<bool>(&(defi->second), false, 0, 0);
            break;
        case PROP_QUATERNION:
            prop = OGRE_NEW OgitorsProperty<Ogre::Quaternion>(&(defi->second), Ogre::Quaternion::IDENTITY, 0, 0);
            break;
        default: assert(prop != 0 && "Invalid Custom Property Type");return 0;
        };
        
        OgitorsPropertySet::addProperty(prop);

        return &(defi->second);
    }
    //---------------------------------------------------------------------
    OgitorsPropertyDef *OgitorsCustomPropertySet::addProperty(const Ogre::String& name, const OgitorsPropertyValue value)
    {
        OgitorsPropertyMap::iterator it = mPropertyMap.find(name);
        if(it != mPropertyMap.end())
            return 0;

        OgitorsPropertyDefMap::iterator defi;
        defi = mDefinitions.insert(OgitorsPropertyDefMap::value_type(name, OgitorsPropertyDef(name, name, name, value.propType))).first;
        
        OgitorsPropertyBase *prop = 0;

        switch(value.propType)
        {
        case PROP_INT:
            prop = OGRE_NEW OgitorsProperty<int>(&(defi->second), 0, 0, 0);
            static_cast<OgitorsProperty<int>*>(prop)->init(Ogre::any_cast<int>(value.val));
            break;
        case PROP_REAL:
            prop = OGRE_NEW OgitorsProperty<Ogre::Real>(&(defi->second), 0, 0, 0);
            static_cast<OgitorsProperty<Ogre::Real>*>(prop)->init(Ogre::any_cast<Ogre::Real>(value.val));
            break;
        case PROP_STRING:
            prop = OGRE_NEW OgitorsProperty<Ogre::String>(&(defi->second), "", 0, 0);
            static_cast<OgitorsProperty<Ogre::String>*>(prop)->init(Ogre::any_cast<Ogre::String>(value.val));
            break;
        case PROP_VECTOR2:
            prop = OGRE_NEW OgitorsProperty<Ogre::Vector2>(&(defi->second), Ogre::Vector2::ZERO, 0, 0);
            static_cast<OgitorsProperty<Ogre::Vector2>*>(prop)->init(Ogre::any_cast<Ogre::Vector2>(value.val));
            break;
        case PROP_VECTOR3:
            prop = OGRE_NEW OgitorsProperty<Ogre::Vector3>(&(defi->second), Ogre::Vector3::ZERO, 0, 0);
            static_cast<OgitorsProperty<Ogre::Vector3>*>(prop)->init(Ogre::any_cast<Ogre::Vector3>(value.val));
            break;
        case PROP_VECTOR4:
            prop = OGRE_NEW OgitorsProperty<Ogre::Vector4>(&(defi->second), Ogre::Vector4::ZERO, 0, 0);
            static_cast<OgitorsProperty<Ogre::Vector4>*>(prop)->init(Ogre::any_cast<Ogre::Vector4>(value.val));
            break;
        case PROP_COLOUR:
            prop = OGRE_NEW OgitorsProperty<Ogre::ColourValue>(&(defi->second), Ogre::ColourValue(0,0,0), 0, 0);
            static_cast<OgitorsProperty<Ogre::ColourValue>*>(prop)->init(Ogre::any_cast<Ogre::ColourValue>(value.val));
            break;
        case PROP_BOOL:
            prop = OGRE_NEW OgitorsProperty<bool>(&(defi->second), false, 0, 0);
            static_cast<OgitorsProperty<bool>*>(prop)->init(Ogre::any_cast<bool>(value.val));
            break;
        case PROP_QUATERNION:
            prop = OGRE_NEW OgitorsProperty<Ogre::Quaternion>(&(defi->second), Ogre::Quaternion::IDENTITY, 0, 0);
            static_cast<OgitorsProperty<Ogre::Quaternion>*>(prop)->init(Ogre::any_cast<Ogre::Quaternion>(value.val));
            break;
        default: assert(prop != 0 && "Invalid Custom Property Type");return 0;
        };
        
        OgitorsPropertySet::addProperty(prop);

        return &(defi->second);
    }
    //---------------------------------------------------------------------
    void OgitorsCustomPropertySet::removeProperty(const Ogre::String& propname)
    {
        OgitorsPropertyDefMap::iterator defi = mDefinitions.find(propname);
        assert(defi != mDefinitions.end());

        if(defi->second.getAutoOptionType() == AUTO_OPTIONS_NONE)
        {
            PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(defi->second.getOptions());
            OGRE_DELETE options;
        }

        OgitorsPropertySet::removeProperty(propname);

        mDefinitions.erase(defi);
    }
    //---------------------------------------------------------------------
    void OgitorsCustomPropertySet::cloneSet(OgitorsCustomPropertySet& clone)
    {
        for(unsigned int i = 0;i < mPropertyVector.size();i++)
        {
            OgitorsPropertyBase         *prop    = mPropertyVector[i];
            const OgitorsPropertyDef    *def     = prop->getDefinition();
            const PropertyOptionsVector *options = def->getOptions();

            OgitorsPropertyDef *clonedef = clone.addProperty(prop->getName(), prop->getType());
            clone.getProperty(prop->getName())->setValue(prop->getValue());
          
            if(options)
            {
                if(def->getAutoOptionType() == AUTO_OPTIONS_NONE)
                {
                    PropertyOptionsVector *cloneoptions = new PropertyOptionsVector;
                    *cloneoptions = *options;
                    clonedef->setOptions(cloneoptions);
                }
                else
                {
                    clonedef->setOptions(const_cast<PropertyOptionsVector *>(options));
                    clonedef->setAutoOptionType(def->getAutoOptionType());
                }
            }

            clonedef->setFieldNames(def->getFieldName(0), def->getFieldName(1), def->getFieldName(2), def->getFieldName(3));
        }
    }
    //---------------------------------------------------------------------
    void OgitorsCustomPropertySet::initFromSet(OgitorsCustomPropertySet& set)
    {
        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW CustomSetRebuiltUndo(this));
        
        OgitorsPropertyDefMap::iterator it = mDefinitions.begin();

        while(it != mDefinitions.end())
        {
            if(it->second.getAutoOptionType() == AUTO_OPTIONS_NONE)
            {
                PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(it->second.getOptions());
                OGRE_DELETE options;
            }
            it++;
        }

        for (OgitorsPropertyMap::iterator i = mPropertyMap.begin(); i != mPropertyMap.end(); ++i)
        {
            OGRE_DELETE i->second;
        }
        
        for (unsigned int c = 0; c < mPropertyConnections.size(); c++)
        {
            OGRE_DELETE mPropertyConnections[c];
        }

        mDefinitions.clear();
        mPropertyMap.clear();
        mPropertyConnections.clear();
        mPropertyVector.clear();

        for(unsigned int i = 0;i < set.mPropertyVector.size();i++)
        {
            OgitorsPropertyBase         *prop    = set.mPropertyVector[i];
            const OgitorsPropertyDef    *def     = prop->getDefinition();
            const PropertyOptionsVector *options = def->getOptions();

            OgitorsPropertyDef *initdef = addProperty(prop->getName(), prop->getType());
            getProperty(prop->getName())->setValue(prop->getValue());
            
            *initdef = *def;
          
            if(options)
            {
                if(def->getAutoOptionType() == AUTO_OPTIONS_NONE)
                {
                    PropertyOptionsVector *initoptions = new PropertyOptionsVector;
                    *initoptions = *options;
                    initdef->setOptions(initoptions);
                }
                else
                {
                    initdef->setOptions(const_cast<PropertyOptionsVector*>(options));
                    initdef->setAutoOptionType(def->getAutoOptionType());
                }
            }
        }

        for(unsigned int i = 0;i < mListeners.size();i++)
            mListeners[i]->OnPropertySetRebuilt(this);
    }
    //---------------------------------------------------------------------
    void OgitorsCustomPropertySet::addFromSet(OgitorsCustomPropertySet& set)
    {
        OgitorsUndoManager::getSingletonPtr()->AddUndo(OGRE_NEW CustomSetRebuiltUndo(this));
        
        for(unsigned int i = 0;i < set.mPropertyVector.size();i++)
        {
            OgitorsPropertyBase         *prop    = set.mPropertyVector[i];
            const OgitorsPropertyDef    *def     = prop->getDefinition();
            const PropertyOptionsVector *options = def->getOptions();

            // If the a property with the same name exists, just ignore it
            if(mPropertyMap.find(prop->getName()) != mPropertyMap.end())
                continue;

            OgitorsPropertyDef *initdef = addProperty(prop->getName(), prop->getType());
            getProperty(prop->getName())->setValue(prop->getValue());
            *initdef = *def;
          
            if(options)
            {
                if(def->getAutoOptionType() == AUTO_OPTIONS_NONE)
                {
                    PropertyOptionsVector *initoptions = new PropertyOptionsVector;
                    *initoptions = *options;
                    initdef->setOptions(initoptions);
                }
                else
                {
                    initdef->setOptions(const_cast<PropertyOptionsVector*>(options));
                    initdef->setAutoOptionType(def->getAutoOptionType());
                }
            }
        }

        for(unsigned int i = 0;i < mListeners.size();i++)
            mListeners[i]->OnPropertySetRebuilt(this);
    }
    //---------------------------------------------------------------------
    void OgitorsCustomPropertySet::changePropertyName(Ogre::String propname, const Ogre::String& newname)
    {
        OgitorsPropertyMap::iterator i = mPropertyMap.find(propname);
        assert(i != mPropertyMap.end());

        OgitorsPropertyBase *prop = i->second;
        OgitorsPropertyType pType = i->second->getType();
        mPropertyMap.erase(i);
        mPropertyMap.insert(OgitorsPropertyMap::value_type(newname, prop));

        OgitorsPropertyDefMap::iterator defi = mDefinitions.find(propname);
        assert(defi != mDefinitions.end());
        OgitorsPropertyDefMap::iterator defi2 = mDefinitions.insert(OgitorsPropertyDefMap::value_type(newname, OgitorsPropertyDef(newname, newname, newname, pType))).first;

        PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(defi->second.getOptions());
        defi2->second.setOptions(options);
        defi2->second.setFieldNames(defi->second.getFieldName(0), defi->second.getFieldName(1), defi->second.getFieldName(2), defi->second.getFieldName(3));
        defi2->second.setAutoOptionType(defi->second.getAutoOptionType());

        mDefinitions.erase(defi);
        prop->setDefinition(&(defi2->second));
    }
    //---------------------------------------------------------------------
    void OgitorsCustomPropertySet::changePropertyType(Ogre::String propname, OgitorsPropertyType pType)
    {
        OgitorsPropertyMap::iterator it = mPropertyMap.find(propname);
        assert(it != mPropertyMap.end());

        OgitorsPropertyBase *oldprop = it->second;
        OGRE_DELETE it->second;
        mPropertyMap.erase(it);

        OgitorsPropertyDefMap::iterator defi = mDefinitions.find(propname);
        assert(defi != mDefinitions.end());
        
        if(defi->second.getAutoOptionType() == AUTO_OPTIONS_NONE)
        {
            PropertyOptionsVector *options = const_cast<PropertyOptionsVector*>(defi->second.getOptions());
            OGRE_DELETE options;
        }
        mDefinitions.erase(defi);

        int position = -1;
        for(unsigned int i = 0;i < mPropertyVector.size();i++)
            if(mPropertyVector[i] == oldprop)
            {
                position = i;
                break;
            }
        
        assert(position != -1);
        
        defi = mDefinitions.insert(OgitorsPropertyDefMap::value_type(propname, OgitorsPropertyDef(propname, propname, propname, pType))).first;

        oldprop = 0;

        switch(pType)
        {
        case PROP_INT:
            oldprop = OGRE_NEW OgitorsProperty<int>(&(defi->second), 0, 0, 0);
            break;
        case PROP_REAL:
            oldprop = OGRE_NEW OgitorsProperty<Ogre::Real>(&(defi->second), 0, 0, 0);
            break;
        case PROP_STRING:
            oldprop = OGRE_NEW OgitorsProperty<Ogre::String>(&(defi->second), "", 0, 0);
            break;
        case PROP_VECTOR2:
            oldprop = OGRE_NEW OgitorsProperty<Ogre::Vector2>(&(defi->second), Ogre::Vector2::ZERO, 0, 0);
            break;
        case PROP_VECTOR3:
            oldprop = OGRE_NEW OgitorsProperty<Ogre::Vector3>(&(defi->second), Ogre::Vector3::ZERO, 0, 0);
            break;
        case PROP_VECTOR4:
            oldprop = OGRE_NEW OgitorsProperty<Ogre::Vector4>(&(defi->second), Ogre::Vector4::ZERO, 0, 0);
            break;
        case PROP_COLOUR:
            oldprop = OGRE_NEW OgitorsProperty<Ogre::ColourValue>(&(defi->second), Ogre::ColourValue(0,0,0), 0, 0);
            break;
        case PROP_BOOL:
            oldprop = OGRE_NEW OgitorsProperty<bool>(&(defi->second), false, 0, 0);
            break;
        case PROP_QUATERNION:
            oldprop = OGRE_NEW OgitorsProperty<Ogre::Quaternion>(&(defi->second), Ogre::Quaternion::IDENTITY, 0, 0);
            break;
        default: assert(oldprop != 0 && "Invalid Custom Property Type");
        };

        mPropertyMap.insert(OgitorsPropertyMap::value_type(oldprop->getName(), oldprop));
        mPropertyVector[position] = oldprop;
    }
    //---------------------------------------------------------------------
}

