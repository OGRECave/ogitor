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

/*////////////////////////////////////////////////////////////////////////////////
//This is the General Header to include in any application using Ogitors
////////////////////////////////////////////////////////////////////////////////*/

// non dll-interface foo used as base for dll-interface class bar
#ifdef _MSC_VER
#pragma warning( disable : 4275 )
#pragma warning( disable : 4180 )
#pragma warning( disable : 4305 )
#pragma warning( disable : 4244 )
#endif

#pragma once


namespace Ogitors
{
    class CBaseEditor;
    class CCameraEditor;
    class CEntityEditor;
    class CLightEditor;
    class CMarkerEditor;
    class CMultiSelEditor;
    class CNodeEditor;
    class CParticleEditor;
    class CPlaneEditor;
    class CBillboardSetEditor;
    class CSceneManagerEditor;
    class CMaterialEditor;
    class CTechniqueEditor;
    class CViewportEditor;
    class CVisualHelper;
    class OgitorsRoot;
    class OgitorsView;
    class OgitorsMasterView;
    class OgitorsTemplateView;
    class OgitorsSystem;
    class OgitorsPhysics;
    class OgitorsUndoManager;
    class OgitorsClipboardManager;
    class CBaseSerializer;
    class COgitorsSceneSerializer;
    class ITerrainEditor;
    class IPagingEditor;
    class DragDropHandler;
    class OgitorsUndoBase;
    class PropertyUndo;
    class ObjectCreationUndo;
    class ObjectDeletionUndo;
    class CustomSetRebuiltUndo;
    class UndoCollection;
    class OBBoxRenderable;
    class PGInstanceManager;
    class PGInstanceEditor;
}

    class TiXmlElement;


#include <Ogre.h>
#include "OgitorsDefinitions.h"
#include "StringId.h"
#include "Event.h"
#include "Paging/OgrePagedWorld.h"
#include "Paging/OgrePagedWorldSection.h"
#include "OgitorsProperty.h"
#include "OgitorsUtils.h"


