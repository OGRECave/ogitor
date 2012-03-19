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
///////////////////////////////////////////////////////////////////////////////////*/

#pragma once

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <cassert>
#include <exception>
#include <time.h>
#include <boost/thread.hpp>

#if defined( __WIN32__ ) || defined( _WIN32 )
   #ifdef OFS_EXPORT
     #define OfsExport __declspec (dllexport)
   #else
     #define OfsExport __declspec (dllimport)
   #endif
#else
   #define OfsExport
#endif

#pragma warning( disable : 4251 )

namespace OFS
{

#define VERSION_MAJOR_0 '0'
#define VERSION_MAJOR_1 '1'
#define VERSION_MINOR   '3'
#define VERSION_FIX     '0'

#define AUTO_MUTEX mutable boost::recursive_mutex OfsMutex;
#define LOCK_AUTO_MUTEX boost::recursive_mutex::scoped_lock ofsAutoMutexLock(OfsMutex);
#define STATIC_AUTO_MUTEX_DECL(a) boost::recursive_mutex a::OfsStaticMutex;
#define STATIC_AUTO_MUTEX static boost::recursive_mutex OfsStaticMutex;
#define STATIC_LOCK_AUTO_MUTEX boost::recursive_mutex::scoped_lock ofsAutoMutexLock(OfsStaticMutex);

    extern const unsigned int MAX_BUFFER_SIZE;


    enum OfsResult
    {
        OFS_INVALID_UUID = -11,
        OFS_PREVIOUS_VERSION = -10,
        OFS_UNKNOWN_VERSION = -9,
        OFS_ACCESS_DENIED = -8,
        OFS_DIRECTORY_NOT_EMPTY = -7,
        OFS_INVALID_PATH = -6,
        OFS_FILE_CORRUPT = -5,
        OFS_IO_ERROR = -4,
        OFS_DISK_FULL = -3,
        OFS_FILE_NOT_FOUND = -2,
        OFS_INVALID_FILE = -1,
        OFS_OK = 0
    };

    enum MountType
    {
        OFS_MOUNT_CREATE = 0,
        OFS_MOUNT_OPEN = 1,
        OFS_MOUNT_RECOVER = 2
    };

    enum FileOpType
    {
        OFS_READ = 1,
        OFS_WRITE = 2,
        OFS_READWRITE = 3,
        OFS_APPEND = 4
    };

    enum SeekDirection
    {
        OFS_SEEK_BEGIN,
        OFS_SEEK_CURRENT,
        OFS_SEEK_END
    };

    enum FileFlags
    {
        OFS_DIR = 1,
        OFS_FILE = 2,
        OFS_READONLY = 4,
        OFS_HIDDEN = 8
    };

#pragma pack(push)
#pragma pack(4)
    struct UUID
    {
        unsigned char data[16];

        UUID()
        {
            memset(data, 0, 16);
        };
        
        UUID(unsigned int _a, unsigned short _b, unsigned short _c, unsigned char _d0, unsigned char _d1, unsigned char _d2, unsigned char _d3, unsigned char _d4, unsigned char _d5, unsigned char _d6, unsigned char _d7)
        {
            *((unsigned int *)&data[0]) = _a;
            *((unsigned short *)&data[4]) = _b;
            *((unsigned short *)&data[6]) = _c;
            data[8] = _d0;
            data[9] = _d1;
            data[10] = _d2;
            data[11] = _d3;
            data[12] = _d4;
            data[13] = _d5;
            data[14] = _d6;
            data[15] = _d7;
        };

