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
//This File is modified version of the Original Implementation by OGITOR TEAM
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "OgitorsDelegates.h"

namespace Ogitors
{
    class OgitorsPropertySet;

    /** Structure defining one option of a property. */
    struct PropertyOption
    {
        Ogre::String mKey;
        Ogre::Any    mValue;

        PropertyOption(const Ogre::String& key, const Ogre::Any& value) : mKey(key), mValue(value) {}

        static bool comp_func(PropertyOption o1, PropertyOption o2)
        {
            return strcmp(o1.mKey.c_str(), o2.mKey.c_str()) < 0;
        }
    };
    
    typedef Ogre::vector<PropertyOption>::type PropertyOptionsVector;

    /// The type of a property
    enum OgitorsPropertyType
    {
        PROP_SHORT = 0,
        PROP_UNSIGNED_SHORT = 1,
        PROP_INT = 2,
        PROP_UNSIGNED_INT = 3,
        PROP_LONG = 4, 
        PROP_UNSIGNED_LONG = 5,
        PROP_REAL = 6,
        PROP_STRING = 7,
        PROP_VECTOR2 = 8, 
        PROP_VECTOR3 = 9,
        PROP_VECTOR4 = 10, 
        PROP_COLOUR = 11,
        PROP_BOOL = 12,
        PROP_QUATERNION = 13, 
        PROP_MATRIX3 = 14,
        PROP_MATRIX4 = 15, 

        PROP_UNKNOWN = 999
    };

    /// The type of auto options for properties
    enum AutoOptionType
    {
        AUTO_OPTIONS_NONE = 0,
        AUTO_OPTIONS_MATERIAL = 1,
        AUTO_OPTIONS_MESH = 2,
        AUTO_OPTIONS_TARGET = 3
    };

    /** Definition of a property of an object.
    @remarks
    This definition is shared between all instances of an object and therefore
    has no value. Property contains values.
    */
    class OgitorExport OgitorsPropertyDef : public Ogre::GeneralAllocatedObject
    {
    public:

        /* Construct a property.
        @param name The name of the property
        @param desc A (potentially) long description of the property
        @param pType The type of the property
        */
        OgitorsPropertyDef(const Ogre::String& name, const Ogre::String& displayname, const Ogre::String& desc, OgitorsPropertyType pType, bool read = true, bool write = true, bool trackchanges = true)
            : mName(name), mDisplayName(displayname), mDesc(desc), mType(pType), mCanRead(read), mCanWrite(write), mTrackChanges(trackchanges) 
        {
            mFieldNames[0] = "X";
            mFieldNames[1] = "Y";
            mFieldNames[2] = "Z";
            mFieldNames[3] = "W";

            mMinValue = new Ogre::Any();
            mMaxValue = new Ogre::Any();
            mStepSize = new Ogre::Any();

            mOptions = 0;
            mAutoOptionType = AUTO_OPTIONS_NONE;
        }

        /// Get the name of the property
        const Ogre::String& getName() const { return mName; }

        /// Get the display name of the property
        const Ogre::String& getDisplayName() const { return mDisplayName; }

        /// Get the description of the property
        const Ogre::String& getDescription() const { return mDesc; }

        /// Get the type of the property
        inline OgitorsPropertyType getType() const { return mType; }

        /// Return true if property can be displayed
        inline bool canRead() const { return mCanRead; }

        /// Return true if property can be modified through properties view
        inline bool canWrite() const { return mCanWrite; }

        /// Set access rights
        void setAccess(bool read, bool write) { mCanRead = read; mCanWrite = write; }
        
        /// Return true if property should be tracked by the set for changes
        inline bool getTrackChanges() const { return mTrackChanges; }

        /// Set track changes
        void setTrackChanges(bool track) { mTrackChanges = track; }

        /// Return options of the property
        const PropertyOptionsVector *getOptions() const { return mOptions; }

        /// Set options of the property
        void setOptions(PropertyOptionsVector *opt) { mOptions = opt; }

        /// Return FieldName of the property
        const Ogre::String& getFieldName(int index) const 
        { 
            assert((index > -1) && (index < 4));
            return mFieldNames[index]; 
        }

