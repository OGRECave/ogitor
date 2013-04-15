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

#include "ofs_base.h"

namespace OFS
{

    class OfsExport _OfsRfs: public _OfsBase
    {
        friend class _OfsBase;
        friend class OfsPtr;
    public:

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
        * Link a File System to a directory
        * @param filename path for file system to link
        * @param directory path to directory to link to
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    linkFileSystem(const char *filename, const char *directory); 
        /**
        * UnLink a File System to a directory
        * @param filename path for file system to unlink
        * @param directory path to directory to unlink from
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    unlinkFileSystem(const char *filename, const char *directory); 
        /**
        * Fetch File System Links of a directory
        * @param directory path for directory to list links
        * @param list Map of Name / OfsPtr pairs
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getDirectoryLinks(const char *directory, NameOfsPtrMap& list);
        /**
        * Rebuild UUID Map by visiting every file in file table
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    rebuildUUIDMap();
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
        * Moves a given directory
        * @param dirname path of directory to move
        * @param newname new name of the directory
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    moveDirectory(const char *dirname, const char *dest);
        /**
        * Moves a given file/directory to recycle-bin
        * @param path source path of file/directory to move
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    moveToRecycleBin(const char *path);
        /**
        * Restores a given file/directory from recycle-bin
        * @param id ID of file/directory to restore, we don't use names 
        *           since there may be multiple items with same name
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    restoreFromRecycleBin(int id);
        /**
        * Deletes all Files in recycle bin and frees the space used
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    emptyRecycleBin();
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
        OfsResult    createFileUUID(OFSHANDLE& handle, const char *filename, const UUID& uuid, ofs64 file_size = 0, unsigned int data_size = 0, const char *data = NULL);
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
        OfsResult    truncateFile(OFSHANDLE& handle, ofs64 file_size = -1);
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
        * @return Total file size of returned files
        */
        ofs64 listFilesRecursive(const std::string& path, FileList& list);
        /**
        * List files/folders in the recycle bin
        * @return List of files matching the filter
        */
        FileList     listRecycleBinFiles();
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
        * Retrieves file size of file
        * @param filename path to the file
        * @param size Returns file's size
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileSize(const char *filename, ofs64& size); 
        /**
        * Retrieves file size of file
        * @param handle handle to the file
        * @param size Returns file's size
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileSize(OFSHANDLE& handle, ofs64& size); 
        /**
        * Sets file flags like read-only/hidden
        * @param filename path to the file
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setFileFlags(const char *filename, unsigned int flags); 
        /**
        * Sets file flags like read-only/hidden
        * @param handle handle to the file
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setFileFlags(OFSHANDLE& handle, unsigned int flags); 
        /**
        * Retrieves file flags like read-only/hidden
        * @param filename path to the file
        * @param flags returns combination of current file flags
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileFlags(const char *filename, unsigned int& flags); 
        /**
        * Retrieves file flags like read-only/hidden
        * @param handle handle to the file
        * @param flags returns combination of current file flags
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    getFileFlags(OFSHANDLE& handle, unsigned int& flags); 
        /**
        * Sets directory flags like read-only/hidden
        * @param dirpath path to the directory
        * @param flags combination of flags to set
        * @return Result of operation, OFS_OK if successful
        */
        OfsResult    setDirFlags(const char *dirpath, unsigned int flags); 
        /**
        * Retrieves directory flags like read-only/hidden
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
        ofs64 seekr(OFSHANDLE& handle, ofs64 pos, SeekDirection dir);
        /**
        * Sets Write Position in a file
        * @param handle handle of the file
        * @param pos relative position to set
        * @param dir direction of seek operation
        * @return Returns final write position
        */
        ofs64 seekw(OFSHANDLE& handle, ofs64 pos, SeekDirection dir);
        /**
        * Retrieves Current Read Position in a file
        * @param handle handle of the file
        * @return Returns current read position
        */
        ofs64 tellr(OFSHANDLE& handle);
        /**
        * Retrieves Current Write Position in a file
        * @param handle handle of the file
        * @return Returns current write position
        */
        ofs64 tellw(OFSHANDLE& handle);
        /**
        * Checks if Read Pointer is at the End Of File
        * @param handle handle of the file
        * @return True if eof reached during read operation
        */
        bool         eof(OFSHANDLE& handle);

    private:
        int mNextAvailableId;

        /* Private Constructor */
        _OfsRfs();
        /* Private Destructor */
        ~_OfsRfs();
        _OfsRfs& operator=(_OfsRfs& other); //disallow operator=
        _OfsRfs(const _OfsRfs& other); //disallow copy ctor

        /* Mounts a file as file system */
        OfsResult     _mount(const char *file, unsigned int op = OFS_MOUNT_OPEN);
        /* Unmount file system */
        void          _unmount(); 

        /* Reads the file system header */
        OfsResult     _readHeader();

        /* Clears the state of file system (during error) */
        inline void   _clear();
        /* Deallocates children of an entry recursively */
        inline void   _deallocateChildren(OfsEntryDesc* parent);

        /* Retrieves file system stats recursively */
        void          _getFileSystemStatsRecursive(OfsEntryDesc *desc, FileSystemStats& stats);
        /* Retrieves directory descriptor of a given path, null if not found */
        OfsEntryDesc* _getDirectoryDesc(const char *filename);
        /* Retrieves file descriptor of a given file in a given directory, null if not found */
        OfsEntryDesc* _getFileDesc(OfsEntryDesc *dir_desc, std::string filename);
        /* Retrieves file descriptor of a given child in a given directory, null if not found */
        OfsEntryDesc* _findChild(OfsEntryDesc *dir_desc, std::string child_name);
        /* Retrieves filename from a given path */
        std::string   _extractFileName(const char *filename);
        /* Internal createDirectory implementation */
        OfsEntryDesc* _createDirectory(OfsEntryDesc *parent, const std::string& name, const UUID& uuid = UUID_ZERO);
        /* Internal createFile implementation */
        OfsEntryDesc* _createFile(OFSHANDLE &handle, OfsEntryDesc *parent, const std::string& name, ofs64 file_size, const UUID& uuid = UUID_ZERO, unsigned int data_size = 0, const char *data = NULL);
        /* Internal deleteDirectory implementation */
        OfsResult     _deleteDirectory(OfsEntryDesc *dir);
        /* Internal deleteFile implementation */
        OfsResult     _deleteFile(OfsEntryDesc *file);
        /* Internal setFileFlags implementation */
        inline void   _setFileFlags(OfsEntryDesc *file, unsigned int flags);
        /* Internal  function to find an entry by id */
        OfsEntryDesc* _findDescById(OfsEntryDesc* base, int id);
    };

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
    
}
