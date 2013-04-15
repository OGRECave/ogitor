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
////
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

#if (defined( __WIN32__ ) || defined( _WIN32 )) && ! defined( __GNUC__ )
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

const char VERSION_MAJOR_0  = '0';
const char VERSION_MAJOR_1  = '1';
const char VERSION_MINOR    = '4';
const char VERSION_FIX      = '0';

const int ROOT_DIRECTORY_ID = -1;
const int RECYCLEBIN_DIRECTORY_ID = -2;


#define AUTO_MUTEX mutable boost::recursive_mutex OfsMutex;
#define LOCK_AUTO_MUTEX boost::recursive_mutex::scoped_lock ofsAutoMutexLock(OfsMutex);
#define STATIC_AUTO_MUTEX_DECL(a) boost::recursive_mutex a::OfsStaticMutex;
#define STATIC_AUTO_MUTEX static boost::recursive_mutex OfsStaticMutex;
#define STATIC_LOCK_AUTO_MUTEX boost::recursive_mutex::scoped_lock ofsAutoMutexLock(OfsStaticMutex);

#if (defined( __WIN32__ ) || defined( _WIN32 )) && ! defined( __GNUC__ )
typedef __int64 ofs64;
#define fseeko _fseeki64
#define ftello _ftelli64
#else
typedef off_t ofs64;
 #if (defined( __WIN32__ ) || defined( _WIN32 ))
 #undef fseeko
 #define fseeko fseek
 #define ftello ftell
 #endif
