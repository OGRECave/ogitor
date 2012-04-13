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
#include "DefaultEvents.h"
#include "EventManager.h"

namespace Ogitors
{
    const event_id_type EventManager::LOAD_STATE_CHANGE              = event_id_type("load_state_change");
    const event_id_type EventManager::MODIFIED_STATE_CHANGE          = event_id_type("modified_state_change");
    const event_id_type EventManager::UPDATE_FRAME                   = event_id_type("update_frame");
    const event_id_type EventManager::UNDOMANAGER_NOTIFICATION       = event_id_type("undomanager_notification");
    const event_id_type EventManager::RUN_STATE_CHANGE               = event_id_type("run_state_change");
    const event_id_type EventManager::EDITOR_TOOL_CHANGE             = event_id_type("editor_tool_change");
    const event_id_type EventManager::TERRAIN_EDITOR_CHANGE          = event_id_type("terrain_editor_change");
    const event_id_type EventManager::AFTER_SCENE_EXPORT             = event_id_type("after_scene_export");
    const event_id_type EventManager::GLOBAL_PREPARE_VIEW            = event_id_type("global_prepare_view");

    event_id_type LoadStateChangeEvent::mID(EventManager::LOAD_STATE_CHANGE);
    event_id_type RunStateChangeEvent::mID(EventManager::RUN_STATE_CHANGE);
    event_id_type SceneModifiedChangeEvent::mID(EventManager::MODIFIED_STATE_CHANGE);
    event_id_type EditorToolChangeEvent::mID(EventManager::EDITOR_TOOL_CHANGE);
    event_id_type UpdateFrameEvent::mID(EventManager::UPDATE_FRAME);
    event_id_type UndoManagerNotificationEvent::mID(EventManager::UNDOMANAGER_NOTIFICATION);
    event_id_type TerrainEditorChangeEvent::mID(EventManager::TERRAIN_EDITOR_CHANGE);
    event_id_type AfterSceneExportEvent::mID(EventManager::AFTER_SCENE_EXPORT);
    event_id_type GlobalPrepareViewEvent::mID(EventManager::GLOBAL_PREPARE_VIEW);

    // Pure virtual Destructor
    IEvent::~IEvent()
    {
    }
}