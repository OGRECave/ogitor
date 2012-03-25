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
//This Header is used to define any library wide structs and enums and defines
////////////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "OgitorsExports.h"
#include "OgitorsGlobals.h"

namespace Ogitors
{

#define OTR(a) mSystem->Translate(a)

    const char PROJECT_RESOURCE_GROUP[]         = "ProjectResources";
    const char PROJECT_TEMP_RESOURCE_GROUP[]    = "ProjectTemp";
      
    /** Enum containing a unique ID for any editor class */
    enum EDITORTYPE 
    {
        ETYPE_BASE = 0,         /** Base editor type */
        ETYPE_SCENEMANAGER,     /** Scene manager editor type */
        ETYPE_VIEWPORT,         /** Viewport editor type */
        ETYPE_FOLDER,           /** Folder editor type */
        ETYPE_PAGINGMANAGER,    /** Paging manager editor type */
        ETYPE_NODE,             /** Node editor type */
        ETYPE_SKY_MANAGER,      /** Sky manager editor type */
        ETYPE_LIGHT,            /** Light editor type */
        ETYPE_TERRAIN_MANAGER,  /** Terrain manager editor type */
        ETYPE_WATER_MANAGER,    /** Terrain manager editor type */
        ETYPE_CUSTOM_MANAGER,   /** Custom manager editor type */
        ETYPE_CAMERA,           /** Camera editor type */
        ETYPE_ENTITY,           /** Entity editor type */
        ETYPE_PLANE,            /** Plane editor type */
        ETYPE_PARTICLE,         /** Particle editor type */
        ETYPE_BILLBOARDSET,     /** Billboard set editor type */
        ETYPE_MOVABLE,          /** Movable editor type */
        ETYPE_GENERALPURPOSE,   /** Material editor type */
        ETYPE_MULTISEL          /** Multi-selection editor type */
    };

    /** String'ified EDITORTYPE enumeration @see EDITORTYPE */
    const Ogre::String EDITORTYPENAMES[] = 
    {
        "ETYPE_BASE",           /** String'ification of EDITORTYPE::ETYPE_BASE */
        "ETYPE_SCENEMANAGER",   /** String'ification of EDITORTYPE::ETYPE_SCENEMANAGER */
        "ETYPE_VIEWPORT",       /** String'ification of EDITORTYPE::ETYPE_VIEWPORT */
        "ETYPE_FOLDER",         /** String'ification of EDITORTYPE::ETYPE_FOLDER */
        "ETYPE_PAGINGMANAGER",  /** String'ification of EDITORTYPE::ETYPE_PAGINGMANAGER */
        "ETYPE_SKY_MANAGER",    /** String'ification of EDITORTYPE::ETYPE_SKY_MANAGER */
        "ETYPE_LIGHT",          /** String'ification of EDITORTYPE::ETYPE_LIGHT */
        "ETYPE_WATER_MANAGER",  /** String'ification of EDITORTYPE::ETYPE_WATER_MANAGER */
        "ETYPE_TERRAIN_MANAGER",/** String'ification of EDITORTYPE::ETYPE_TERRAIN_MANAGER */
        "ETYPE_CUSTOM_MANAGER", /** String'ification of EDITORTYPE::ETYPE_CUSTOM_MANAGER */
        "ETYPE_NODE",           /** String'ification of EDITORTYPE::ETYPE_NODE */
        "ETYPE_CAMERA",         /** String'ification of EDITORTYPE::ETYPE_CAMERA */
        "ETYPE_ENTITY",         /** String'ification of EDITORTYPE::ETYPE_ENTITY */
        "ETYPE_PLANE",          /** String'ification of EDITORTYPE::ETYPE_PLANE */
        "ETYPE_PARTICLE",       /** String'ification of EDITORTYPE::ETYPE_PARTICLE */    
        "ETYPE_BILLBOARDSET",   /** String'ification of EDITORTYPE::ETYPE_BILLBOARDSET */
        "ETYPE_MOVABLE",        /** String'ification of EDITORTYPE::ETYPE_MOVABLE */
        "ETYPE_EDITABLEMESH",   /** String'ification of EDITORTYPE::ETYPE_EDITABLEMESH */
        "ETYPE_MULTISEL",       /** String'ification of EDITORTYPE::ETYPE_MULTISEL */
        "ETYPE_MATERIAL",       /** String'ification of EDITORTYPE::ETYPE_MATERIAL */
        "ETYPE_TECHNIQUE"       /** String'ification of EDITORTYPE::ETYPE_TECHNIQUE */
    };
    /** Last unique ID for an editor */
    const unsigned int LAST_EDITOR = ETYPE_MULTISEL + 1;