#endif

    class FileStream
    {
    public:
        FileStream()
        {
            m_pFile = NULL;
        }

        ~FileStream()
        {
            close();
        }

        void close();

        void open( const char *file, const char *mode, int flag = 0 );

        bool fail()
        {
            return ( m_pFile == NULL );
        }

        void flush()
        {
            assert( m_pFile != NULL );
            fflush( m_pFile );
        }

        void seek( ofs64 pos, int seekdir = SEEK_SET )
        {
            assert( m_pFile != NULL );
            fseeko( m_pFile, pos, seekdir );
        }

        ofs64 tell()
        {
            assert( m_pFile != NULL );
            return ftello( m_pFile );
        }

        bool eof()
        {
            assert( m_pFile != NULL );
            return ( feof( m_pFile ) != 0 );
        }

        size_t write( const void *data, size_t size );

        size_t read( void *data, size_t size )
        {
            assert( m_pFile != NULL );
            return fread( data, 1, size, m_pFile );
        }

		void fill( ofs64 len );

    protected:
        FILE *m_pFile;

    };


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
        OFS_MOUNT_RECOVER = 2,
        OFS_MOUNT_LINK = 4
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
        OFS_DIR      = 0x00000001,
        OFS_FILE     = 0x00000002,
        OFS_READONLY = 0x00000004,
        OFS_HIDDEN   = 0x00000008,
        OFS_LINK     = 0x80000000
    };

    enum FileSystemType
    {
        OFS_PACKED = 1,
        OFS_RFS = 2
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

#if (defined( __WIN32__ ) || defined( _WIN32 )) && ! defined( __GNUC__ )
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

#if (defined( __WIN32__ ) || defined( _WIN32 )) && ! defined( __GNUC__ )
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
        int          id;
        std::string  name;
        unsigned int flags;
        UUID         uuid; 
        ofs64        file_size;
        time_t       create_time;
        time_t       modified_time;

        static bool Compare(FileEntry elem1, FileEntry elem2)
        {
#ifdef linux
	return (strcasecmp(elem1.name.c_str(), elem2.name.c_str()) < 0);
#else
	return (_strcmpi(elem1.name.c_str(), elem2.name.c_str()) < 0);
#endif

        };
    };

    typedef std::vector<FileEntry> FileList;

    struct FileSystemStats
    {
        ofs64 NumDirectories;
        ofs64 NumFiles;
        ofs64 UsedAllocations;
        ofs64 FreeAllocations;
        ofs64 UsedSpace;
        ofs64 FreeSpace;
        ofs64 ActualUsedSpace;
        ofs64 ActualFreeSpace;
        ofs64 TotalFileSize;
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
#define OFS_EXCEPT(desc) throw Exception(desc, __FILE__, __LINE__ );
#endif

    class OfsPtr;
    class OFSHANDLE;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    typedef void (*LogCallBackFunction)(std::string);

    class OfsExport _OfsBase
    {
        friend class OfsPtr;
    public:

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


        /* Holds information about an allocated block (for keeping in memory) */
        struct BlockData
        {
            unsigned int Type;        /* Block Type */
            ofs64        Start;       /* Starting position of Block in File, just after header */
            ofs64        Length;      /* Length of the block in file not including file block header */ 
            ofs64        NextBlock;   /* Position of Next Block in file */
        };


        typedef std::map<std::string, OfsPtr> NameOfsPtrMap;

        /* Entry Descriptor, contains all information needed for entry (in memory) */
        struct OfsEntryDesc
        {
            _OfsBase      *Owner;                 /* Owner of the Entry */
            int           Id;                     /* Id of the Owner Entry */
            int           ParentId;               /* Id of the Owner Entry's Parent Directory, -1 if root directory */
            unsigned int  Flags;                  /* File Flags */
            int           OldParentId;            /* Id of the Owner Entry's Parent Directory, -1 if root directory */
            UUID          Uuid;                   /* UUID of Entry */
            time_t        CreationTime;           /* Entry's Creation Time */
            ofs64         FileSize;               /* Entry's File Size, 0 for Directories */
            std::string   Name;                   /* Entry's Name */
            OfsEntryDesc *Parent;                 /* Pointer to Entry's Parent's descriptor */
            int           UseCount;               /* Number of handles using this entry */
            bool          WriteLocked;            /* If true, Entry is not suitable for WRITE operations */
            std::vector<BlockData> UsedBlocks;    /* Vector of BlockData used by this entry */
            std::vector<OfsEntryDesc*> Children;  /* Vector of Entry's children */
            std::vector<CallBackData> Triggers;   /* Vector of Entry's triggers */
            NameOfsPtrMap Links;                  /* Map of of OfsPtr links */
        };

        typedef std::map<int, OfsEntryDesc*> IdDescMap;
        typedef std::map<UUID, OfsEntryDesc*> UuidDescMap;
        typedef std::map<int, OFSHANDLE*> IdHandleMap;


        /**
        * Retrieves Name of the file used by Virtual File System
        * @return Name of the file used by Virtual File System
        */
        const std::string& getFileSystemName() const { return mFileName; };

        /**
        * Retrieves Base Directory of the file used by Virtual File System
        * @return Base Directory of the file used by Virtual File System
        */
        std::string getFileSystemDirectory();

        /**
        * Retrieves Type of the Virtual File System
        * @return Type of the Virtual File System
        */
        FileSystemType getFileSystemType() { return mFileSystemType; }

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
        virtual OfsResult    getFileSystemStats(FileSystemStats& stats) = 0;
        /**
        * Adds a new file system trigger
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to get notified about
        * @param _func Function to call for notification
        * @param _data User data to be passed to notification function
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    addTrigger(void *_owner, CallBackType _type, CallBackFunction _func, void *_data = 0)  = 0;
        /**
        * Removes a file system trigger
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to remove notification about
        */
        virtual void         removeTrigger(void *_owner, CallBackType _type) = 0;
        /**
        * Adds a new file trigger
        * @param filename Name of the file to add trigger for
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to get notified about
        * @param _func Function to call for notification
        * @param _data User data to be passed to notification function
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    addFileTrigger(const char *filename, void *_owner, CallBackType _type, CallBackFunction _func, void *_data = 0) = 0;
        /**
        * Removes a file trigger
        * @param filename Name of the file to remove trigger for
        * @param _owner The owner class or id for the trigger
        * @param _type Type of events to remove notification about
        */
        virtual void         removeFileTrigger(const char *filename, void *_owner, CallBackType _type) = 0;
        /**
        * Makes a defragmented copy of the file system
        * @param dest path of the destination file
        * @param logCallBackFunc method that gets called for each processed OFS component
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    defragFileSystemTo(const char *dest, LogCallBackFunction* logCallbackFunc = NULL) = 0;
        /**
        * Copies the current file system and switches to it
        * @param dest path of the destination file
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    moveFileSystemTo(const char *dest) = 0;
        /**
        * Switches to the new file system
        * @param dest path of the destination file to switch to
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    switchFileSystemTo(const char *dest) = 0;
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
        * Link a File System to a directory
        * @param filename path for file system to link
        * @param directory path to directory to link to
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    linkFileSystem(const char *filename, const char *directory) = 0; 
        /**
        * UnLink a File System to a directory
        * @param filename path for file system to unlink
        * @param directory path to directory to unlink from
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    unlinkFileSystem(const char *filename, const char *directory) = 0; 
        /**
        * Fetch File System Links of a directory
        * @param directory path for directory to list links
        * @param list Map of Name / OfsPtr pairs
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getDirectoryLinks(const char *directory, NameOfsPtrMap& list) = 0;
        /**
        * Rebuild UUID Map by visiting every file in file table
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    rebuildUUIDMap() = 0;
        /**
        * Creates a new directory
        * @param filename path for new directory
        * @param uuid uuid to be assigned to the directory
        * @param force If true, all directories in the path are created
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    createDirectoryUUID(const char *filename, const UUID& uuid, bool force = false) = 0; 
        /**
        * Deletes a given directory
        * @param filename path for directory to delete
        * @param force If true, all directories in the path are deleted
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    deleteDirectory(const char *path, bool force = false) = 0;
        /**
        * Deletes a given file
        * @param filename path for file to delete
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    deleteFile(const char *filename) = 0;
        /**
        * Renames a given file
        * @param filename path for file to rename
        * @param newname new name of the file
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    renameFile(const char *filename, const char *newname) = 0;
        /**
        * Copies a given file
        * @param src source path of file to copy
        * @param dest destination filename
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    copyFile(const char *src, const char *dest) = 0;
        /**
        * Moves a given file to a new location
        * @param src source path of file to move
        * @param dest destination filename
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    moveFile(const char *src, const char *dest) = 0;
        /**
        * Renames a given directory
        * @param dirname path of directory to rename
        * @param newname new name of the directory
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    renameDirectory(const char *dirname, const char *newname) = 0;
        /**
        * Moves a given directory
        * @param dirname path of directory to move
        * @param newname new name of the directory
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    moveDirectory(const char *dirname, const char *dest) = 0;
        /**
        * Moves a given file/directory to recycle-bin
        * @param path source path of file/directory to move
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    moveToRecycleBin(const char *path) = 0;
        /**
        * Restores a given file/directory from recycle-bin
        * @param id ID of file/directory to restore, we don't use names 
        *           since there may be multiple items with same name
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    restoreFromRecycleBin(int id) = 0;
        /**
        * Deletes all Files in recycle bin and frees the space used
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    emptyRecycleBin() = 0;
        /**
        * Creates a new file
        * @param handle File Handle, once the file is created, this handle will point to it
        * @param filename path of file to be created
        * @param file_size initial file size
        * @param data_size size of data passed
        * @param data initial data to fill in the file
        * @return Result of operation, OFS_OK if successful
        */
        inline OfsResult    createFile(OFSHANDLE& handle, const char *filename, ofs64 file_size = 0, unsigned int data_size = 0, const char *data = NULL)
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
        virtual OfsResult    createFileUUID(OFSHANDLE& handle, const char *filename, const UUID& uuid, ofs64 file_size = 0, unsigned int data_size = 0, const char *data = NULL) = 0;
        /**
        * Opens a file
        * @param handle File Handle, once the file is opened, this handle will point to it
        * @param filename path of file to be opened
        * @param open_mode combination of OFS_READ, OFS_WRITE, OFS_APPEND
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    openFile(OFSHANDLE& handle, const char *filename, unsigned int open_mode = OFS_READ) = 0;
        /**
        * Opens a file
        * @param handle File Handle, once the file is opened, this handle will point to it
        * @param uuid UUID of the file to open
        * @param open_mode combination of OFS_READ, OFS_WRITE, OFS_APPEND
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    openFile(OFSHANDLE& handle, const UUID& uuid, unsigned int open_mode = OFS_READ) = 0;
        /**
        * Closes a file
        * @param handle File Handle of the file to be closed
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    closeFile(OFSHANDLE& handle) = 0;
        /**
        * Truncates a file 
        * @param handle File Handle of the file to be truncated
        * @param file_size New file_size of the file, pass -1 to truncate at Write Position, must be less than or equal to original File Size
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    truncateFile(OFSHANDLE& handle, ofs64 file_size = -1) = 0;
        /**
        * List files/folders in a given directory
        * @param path path of the directory
        * @param file_flags Filters the results by given flag
        * @return List of files matching the filter
        */
        virtual FileList     listFiles(const char *path, unsigned int file_flags = OFS_FILE | OFS_DIR) = 0;
        /**
        * List files/folders in a given directory including subfolders, the filenames contain full path
        * @param path path of the directory
        * @param list Filled with results
        * @return Total file size of returned files
        */
        virtual ofs64 listFilesRecursive(const std::string& path, FileList& list) = 0;
        /**
        * List files/folders in the recycle bin
        * @return List of files matching the filter
        */
        virtual FileList     listRecycleBinFiles() = 0;
        /**
        * Retrieves Name of the file
        * @param handle handle to the file
        * @param filename Returns Name of the file pointed by handle
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult getFileName(OFSHANDLE& handle, std::string& filename) = 0;
        /**
        * Retrieves FileEntry structure of directory
        * @param path path to the directory
        * @param entry Returns directory's entry data
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getDirEntry(const char *path, FileEntry& entry) = 0; 
        /**
        * Retrieves FileEntry structure of file
        * @param filename path to the file
        * @param entry Returns file's entry data
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileEntry(const char *filename, FileEntry& entry) = 0; 
        /**
        * Retrieves FileEntry structure of file
        * @param handle handle to the file
        * @param entry Returns file's entry data
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileEntry(OFSHANDLE& handle, FileEntry& entry) = 0; 
        /**
        * Retrieves creation time of file
        * @param filename path to the file
        * @param creation_time Returns file's creation time
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getCreationTime(const char *filename, time_t& creation_time) = 0; 
        /**
        * Retrieves creation time of file
        * @param handle handle to the file
        * @param creation_time Returns file's creation time
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getCreationTime(OFSHANDLE& handle, time_t& creation_time) = 0; 
        /**
        * Retrieves modification time of file
        * @param filename path to the file
        * @param creation_time Returns file's modification time
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getModificationTime(const char *filename, time_t& mod_time) = 0; 
        /**
        * Retrieves modification time of file
        * @param handle handle to the file
        * @param creation_time Returns file's modification time
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getModificationTime(OFSHANDLE& handle, time_t& mod_time) = 0; 
        /**
        * Retrieves file size of file
        * @param filename path to the file
        * @param size Returns file's size
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileSize(const char *filename, ofs64& size) = 0; 
        /**
        * Retrieves file size of file
        * @param handle handle to the file
        * @param size Returns file's size
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileSize(OFSHANDLE& handle, ofs64& size) = 0; 
        /**
        * Sets file flags like read-only/hidden
        * @param filename path to the file
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    setFileFlags(const char *filename, unsigned int flags) = 0; 
        /**
        * Sets file flags like read-only/hidden
        * @param handle handle to the file
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    setFileFlags(OFSHANDLE& handle, unsigned int flags) = 0; 
        /**
        * Retrieves file flags like read-only/hidden
        * @param filename path to the file
        * @param flags returns combination of current file flags
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileFlags(const char *filename, unsigned int& flags) = 0; 
        /**
        * Retrieves file flags like read-only/hidden
        * @param handle handle to the file
        * @param flags returns combination of current file flags
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileFlags(OFSHANDLE& handle, unsigned int& flags) = 0; 
        /**
        * Sets directory flags like read-only/hidden
        * @param dirpath path to the directory
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    setDirFlags(const char *dirpath, unsigned int flags) = 0; 
        /**
        * Retrieves directory flags like read-only/hidden
        * @param dirpath path to the directory
        * @param flags returns combination of current directory flags
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getDirFlags(const char *dirpath, unsigned int& flags) = 0; 
        /**
        * Sets file uuid
        * @param filename path to the file
        * @param uuid new uuid to set
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    setFileUUID(const char *filename, const UUID& uuid) = 0; 
        /**
        * Sets file uuid
        * @param handle handle to the file
        * @param uuid new uuid to set
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    setFileUUID(OFSHANDLE& handle, const UUID& uuid) = 0; 
        /**
        * Retrieves file uuid
        * @param filename path to the file
        * @param uuid returns file's uuid in this parameter
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileUUID(const char *filename, UUID& uuid) = 0; 
        /**
        * Retrieves file uuid
        * @param handle handle to the file
        * @param uuid returns file's uuid in this parameter
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getFileUUID(OFSHANDLE& handle, UUID& uuid) = 0; 
        /**
        * Sets directory uuid
        * @param dirpath path to the directory
        * @param uuid new uuid to set
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    setDirUUID(const char *dirpath, const UUID& uuid) = 0; 
        /**
        * Retrieves directory uuid
        * @param dirpath path to the directory
        * @param uuid returns directory's uuid in this parameter
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    getDirUUID(const char *dirpath, UUID& uuid) = 0; 
        /**
        * Checks if a file exists
        * @param filename path of the file to check
        * @return True if file exists
        */
        virtual bool         exists(const char *filename) = 0;
        /**
        * Reads data from a given file (handle)
        * @param handle handle of the file
        * @param dest Buffer to read data to
        * @param length Length of data to read (in Bytes)
        * @param actual_read LEngth of data actually read
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    read(OFSHANDLE& handle, char *dest, unsigned int length, unsigned int *actual_read = NULL) = 0;
        /**
        * Writes data to a given file (handle)
        * @param handle handle of the file
        * @param src Buffer to write data from
        * @param length Length of data to write (in Bytes)
        * @return Result of operation, OFS_OK if successful
        */
        virtual OfsResult    write(OFSHANDLE& handle, const char *src, unsigned int length) = 0;
        /**
        * Sets Position in a file
        * @param handle handle of the file
        * @param pos relative position to set
        * @param dir direction of seek operation
        * @return Returns final read position
        */
        virtual ofs64 seek(OFSHANDLE& handle, ofs64 pos, SeekDirection dir) = 0;
        /**
        * Retrieves Current Read Position in a file
        * @param handle handle of the file
        * @return Returns current read position
        */
        virtual ofs64 tell(OFSHANDLE& handle) = 0;
        /**
        * Checks if Read Pointer is at the End Of File
        * @param handle handle of the file
        * @return True if eof reached during read operation
        */
        virtual bool         eof(OFSHANDLE& handle) = 0;

        /* Retrieves directory descriptor of a given path, null if not found */
        virtual OfsEntryDesc* _getDirectoryDesc(const char *filename) = 0;
        /* Retrieves file descriptor of a given file in a given directory, null if not found */
        virtual OfsEntryDesc* _getFileDesc(OfsEntryDesc *dir_desc, std::string filename) = 0;
        /* Retrieves file descriptor of a given child in a given directory, null if not found */
        virtual OfsEntryDesc* _findChild(OfsEntryDesc *dir_desc, std::string child_name) = 0;

    protected:
        STATIC_AUTO_MUTEX
        AUTO_MUTEX
        FileSystemType            mFileSystemType;      // Variable defining underlying file system type
        std::string               mFileName;            // Underlying filename of the file system
        bool                      mActive;              // Is a file system mounted?
        OfsEntryDesc              mRootDir;             // Root directory definition
        OfsEntryDesc              mRecycleBinRoot;      // Recycle Bin's Root directory definition
        IdHandleMap               mActiveFiles;         // Map holding currently open files
        UuidDescMap               mUuidMap;             // Map holding entry descriptors indexed with UUID
        int                       mUseCount;            // Number of objects using this file system instance
        bool                      mRecoveryMode;        // Is recovery mode activated?
        bool                      mLinkMode;            // Is link mode activated?
        std::vector<CallBackData> mTriggers;            // Vector of File System Triggers 
        LogCallBackFunction       mLogCallBackFunc;     // Function pointer to callback handler
        


        typedef std::map<std::string, _OfsBase*> NameOfsHandleMap;
        static NameOfsHandleMap   mAllocatedHandles;    // Static map containing all file system instances
        static OfsResult mount(_OfsBase** ptr, const char *file, unsigned int op);

        /* Protected Constructor */
        _OfsBase( FileSystemType type );
        /* Protected Destructor */
        virtual ~_OfsBase();
        _OfsBase& operator=(_OfsBase& other); //disallow operator=
        _OfsBase(const _OfsBase& other); //disallow copy ctor

        /* Increases Usage Count */
        void          _incUseCount();
        /* Decreases Usage Count, and deletes instance when 0 is reached */
        void          _decUseCount();
        /* Mounts a file as file system */
        virtual OfsResult     _mount(const char *file, unsigned int op = OFS_MOUNT_OPEN) = 0;
        /* Unmount file system */
        virtual void          _unmount() = 0; 

    };

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
    class _Ofs;
    class _OfsRfs;

    class OfsExport OFSHANDLE
    {
        friend class _OfsBase;
        friend class _Ofs;
        friend class _OfsRfs;
    public:

        OFSHANDLE() : mEntryDesc(NULL), mAccessFlags(0), mBlock(0), mBlockEnd(0), mPos(0), mRealPos(0) {}
        ~OFSHANDLE() { };

        inline unsigned int getAcessFlags() const { return mAccessFlags; };
        
        /**
        * Validates the file handle
        * @return true if file handle is valid
        */
        inline bool _valid() { return mEntryDesc != NULL; };

    protected:
        _OfsBase::OfsEntryDesc *mEntryDesc;
        FileStream    mStream;
        unsigned int  mAccessFlags;
        unsigned int  mBlock;

        ofs64         mBlockEnd;
        ofs64         mPos;
        ofs64         mRealPos;

        OFSHANDLE(_OfsBase::OfsEntryDesc *_entryDesc) : mEntryDesc(_entryDesc), mAccessFlags(0), mBlock(0), mPos(0), mRealPos(0)
        {
        };

        /**
        * Prepares Read and Write pointers' initial positions
        * @param append pass true to set write pointer to end of file 
        */
        void  _preparePointers(bool append);

        /**
        * Sets the pointer position
        * @param value New write position
        */
        void _setPos(ofs64 value);

    private:

        /* Assignment operator */
        OFSHANDLE& operator=(OFSHANDLE& other)
        {
            assert(0 && "OFSHANDLE MUST NOT BE COPIED");
            return *this;
        }

        /* Construction operator */
        OFSHANDLE(const OFSHANDLE& other)
        {
            assert(0 && "OFSHANDLE MUST NOT BE COPIED");
        }
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
        ~OfsPtr()
        {
            unmount();
        }

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
        inline _OfsBase * operator->() const
        {
            assert(mPtr != 0 && "Null Ofs Pointer");
            return mPtr;
        };

        /* Assignment operator */
        OfsPtr& operator=(OfsPtr& other);

        /* Construction operator */
        OfsPtr(const OfsPtr& other)
        {
            mPtr = other.mPtr;
            if(mPtr != 0)
                mPtr->_incUseCount();
        }

    private:
        _OfsBase *mPtr; /* Pointer to the actual file system */
    };

}
