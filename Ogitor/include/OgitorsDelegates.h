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

#pragma once

namespace Ogitors
{

    class OgitorsPropertyBase;

        //! Property assignation class
        /*!  
            Property assignation template class - internal use only!
        */
    template <typename T>
    class PropertySetterFunction: public Ogre::GeneralAllocatedObject
    {
    public:
        PropertySetterFunction()
            : object_ptr(0)
            , stub_ptr(0)
        {}

        template <class C, bool (C::*TMethod)(OgitorsPropertyBase*, const T&)>
        static PropertySetterFunction from_method(C* object_ptr)
        {
            PropertySetterFunction d;
            d.object_ptr = object_ptr;
            d.stub_ptr = &method_stub<C, TMethod>; // #1

            return d;
        }

        bool operator()(OgitorsPropertyBase* property, const T& value) const
        {
            return (*stub_ptr)(object_ptr, property, value);
        }

    private:
        typedef bool (*stub_type)(void* object_ptr, OgitorsPropertyBase*, const T&);

        void* object_ptr;
        stub_type stub_ptr;

        template <class C, bool (C::*TMethod)(OgitorsPropertyBase*, const T&)>
        static bool method_stub(void* object_ptr, OgitorsPropertyBase* property, const T& value)
        {
            C* p = static_cast<C*>(object_ptr);
            return (p->*TMethod)(property, value); // #2
        }
    };

    template <typename T>
    class PropertyGetterFunction: public Ogre::GeneralAllocatedObject
    {
    public:
        PropertyGetterFunction()
            : object_ptr(0)
            , stub_ptr(0)
        {}

        template <class C, T (C::*TMethod)(void)>
        static PropertyGetterFunction from_method(C* object_ptr)
        {
            PropertyGetterFunction d;
            d.object_ptr = object_ptr;
            d.stub_ptr = &method_stub<C, TMethod>; // #1

            return d;
        }

        T operator()(void) const
        {
            return (*stub_ptr)(object_ptr);
        }

    private:
        typedef T (*stub_type)(void* object_ptr);

        void* object_ptr;
        stub_type stub_ptr;

        template <class C, T (C::*TMethod)(void)>
        static T method_stub(void* object_ptr)
        {
            C* p = static_cast<C*>(object_ptr);
            return (p->*TMethod)(); // #2
        }
    };

    class OgitorsSignalFunction: public Ogre::GeneralAllocatedObject
    {
    public:
        OgitorsSignalFunction()
            : object_ptr(0)
            , stub_ptr(0)
        {}

        template <class C, void (C::*TMethod)(const OgitorsPropertyBase*, Ogre::Any)>
        static OgitorsSignalFunction from_method(C* object_ptr)
        {
            OgitorsSignalFunction d;
            d.object_ptr = object_ptr;
            d.stub_ptr = &method_stub<C, TMethod>; // #1

            return d;
        }

        void operator()(const OgitorsPropertyBase* property, Ogre::Any value) const
        {
            return (*stub_ptr)(object_ptr, property, value);
        }

    private:
        typedef void (*stub_type)(void* object_ptr, const OgitorsPropertyBase*, Ogre::Any);

        void* object_ptr;
        stub_type stub_ptr;

        template <class C, void (C::*TMethod)(const OgitorsPropertyBase*,Ogre::Any)>
        static void method_stub(void* object_ptr, const OgitorsPropertyBase* property, Ogre::Any value)
        {
            C* p = static_cast<C*>(object_ptr);
            return (p->*TMethod)(property, value); // #2
        }
    };

    namespace detail
    {
    struct connection_data: public Ogre::GeneralAllocatedObject
    {
        OgitorsSignalFunction function;
        int ref_count;
        bool connected;
    };
    }

    class OgitorsSignal;
    class OgitorsScopedConnection;