        std::string toString()
        {
            char dest[36];

            unsigned int a = *((unsigned int *)&data[0]);
            unsigned short b = *((unsigned short *)&data[4]);
            unsigned short c = *((unsigned short *)&data[6]);

#if defined( __WIN32__ ) || defined( _WIN32 )
            sprintf_s(dest, 36, "%08X-%04hX-%04hX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", a, b, c, data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
#else
            sprintf(dest, "%08X-%04hX-%04hX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", a, b, c, data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
#endif

            return std::string(dest);
        };

        void fromString(const std::string& str)
        {
            unsigned int a;
            unsigned short b, c;

#if defined( __WIN32__ ) || defined( _WIN32 )
            sscanf_s(str.c_str(), "%08X-%04hX-%04hX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", &a, &b, &c, &data[8], &data[9], &data[10], &data[11], &data[12], &data[13], &data[14], &data[15]);
#else
            sscanf(str.c_str(), "%08X-%04hX-%04hX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX", &a, &b, &c, &data[8], &data[9], &data[10], &data[11], &data[12], &data[13], &data[14], &data[15]);
#endif
            *((unsigned int *)&data[0]) = a;
            *((unsigned short *)&data[4]) = b;
            *((unsigned short *)&data[6]) = c;
        };

        const UUID& operator=(const UUID& uuid)
        {
            memcpy(data, uuid.data, 16);
            return *this;
        };

        bool operator==(const UUID& uuid) const
        {
            return (memcmp(data, uuid.data, 16) == 0);
        };

        bool operator!=(const UUID& uuid) const
        {
            return (memcmp(data, uuid.data, 16) != 0);
        };

        bool operator<(const UUID& uuid) const
        {
            return (memcmp(data, uuid.data, 16) < 0);
        };

        bool operator>(const UUID& uuid) const
        {
            return (memcmp(data, uuid.data, 16) > 0);
        };

        bool operator<=(const UUID& uuid) const
        {
            return (memcmp(data, uuid.data, 16) <= 0);
        };

        bool operator>=(const UUID& uuid) const
        {
            return (memcmp(data, uuid.data, 16) >= 0);
        };
    };

    struct OTIME
    {
        char data[8];

        OTIME()
        {
            memset(data, 0, 8);
        };

        OTIME(time_t _tm)
        {
            *((time_t *)data) = _tm;
        };

        operator time_t()
        {
            return *((time_t *)data);
        };

        const OTIME& operator=(const time_t& _tm)
        {
            *((time_t *)data) = _tm;
            return *this;
        };

        const OTIME& operator=(const OTIME& _tm)
        {
            memcpy(data, _tm.data, 8);
            return *this;
        };
    };

#pragma pack(pop)

    extern const UUID UUID_ZERO;
    
    struct FileEntry
    {
        std::string  name;
        unsigned int flags;
        UUID         uuid; 
        unsigned int file_size;
        time_t       create_time;
        time_t       modified_time;

        static bool Compare ( FileEntry elem1, FileEntry elem2 )
        {
            return (strcmp(elem1.name.c_str(), elem2.name.c_str()) < 0);
        };
    };

    typedef std::vector<FileEntry> FileList;

    struct FileSystemStats
    {
        unsigned int NumDirectories;
        unsigned int NumFiles;
        unsigned int UsedAllocations;
        unsigned int FreeAllocations;
        unsigned int UsedSpace;
        unsigned int FreeSpace;
        unsigned int ActualUsedSpace;
        unsigned int ActualFreeSpace;
        unsigned int TotalFileSize;
    };
    
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    class OfsExport Exception : public std::exception
    {
    protected:
        long mLine;
        std::string mDescription;
        std::string mFile;
        mutable std::string mFullDesc;

    public:
        /** constructor.
        */
        Exception( const std::string& description, const char* file = NULL, long line = 0) : mLine(line), mDescription(description), mFile(file), mFullDesc("") {};

        /** Copy constructor.
        */
        Exception(const Exception& rhs) : mLine(rhs.mLine), mDescription(rhs.mDescription), mFile(rhs.mFile), mFullDesc(rhs.mFullDesc) {}; 

		/// Needed for  compatibility with std::exception
		~Exception() throw() {}

        /** Assignment operator.
        */
        void operator = (const Exception& rhs)
        {
            mDescription = rhs.mDescription;
            mFile = rhs.mFile;
            mLine = rhs.mLine;
            mFullDesc = rhs.mFullDesc;
        };

		/** Returns a string with 'description' 'file' and 'line' field of this exception.
        */
        const std::string &getDescription(void) const
        {
            if(mFullDesc.length() == 0)
            {
                std::stringstream desc;

			    desc <<  "OFS EXCEPTION: " << mDescription;

			    if( mLine > 0 )
			    {
				    desc << " at " << mFile << " (line " << mLine << ")";
			    }

                mFullDesc = desc.str();
            }

            return mFullDesc;
        };

		/// Override std::exception::what
		const char* what() const throw() { return getDescription().c_str(); }
        
    };

#ifndef OFS_EXCEPT
#define OFS_EXCEPT(desc) throw OFS::Exception(desc, __FILE__, __LINE__ );
#endif

    class OfsPtr;
    class OFSHANDLE;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    typedef void (*LogCallBackFunction)(std::string);

    class OfsExport _Ofs
    {
        friend class OfsPtr;
    public:

        /* Determines type of Allocated Blocks in OFS Files
         * OFS_FREE_BLOCK is a mask which marks a block as FREE
         * OFS_MAIN_BLOCK is the first block of any file or folder which contains all required header data
         * OFS_EXTENDED_BLOCK is any extra block needed for extending the file, only contains a short header
         */
        enum BlockType
        {
            OFS_FREE_BLOCK = 0xFF000000,
            OFS_MAIN_BLOCK = 1,
            OFS_EXTENDED_BLOCK = 2
        };

#pragma pack(push)
#pragma pack(4)

        /* OFS File Header */
        struct strFileHeader
        {
            unsigned char ID[4];               /* The file identifier */
            unsigned char VERSION[4];          /* Version of the OFS File */
            unsigned int  BLOCK_HEADER_SIG[2]; /* Identifiers used as Block Header Signatures */
            unsigned int  LAST_ID;             /* The Last ID used for any entry */
            unsigned int  RESERVED[27];        /* RESERVED */
        };

        /* Holds information about an allocated block (for writing to file as block header) */
        struct strBlockHeader
        {
            unsigned int Signature[2]; /* Block Signature */
            unsigned int Type;         /* Block Type */
            unsigned int Length;       /* Length of the block in file not including this header */
        };

        /* Short entry header for EXTENDED BLOCKS */
        struct strExtendedEntryHeader
        {
             int          Id;           /* Id of the Owner Entry */
             int          ParentId;     /* Id of the Owner Entry's Parent Directory, -1 if root directory */
             unsigned int NextBlock;    /* Position of Next Block in file (owned by the same entry) */
             unsigned int Index;        /* Index of this Extended Data */
        };

        /* Full Entry Header, contains all information needed for entry */
        struct strMainEntryHeader
        {
            int          Id;              /* Id of the Owner Entry */
            int          ParentId;        /* Id of the Owner Entry's Parent Directory, -1 if root directory */
            unsigned int NextBlock;       /* File Position of Next Block owned by this entry */
            unsigned int Flags;           /* File Flags */
            unsigned int FileSize;        /* Entry's File Size, 0 for Directories */
            unsigned int RESERVED[5];     /* RESERVED */
            char         Name[256];       /* Entry's Name */
            OTIME        CreationTime;    /* Entry's Creation Time */
            UUID         Uuid;            /* UUID of Entry */
        };

#pragma pack(pop)

        /* Holds information about an allocated block (for keeping in memory) */
        struct BlockData
        {
            unsigned int Type;        /* Block Type */
            unsigned int Start;       /* Starting position of Block in File, just after header */
            unsigned int Length;      /* Length of the block in file not including file block header */ 
            unsigned int NextBlock;   /* Position of Next Block in file */
        };

        struct OfsEntryDesc;

        typedef void (*CallBackFunction)(void *userData, OfsEntryDesc* arg1, const char *arg2);
        

        enum CallBackType
        {
            CLBK_CONTENT = 0,
            CLBK_RENAME = 1,
            CLBK_CREATE = 2,
            CLBK_DELETE = 3
        };
        
        struct CallBackData
        {
            CallBackType      type;
            CallBackFunction  func;
            void             *data;
            void             *owner;
        };
             
        /* Entry Descriptor, contains all information needed for entry (in memory) */
        struct OfsEntryDesc
        {
            int           Id;                     /* Id of the Owner Entry */
            int           ParentId;               /* Id of the Owner Entry's Parent Directory, -1 if root directory */
            unsigned int  Flags;                  /* File Flags */
            UUID          Uuid;                   /* UUID of Entry */
            time_t        CreationTime;           /* Entry's Creation Time */
            unsigned int  FileSize;               /* Entry's File Size, 0 for Directories */
            std::string   Name;                   /* Entry's Name */
            OfsEntryDesc *Parent;                 /* Pointer to Entry's Parent's descriptor */
            int           UseCount;               /* Number of handles using this entry */
            bool          WriteLocked;            /* If true, Entry is not suitable for WRITE operations */
            std::vector<BlockData> UsedBlocks;    /* Vector of BlockData used by this entry */
            std::vector<OfsEntryDesc*> Children;  /* Vector of Entry's children */
            std::vector<CallBackData> Triggers;   /* Vector of Entry's triggers */
        };

        typedef std::map<unsigned int, BlockData> PosBlockDataMap;
        typedef std::map<int, OfsEntryDesc*> IdDescMap;
        typedef std::map<UUID, OfsEntryDesc*> UuidDescMap;
        typedef std::map<int, OFSHANDLE*> IdHandleMap;
        typedef std::map<std::string, _Ofs*> NameOfsHandleMap;
        typedef std::vector<BlockData> BlockDataVector;


        /**
        * Retrieves Name of the file used by Virtual File System
        * @return Name of the file used by Virtual File System
        */
        const std::string& getFileSystemName() const { return mFileName; };

        /**
        * Checks if file system is mounted
        * @return true if file system is mounted
        */
        bool         isActive();
        /**
        * Retrieves various statistics about file system
        * @param stats Struct used to return statistics
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileSystemStats(FileSystemStats& stats);
        /**
        * Adds a new file system trigger
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to get notified about
        * @param _func Function to call for notification
        * @param _data User data to be passed to notification function
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    addTrigger(void *_owner, CallBackType _type, CallBackFunction _func, void *_data = 0);
        /**
        * Removes a file system trigger
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to remove notification about
        */
        void         removeTrigger(void *_owner, CallBackType _type);
        /**
        * Adds a new file trigger
        * @param filename Name of the file to add trigger for
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to get notified about
        * @param _func Function to call for notification
        * @param _data User data to be passed to notification function
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    addFileTrigger(const char *filename, void *_owner, CallBackType _type, CallBackFunction _func, void *_data = 0);
        /**
        * Removes a file trigger
        * @param filename Name of the file to remove trigger for
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to remove notification about
        */
        void         removeFileTrigger(const char *filename, void *_owner, CallBackType _type);
        /**
        * Makes a defragmented copy of the file system
        * @param dest path of the destination file
        * @param logCallBackFunc method that gets called for each processed OFS component
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    defragFileSystemTo(const char *dest, LogCallBackFunction* logCallbackFunc = NULL);
        /**
        * Copies the current file system and switches to it
        * @param dest path of the destination file
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    moveFileSystemTo(const char *dest);
        /**
        * Switches to the new file system
        * @param dest path of the destination file to switch to
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    switchFileSystemTo(const char *dest);
        /**
        * Creates a new directory
        * @param filename path for new directory
        * @param force If true, all directories in the path are created
        * @return Result of operation, OFS_OK if successful
        */
        inline OfsResult    createDirectory(const char *filename, bool force = false)
        {
            return createDirectoryUUID(filename, UUID_ZERO, force);
        };
        /**
        * Creates a new directory
        * @param filename path for new directory
        * @param uuid uuid to be assigned to the directory
        * @param force If true, all directories in the path are created
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    createDirectoryUUID(const char *filename, const UUID& uuid, bool force = false); 
        /**
        * Deletes a given directory
        * @param filename path for directory to delete
        * @param force If true, all directories in the path are deleted
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    deleteDirectory(const char *path, bool force = false);
        /**
        * Deletes a given file
        * @param filename path for file to delete
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    deleteFile(const char *filename);
        /**
        * Renames a given file
        * @param filename path for file to rename
        * @param newname new name of the file
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    renameFile(const char *filename, const char *newname);
        /**
        * Copies a given file
        * @param src source path of file to copy
        * @param dest destination filename
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    copyFile(const char *src, const char *dest);
        /**
        * Moves a given file to a new location
        * @param src source path of file to move
        * @param dest destination filename
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    moveFile(const char *src, const char *dest);
        /**
        * Renames a given directory
        * @param dirname path of directory to rename
        * @param newname new name of the directory
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    renameDirectory(const char *dirname, const char *newname);
        /**
        * Creates a new file
        * @param handle File Handle, once the file is created, this handle will point to it
        * @param filename path of file to be created
        * @param file_size initial file size
        * @param data_size size of data passed
        * @param data initial data to fill in the file
        * @return Result of operation, OFS_OK if successful
        */
        inline OfsResult    createFile(OFSHANDLE& handle, const char *filename, unsigned int file_size = 0, unsigned int data_size = 0, const char *data = NULL)
        {
            return createFileUUID(handle, filename, UUID_ZERO, file_size, data_size, data);
        };
        /**
        * Creates a new file
        * @param handle File Handle, once the file is created, this handle will point to it
        * @param filename path of file to be created
        * @param uuid uuid to be assigned to the file
        * @param file_size initial file size
        * @param data_size size of data passed
        * @param data initial data to fill in the file
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    createFileUUID(OFSHANDLE& handle, const char *filename, const UUID& uuid, unsigned int file_size = 0, unsigned int data_size = 0, const char *data = NULL);
        /**
        * Opens a file
        * @param handle File Handle, once the file is opened, this handle will point to it
        * @param filename path of file to be opened
        * @param open_mode combination of OFS_READ, OFS_WRITE, OFS_APPEND
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    openFile(OFSHANDLE& handle, const char *filename, unsigned int open_mode = OFS_READ);
        /**
        * Opens a file
        * @param handle File Handle, once the file is opened, this handle will point to it
        * @param uuid UUID of the file to open
        * @param open_mode combination of OFS_READ, OFS_WRITE, OFS_APPEND
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    openFile(OFSHANDLE& handle, const UUID& uuid, unsigned int open_mode = OFS_READ);
        /**
        * Closes a file
        * @param handle File Handle of the file to be closed
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    closeFile(OFSHANDLE& handle);
        /**
        * Truncates a file 
        * @param handle File Handle of the file to be truncated
        * @param file_size New file_size of the file, pass -1 to truncate at Write Position, must be less than or equal to original File Size
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    truncateFile(OFSHANDLE& handle, int file_size = -1);
        /**
        * List files/folders in a given directory
        * @param path path of the directory
        * @param file_flags Filters the results by given flag
        * @return List of files matching the filter
        */
        FileList     listFiles(const char *path, unsigned int file_flags = OFS_FILE | OFS_DIR);
        /**
        * List files/folders in a given directory including subfolders, the filenames contain full path
        * @param path path of the directory
        * @param list Filled with results
        * @return Total filesize of returned files
        */
        unsigned int listFilesRecursive(const std::string& path, FileList& list);
        /**
        * Retrieves Name of the file
        * @param handle handle to the file
        * @param filename Returns Name of the file pointed by handle
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult getFileName(OFSHANDLE& handle, std::string& filename);
        /**
        * Retrieves FileEntry structure of directory
        * @param path path to the directory
        * @param entry Returns directory's entry data
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getDirEntry(const char *path, FileEntry& entry); 
        /**
        * Retrieves FileEntry structure of file
        * @param filename path to the file
        * @param entry Returns file's entry data
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileEntry(const char *filename, FileEntry& entry); 
        /**
        * Retrieves FileEntry structure of file
        * @param handle handle to the file
        * @param entry Returns file's entry data
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileEntry(OFSHANDLE& handle, FileEntry& entry); 
        /**
        * Retrieves creation time of file
        * @param filename path to the file
        * @param creation_time Returns file's creation time
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getCreationTime(const char *filename, time_t& creation_time); 
        /**
        * Retrieves creation time of file
        * @param handle handle to the file
        * @param creation_time Returns file's creation time
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getCreationTime(OFSHANDLE& handle, time_t& creation_time); 
        /**
        * Retrieves modification time of file
        * @param filename path to the file
        * @param creation_time Returns file's modification time
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getModificationTime(const char *filename, time_t& mod_time); 
        /**
        * Retrieves modification time of file
        * @param handle handle to the file
        * @param creation_time Returns file's modification time
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getModificationTime(OFSHANDLE& handle, time_t& mod_time); 
        /**
        * Retrieves filesize of file
        * @param filename path to the file
        * @param size Returns file's size
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileSize(const char *filename, unsigned int& size); 
        /**
        * Retrieves filesize of file
        * @param handle handle to the file
        * @param size Returns file's size
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileSize(OFSHANDLE& handle, unsigned int& size); 
        /**
        * Sets file flags like readonly/hidden
        * @param filename path to the file
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setFileFlags(const char *filename, unsigned int flags); 
        /**
        * Sets file flags like readonly/hidden
        * @param handle handle to the file
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setFileFlags(OFSHANDLE& handle, unsigned int flags); 
        /**
        * Retrieves file flags like readonly/hidden
        * @param filename path to the file
        * @param flags returns combination of current file flags
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileFlags(const char *filename, unsigned int& flags); 
        /**
        * Retrieves file flags like readonly/hidden
        * @param handle handle to the file
        * @param flags returns combination of current file flags
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileFlags(OFSHANDLE& handle, unsigned int& flags); 
        /**
        * Sets directory flags like readonly/hidden
        * @param dirpath path to the directory
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setDirFlags(const char *dirpath, unsigned int flags); 
        /**
        * Retrieves directory flags like readonly/hidden
        * @param dirpath path to the directory
        * @param flags returns combination of current directory flags
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getDirFlags(const char *dirpath, unsigned int& flags); 
        /**
        * Sets file uuid
        * @param filename path to the file
        * @param uuid new uuid to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setFileUUID(const char *filename, const UUID& uuid); 
        /**
        * Sets file uuid
        * @param handle handle to the file
        * @param uuid new uuid to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setFileUUID(OFSHANDLE& handle, const UUID& uuid); 
        /**
        * Retrieves file uuid
        * @param filename path to the file
        * @param uuid returns file's uuid in this parameter
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileUUID(const char *filename, UUID& uuid); 
        /**
        * Retrieves file uuid
        * @param handle handle to the file
        * @param uuid returns file's uuid in this parameter
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileUUID(OFSHANDLE& handle, UUID& uuid); 
        /**
        * Sets directory uuid
        * @param dirpath path to the directory
        * @param uuid new uuid to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setDirUUID(const char *dirpath, const UUID& uuid); 
        /**
        * Retrieves directory uuid
        * @param dirpath path to the directory
        * @param uuid returns directory's uuid in this parameter
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getDirUUID(const char *dirpath, UUID& uuid); 
        /**
        * Checks if a file exists
        * @param filename path of the file to check
        * @return True if file exists
        */
        bool         exists(const char *filename); 

        /**
        * Reads data from a given file (handle)
        * @param handle handle of the file
        * @param dest Buffer to read data to
        * @param length Length of data to read (in Bytes)
        * @param actual_read LEngth of data actually read
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    read(OFSHANDLE& handle, char *dest, unsigned int length, unsigned int *actual_read = NULL);
        /**
        * Writes data to a given file (handle)
        * @param handle handle of the file
        * @param src Buffer to write data from
        * @param length Length of data to write (in Bytes)
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    write(OFSHANDLE& handle, const char *src, unsigned int length);
        /**
        * Sets Read Position in a file
        * @param handle handle of the file
        * @param pos relative position to set
        * @param dir direction of seek operation
        * @return Returns final read position
        */
        unsigned int seekr(OFSHANDLE& handle, int pos, SeekDirection dir);
        /**
        * Sets Write Position in a file
        * @param handle handle of the file
        * @param pos relative position to set
        * @param dir direction of seek operation
        * @return Returns final write position
        */
        unsigned int seekw(OFSHANDLE& handle, int pos, SeekDirection dir);
        /**
        * Retrieves Current Read Position in a file
        * @param handle handle of the file
        * @return Returns current read position
        */
        unsigned int tellr(OFSHANDLE& handle);
        /**
        * Retrieves Current Write Position in a file
        * @param handle handle of the file
        * @return Returns current write position
        */
        unsigned int tellw(OFSHANDLE& handle);
        /**
        * Checks if Read Pointer is at the End Of File
        * @param handle handle of the file
        * @return True if eof reached during read operation
        */
        bool         eof(OFSHANDLE& handle);

    private:
        AUTO_MUTEX
        STATIC_AUTO_MUTEX
        std::string               mFileName;            // Underlying filename of the file system
        bool                      mActive;              // Is a file system mounted?
        std::fstream              mStream;              // Handle of underlying file system
        strFileHeader             mHeader;              // File System Header
        OfsEntryDesc              mRootDir;             // Root directory definition
        BlockDataVector           mFreeBlocks;          // Vector holding free(available) blocks in file system 
        IdHandleMap               mActiveFiles;         // Map holding currently open files
        UuidDescMap               mUuidMap;             // Map holding entry descriptors indexed with UUID
        int                       mUseCount;            // Number of objects using this file system instance
        bool                      mRecoveryMode;        // Is recovery mode activated?
        std::vector<CallBackData> mTriggers;            // Vector of File System Triggers 
        LogCallBackFunction       mLogCallBackFunc;     // Function pointer to callback handler
        
        static NameOfsHandleMap   mAllocatedHandles; // Static map containing all file system instances

        /* Private Constructor */
        _Ofs();
        /* Private Destructor */
        ~_Ofs();
        _Ofs& operator=(_Ofs& other); //disallow operator=
        _Ofs(const _Ofs& other); //disallow copy ctor

        /* Increases Usage Count */
        void          _incUseCount();
        /* Decreases Usage Count, and deletes instance when 0 is reached */
        void          _decUseCount();
        /* Mounts a file as file system */
        OfsResult     _mount(const char *file, unsigned int op = OFS_MOUNT_OPEN);
        /* Unmount file system */
        void          _unmount(); 

        /* Reads the file system header */
        OfsResult     _readHeader();
        /* Writes the file system header */
        OfsResult     _writeHeader();

        /* Clears the state of file system (during error) */
        inline void   _clear();
        /* Deallocates children of an entry recursively */
        inline void   _deallocateChildren(OfsEntryDesc* parent);
        /* Marks a used block as free (available) */
        inline void   _markUnused(BlockData data);
        /* Allocates a data block for use, either from free blocks or creates a new block and writes given header */
        inline void   _allocateFileBlock(OfsEntryDesc *desc, strMainEntryHeader& mainEntry, unsigned int block_size, unsigned int data_size = 0, const char *data = NULL);
        /* Allocates a data block for use, either from free blocks or creates a new block, writes a short header */
        inline void   _allocateExtendedFileBlock(OfsEntryDesc *desc, unsigned int block_size, unsigned int data_size = 0, const char *data = NULL);

        void          _getFileSystemStatsRecursive(OfsEntryDesc *desc, FileSystemStats& stats);
        /* Retrieves directory descriptor of a given path, null if not found */
        OfsEntryDesc* _getDirectoryDesc(const char *filename);
        /* Retrieves file descriptor of a given file in a given directory, null if not found */
        OfsEntryDesc* _getFileDesc(OfsEntryDesc *dir_desc, std::string filename);
        /* Retrieves filename from a given path */
        std::string   _extractFileName(const char *filename);
        /* Internal createDirectory implementation */
        OfsEntryDesc* _createDirectory(OfsEntryDesc *parent, const std::string& name, const UUID& uuid = UUID_ZERO);
        /* Internal createFile implementation */
        OfsEntryDesc* _createFile(OfsEntryDesc *parent, const std::string& name, unsigned int file_size, const UUID& uuid = UUID_ZERO, unsigned int data_size = 0, const char *data = NULL);
        /* Internal deleteDirectory implementation */
        OfsResult     _deleteDirectory(OfsEntryDesc *dir);
        /* Internal deleteFile implementation */
        OfsResult     _deleteFile(OfsEntryDesc *file);
        /* Internal setFileFlags implementation */
        inline void   _setFileFlags(OfsEntryDesc *file, unsigned int flags);

        /* Searches given filename in the list of file system instances, 
         * returns it if found or creates a new one and adds it to the list otherwise */
        static OfsResult _mount(_Ofs **_ptr, const char *file, unsigned int op = OFS_MOUNT_OPEN);
    };

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
    
    class OfsExport OFSHANDLE
    {
        friend class _Ofs;
    public:

        OFSHANDLE() : mEntryDesc(NULL), mAccessFlags(0), mReadBlock(0), mReadBlockEnd(0), mReadPos(0), mRealReadPos(0), 
                      mWriteBlock(0), mWriteBlockEnd(0), mWritePos(0), mRealWritePos(0) {}
        ~OFSHANDLE() { };

        inline unsigned int getAcessFlags() const { return mAccessFlags; };
        
    private:
        _Ofs::OfsEntryDesc *mEntryDesc;
        unsigned int  mAccessFlags;
        unsigned int  mReadBlock;
        unsigned int  mReadBlockEnd;
        unsigned int  mReadPos;
        unsigned int  mRealReadPos;
        unsigned int  mWriteBlock;
        unsigned int  mWriteBlockEnd;
        unsigned int  mWritePos;
        unsigned int  mRealWritePos;

        OFSHANDLE(_Ofs::OfsEntryDesc *_entryDesc) : mEntryDesc(_entryDesc), mAccessFlags(0), mReadBlock(0), mReadPos(0), mRealReadPos(0), mWriteBlock(0), mWritePos(0), mRealWritePos(0) 
        {
        };

        /**
        * Validates the file handle
        * @return true if file handle is valid
        */
        inline bool _valid() { return mEntryDesc != NULL; };
        
        /**
        * Prepares Read and Write pointers' initial positions
        * @param append pass true to set write pointer to end of file 
        */
        void        _prepareReadWritePointers(bool append);

        /**
        * Sets the Write pointer position
        * @param value New write position
        */
        inline void _setWritePos(unsigned int value);

        /**
        * Sets the Read pointer position
        * @param value New read position
        */
        inline void _setReadPos(unsigned int value);
    };

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    class OfsExport OfsPtr
    {
    public:
        /* Constructor */
        OfsPtr() : mPtr(0) {};
        /* Destructor */
        ~OfsPtr();


        /**
        * Mounts a given OFS file as a virtual file system
        * @param file path of the file to mount
        * @param op Mount type : OFS_MOUNT_OPEN or OFS_MOUNT_CREATE
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    mount(const char *file, unsigned int op = OFS_MOUNT_OPEN);

        /**
        * Releases already mounted file system
        */
        void         unmount(); 

        /**
        * Checks if object has a valid file system pointer
        * @return True if object is holding a mounted file system
        */
        inline bool valid() const
        {
            return (mPtr != 0);
        };

        /* Operator to access internal file system pointer */
        inline _Ofs * operator->() const
        {
            assert(mPtr != 0 && "Null Ofs Pointer");
            return mPtr;
        };

        /* Assignment operator */
        OfsPtr& operator=(OfsPtr& other)
        {
            if(mPtr == other.mPtr)
                return *this;

            if(mPtr != 0)
                mPtr->_decUseCount();

            mPtr = other.mPtr;
            if(mPtr != 0)
                mPtr->_incUseCount();

            return *this;
        }

        /* Consruction operator */
        OfsPtr(const OfsPtr& other)
        {
            mPtr = other.mPtr;
            if(mPtr != 0)
                mPtr->_incUseCount();
        }

    private:
        _Ofs *mPtr; /* Pointer to the actual file system */
    };

//------------------------------------------------------------------------------

    class OfsExport OfsConverter
    {
    public:
        OfsConverter();
        ~OfsConverter();

        bool convert(std::string infile, std::string outfile);

    private:
        
        std::fstream mInStream;       // Handle of file system to be converted
        std::fstream mOutStream;      // Handle of destination file system

        void _deallocateChildren(_Ofs::OfsEntryDesc* parent);
        bool _convertv10_v13(std::string infile, std::string outfile);
        bool _convertv11_v13(std::string infile, std::string outfile);
        bool _convertv12_v13(std::string infile, std::string outfile);
    };

//------------------------------------------------------------------------------

}