    typedef Ogre::vector<Ogre::UTFString>::type UTFStringVector;

    /// Object List Definition
    typedef OgitorExport Ogre::vector<CBaseEditor*>::type ObjectVector;

    /// A Hashed by Name Storage Definition for All Editors
    typedef OgitorExport HashMap<Ogre::String,CBaseEditor*> NameObjectPairList;

    /** Load state enumeration */
    enum LoadState
    {
        LS_UNLOADED = 0,
        LS_LOADING = 1,
        LS_LOADED = 2
    };

    /** Run state enumeration */
    enum RunState
    {
        RS_STOPPED = 0,
        RS_PAUSED = 1,
        RS_RUNNING = 2
    };

    /** Default World Sections enumeration */
    enum OgitorWorldSectionId
    {
        SECT_TERRAIN = 0,
        SECT_GENERAL = 1,
        SECT_MAX = 32
    };

    /** The Axis enumeration */
    enum AXISTYPE 
    {
        AXIS_X = 1,     /** X Axis */
        AXIS_Y = 2,     /** Y Axis */
        AXIS_XY = 3,    /** XY Axis */
        AXIS_Z = 4,     /** Z Axis */
        AXIS_XZ = 5,    /** XZ Axis */
        AXIS_YZ = 6,    /** YZ Axis */
        AXIS_ALL = 7    /** XYZ Axis */
    };
    /** Capabilities enumeration for editor class(es) */
    enum EDITFLAGS 
    {
        CAN_MOVE   = 1,             /** Determines if editor object can be moved */
        CAN_SCALE  = 2,             /** Determines if editor object can be scaled */
        CAN_ROTATEX = 4,            /** Determines if editor object can be rotated around X Axis*/
        CAN_ROTATEY = 8,            /** Determines if editor object can be rotated around Y Axis */
        CAN_ROTATEZ = 16,           /** Determines if editor object can be rotated around Z Axis */
        CAN_ROTATE = 28,            /** Enable All Rotations Mask */
        CAN_DELETE = 32,            /** Determines if editor object can be deleted */
        CAN_UNDO   = 64,            /** Determines if editor object can perform undo/redo operation(s) */
        CAN_CLONE  = 128,           /** Determines if editor object can be cloned */
        CAN_FOCUS  = 256,           /** Determines if editor object can be focused to when user presses focus key */
        CAN_DRAG   = 512,           /** Determines if editor object can be dragged */
        CAN_DROP   = 1024,          /** Determines if editor object can be dropped to */
        CAN_USEMATERIAL = 2048,     /** Determines if editor object can receive a material through drag'n'drop */
        CAN_ACCEPTCOPY = 4096,      /** Determines if editor object can be copied */
        CAN_ACCEPTPASTE = 8192,     /** Determines if editor object is a paste target */
        CAN_PAGE = 16384            /** Determines if editor object is pageable */
    };

    /** Possible types of editor tools enumeration */
    enum EDITORTOOLS 
    {
        TOOL_SELECT = 0,    /** Selection mode, allowing for selection of object(s) */
        TOOL_MOVE,          /** Displacement mode, allowing for moving object(s) */
        TOOL_ROTATE,        /** Rotation mode, allowing for changing object(s) orientation */
        TOOL_SCALE,         /** Scale mode, allowing for scaling of object(s) */
        TOOL_DEFORM,        /** Deformation mode, allowing for deformation of certain meshes (terrain) */
        TOOL_SPLAT,         /** Splatting mode, allowing for splatting of textures on certain meshes (terrain) */
        TOOL_SPLATGRASS,    /** Grass Splatting mode, allowing for splatting of grass textures on certain meshes (terrain) */
        TOOL_PAINT,         /** Painting mode, allowing for painting on certain meshes (terrain) */
        TOOL_SMOOTH         /** Smoothening mode, allowing for decreasing of amplitude of the mesh */
    };
    