    class OgitorsConnection: public Ogre::GeneralAllocatedObject
    {
        friend class OgitorsSignal;
        friend class OgitorsScopedConnection;
    public:    
        OgitorsConnection() : mData(0) {}
        OgitorsConnection(const OgitorsConnection& c) : mData(c.mData && c.mData->ref_count > 0 ? c.mData : 0)
        {
            if(mData)
            {
                mData->ref_count++;
            }
        }

        virtual ~OgitorsConnection()
        {
            if(mData)
            {
                mData->ref_count--;
                if(mData->ref_count < 1)
                    OGRE_DELETE mData;
            }
        }

        OgitorsConnection& operator=(OgitorsConnection& c)
        {
            if(&c == this)
                return *this;

            if(c.mData != mData)
            {
                disconnect();
                mData = c.mData;
                mData->ref_count++;
            }
            return *this;
        }

        void _setData(detail::connection_data *data)
        {
            if(data != mData)
            {
                disconnect();
                mData = data;
                mData->ref_count++;
            }
        }

        inline bool connected()
        {
            return (mData && mData->connected);
        }

        void disconnect()
        {
           if(mData)
           {
               mData->ref_count--;
               if(mData->connected)
               {
                   mData->connected = false;
               }
               else if(mData->ref_count < 1)
               {
                   OGRE_DELETE mData;
               }
               mData = 0;
           }
        };
    protected:
        detail::connection_data *mData;
    };

    class OgitorsScopedConnection: public OgitorsConnection
    {
    public:
        OgitorsScopedConnection() {}

        virtual ~OgitorsScopedConnection()
        {
            disconnect();
        }

        OgitorsScopedConnection& operator=(OgitorsConnection& c)
        {
            if(&c == this)
                return *this;

            if(c.mData != mData)
            {
                disconnect();
                mData = c.mData;
                mData->ref_count++;
            }
            return *this;
        }

        OgitorsScopedConnection& operator=(OgitorsScopedConnection& c)
        {
            if(&c == this)
                return *this;

            if(c.mData != mData)
            {
                disconnect();
                mData = c.mData;
                mData->ref_count++;
            }
            return *this;
        }
    };

    class OgitorsSignal: public Ogre::GeneralAllocatedObject
    {
    public:
        typedef Ogre::vector<detail::connection_data*>::type ConnectionVector;

        OgitorsSignal()
        {
        }

        ~OgitorsSignal()
        {
            for(unsigned int i = 0;i < mConnections.size();i++)    
            {
                mConnections[i]->connected = false;

                if(mConnections[i]->ref_count < 1)
                { 
                    OGRE_DELETE mConnections[i];
                }
            }
            mConnections.clear();
        }

        detail::connection_data *connect(const OgitorsSignalFunction& func)
        {
            detail::connection_data *data = OGRE_NEW detail::connection_data;
            data->ref_count = 0;
            data->connected = true;
            data->function = func;
            mConnections.push_back(data);
            return data;
        }

        void invoke(OgitorsPropertyBase* property, const Ogre::Any &value)
        {
            for(int i = 0;i < (int)mConnections.size();i++)    
            {
                if(mConnections[i]->connected)
                     mConnections[i]->function(property, value);
                else 
                { 
                    if(mConnections[i]->ref_count < 1)
                        OGRE_DELETE mConnections[i];
                    mConnections.erase(mConnections.begin() + i);
                    --i;
                }
            }
        }

        void disconnect(OgitorsConnection *connection)
        {
            for(unsigned int i = 0;i < mConnections.size();i++)    
            {
                if(mConnections[i] == connection->mData)
                {
                    mConnections[i]->connected = false;

                    if(mConnections[i]->ref_count < 1)
                        OGRE_DELETE mConnections[i];

                    mConnections.erase(mConnections.begin() + i);
                    break;
                }
            }
        };

    protected:
        bool mConnected;
        ConnectionVector mConnections;
    };

    /** Type definition for a vector of OgitorsScopedConnection(s) */
    typedef Ogre::vector<Ogitors::OgitorsScopedConnection*>::type ScopedConnectionVector;
}