        /// Set FieldNames of the property
        void  setFieldNames(const Ogre::String& x = "X", const Ogre::String& y = "Y", const Ogre::String& z = "Z", const Ogre::String& w = "W") 
        { 
            mFieldNames[0] = x; 
            mFieldNames[1] = y; 
            mFieldNames[2] = z; 
            mFieldNames[3] = w; 
        }

        /// Set FieldName of the property at the index specified by index parameter
        void  setFieldName(int index, const Ogre::String& name)
        { 
            assert((index > -1) && (index < 4));
            mFieldNames[index] = name; 
        }

        /// Return MinValue of the property
        const Ogre::Any& getMinValue() const { return *mMinValue; }
        
        /// Set MinValue of the property
        void  setMinValue(const Ogre::Any& value) { *mMinValue = value; }

        /// Return MaxValue of the property
        const Ogre::Any& getMaxValue() const { return *mMaxValue; }

        /// Set MaxValue of the property
        void  setMaxValue(const Ogre::Any& value) { *mMaxValue = value; }

        /// Set MinValue and MaxValue of the property, plus optional stepSize
        void  setRange(const Ogre::Any& min, const Ogre::Any& max, const Ogre::Any& stepSize = Ogre::Any())
        {
            setMinValue(min);
            setMaxValue(max);

            if(!stepSize.isEmpty())
                setStepSize(stepSize);
        }

        /// Return StepSize of the property
        const Ogre::Any& getStepSize() const { return *mStepSize; }

        /// Set StepSize of the property
        void  setStepSize(const Ogre::Any& value) { *mStepSize = value; }

        inline AutoOptionType getAutoOptionType() const { return mAutoOptionType; }
        void setAutoOptionType(const AutoOptionType newtype) { mAutoOptionType = newtype; }

        /// Get a string name of a property type
        static const Ogre::String& getTypeName(OgitorsPropertyType theType);

        static OgitorsPropertyType getTypeForValue(const short& val)                { return PROP_SHORT; }
        static OgitorsPropertyType getTypeForValue(const unsigned short& val)       { return PROP_UNSIGNED_SHORT; }
        static OgitorsPropertyType getTypeForValue(const int& val)                  { return PROP_INT; }
        static OgitorsPropertyType getTypeForValue(const unsigned int& val)         { return PROP_UNSIGNED_INT; }
        static OgitorsPropertyType getTypeForValue(const long& val)                 { return PROP_LONG; }
        static OgitorsPropertyType getTypeForValue(const unsigned long& val)        { return PROP_UNSIGNED_LONG; }
        static OgitorsPropertyType getTypeForValue(const Ogre::Real& val)           { return PROP_REAL; }
        static OgitorsPropertyType getTypeForValue(const Ogre::String& val)         { return PROP_STRING; }
        static OgitorsPropertyType getTypeForValue(const Ogre::Vector2& val)        { return PROP_VECTOR2; }
        static OgitorsPropertyType getTypeForValue(const Ogre::Vector3& val)        { return PROP_VECTOR3; }
        static OgitorsPropertyType getTypeForValue(const Ogre::Vector4& val)        { return PROP_VECTOR4; }
        static OgitorsPropertyType getTypeForValue(const Ogre::ColourValue& val)    { return PROP_COLOUR; }
        static OgitorsPropertyType getTypeForValue(const bool& val)                 { return PROP_BOOL; }
        static OgitorsPropertyType getTypeForValue(const Ogre::Quaternion& val)     { return PROP_QUATERNION; }
        static OgitorsPropertyType getTypeForValue(const Ogre::Matrix3& val)        { return PROP_MATRIX3; }
        static OgitorsPropertyType getTypeForValue(const Ogre::Matrix4& val)        { return PROP_MATRIX4; }

    protected:
        // no default construction
        OgitorsPropertyDef() {}

        Ogre::String            mName;
        Ogre::String            mDisplayName;
        Ogre::String            mDesc;
        OgitorsPropertyType     mType;
        bool                    mCanRead;
        bool                    mCanWrite;
        bool                    mTrackChanges;
        PropertyOptionsVector*  mOptions;
        Ogre::String            mFieldNames[4];
        Ogre::Any*              mMinValue;
        Ogre::Any*              mMaxValue;
        Ogre::Any*              mStepSize;
        AutoOptionType          mAutoOptionType;
    };

