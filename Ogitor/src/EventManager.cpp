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

#include "OgitorsPrerequisites.h"
#include "Event.h"
#include "EventManager.h"

namespace Ogitors
{
    template<> EventManager* Singleton<EventManager>::ms_Singleton = 0;

    void EventManager::connectEvent(const event_id_type& id, void *listener, bool ignoreSender, void *sender, bool ignoreReceiver, void *receiver, EventCallBack handler_func)
    {
        listener_data data;
        data.handler_func = handler_func;
        data.mIgnoreSender = ignoreSender;
        data.mIgnoreReceiver = ignoreReceiver;
        data.mSender = sender;
        data.mReceiver = receiver;

        event_handler_map::iterator it = mEventHandlers.find(id);
        if(it == mEventHandlers.end())
        {
            listener_map listmap;
            listmap[listener] = data;
            mEventHandlers.insert(event_handler_map::value_type(id, listmap));
        }
        else
        {
            if(it->second.find(listener) == it->second.end())
            {
                it->second.insert(listener_map::value_type(listener, data));
                return;
            }
            assert(false && "Listener already exists!!!");
        }
    }

    //-----------------------------------------------------------------------------------------

    void EventManager::disconnectEvent(const event_id_type& id, void *listener)
    {
        event_handler_map::iterator it = mEventHandlers.find(id);

        if(it != mEventHandlers.end())
        {
            listener_map::iterator lit = it->second.find(listener);
            if(lit != it->second.end())
            {
                it->second.erase(lit);
                return;
            }
        }

        assert(false && "Listener does not exist!!!");
    }

    //-----------------------------------------------------------------------------------------

    void EventManager::sendEvent(void *sender, void *receiver, IEvent* event)
    {
        event_handler_map::iterator it = mEventHandlers.find(event->getID());

        if(it != mEventHandlers.end())
        {
            listener_map::iterator lit_end = it->second.end();
            for(listener_map::iterator lit = it->second.begin();lit != lit_end;lit++)
            {
                if((lit->second.mIgnoreSender || lit->second.mSender == sender) && (lit->second.mIgnoreReceiver || lit->second.mReceiver == receiver))
                    lit->second.handler_func(event);
            }
        }
    }
}

//-----------------------------------------------------------------------------------------