    /** Additional editor tools capabilities enumeration */
    enum EDITORTOOLSEX 
    {
        TOOL_EX_NONE = 0,     /** Default inaction */
        TOOL_EX_CLONE,        /** Allows for cloning of selected object(s) */
        TOOL_EX_INSTANCECLONE /** Allows for cloning instances of selected object(s) */
    };

    /** This is the Query Flag for Widgets (To make picking them easier) */
    const unsigned int QUERYFLAG_WIDGET = 1;
    /** This is the Query Flag for any other MovableObject */
    const unsigned int QUERYFLAG_MOVABLE = 2;

    /** Maximum type index of an object */
    const unsigned int OGITOR_MAX_OBJECT_TYPE = 128;

    /** Dialog result (return) enumeration */
    enum DIALOGRET 
    {
        DLGRET_YES = 0, /** Dialog "Yes" return type */
        DLGRET_NO,      /** Dialog "No" return type */
        DLGRET_CANCEL   /** Dialog "Cancel" return type */
    };

    /** Dialog type enumeration */
    enum DIALOGTYPE 
    {
        DLGTYPE_OK = 0,             /** "OK" message box dialog type */
        DLGTYPE_YESNO = 1,          /** "Yes/No" confirmation dialog type */
        DLGTYPE_YESNOCANCEL = 2     /** "Yes/No/Cancel" confirmation dialog type */
    };
    
    /** Mouse cursor type enumeration */
    enum OgitorsMouseCursorType
    {
        MOUSE_ARROW = 0,            /** Arrow mouse cursor type */
        MOUSE_CROSS = 1             /** Cross mouse cursor type */
    };

    /** Ogitor mouse button enumeration */
    enum OgitorsMouseButtons
    {
        OMB_LEFT = 1,       /** Left mouse button type */
        OMB_RIGHT = 2,      /** Right mouse button type */
        OMB_MIDDLE = 4      /** Middle mouse button type */
    };
    
    /** Ogitor special keys bindings structure */
    struct OgitorsSpecialKeys
    {
       unsigned int SPK_LEFT;                       /** Left key flag */
       unsigned int SPK_RIGHT;                      /** Right key flag */
       unsigned int SPK_FORWARD;                    /** Forward key flag */
       unsigned int SPK_BACKWARD;                   /** Backwards key flag */
       unsigned int SPK_UP;                         /** Up key flag */
       unsigned int SPK_DOWN;                       /** Down key flag */
       unsigned int SPK_OBJECT_CENTERED_MOVEMENT;   /** Center-on-object key flag */
       unsigned int SPK_FOCUS_OBJECT;               /** Gain focus on selected object key flag */
       unsigned int SPK_CLONE;                      /** Clone object key flag */
       unsigned int SPK_DELETE;                     /** Delete object key flag */

       unsigned int SPK_REVERSE_UPDATE;             /** Reverses the direction of the tool (deform/splat) */
       unsigned int SPK_SWITCH_AXIS;                /** Next axis selection key flag */
       unsigned int SPK_SNAP;                       /** Snap to grid/ground (when free move) key flag */
       unsigned int SPK_ADD_TO_SELECTION;           /** Add to selection key flag */
       unsigned int SPK_SUBTRACT_FROM_SELECTION;    /** Subtract from selection key flag */
       unsigned int SPK_ALWAYS_SELECT;              /** Always select key flag */
    };