    /// Map from property name to shared definition
    typedef Ogre::map<Ogre::String, OgitorsPropertyDef>::type OgitorsPropertyDefMap;

    /** Base interface for an instance of a property.
    */
    class OgitorExport OgitorsPropertyBase : public Ogre::GeneralAllocatedObject
    {
    public:
        /// Constructor
        OgitorsPropertyBase(OgitorsPropertyDef* def, unsigned int tag) : mDef(def), mTag(tag), mRefCount(1) {}
        virtual ~OgitorsPropertyBase() {}

        /// Get the name of the property
        const Ogre::String& getName() const { return mDef->getName(); }

        unsigned int getTag() const { return mTag; } 

        /// Get the description of the property
        const OgitorsPropertyDef* getDefinition() const { return mDef; }

        /// Get the type of the property
        OgitorsPropertyType getType() const { return mDef->getType(); }

        /// Return the current value as an Any
        virtual Ogre::Any getValue() const = 0;

        /// Set the current value as an Any
        virtual void setValue(const Ogre::Any& value) = 0;

        /// Return the old value as an Any
        virtual Ogre::Any getOldValue() const = 0;

        /// Return the index old value as an Any
        virtual Ogre::String getOptionName() = 0;
        /// Set the value using option string as index
        virtual void setByOptionValue(const Ogre::String& value) = 0;
        /// Set a new definition
        void setDefinition(OgitorsPropertyDef *def) { mDef = def; }

        inline void connect(OgitorsSignalFunction function, OgitorsConnection& connection)
        {
            connection._setData(mSignal.connect(function));
        }

        inline void connect(OgitorsSignalFunction function, OgitorsScopedConnection& connection)
        {
            connection._setData(mSignal.connect(function));
        }

        virtual void connectTo(OgitorsPropertyBase* property) = 0;

        void _addRef() { ++mRefCount; }

        void _release() { --mRefCount; }

    protected:
        // disallow default construction
        OgitorsPropertyBase() {}
        OgitorsPropertyDef*         mDef;
        unsigned int                mTag;
        OgitorsSignal               mSignal;
        OgitorsScopedConnection     mConnection;
        int                         mRefCount;
    };

    /** Property instance with pass through calls to a given object. */
    template <typename T>
    class OgitorsProperty : public OgitorsPropertyBase
    {
    public:
        typedef PropertySetterFunction<T> setter_func;
        
        /** Construct a property which is able to directly call a given 
        getter and setter on a specific object instance, via functors.
        */
        OgitorsProperty(OgitorsPropertyDef* def, T value, unsigned int tag = 0, setter_func *setter = 0)
            : OgitorsPropertyBase(def, tag)
        {
            mValue = value;
            mOldValue = value;
            mSetter = 0;
            if(setter)
            {
                mSetter = OGRE_NEW setter_func;
                *mSetter = *setter; 
            }
        }

        /** Construct a property which is able to directly call a given 
        getter and setter on a specific object instance, via functors.
        */
        OgitorsProperty(OgitorsPropertyDef* def, T value, unsigned int tag, setter_func setter)
            : OgitorsPropertyBase(def, tag)
        {
            mValue = value;
            mOldValue = value;
            mSetter = OGRE_NEW setter_func;
            *mSetter = setter; 
        }

        inline void setSetterFunction(setter_func* func)
        {
            OGRE_DELETE mSetter;
            if(func)
            {
                mSetter = OGRE_NEW setter_func;
                *mSetter = *func;
            }
            else
                mSetter = 0;
        }

        /** Set the property value.
        */
        virtual void set(T val)
        {
            if(val == mValue)
                return;

            mOldValue = mValue;
            mValue = val;

            if(mSetter)
            {
                if(!((*mSetter)(this, val)))
                {
                    mValue = mOldValue;
                    return;
                }
            }
            mConnection.disconnect();
            Ogre::Any anyValue(mValue);
            mSignal.invoke(this, anyValue);
        }

        /** Set the initial property value.
        */
        virtual void init(T val)
        {
            mValue = val;
        }

