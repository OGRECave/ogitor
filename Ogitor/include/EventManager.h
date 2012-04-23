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

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "OgitorsSingleton.h"

#define EVENT_CALLBACK(classname , function) Ogitors::EventManager::EventCallBack::from_method<classname, &classname::function>(this)

//-----------------------------------------------------------------------------------------

namespace Ogitors
{
    class IEvent;

    //-----------------------------------------------------------------------------------------
    
    template<typename T> T event_cast(IEvent *evt)
    {
#ifdef _DEBUG
        return dynamic_cast<T>(evt);
#else
        return static_cast<T>(evt);
#endif
    }

    //-----------------------------------------------------------------------------------------

    class event_callback
    {
    public:
        event_callback()
            : object_ptr(0)
            , stub_ptr(0)
        {}

        template <class C, void (C::*Method)(IEvent*)>
        static event_callback from_method(C* object_ptr)
        {
            event_callback d;
            d.object_ptr = object_ptr;
            d.stub_ptr = &method_stub<C, Method>; // #1

            return d;
        }

        void operator()(IEvent* value) const
        {
            (*stub_ptr)(object_ptr, value);
        }

    private:
        typedef void (*stub_type)(void* object_ptr, IEvent*);

        void* object_ptr;
        stub_type stub_ptr;

        template <class C, void (C::*Method)(IEvent*)>
        static void method_stub(void* object_ptr, IEvent* value)
        {
            C* p = static_cast<C*>(object_ptr);
            (p->*Method)(value); // #2
        }
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport EventManager : public Singleton<EventManager>
    {
    public:
        typedef event_callback EventCallBack;

        struct listener_data
        {
            event_callback  handler_func;
            bool            mIgnoreSender;
            bool            mIgnoreReceiver;
            void           *mSender;
            void           *mReceiver;
        };

        // Built-in Ogitor events
        static const event_id_type LOAD_STATE_CHANGE;
        static const event_id_type MODIFIED_STATE_CHANGE;
        static const event_id_type UPDATE_FRAME;
        static const event_id_type UNDOMANAGER_NOTIFICATION;
        static const event_id_type RUN_STATE_CHANGE;
        static const event_id_type EDITOR_TOOL_CHANGE;
        static const event_id_type TERRAIN_EDITOR_CHANGE;
        static const event_id_type AFTER_SCENE_EXPORT;
        static const event_id_type GLOBAL_PREPARE_VIEW;
        static const event_id_type SELECTION_CHANGE;

        typedef std::map<void *, listener_data> listener_map;
        typedef std::map<event_id_type, listener_map> event_handler_map;

        void sendEvent(void *sender, void *receiver, IEvent* event);
	
        void connectEvent(const event_id_type& id, void *listener, bool ignoreSender, void *sender, bool ignoreReceiver, void *receiver, EventCallBack handler_func);
        void disconnectEvent(const event_id_type& id, void *listener);

    private:
        event_handler_map    mEventHandlers;
    };
}

//-----------------------------------------------------------------------------------------

#endif

//-----------------------------------------------------------------------------------------