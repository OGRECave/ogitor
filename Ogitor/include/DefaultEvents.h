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

#ifndef DEFAULT_EVENT_H
#define DEFAULT_EVENT_H

namespace Ogitors
{
    class OgitorExport LoadStateChangeEvent : public IEvent
    {
    public:
        LoadStateChangeEvent(LoadState type) : mType(type) {}
        ~LoadStateChangeEvent() {}
	
        const event_id_type& getID() const { return mID; }
        LoadState getType() const { return mType; }

    private:
        static event_id_type mID;
        LoadState mType;
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport RunStateChangeEvent : public IEvent
    {
    public:
        RunStateChangeEvent(RunState type) : mType(type) {}
        ~RunStateChangeEvent() {}
	
        const event_id_type& getID() const { return mID; }
        RunState getType() const { return mType; }

    private:
        static event_id_type mID;
        RunState mType;
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport SceneModifiedChangeEvent : public IEvent
    {
    public:
        SceneModifiedChangeEvent(bool state) : mState(state) {}
        ~SceneModifiedChangeEvent() {}
	
        const event_id_type& getID() const { return mID; }
        bool getState() const { return mState; }

    private:
        static event_id_type mID;
        bool mState;
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport TerrainEditorChangeEvent : public IEvent
    {
    public:
        TerrainEditorChangeEvent(bool active, ITerrainEditor* handle) : mActive(active), mHandle(handle) {}
        ~TerrainEditorChangeEvent() {}
	
        const event_id_type& getID() const { return mID; }
        bool getActive() const { return mActive; }
        ITerrainEditor* getHandle() { return mHandle; }

    private:
        static event_id_type mID;
        bool mActive;
        ITerrainEditor* mHandle;
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport EditorToolChangeEvent : public IEvent
    {
    public:
        EditorToolChangeEvent(unsigned int type) : mType(type) {}
        ~EditorToolChangeEvent() {}
	
        const event_id_type& getID() const { return mID; }
        unsigned int getType() const { return mType; }

    private:
        static event_id_type mID;
        unsigned int mType;
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport UpdateFrameEvent : public IEvent
    {
    public:
        UpdateFrameEvent(float timePassed) : mTimePassed(timePassed) {}
        ~UpdateFrameEvent() {}
	
        const event_id_type& getID() const { return mID; }
        float getTimePassed() const { return mTimePassed; }

    private:
        static event_id_type mID;
        float mTimePassed;
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport UndoManagerNotificationEvent : public IEvent
    {
    public:
        UndoManagerNotificationEvent(int type, bool enabled, std::string& caption) : mType(type), mEnabled(enabled), mCaption(caption) {}
        ~UndoManagerNotificationEvent() {}
	
        const event_id_type& getID() const { return mID; }
        int getType() const { return mType; }
        bool getEnabled() const { return mEnabled; }
        const std::string& getCaption() const { return mCaption; }

    private:
        static event_id_type mID;
        int                  mType;
        bool                 mEnabled;
        std::string          mCaption;
    };

    //-----------------------------------------------------------------------------------------

    class OgitorExport AfterSceneExportEvent : public IEvent
    {
    public:
        AfterSceneExportEvent(std::string name) : mName(name) {}
        ~AfterSceneExportEvent() {}

        const event_id_type& getID() const { return mID; }
        const std::string& getName() const { return mName; }

    private:
        static event_id_type    mID;
        std::string             mName;
    };
}

#endif