        /** Set the initial property value and signal the value change.
        */
        virtual void initAndSignal(T val)
        {
            mOldValue = mValue;
            mValue = val;

            Ogre::Any anyValue(mValue);
            mSignal.invoke(this, anyValue);
        }

        /** Set the property value from options.
        */
        virtual void setByOptionValue(const Ogre::String& value)
        {
            const PropertyOptionsVector *options = mDef->getOptions();
            assert(options != 0);
            
            unsigned int i = 0;

            for(i = 0;i < options->size();i++)
            {
                if((*options)[i].mKey == value)
                {
                    set(Ogre::any_cast<T>((*options)[i].mValue));
                    return;
                }
            }

            assert(i < 0 && "Could not find the option in the options vector");
        }

        virtual Ogre::String getOptionName()
        {
            const PropertyOptionsVector *options = mDef->getOptions();
            assert(options != 0);

            unsigned int i = 0;

            for(i = 0;i < options->size();i++)
            {
                if(Ogre::any_cast<T>((*options)[i].mValue) == mValue)
                {
                    return (*options)[i].mKey;
                }
            }

            return "";
        }

        virtual T get() const
        {
            return mValue;
        }

        virtual T getOld() const
        {
            return mOldValue;
        }

        Ogre::Any getValue() const
        {
            return Ogre::Any(get());
        }

        Ogre::Any getOldValue() const
        {
            return Ogre::Any(mOldValue);
        }

        void setValue(const Ogre::Any& value)
        {
            T val = Ogre::any_cast<T>(value);

            if(val == mValue)
                return;

            mOldValue = mValue;
            mValue = val;

            if(mSetter)
            {
                if(!((*mSetter)(this, val)))
                {
                    mValue = mOldValue;
                    return;
                }
            }
            mConnection.disconnect();
            Ogre::Any anyValue(mValue);
            mSignal.invoke(this, anyValue);
        }

        void connectTo(OgitorsPropertyBase* property)
        {
            assert(getType() == property->getType());
            mConnection.disconnect();
            if(property)
            {
                property->connect(OgitorsSignalFunction::from_method<OgitorsProperty<T>, &OgitorsProperty<T>::slotTargetChange>(this), mConnection);
                initAndSignal(static_cast<OgitorsProperty<T>*>(property)->get());
            }
        }

        void slotTargetChange(const OgitorsPropertyBase* property, const Ogre::Any val)
        {
            T newVal = Ogre::any_cast<T>(val);
            
            mOldValue = mValue;
            mValue = newVal;
            
            if(mSetter)
            {
                if(!((*mSetter)(this, newVal)))
                {
                    mValue = mOldValue;
                    return;
                }
            }

            mSignal.invoke(this, val);
        }

    protected:
        T            mValue;
        T            mOldValue;
        setter_func *mSetter;

        // disallow default construction
        OgitorsProperty() {}
        ~OgitorsProperty() 
        {
            OGRE_DELETE mSetter;
        }
    };

    class CBaseEditor;

    class OgitorsParentProperty : public OgitorsPropertyBase
    {
    public:
        typedef PropertySetterFunction<unsigned long> setter_func;
        
        /** Construct a property which is able to directly call a given 
        getter and setter on a specific object instance, via functors.
        */
        OgitorsParentProperty(OgitorsPropertyDef* def, unsigned long value, setter_func *setter)
            : OgitorsPropertyBase(def, 0)
        {
            mValue = value;
            mOldValue = value;
            mSetter = OGRE_NEW setter_func;
            *mSetter = *setter; 
        }

        /** Construct a property which is able to directly call a given 
        getter and setter on a specific object instance, via functors.
        */
        OgitorsParentProperty(OgitorsPropertyDef* def, unsigned long value, setter_func setter)
            : OgitorsPropertyBase(def, 0)
        {
            mValue = value;
            mOldValue = value;
            mSetter = OGRE_NEW setter_func;
            *mSetter = setter; 
        }

        /** Set the property value.
        */
        virtual void set(unsigned long val)
        {
            if(val == mValue)
                return;

            mOldValue = mValue;
            mValue = val;

            (*mSetter)(this, val);

            Ogre::Any anyValue(mValue);
            mSignal.invoke(this, anyValue);
        }