    /** Project options structure */
    struct PROJECTOPTIONS
    {
      bool               IsNewProject;              /** Flag specifying if project is newly created */
      Ogre::String       CreatedIn;                 /** The base directory of the Project Directory */
      Ogre::String       ProjectDir;                /** Root directory in which project file(s) reside (absolute) */
      Ogre::String       ProjectName;               /** Name of the project */
      Ogre::String       SceneManagerName;          /** Scene manager name */
      Ogre::String       TerrainDirectory;          /** Terrain data directory name (relative)*/
      Ogre::String       HydraxDirectory;           /** Hydrax (water) data directory name (relative)*/
      Ogre::String       CaelumDirectory;           /** Caelum (sky) data directory name (relative) */
      Ogre::String       ParticleUniverseDirectory; /** ParticleUniverse data directory name (relative) */
      Ogre::String       PagedGeometryDirectory;    /** PagedGeometry data directory name (relative) */
      Ogre::String       SceneManagerConfigFile;    /** Scene manager configuration file name */   
      Ogre::StringVector ResourceDirectories;       /** A string list of resource directory(ies) */
      float              CameraSpeed;               /** Camera Speed in turns of units per second */
      Ogre::Vector3      CameraPositions[11];       /** Camera positions array (scene can have multiple cameras) */
      Ogre::Quaternion   CameraOrientations[11];    /** Camera orientations array (scene can have multiple cameras) */
      int                CameraSaveCount;           /** Index of last created camera */
      Ogre::ColourValue  SelectionRectColour;       /** Colour of the Selection Rectangle */
      Ogre::ColourValue  SelectionBBColour;         /** Colour of the Selection Bounding Box */
      Ogre::ColourValue  HighlightBBColour;         /** Colour of the Highlight Bounding Box */
      Ogre::ColourValue  SelectHighlightBBColour;   /** Colour of the Selection + Highlight Bounding Box */
      Ogre::ColourValue  GridColour;                /** Colour of the Viewport Grid */
      Ogre::Real         GridSpacing;               /** Spacing between Viewport Grid Cells */
      Ogre::Real         SnapAngle;                 /** Angle to use as interval during snap to angle */
      Ogre::String       LayerNames[31];            /** Array of Layer Names */ 
      bool               LayerVisible[31];          /** Array of Layer Visibilities */
      int                LayerCount;                /** Number of Layers */
      float              WalkAroundHeight;          /** The height at which camera will be kept during walk around mode */ 
      Ogre::Real         VolumeSelectionDepth;      /** Depth of Volume Selection Box */
      unsigned int       ObjectCount;               /** Number of objects in scene */
      bool               AutoBackupEnabled;         /** Flag specifying if auto backup is enabled */
      int                AutoBackupPeriod;          /** Period of time between auto backups */
      int                AutoBackupPeriodType;      /** Period type of auto backups: 0 = minutes, 1 = hours */
      Ogre::String       AutoBackupFolder;          /** Folder the backups are stored in */
      int                AutoBackupNumber;          /** Number of backups to be stored */
    };

    /** Ogitor Plugin Features enumeration */
    enum PluginFeatureTypes
    {
        PLUGIN_FEATURE_FACTORY = 1,                 /** Plugin has registered editor factory(ies) */
        PLUGIN_FEATURE_SERIALIZER = 2,              /** Plugin has registered serializer(s) */
        PLUGIN_FEATURE_TOOLBAR = 4,                 /** Plugin has registered toolbar(s) */
        PLUGIN_FEATURE_DOCKWINDOW = 8,              /** Plugin has registered docking window(s) */
        PLUGIN_FEATURE_TABWINDOW = 16,              /** Plugin has registered tabbed render window(s) */
        PLUGIN_FEATURE_SCRIPTINTERPRETER = 32,      /** Plugin has registered script interpreter(s) */
        PLUGIN_FEATURE_PREFWINDOW = 64              /** Plugin has registered preferences sections */
    };

    /** Structure to hold data about plugin entries */
    struct PLUGINENTRY
    {
        Ogre::String        mName;                   /** Name of the plugin */
        Ogre::String        mPluginPath;             /** Path to the plugin */
        bool                mLoaded;                 /** Indicator if plugin is loaded or not */
        bool                mLoadingError;           /** Indicator if an error occurred during loading */
        Ogre::DynLib       *mLibrary;                /** Pointer to plugin's library */
        Ogre::StringVector  mEditorObjects;          /** List of Editor Objects this plugin registered */ 
        Ogre::StringVector  mSerializers;            /** List of Serializers this plugin registered */ 
        Ogre::StringVector  mScriptInterpreters;     /** List of Script Interpreters this plugin registered */ 
        std::vector<void*>  mToolbars;               /** List of Toolbars this plugin registered */ 
        std::vector<void*>  mDockWidgets;            /** List of DockWidgets this plugin registered */ 
        std::vector<void*>  mTabWidgets;             /** List of TabWidgets this plugin registered */ 
        std::vector<void*>  mPrefWidgets;            /** List of PrefWidgets this plugin registered */
        unsigned int        mFeatures;               /** Bit field to store what his plugin adds to application*/  
    };