        /** Set the initial property value.
        */
        virtual void init(CBaseEditor* val)
        {
            mValue = (unsigned long)val;
        }

        /** Set the initial property value and signal the value change.
        */
        virtual void initAndSignal(CBaseEditor* val)
        {
            mOldValue = mValue;
            mValue = (unsigned long)val;

            Ogre::Any anyValue(mValue);
            mSignal.invoke(this, anyValue);
        }

        /** Set the property value from options.
        */
        virtual void setByOptionValue(const Ogre::String& value)
        {
        }

        virtual Ogre::String getOptionName()
        {
            return "";
        }

        virtual CBaseEditor* get() const
        {
            unsigned long val = mValue;
            return (CBaseEditor*)val;
        }

        virtual CBaseEditor* getOld() const
        {
            unsigned long val = mOldValue;
            return (CBaseEditor*)val;
        }

        Ogre::Any getValue() const
        {
            return Ogre::Any(mValue);
        }

        Ogre::Any getOldValue() const
        {
            return Ogre::Any(mOldValue);
        }

        void setValue(const Ogre::Any& value)
        {
            unsigned long val = Ogre::any_cast<unsigned long>(value);

            if(val == mValue)
                return;

            mOldValue = mValue;
            mValue = val;

            (*mSetter)(this, val);

            Ogre::Any anyValue(mValue);
            mSignal.invoke(this, anyValue);
        }

        void connectTo(OgitorsPropertyBase* property)
        {
        }

    protected:
        unsigned long mValue;
        unsigned long mOldValue;
        setter_func  *mSetter;

        // disallow default construction
        OgitorsParentProperty() {}
        ~OgitorsParentProperty() 
        {
            OGRE_DELETE mSetter;
        }
    };

    /** A simple structure designed just as a holder of property values between
    the instances of objects they might target. There is just enough information
    here to be able to interpret the results accurately and store the original
    for potential later use, but no more.
    */
    struct OgitorsPropertyValue : public Ogre::GeneralAllocatedObject
    {
        OgitorsPropertyType propType;
        Ogre::Any val;
        Ogre::String origVal;

        OgitorsPropertyValue() : propType(PROP_UNKNOWN)
        {
        }
        
        OgitorsPropertyValue(const OgitorsPropertyType type, const Ogre::Any& value) : propType(type), val(value)
        {
        }

        static OgitorsPropertyValue createFromString(OgitorsPropertyType type, Ogre::String value)
        {
            OgitorsPropertyValue propVal;
            propVal.propType = type;
            propVal.origVal = value;

            switch(type)
            {
            case PROP_SHORT:
                propVal.val =  Ogre::Any((short)Ogre::StringConverter::parseInt(value));break;
            case PROP_UNSIGNED_SHORT:
                propVal.val =  Ogre::Any((unsigned short)Ogre::StringConverter::parseUnsignedInt(value));break;
            case PROP_INT:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseInt(value));break;
            case PROP_UNSIGNED_INT:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseUnsignedInt(value));break;
            case PROP_LONG:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseLong(value));break;
            case PROP_UNSIGNED_LONG:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseUnsignedLong(value));break;
            case PROP_REAL:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseReal(value));break;
            case PROP_STRING:
                propVal.val =  Ogre::Any(value);break;
            case PROP_VECTOR2:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseVector2(value));break;
            case PROP_VECTOR3:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseVector3(value));break;
            case PROP_VECTOR4:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseVector4(value));break;
            case PROP_COLOUR:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseColourValue(value));break;
            case PROP_BOOL:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseBool(value));break;
            case PROP_QUATERNION:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseQuaternion(value));break;
            case PROP_MATRIX3:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseMatrix3(value));break;
            case PROP_MATRIX4:
                propVal.val =  Ogre::Any(Ogre::StringConverter::parseMatrix4(value));break;
            };

            return propVal;
        }
    };

    const OgitorsPropertyValue EMPTY_PROPERTY_VALUE(PROP_STRING, Ogre::Any(Ogre::String("")));

    /// Defines a transferable map of properties using wrapped value types (Ogre::Any)
    typedef Ogre::map<Ogre::String, OgitorsPropertyValue>::type OgitorsPropertyValueMap;

    typedef Ogre::vector<OgitorsPropertyBase*>::type OgitorsPropertyVector;

    enum PropertySetOwnerTypes
    {
        PROPSETOWNER_ROOT = 0,
        PROPSETOWNER_EDITOR = 1,
        PROPSETOWNER_CUSTOM = 2
    };

    enum PropertySetType
    {
       PROPSET_OBJECT = 0,
       PROPSET_CUSTOM = 1,
       PROPSET_PHYSICS = 2
    };

    struct PropertySetOwnerData
    {
        unsigned int mOwnerType;
        void *       mOwnerPtr;
    };

    class OgitorsPropertySet;

    class OgitorExport OgitorsPropertySetListener : public Ogre::GeneralAllocatedObject
    {
    public:
        OgitorsPropertySetListener() {};
        virtual ~OgitorsPropertySetListener() {};
        
        virtual void OnPropertyRemoved(OgitorsPropertySet* set, OgitorsPropertyBase* property) = 0;
        virtual void OnPropertyAdded(OgitorsPropertySet* set, OgitorsPropertyBase* property) = 0;
        virtual void OnPropertyChanged(OgitorsPropertySet* set, OgitorsPropertyBase* property) = 0;
        virtual void OnPropertySetRebuilt(OgitorsPropertySet* set) = 0;
    };

    /** Defines a complete set of properties for a single object instance.
    */
    class OgitorExport OgitorsPropertySet : public Ogre::GeneralAllocatedObject
    {
    public:
        OgitorsPropertySet();
        virtual ~OgitorsPropertySet();

        /** Returns the type of this set. 
        */
        inline PropertySetType getType() { return mType; }

        /** Returns TRUE if this set is currently Empty. 
        */
        inline bool isEmpty() { return mPropertyVector.size() == 0; }

        /** Adds a property to this set. 
        @remarks
        The PropertySet is responsible for deleting this object.
        */
        void addProperty(OgitorsPropertyBase* prop);

        /** Removes a property from this set. 
        @remarks
        The PropertySet deletes the property upon removal.
        */
        void removeProperty(OgitorsPropertyBase* prop);

        /** Removes a property from this set. 
        @remarks
        The PropertySet deletes the property upon removal.
        */
        virtual void removeProperty(const Ogre::String& propname);

        /** Gets the property object for a given property name. 
        @remarks
        Note that this property will need to be cast to a templated property
        compatible with the type you will be setting. You might find the 
        overloaded set and get<type> methods quicker if 
        you already know the type.
        */
        OgitorsPropertyBase* getProperty(const Ogre::String& name) const;

        /** Reports whether this property set contains a named property. */
        bool hasProperty(const Ogre::String& name) const;

        typedef Ogre::map<Ogre::String, OgitorsPropertyBase*>::type OgitorsPropertyMap;
        typedef Ogre::MapIterator<OgitorsPropertyMap> OgitorsPropertyIterator;

        /// Get an iterator over the available properties
        OgitorsPropertyIterator getPropertyIterator();

        /** Gets an independently usable collection of property values from the
        current state.
        */
        OgitorsPropertyValueMap getValueMap() const;

        /** Returns an ordered vector of properties in the set.
        */
        OgitorsPropertyVector getPropertyVector() const { return mPropertyVector; }

        /** Sets ordered vector of properties in the set.
        */
        void _setPropertyVector(OgitorsPropertyVector& vec) 
        { 
            assert(vec.size() == mPropertyVector.size());
            mPropertyVector = vec; 
        }

        /** Sets the current state from a given value map.
        */
        void setValueMap(const OgitorsPropertyValueMap& values);

        /** Init the current state from a given value map.
        */
        void initValueMap(OgitorsPropertyValueMap& values);

        /** Get a named property value. 
        */
        template<typename T>
        void getValue(const Ogre::String& name, T& value) const
        {
            getPropertyImpl(name, value, OgitorsPropertyDef::getTypeForValue(value));
        }

        /** Set a named property value (via pointer to avoid copy). 
        */
        template<typename T>
        void setValue(const Ogre::String& name, const T* value)
        {
            setPropertyImpl(name, *value, OgitorsPropertyDef::getTypeForValue(*value));
        }

        /** Set a named property value. 
        */
        template<typename T>
        void setValue(const Ogre::String& name, T value)
        {
            setPropertyImpl(name, value, OgitorsPropertyDef::getTypeForValue(value));
        }

        /** Special-case char*, convert to String automatically. 
        */
        void setValue(const Ogre::String& name, const char* pChar)
        {
            Ogre::String v(pChar);
            setPropertyImpl(name, v, PROP_STRING);
        }

        /** Retrieve the owner type and owner pointer of this set. 
        */
        const PropertySetOwnerData getOwnerData() const { return mOwnerData; }

        /** Define the owner type and owner pointer of this set. 
        */
        void setOwnerData(PropertySetOwnerData& data) { mOwnerData = data; }

        /** Register a listener for this set, 
        *   so that the listener will be notified of all changes done to the set. 
        */
        void addListener(OgitorsPropertySetListener* listener);

        /** Remove a listener from the listeners list. 
        */
        void removeListener(OgitorsPropertySetListener* listener);

        /** Convert an property value to another type, e.g. needed if between
        *   Ogitor runs, the property types of an editor have been changed
        */
        void changePropertyType(OgitorsPropertyValue* currentValue, OgitorsPropertyType targetType);

        void _addRef() { ++mRefCount; }

        void _release() { --mRefCount; }

    protected:
        PropertySetType        mType;
        OgitorsPropertyMap     mPropertyMap;
        OgitorsPropertyVector  mPropertyVector;
        PropertySetOwnerData   mOwnerData;
        ScopedConnectionVector mPropertyConnections;
        int                    mRefCount;
        
        Ogre::vector<OgitorsPropertySetListener*>::type mListeners;

        /// Set a named property value, internal implementation (type match required)
        template <typename T>
        void setPropertyImpl(const Ogre::String& name, const T& val, OgitorsPropertyType typeCheck)
        {
            OgitorsPropertyBase* baseProp = getProperty(name);
            assert(baseProp->getType() == typeCheck);
            static_cast<OgitorsProperty<T>*>(baseProp)->set(val);
        }

        /// Set a named property value, internal implementation (type match required)
        template <typename T>
        void getPropertyImpl(const Ogre::String& name, T& refVal, OgitorsPropertyType typeCheck) const
        {
            OgitorsPropertyBase* baseProp = getProperty(name);
            assert(baseProp->getType() == typeCheck);
            refVal = static_cast<OgitorsProperty<T>*>(baseProp)->get();
        }

        /* Callback to track property value changes in this set */
          void propertyChangeTracker(const Ogitors::OgitorsPropertyBase* property, Ogre::Any value); 
    };

    class OgitorExport OgitorsCustomPropertySet : public OgitorsPropertySet
    {
    public:
        OgitorsCustomPropertySet();
        ~OgitorsCustomPropertySet();

        /** Adds a property to this set. 
        @remarks
        The PropertySet is responsible for deleting this object.
        */
        OgitorsPropertyDef *addProperty(const Ogre::String& name, OgitorsPropertyType pType);
        /** Adds a property to this set. 
        @remarks
        The PropertySet is responsible for deleting this object.
        */
        OgitorsPropertyDef *addProperty(const Ogre::String& name, const OgitorsPropertyValue value);
        /** Removes a property from this set. 
        @remarks
        The PropertySet deletes the property upon removal.
        */
        virtual void removeProperty(const Ogre::String& propname);
        /** Renames a property in this set. 
        */
        virtual void changePropertyName(Ogre::String propname, const Ogre::String& newname);
        /** Changes the property type of a property in this set. 
        */
        virtual void changePropertyType(Ogre::String propname, OgitorsPropertyType pType);

        /** Creates a clone of this set */
        void cloneSet(OgitorsCustomPropertySet& clone);

        /** Initializes this set by copying another set */
        void initFromSet(OgitorsCustomPropertySet& set);

        /** Copies members of a set to this set */
        void addFromSet(OgitorsCustomPropertySet& set);

    protected:
        OgitorsPropertyDefMap     mDefinitions;
    };
}