    typedef Ogre::map<void *, PLUGINENTRY>::type PluginEntryMap;

    class CBaseEditorFactory;

    typedef Ogre::map<Ogre::String, CBaseEditorFactory*>::type EditorObjectFactoryMap;

    //! A mouse listener class
    /*!  
    A mouse listener class that is used to capture mouse events, button(s) pressed and/or mouse position
    */ 
    class MouseListener
    {
    public:
        /**
        * Constructor
        */
        MouseListener() {};
        /**
        * Destructor
        */
        virtual ~MouseListener() {};

        /**
        * Delegate function for mouse move event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags 
        */
        virtual void OnMouseMove (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse leave event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseLeave (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse left button down event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseLeftDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse left button up event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseLeftUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse right button down event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseRightDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse right button up event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags
        */
        virtual void OnMouseRightUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse middle button down event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseMiddleDown (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse middle button up event
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseMiddleUp (CViewportEditor *viewport, Ogre::Vector2 point, unsigned int buttons) = 0;
        /**
        * Delegate function for mouse wheel movement (up or down)
        * @param viewport window to receive the input
        * @param point new mouse position
        * @param delta change of mouse scrolling position
        * @param buttons mouse button flags  
        */
        virtual void OnMouseWheel (CViewportEditor *viewport, Ogre::Vector2 point, float delta, unsigned int buttons) = 0;
    };

    //! A keyboard listener class
    /*!  
    A keyboard listener class that is used to capture keyboard events
    */ 
    class KeyboardListener
    {
    public:
        /**
        * Constructor
        */
        KeyboardListener() {};
        /**
        * Destructor
        */
        virtual ~KeyboardListener() {};

        /**
        * Delegate function for keyboard key down event
        * @param key key that is down at the moment (not pressed)
        * @return true if key is down, return true, otherwise false
        */
        virtual bool OnKeyDown (unsigned short key) = 0;
        /**
        * Delegate function for keyboard key up (released) event
        * @param key key that has just been released
        * @return true if key had just been released, otherwise false
        */
        virtual bool OnKeyUp (unsigned short key) = 0;
    };
    
    enum DragDropKeyboardModifier {
        DragDropNoModifier           = 0x00000000,
        DragDropShiftModifier        = 0x02000000,
        DragDropControlModifier      = 0x04000000,
        DragDropAltModifier          = 0x08000000
    };

    //! Drag & Drop Handler class
    /*!  
    A Drag & Drop Handler class that is used to handle drag&drop events sent to render window    
    */ 
    class DragDropHandler
    {
    public:
        /**
        * Constructor
        */
        DragDropHandler() {};
        /**
        * Destructor
        */
        virtual ~DragDropHandler() {};

        /**
        * Delegate function for drag enter event
        * @return true if the drag event accepted, otherwise false
        */
        virtual bool OnDragEnter () = 0;
        /**
        * Delegate function for drag leave event
        */
        virtual void OnDragLeave () = 0;
        /**
        * Delegate function for drag move event
        * @param vp the viewport in which the event occurred
        * @param modifier the keyboard modifiers
        * @param position mouse position relative to the viewports width/height
        * @return true if the drag event accepted, otherwise false
        */
        virtual bool OnDragMove (Ogre::Viewport *vp, unsigned int modifier, Ogre::Vector2& position) = 0;
        /**
        * Delegate function for drag dropped event
        * @param vp the viewport in which the event occurred
        * @param delta mouse delta 
        */
        virtual void OnDragWheel (Ogre::Viewport *vp, float delta) = 0;
        /**
        * Delegate function for drag dropped event
        * @param vp the viewport in which the event occurred
        * @param position mouse position relative to the viewports width/height
        */
        virtual void OnDragDropped (Ogre::Viewport *vp, Ogre::Vector2& position) = 0;
    };

    typedef OgitorExport Ogre::map<void *, DragDropHandler*>::type DragDropHandlerMap;

    /**
    * General inline function to validate attribute values read from an XML Element
    * @param val value to be verified
    * @param defaultvalue the default value to be set if val is null
    */
    inline const char * ValidAttr(const char * val,const char *defaultvalue = "")
    {
        if(val)
            return val;
        else
            return defaultvalue;
    }
};
