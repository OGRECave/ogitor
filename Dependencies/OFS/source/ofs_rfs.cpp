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

#include "ofs_rfs.h"
#include "file_ops.h"
#include <algorithm>
#include <stdio.h>


using namespace std;


namespace OFS
{

    std::string constructFullPath( _OfsBase::OfsEntryDesc *desc )
    {
        std::string path = desc->Name;

        while( desc->Id != ROOT_DIRECTORY_ID )
        {
            path = desc->Parent->Name + "/" + path;

            desc = desc->Parent;
        }

        return path;
    }
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    _OfsRfs::_OfsRfs() : _OfsBase(OFS_RFS), mNextAvailableId(1)
    {
    }

//------------------------------------------------------------------------------

    _OfsRfs::~_OfsRfs()
    {
    }

//------------------------------------------------------------------------------
    
    OfsResult _OfsRfs::rebuildUUIDMap()
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::rebuildUUIDMap, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        mUuidMap.clear();
        
        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::linkFileSystem(const char *filename, const char *directory)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::linkFileSystem, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        std::string dest_dir = directory;
        if(dest_dir[dest_dir.length() - 1] != '/')
            dest_dir += "/";

        OfsEntryDesc *dirDesc = _getDirectoryDesc(dest_dir.c_str());

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;

        if(dirDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        NameOfsPtrMap::iterator it = dirDesc->Links.find( filename );

        if( it == dirDesc->Links.end() )
        {
            OfsPtr _ofsptr;
            OfsResult ret;

            std::string name = filename;

            int pos = name.find("::");
            std::string fileSystemName = name;
            
            if( pos > -1 )
            {
                fileSystemName = name.substr(0, pos);
                name.erase(0, pos + 2);
                if(name[name.length() - 1] != '/')
                    name += "/";
            }
            else
                name = "/";
        
            ret = _ofsptr.mount( fileSystemName.c_str(), OFS_MOUNT_OPEN | OFS_MOUNT_LINK );

            if( ret != OFS_OK )
                return OFS_INVALID_FILE;


            OfsEntryDesc *foreignDir = _ofsptr->_getDirectoryDesc(name.c_str());

            if(foreignDir == NULL)
            {
                _ofsptr.unmount();
                return OFS_INVALID_PATH;
            }

            std::vector<OfsEntryDesc*> add_list;

            for( unsigned int i = 0; i < foreignDir->Children.size(); i++ )
            {
                if(_findChild(dirDesc, foreignDir->Children[i]->Name ) == NULL )
                    add_list.push_back( foreignDir->Children[i] );
            }

            dirDesc->Children.insert( dirDesc->Children.end(), add_list.begin(), add_list.end() );

            dirDesc->Links.insert( NameOfsPtrMap::value_type( filename, _ofsptr ) );
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::unlinkFileSystem(const char *filename, const char *directory)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::unlinkFileSystem, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(directory);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;

        if(dirDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        NameOfsPtrMap::iterator it = dirDesc->Links.find( filename );

        if( it != dirDesc->Links.end() )
        {
            std::string name = filename;

            int pos = name.find("::");

            if( pos > -1 )
            {
                name.erase(0, pos + 2);
                if(name[name.length() - 1] != '/')
                    name += "/";
            }
            else
                name = "/";

            OfsEntryDesc *foreignDir = (it->second)->_getDirectoryDesc(name.c_str());

            for( unsigned int i = 0; i < foreignDir->Children.size(); i++ )
            {
                for( unsigned int k = 0; k < dirDesc->Children.size(); k++ )
                {
                    if( dirDesc->Children[k] == foreignDir->Children[i] )
                    {
                        dirDesc->Children.erase( dirDesc->Children.begin() + k );
                        break;
                    }
                }
            }

            dirDesc->Links.erase( it );
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getDirectoryLinks(const char *directory, NameOfsPtrMap& list)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getDirectoryLinks, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(directory);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;

        list = dirDesc->Links;

        return OFS_OK;
    }

//------------------------------------------------------------------------------
    
    void _OfsRfs::_getFileSystemStatsRecursive(OfsEntryDesc *desc, FileSystemStats& stats)
    {
        if(desc->Flags & OFS_LINK)
            return;
        else if(desc->Flags & OFS_FILE)
            stats.NumFiles++;
        else
        {
            stats.NumDirectories++;

            for(unsigned int c = 0;c < desc->Children.size();c++)
                _getFileSystemStatsRecursive(desc->Children[c], stats);
        }

        stats.UsedSpace += desc->FileSize;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileSystemStats(FileSystemStats& stats)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileSystemStats, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        memset(&stats, 0, sizeof(FileSystemStats));

        _getFileSystemStatsRecursive(&mRootDir, stats);

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    void _OfsRfs::_deallocateChildren(OfsEntryDesc* parent)
    {
        for(unsigned int i = 0;i < parent->Children.size();i++)
        {
            if( parent->Children[i]->Owner == this )
            {
                _deallocateChildren(parent->Children[i]);
                delete parent->Children[i];
            }
        }

        parent->Children.clear();
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::_mount(const char *file, unsigned int op)
    {
        LOCK_AUTO_MUTEX

        if(mActive)
        {
            OFS_EXCEPT("_OfsRfs::mount, Operation called on an already mounted file system.");
            return OFS_IO_ERROR;
        }
        
        mFileName = file;

        if((mFileName[mFileName.size() - 1] == '/') || (mFileName[mFileName.size() - 1] == '/'))
            mFileName.erase(mFileName.size() - 1, 1);

        OfsResult ret = OFS_INVALID_FILE;

        if(op & OFS_MOUNT_RECOVER)
            mRecoveryMode = true;
        
        if(op & OFS_MOUNT_LINK)
            mLinkMode = true;

        if(op & OFS_MOUNT_OPEN)
        {
            if( OfsDirectoryExists(file) )
            {
                mActive = true;
                ret = _readHeader();
            }
        }
        else
        {
            if(OfsCreateDirectory(file))
            {
                mActive = true;
                ret = _readHeader();
            }
        }

        if(ret != OFS_OK)
        {
            _clear();
        }
        
        return ret;
    }

//------------------------------------------------------------------------------

    void _OfsRfs::_unmount()
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::unmount, Operation called on an already unmounted file system.");
            return;
        }

        _clear();
    }

//------------------------------------------------------------------------------

    void _OfsRfs::_clear()
    {
        mActive = false;

        mFileName = "";
        mRootDir.UsedBlocks.clear();
        _deallocateChildren(&mRootDir);
        _deallocateChildren(&mRecycleBinRoot);

        mActiveFiles.clear();
        mUuidMap.clear();
        mTriggers.clear();
        mRecoveryMode = false;
        mLinkMode = false;

        mNextAvailableId = 1;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::_readHeader()
    {
        assert(mActive);

        OfsListContents( this, &mRootDir, mNextAvailableId, mFileName.c_str(), mLinkMode);

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    _OfsRfs::OfsEntryDesc* _OfsRfs::_getDirectoryDesc(const char *filename)
    {
        if(*filename == '/')
            ++filename;

        std::string dir;
        std::string tmp = filename;
        const char *pos = filename;
        const char *name_st  = filename;

        OfsEntryDesc *curDesc = &mRootDir;

        while(*filename != 0)
        {
            if(*filename == '/')
            {
                if(filename > pos)
                {
                    dir = tmp.substr(pos - name_st, filename - pos);

                    OfsEntryDesc *tmpDesc = NULL;

                    for(unsigned int i = 0;i < curDesc->Children.size();i++)
                    {
                        if((curDesc->Children[i]->Flags & OFS_DIR) && curDesc->Children[i]->Name == dir)
                        {
                            tmpDesc = curDesc->Children[i];
                            break;
                        }
                    }

                    if(tmpDesc == NULL)
                        return NULL;
                    else
                        curDesc = tmpDesc;
                }

                pos = (filename + 1);
            }

            ++filename;
        }

        if(filename > pos)
        {
            dir = tmp.substr(pos - name_st, filename - pos);

            OfsEntryDesc *tmpDesc = NULL;

            for(unsigned int i = 0;i < curDesc->Children.size();i++)
            {
                if((curDesc->Children[i]->Flags & OFS_DIR) && curDesc->Children[i]->Name == dir)
                {
                    tmpDesc = curDesc->Children[i];
                    break;
                }
            }

            if(tmpDesc != NULL)
                curDesc = tmpDesc;
        }

        return curDesc;
    }

//------------------------------------------------------------------------------

    _OfsRfs::OfsEntryDesc* _OfsRfs::_getFileDesc(OfsEntryDesc *dir_desc, std::string filename)
    {
        for(unsigned int i = 0;i < dir_desc->Children.size();i++)
        {
            if((dir_desc->Children[i]->Flags & OFS_FILE) && dir_desc->Children[i]->Name == filename)
            {
                return dir_desc->Children[i];
            }
        }
        return NULL;
    }

//------------------------------------------------------------------------------
    
    _OfsRfs::OfsEntryDesc* _OfsRfs::_findChild(OfsEntryDesc *dir_desc, std::string child_name)
    {
        for(unsigned int i = 0;i < dir_desc->Children.size();i++)
        {
            if(dir_desc->Children[i]->Name == child_name)
            {
                return dir_desc->Children[i];
            }
        }
        return NULL;
    }

//------------------------------------------------------------------------------

    std::string   _OfsRfs::_extractFileName(const char *filename)
    {
        const char *pos = filename;
        const char *str = filename;

        while(*str != 0)
        {
            if(*str == '/')
                pos = (str + 1);

            ++str;
        }

        return std::string(pos);
    }

//------------------------------------------------------------------------------

    _OfsRfs::OfsEntryDesc* _OfsRfs::_createDirectory(OfsEntryDesc *parent, const std::string& name, const UUID& uuid)
    {
        assert(parent != NULL);

        for(unsigned int i = 0; i < parent->Children.size();i++)
        {
            if(parent->Children[i]->Name == name)
                return parent->Children[i];
        }

        OfsEntryDesc *dir = new OfsEntryDesc();

        if(uuid != UUID_ZERO)
        {
            UuidDescMap::const_iterator it = mUuidMap.find(uuid);

            if(it != mUuidMap.end())
            {
                delete dir;

                OFS_EXCEPT("_OfsRfs::_createDirectory, The UUID supplied already exists.");

                return NULL;
            }
            else
            {
                mUuidMap.insert(UuidDescMap::value_type(uuid, dir));
            }
        }


        dir->Owner = this;
        dir->Id = mNextAvailableId++;
        dir->ParentId = parent->Id;
        dir->Flags = OFS_DIR;
        dir->OldParentId = ROOT_DIRECTORY_ID;
        dir->Name = name;
        dir->FileSize = 0;
        dir->Parent = parent;
        dir->CreationTime = time( NULL );
        dir->UseCount = 0;
        dir->WriteLocked = false;
        dir->Uuid = uuid;

        parent->Children.push_back(dir);

        std::string full_path = mFileName + constructFullPath( dir );

        OfsCreateDirectory( full_path.c_str() );

        return dir;
    }

//------------------------------------------------------------------------------

    OFSHANDLE _OfsRfs::_createFile(OfsEntryDesc *parent, const std::string& name, ofs64 file_size, const UUID& uuid, unsigned int data_size, const char *data)
    {
        assert(parent != NULL);
        
        OFSHANDLE result;
        result.mEntryDesc = NULL;
        
        OfsEntryDesc *file = new OfsEntryDesc();

        file->Owner = this;
        file->Id = mNextAvailableId++;
        file->ParentId = parent->Id;
        file->Flags = OFS_FILE;
        file->OldParentId = ROOT_DIRECTORY_ID;
        file->Name = name;
        file->FileSize = file_size;
        file->Parent = parent;
        file->CreationTime = time( NULL );
        file->UseCount = 0;
        file->WriteLocked = false;
        file->Uuid = uuid;

        parent->Children.push_back(file);

        if(uuid != UUID_ZERO)
        {
            UuidDescMap::const_iterator it = mUuidMap.find(uuid);

            if(it != mUuidMap.end())
            {
                delete file;

                OFS_EXCEPT("_OfsRfs::_createFile, The UUID supplied already exists.");

                return result;
            }
            else
            {
                mUuidMap.insert(UuidDescMap::value_type(uuid, file));
            }
        }

        std::string full_path = mFileName + constructFullPath( file );

        result.mEntryDesc = file;
        result.mStream.open(full_path.c_str(), "wb+");
        result.mStream.write(data, data_size);

        return result;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::deleteDirectory(const char *path, bool force)
    {
        assert(path != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::deleteDirectory, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        int slen = strlen(path);

        if(slen == 0)
            return OFS_INVALID_PATH;

        std::string dir_path = path;
        if(path[slen - 1] != '/')
            dir_path += "/";
        
        OfsEntryDesc *dirDesc = _getDirectoryDesc(dir_path.c_str());

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;

        if(dirDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        if(dirDesc->Children.size() > 0 && !force)
            return OFS_DIRECTORY_NOT_EMPTY;

        if(dirDesc->Parent != NULL)
        {
            NameOfsPtrMap::iterator it = dirDesc->Links.begin();

            while( it != dirDesc->Links.end() )
            {
                std::string name = it->first;

                int pos = name.find("::");

                if( pos > -1 )
                {
                    name.erase(0, pos + 2);
                    if(name[name.length() - 1] == '/')
                        name.erase(name.length() - 1, 1);
                }
                else
                    name = "/";

                OfsEntryDesc *foreignDir = (it->second)->_getDirectoryDesc(name.c_str());

                for( unsigned int i = 0; i < foreignDir->Children.size(); i++ )
                {
                    for( unsigned int k = 0; k < dirDesc->Children.size(); k++ )
                    {
                        if( dirDesc->Children[k] == foreignDir->Children[i] )
                        {
                            dirDesc->Children.erase( dirDesc->Children.begin() + k );
                            break;
                        }
                    }
                }

                it++;
            }

            OfsResult ret = _deleteDirectory(dirDesc);

            std::string fName = _extractFileName(path);
            OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);
            for(unsigned int i = 0;i < mTriggers.size();i++)
            {
                if(mTriggers[i].type == CLBK_DELETE)
                {
                    mTriggers[i].func(mTriggers[i].data, fileDesc, 0);
                }
            }

            return ret;
        }
        else
            return OFS_INVALID_PATH;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::_deleteDirectory(OfsEntryDesc *dir)
    {
        assert(dir != NULL);

        if(dir->Flags & OFS_READONLY)
        {
            return OFS_ACCESS_DENIED;
        }

        std::string full_path = mFileName + constructFullPath( dir );


        OfsEntryDesc *curDesc = NULL;
        OfsResult ret;

        while(dir->Children.size() > 0)
        {
            curDesc = dir->Children[0];
            if(curDesc->Flags & OFS_DIR)
            {
                ret = _deleteDirectory(curDesc);
                if(ret != OFS_OK)
                    return ret;
            }
            else if(curDesc->Flags & OFS_FILE)
            {
                ret = _deleteFile(curDesc);
                if(ret != OFS_OK)
                    return ret;
            }
        }

        for(unsigned int i = 0;i < dir->Parent->Children.size();i++)
        {
            if(dir->Parent->Children[i]->Id == dir->Id)
            {
                dir->Parent->Children.erase(dir->Parent->Children.begin() + i);
                break;
            }
        }

        if( dir->Uuid != UUID_ZERO )
        {
            UuidDescMap::iterator uit = mUuidMap.find(dir->Uuid);

            if( uit != mUuidMap.end() )
               mUuidMap.erase( uit );
        }

        delete dir;

        OfsDeleteDirectory( full_path.c_str() );

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::deleteFile(const char *filename)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::deleteFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        if(fileDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        std::vector<_OfsRfs::CallBackData> saveTrigs = fileDesc->Triggers;

        OfsResult ret = _deleteFile(fileDesc);

        if(ret == OFS_OK)
        {
            for(unsigned int i = 0;i < saveTrigs.size();i++)
            {
                if(saveTrigs[i].type == CLBK_DELETE)
                {
                    saveTrigs[i].func(saveTrigs[i].data, 0, filename);
                }
            }

            for(unsigned int i = 0;i < mTriggers.size();i++)
            {
                if(mTriggers[i].type == CLBK_DELETE)
                {
                    mTriggers[i].func(mTriggers[i].data, 0, filename);
                }
            }
        }

        return ret;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::_deleteFile(OfsEntryDesc *file)
    {
        assert(file != NULL);

        IdHandleMap::const_iterator it = mActiveFiles.find(file->Id);

        if(it != mActiveFiles.end() || (file->Flags & OFS_READONLY))
        {
            return OFS_ACCESS_DENIED;
        }

        std::string full_path = mFileName + constructFullPath( file );

        
        for(unsigned int i = 0;i < file->Parent->Children.size();i++)
        {
            if(file->Parent->Children[i]->Id == file->Id)
            {
                file->Parent->Children.erase(file->Parent->Children.begin() + i);
                break;
            }
        }

        if( file->Uuid != UUID_ZERO )
        {
            UuidDescMap::iterator uit = mUuidMap.find(file->Uuid);

            if( uit != mUuidMap.end() )
               mUuidMap.erase( uit );
        }

        delete file;

        OfsDeleteFile( full_path.c_str() );

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::createDirectoryUUID(const char *filename, const UUID& uuid, bool force)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::createDirectory, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(*filename == '/')
            ++filename;

        const char *end_pos = filename + strlen(filename);
        
        if(end_pos == filename)
            return OFS_INVALID_PATH;

        --end_pos;

        if(*end_pos != '/')
            ++end_pos;

        std::string dir;
        std::string tmp = filename;
        const char *pos = filename;
        const char *name_st  = filename;

        OfsEntryDesc *curDesc = &mRootDir;

        while(filename < end_pos)
        {
            if(*filename == '/')
            {
                dir = tmp.substr(pos - name_st, filename - pos);

                OfsEntryDesc *tmpDesc = NULL;
                for(unsigned int i = 0;i < curDesc->Children.size();i++)
                {
                    if((curDesc->Children[i]->Flags & OFS_DIR) && curDesc->Children[i]->Name == dir)
                    {
                        tmpDesc = curDesc->Children[i];
                        break;
                    }
                }

                if(tmpDesc == NULL)
                {
                    if(curDesc->Flags & OFS_LINK)
                        return OFS_ACCESS_DENIED;

                    if(force)
                        curDesc = _createDirectory(curDesc, dir);
                    else
                        return OFS_INVALID_PATH;
                }
                else
                    curDesc = tmpDesc;

                pos = (filename + 1);
            }

            ++filename;
        }

        dir = tmp.substr(pos - name_st, end_pos - pos);

        if(dir.length() > 0)
        {
            if(curDesc->Flags & OFS_LINK)
                return OFS_ACCESS_DENIED;

            _createDirectory(curDesc, dir, uuid);
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::renameFile(const char *filename, const char *newname)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::renameFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        if(fileDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        std::string nName = _extractFileName(newname);

        int sz = nName.length();
        if(sz > 251)
            nName.erase(251, sz - 251);

        if(_getFileDesc(dirDesc, nName) != NULL || fileDesc->WriteLocked || (fileDesc->Flags & OFS_READONLY))
            return OFS_ACCESS_DENIED;

        std::string full_path1 = mFileName + constructFullPath( fileDesc );
        
        fileDesc->Name = nName;

        std::string full_path2 = mFileName + constructFullPath( fileDesc );

        OfsRenameFile( full_path1.c_str(), full_path2.c_str() );
        
        for(unsigned int i = 0;i < fileDesc->Triggers.size();i++)
        {
            if(fileDesc->Triggers[i].type == CLBK_RENAME)
            {
                fileDesc->Triggers[i].func(fileDesc->Triggers[i].data, fileDesc, filename);
            }
        }

        for(unsigned int i = 0;i < mTriggers.size();i++)
        {
            if(mTriggers[i].type == CLBK_RENAME)
            {
                mTriggers[i].func(mTriggers[i].data, fileDesc, filename);
            }
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::renameDirectory(const char *dirname, const char *newname)
    {
        assert(dirname != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::renameDirectory, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        int slen = strlen(dirname);

        if(slen == 0)
            return OFS_INVALID_PATH;

        std::string dir_path = dirname;
        if(dirname[slen - 1] != '/')
            dir_path += "/";
        
        OfsEntryDesc *dirDesc = _getDirectoryDesc(dir_path.c_str());

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;

        if(dirDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        if(dirDesc->Parent != NULL)
        {
            std::string nName = newname;
            int sz = nName.length() - 1;

            if(nName.find_last_of("/") == sz)
                nName.erase(sz, 1);

            nName = _extractFileName(nName.c_str());

            sz = nName.length();

            if(sz > 251)
                nName.erase(251, sz - 251);

            for(unsigned int i = 0;i < dirDesc->Parent->Children.size();i++)
            {
                if(dirDesc->Parent->Children[i]->Name == nName)
                    return OFS_ACCESS_DENIED;
            }

            if(dirDesc->WriteLocked || (dirDesc->Flags & OFS_READONLY))
                return OFS_ACCESS_DENIED;

            std::string full_path1 = mFileName + constructFullPath( dirDesc );
        
            dirDesc->Name = nName;

            std::string full_path2 = mFileName + constructFullPath( dirDesc );

            OfsRenameFile( full_path1.c_str(), full_path2.c_str() );

            return OFS_OK;
        }
        else
            return OFS_INVALID_PATH;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::openFile(OFSHANDLE& handle, const char *filename, unsigned int open_mode)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::openFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
        {
            return OFS_FILE_NOT_FOUND;
        }
        else
        {
            if(fileDesc->WriteLocked)
                return OFS_ACCESS_DENIED;

            std::string open_path = mFileName + "/";
            open_path += filename;

            if(open_mode & OFS_WRITE)
            {
                if((fileDesc->UseCount > 0) || (fileDesc->Flags & OFS_READONLY) || (fileDesc->Flags & OFS_LINK))
                    return OFS_ACCESS_DENIED;

                if(open_mode & OFS_APPEND)
                {
                    handle.mStream.open(open_path.c_str(), "ab+");
                    if( handle.mStream.fail() )
                        return OFS_ACCESS_DENIED;
                }
                else
                {
                    handle.mStream.open(open_path.c_str(), "wb+");
                    if( handle.mStream.fail() )
                        return OFS_ACCESS_DENIED;
                    fileDesc->FileSize = 0;
                }

                fileDesc->WriteLocked = true;
            }
            else
            {
                handle.mStream.open(open_path.c_str(), "rb+");
                if( handle.mStream.fail() )
                    return OFS_ACCESS_DENIED;
            }
        } 

        if(fileDesc->UseCount == 0)
            mActiveFiles.insert(IdHandleMap::value_type(fileDesc->Id, &handle));

        fileDesc->UseCount++;
        handle.mEntryDesc = fileDesc;
        handle.mAccessFlags = open_mode;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::openFile(OFSHANDLE& handle, const UUID& uuid, unsigned int open_mode)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::openFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        UuidDescMap::const_iterator it = mUuidMap.find(uuid);
        if(it == mUuidMap.end())
            return OFS_FILE_NOT_FOUND;

        if(!(it->second->Flags & OFS_FILE))
            return OFS_FILE_NOT_FOUND;

        OfsEntryDesc *fileDesc = it->second;

        if(fileDesc->WriteLocked)
            return OFS_ACCESS_DENIED;

        std::string open_path = mFileName + constructFullPath( fileDesc );

        if(open_mode & OFS_WRITE)
        {
            if((fileDesc->UseCount > 0) || (fileDesc->Flags & OFS_READONLY) || (fileDesc->Flags & OFS_LINK))
                return OFS_ACCESS_DENIED;

            if(open_mode & OFS_APPEND)
            {
                handle.mStream.open(open_path.c_str(), "ab+");
                if( handle.mStream.fail() )
                    return OFS_ACCESS_DENIED;
            }
            else
            {
                handle.mStream.open(open_path.c_str(), "wb+");
                if( handle.mStream.fail() )
                    return OFS_ACCESS_DENIED;
                fileDesc->FileSize = 0;
            }

            fileDesc->WriteLocked = true;
        }
        else
        {
            handle.mStream.open(open_path.c_str(), "rb+");
            if( handle.mStream.fail() )
                return OFS_ACCESS_DENIED;
        }

        if(fileDesc->UseCount == 0)
            mActiveFiles.insert(IdHandleMap::value_type(fileDesc->Id, &handle));

        fileDesc->UseCount++;
        handle.mEntryDesc = fileDesc;
        handle.mAccessFlags = open_mode;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::createFileUUID(OFSHANDLE& handle, const char *filename, const UUID& uuid, ofs64 file_size, unsigned int data_size, const char *data)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::openFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;

        if(dirDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
        {
            if(uuid != UUID_ZERO)
            {
                UuidDescMap::const_iterator it = mUuidMap.find(uuid);

                if(it != mUuidMap.end())
                {
                    OFS_EXCEPT("_OfsRfs::createFileUUID, The UUID supplied already exists.");
    
                    return OFS_INVALID_UUID;
                }
            }

            handle = _createFile(dirDesc, fName, file_size, uuid, data_size, data);
            fileDesc = handle.mEntryDesc;


            if(fileDesc != NULL)
            {
                mActiveFiles.insert(IdHandleMap::value_type(fileDesc->Id, &handle));
                fileDesc->UseCount++;
            }
        }
        else 
        {
            OfsResult ret = openFile(handle, filename, OFS_READWRITE);
            if(ret == OFS_OK)
            {
                if(handle.mEntryDesc->Uuid != uuid)
                    setFileUUID(handle, uuid);

                if(data != NULL)
                    return write(handle, data, data_size);
            }
            else
                return ret;
        }

        if(fileDesc == NULL)
            return OFS_IO_ERROR;
                
        fileDesc->WriteLocked = true;

        handle.mEntryDesc = fileDesc;
        handle.mAccessFlags = OFS_READWRITE;

        for(unsigned int i = 0;i < mTriggers.size();i++)
        {
            if(mTriggers[i].type == CLBK_CREATE)
            {
                mTriggers[i].func(mTriggers[i].data, fileDesc, 0);
            }
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::closeFile(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::closeFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::closeFile, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if(handle.mEntryDesc->Owner != this)
            return handle.mEntryDesc->Owner->closeFile(handle);

        IdHandleMap::iterator it = mActiveFiles.find(handle.mEntryDesc->Id);

        if(it != mActiveFiles.end())
        {
            handle.mEntryDesc->UseCount--; 
            if(handle.mEntryDesc->UseCount == 0)
            {
                handle.mEntryDesc->WriteLocked = false;
                mActiveFiles.erase(it);
            }
        }

        handle.mStream.close();

        if(handle.mAccessFlags & OFS_WRITE)
        {
            for(unsigned int i = 0;i < handle.mEntryDesc->Triggers.size();i++)
            {
                if(handle.mEntryDesc->Triggers[i].type == CLBK_CONTENT)
                {
                    handle.mEntryDesc->Triggers[i].func(handle.mEntryDesc->Triggers[i].data, handle.mEntryDesc, 0);
                }
            }

            for(unsigned int i = 0;i < mTriggers.size();i++)
            {
                if(mTriggers[i].type == CLBK_CONTENT)
                {
                    mTriggers[i].func(mTriggers[i].data, handle.mEntryDesc, 0);
                }
            }
        }

        handle.mEntryDesc = NULL;
        handle.mAccessFlags = 0;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::truncateFile(OFSHANDLE& handle, ofs64 file_size)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::closeFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::truncateFile, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if(!(handle.mAccessFlags & OFS_WRITE) || (handle.mAccessFlags & OFS_LINK))
            return OFS_ACCESS_DENIED;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    void _OfsRfs::_setFileFlags(OfsEntryDesc *file, unsigned int flags)
    {
        assert(file != NULL);

        flags &= (OFS_READONLY | OFS_HIDDEN);
        file->Flags = (file->Flags & (OFS_FILE | OFS_DIR)) | flags;

//IMPLEMENT HERE

        if(file->Flags & OFS_DIR)
        {
            for(unsigned int i = 0;i < file->Children.size();i++)
            {
                _setFileFlags(file->Children[i], flags);
            }
        }
    }

//------------------------------------------------------------------------------

    FileList _OfsRfs::listFiles(const char *path, unsigned int file_flags)
    {
        assert(path != NULL);

        LOCK_AUTO_MUTEX

        FileList output;

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::listFiles, Operation called on an unmounted file system.");
            return output;
        }

        int slen = strlen(path);

        std::string dir_path = path;
        
        if(slen > 0)
        {
            if(path[slen - 1] != '/')
                dir_path += "/";
        }
        else
            dir_path += "/";


        OfsEntryDesc *dirDesc = _getDirectoryDesc(dir_path.c_str());

        if(dirDesc == NULL)
            return output;

        FileEntry entry;

        for(unsigned int i = 0;i < dirDesc->Children.size();i++)
        {
            if(dirDesc->Children[i]->Flags & file_flags)
            {
                entry.id = dirDesc->Children[i]->Id;
                entry.name = dirDesc->Children[i]->Name;
                entry.flags = dirDesc->Children[i]->Flags;
                entry.uuid = dirDesc->Children[i]->Uuid;
                entry.file_size = dirDesc->Children[i]->FileSize;
                entry.create_time = dirDesc->Children[i]->CreationTime;
                entry.modified_time = dirDesc->Children[i]->CreationTime;

                output.push_back(entry);
            }
        }

        return output;
    }

//------------------------------------------------------------------------------

    FileList _OfsRfs::listRecycleBinFiles()
    {
        LOCK_AUTO_MUTEX

        FileList output;

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::listRecycleBinFiles, Operation called on an unmounted file system.");
            return output;
        }

        FileEntry entry;

        for(unsigned int i = 0;i < mRecycleBinRoot.Children.size();i++)
        {
            entry.id = mRecycleBinRoot.Children[i]->Id;
            entry.name = mRecycleBinRoot.Children[i]->Name;
            entry.flags = mRecycleBinRoot.Children[i]->Flags;
            entry.uuid = mRecycleBinRoot.Children[i]->Uuid;
            entry.file_size = mRecycleBinRoot.Children[i]->FileSize;
            entry.create_time = mRecycleBinRoot.Children[i]->CreationTime;
            entry.modified_time = mRecycleBinRoot.Children[i]->CreationTime;

            output.push_back(entry);
        }

        return output;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::read(OFSHANDLE& handle, char *dest, unsigned int length, unsigned int *actual_read)
    {
        assert(dest != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::read, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::read, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if(handle.mEntryDesc->Owner != this)
            return handle.mEntryDesc->Owner->read(handle, dest, length, actual_read);

        if(!(handle.mAccessFlags & OFS_READ))
            return OFS_ACCESS_DENIED;

        OfsEntryDesc *desc = handle.mEntryDesc;

        length = handle.mStream.read(dest, length);

        if(actual_read != NULL)
            *actual_read = length;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::write(OFSHANDLE& handle, const char *src, unsigned int length)
    {
        assert(src != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::write, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::write, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if(!(handle.mAccessFlags & OFS_WRITE))
            return OFS_ACCESS_DENIED;

        OfsEntryDesc *desc = handle.mEntryDesc;

        length = handle.mStream.write(src, length);

        ofs64 pos = handle.mStream.tell();

        if( pos < handle.mEntryDesc->FileSize )
            handle.mEntryDesc->FileSize = pos;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    ofs64 _OfsRfs::seekr(OFSHANDLE& handle, ofs64 pos, SeekDirection dir)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::seekr, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::seekr, Supplied OfsHandle is not valid.");
            return 0;
        }

        switch(dir)
        {
        case OFS_SEEK_BEGIN:handle.mStream.seek( pos, SEEK_SET);
                            break;
        case OFS_SEEK_CURRENT:handle.mStream.seek( pos, SEEK_CUR);
                              break;
        case OFS_SEEK_END:handle.mStream.seek( pos, SEEK_END);
                          break;
        }

        return handle.mStream.tell();
    }

//------------------------------------------------------------------------------

    ofs64 _OfsRfs::seekw(OFSHANDLE& handle, ofs64 pos, SeekDirection dir)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::seekw, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::seekw, Supplied OfsHandle is not valid.");
            return 0;
        }

        switch(dir)
        {
        case OFS_SEEK_BEGIN:handle.mStream.seek( pos, SEEK_SET);
                            break;
        case OFS_SEEK_CURRENT:handle.mStream.seek( pos, SEEK_CUR);
                              break;
        case OFS_SEEK_END:handle.mStream.seek( pos, SEEK_END);
                          break;
        }

        return handle.mStream.tell();
    }

//------------------------------------------------------------------------------

    ofs64 _OfsRfs::tellr(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::tellr, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::tellr, Supplied OfsHandle is not valid.");
            return 0;
        }

        return handle.mStream.tell();
    }

//------------------------------------------------------------------------------

    ofs64 _OfsRfs::tellw(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::tellw, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::tellw, Supplied OfsHandle is not valid.");
            return 0;
        }

        return handle.mStream.tell();
    }

//------------------------------------------------------------------------------

    bool _OfsRfs::eof(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::eof, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::eof, Supplied OfsHandle is not valid.");
            return 0;
        }

        return handle.mStream.eof();
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileName(OFSHANDLE& handle, std::string& filename)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileName, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::getFileName, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        filename = handle.mEntryDesc->Name;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getDirEntry(const char *path, FileEntry& entry)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getDirEntry, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(path);

        if(dirDesc != NULL)
        {
            entry.id = dirDesc->Id;
            entry.name = dirDesc->Name;
            entry.flags = dirDesc->Flags;
            entry.uuid = dirDesc->Uuid;
            entry.file_size = dirDesc->FileSize;
            entry.create_time = dirDesc->CreationTime;
            entry.modified_time = dirDesc->CreationTime;
            
            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileEntry(const char *filename, FileEntry& entry)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileEntry, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc != NULL)
        {
            entry.id = fileDesc->Id;
            entry.name = fileDesc->Name;
            entry.flags = fileDesc->Flags;
            entry.uuid = fileDesc->Uuid;
            entry.file_size = fileDesc->FileSize;
            entry.create_time = fileDesc->CreationTime;
            entry.modified_time = fileDesc->CreationTime;
            
            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileEntry(OFSHANDLE& handle, FileEntry& entry)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileEntry, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::getFileEntry, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        entry.id = handle.mEntryDesc->Id;
        entry.name = handle.mEntryDesc->Name;
        entry.flags = handle.mEntryDesc->Flags;
        entry.uuid = handle.mEntryDesc->Uuid;
        entry.file_size = handle.mEntryDesc->FileSize;
        entry.create_time = handle.mEntryDesc->CreationTime;
        entry.modified_time = handle.mEntryDesc->CreationTime;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getCreationTime(const char *filename, time_t& creation_time)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getCreationTime, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc != NULL)
        {
            creation_time = fileDesc->CreationTime;
            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getModificationTime(const char *filename, time_t& mod_time)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getModificationTime, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc != NULL)
        {
            mod_time = fileDesc->CreationTime;
            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileSize(const char *filename, ofs64& size)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileSize, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc != NULL)
        {
            size = fileDesc->FileSize;
            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::setFileFlags(const char *filename, unsigned int flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::setFileFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
            return OFS_FILE_NOT_FOUND;
        else if( fileDesc->Flags & OFS_LINK )
            return OFS_ACCESS_DENIED;
        else
        {
            if(fileDesc->WriteLocked)
                return OFS_ACCESS_DENIED;

            _setFileFlags(fileDesc, flags);
            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::setFileFlags(OFSHANDLE& handle, unsigned int flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::setFileFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::setFileFlags, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if( handle.mEntryDesc->Flags & OFS_LINK )
            return OFS_ACCESS_DENIED;

        _setFileFlags(handle.mEntryDesc, flags);

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileFlags(const char *filename, unsigned int& flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc != NULL)
        {
            flags = fileDesc->Flags;

            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileFlags(OFSHANDLE& handle, unsigned int& flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::getFileFlags, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        flags = handle.mEntryDesc->Flags;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::setFileUUID(const char *filename, const UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::setFileUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
            return OFS_FILE_NOT_FOUND;
        else if( fileDesc->Flags & OFS_LINK )
            return OFS_ACCESS_DENIED;
        else
        {
            if(fileDesc->Uuid != uuid)
            {
                if(fileDesc->WriteLocked)
                    return OFS_ACCESS_DENIED;

                if(fileDesc->Uuid != UUID_ZERO)
                {
                    UuidDescMap::iterator it = mUuidMap.find(fileDesc->Uuid);

                    assert(it != mUuidMap.end());

                    mUuidMap.erase(it);
                }

                if(uuid != UUID_ZERO)
                    mUuidMap.insert(UuidDescMap::value_type(uuid, fileDesc));

                fileDesc->Uuid = uuid;
            }
            
            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::setFileUUID(OFSHANDLE& handle, const UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::setFileUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::setFileUUID, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if( handle.mEntryDesc->Flags & OFS_LINK )
            return OFS_ACCESS_DENIED;

        if(handle.mEntryDesc->Uuid != uuid)
        {
            if(handle.mEntryDesc->Uuid != UUID_ZERO)
            {
                UuidDescMap::iterator it = mUuidMap.find(handle.mEntryDesc->Uuid);

                assert(it != mUuidMap.end());

                mUuidMap.erase(it);
            }

            if(uuid != UUID_ZERO)
                mUuidMap.insert(UuidDescMap::value_type(uuid, handle.mEntryDesc));

            handle.mEntryDesc->Uuid = uuid;
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileUUID(const char *filename, UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc != NULL)
        {
            uuid = fileDesc->Uuid;

            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileUUID(OFSHANDLE& handle, UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::getFileUUID, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        uuid = handle.mEntryDesc->Uuid;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::setDirUUID(const char *dirpath, const UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::setDirUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(dirpath);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;
        else if( dirDesc->Flags & OFS_LINK )
            return OFS_ACCESS_DENIED;
        else
        {
            if(dirDesc->Uuid != uuid)
            {
                if(dirDesc->Uuid != UUID_ZERO)
                {
                    UuidDescMap::iterator it = mUuidMap.find(dirDesc->Uuid);

                    assert(it != mUuidMap.end());

                    mUuidMap.erase(it);
                }

                if(uuid != UUID_ZERO)
                    mUuidMap.insert(UuidDescMap::value_type(uuid, dirDesc));

                dirDesc->Uuid = uuid;
            }
            
            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getDirUUID(const char *dirpath, UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getDirUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(dirpath);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;
        else
        {
            uuid = dirDesc->Uuid;

            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::setDirFlags(const char *dirpath, unsigned int flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::setDirFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(dirpath);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;
        else if( dirDesc->Flags & OFS_LINK )
            return OFS_ACCESS_DENIED;
        else
        {
            _setFileFlags(dirDesc, flags);

            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getDirFlags(const char *dirpath, unsigned int& flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getDirFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(dirpath);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;
        else
        {
            flags = dirDesc->Flags;

            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getCreationTime(OFSHANDLE& handle, time_t& creation_time)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getCreationTime, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::getCreationTime, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        creation_time = handle.mEntryDesc->CreationTime;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getModificationTime(OFSHANDLE& handle, time_t& mod_time) 
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getModificationTime, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::getModificationTime, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        mod_time = handle.mEntryDesc->CreationTime;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::getFileSize(OFSHANDLE& handle, ofs64& size)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::getFileSize, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_OfsRfs::getFileSize, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        size = handle.mEntryDesc->FileSize;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    bool _OfsRfs::exists(const char *filename)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::exists, Operation called on an unmounted file system.");
            return false;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return false;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        return (fileDesc != NULL);
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::copyFile(const char *src, const char *dest)
    {
        LOCK_AUTO_MUTEX

        OFSHANDLE srcHandle, destHandle;
        OfsResult ret = OFS_OK;

        if((ret = openFile(srcHandle, src)) != OFS_OK)
            return ret;

        ofs64 file_size = 0;

        getFileSize(srcHandle, file_size);

        if(file_size < 1024 * 1024 * 64)
        {
            UUID uuid;

            char *buffer = new char[(unsigned int)file_size];
            read(srcHandle, buffer, (unsigned int)file_size);
            closeFile(srcHandle);

            ret = createFileUUID(destHandle, dest, UUID_ZERO, file_size, (unsigned int)file_size, buffer);
            
            delete [] buffer;

            if(ret == OFS_OK)
                closeFile(destHandle);
            
            return ret;
        }

        closeFile(srcHandle);
        return OFS_IO_ERROR;
    }

//------------------------------------------------------------------------------------------

    OfsResult _OfsRfs::moveFile(const char *src, const char *dest)
    {
        LOCK_AUTO_MUTEX

        OFSHANDLE srcHandle;
        OfsResult ret = OFS_OK;

        OfsEntryDesc *dirDesc = _getDirectoryDesc(dest);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;            

        if((ret = openFile(srcHandle, src)) != OFS_OK)
            return OFS_INVALID_FILE;

        OfsEntryDesc *srcDesc = srcHandle.mEntryDesc;

        if( srcDesc->Flags & OFS_READONLY || srcDesc->Flags & OFS_LINK )
        {
            closeFile(srcHandle);
            return OFS_ACCESS_DENIED;
        }

        closeFile(srcHandle);

        ret = deleteFile(dest);

        if(ret == OFS_OK || ret == OFS_FILE_NOT_FOUND)
        {
            ret = copyFile(src, dest);
            if( ret == OFS_OK )
                ret = deleteFile(src);
        }

        return ret;
    }

//------------------------------------------------------------------------------

    OfsResult _OfsRfs::moveDirectory(const char *dirname, const char *dest)
    {
        LOCK_AUTO_MUTEX

        OFSHANDLE srcHandle;
        OfsResult ret = OFS_OK;

        /* call this before createDirectory because we might be moving
        to a new directory within the same directory */
        FileList allFiles = listFiles(dirname);

        OfsEntryDesc *dirDesc = _getDirectoryDesc(dest);

        if(dirDesc == NULL)
        {
            createDirectory(dest, true);
            dirDesc = _getDirectoryDesc(dest);
        }

        if(dirDesc == NULL)
        {
            return OFS_INVALID_PATH;            
        }

        for(unsigned int i = 0;i < allFiles.size();i++)
        {
            std::string currentLoc = dirname;
            if(dirname[strlen(dirname) - 1] != '/')
                currentLoc += "/";
            currentLoc += allFiles[i].name;

            std::string destLoc = dest;
            if(dirname[strlen(dest) - 1] != '/')
                destLoc += "/";
            destLoc += allFiles[i].name;

            if(allFiles[i].flags & OFS_DIR)
            {
                ret = moveDirectory(currentLoc.c_str(), destLoc.c_str());
            } 
            else 
            {
                ret = moveFile(currentLoc.c_str(), destLoc.c_str());
            }

            if (ret != OFS_OK)
                return ret;
        }
        
        /* Delete the directory only if it is empty as it might contain our new folder */
        deleteDirectory(dirname, false);

        return ret;
    }

//------------------------------------------------------------------------------------------
    
    OfsResult _OfsRfs::moveToRecycleBin(const char *path)
    {
        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    _OfsRfs::OfsEntryDesc* _OfsRfs::_findDescById( _OfsRfs::OfsEntryDesc* base, int id )
    {
        std::vector<OfsEntryDesc*> list;
        OfsEntryDesc* desc;

        for( unsigned int i = 0; i < base->Children.size(); i++)
        {
            desc = base->Children[i];
            if( (desc->Flags & OFS_DIR) && !(desc->Flags & OFS_LINK))
            {
                if( desc->Id == id  )
                    return desc;
                else
                    list.push_back( desc );
            }
        }

        for( unsigned int i = 0; i < list.size(); i++)
        {
            desc = _findDescById( base->Children[i], id );
            if( desc != NULL ) 
                return desc;
        }

        return NULL;
    }

//------------------------------------------------------------------------------------------

    OfsResult _OfsRfs::restoreFromRecycleBin(int id)
    {
        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    OfsResult _OfsRfs::emptyRecycleBin()
    {
        _deallocateChildren(&mRecycleBinRoot);

        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    OfsResult _OfsRfs::moveFileSystemTo(const char *dest)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::moveFileSystemTo, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsResult ret = OFS_INVALID_FILE;

        {
            STATIC_LOCK_AUTO_MUTEX
        
            NameOfsHandleMap::iterator it = mAllocatedHandles.find(std::string(dest));

            if(it != mAllocatedHandles.end())
                return ret;
        }

        FileList allFiles;

        listFilesRecursive("/", allFiles);

        std::sort(allFiles.begin(), allFiles.end(), FileEntry::Compare);

        OfsPtr destFile;
        ret = destFile.mount(dest, OFS_MOUNT_CREATE);

        if(ret == OFS_OK)
        {
            OFSHANDLE out_handle;
            OFSHANDLE in_handle;

            ofs64 output_amount = 0;

            char *tmp_buffer = new char[MAX_BUFFER_SIZE];

            for(unsigned int i = 0;i < allFiles.size();i++)
            {
                if(allFiles[i].flags & OFS_LINK)
                    continue;

                if(allFiles[i].flags & OFS_DIR)
                {
                    destFile->createDirectoryUUID(allFiles[i].name.c_str(), allFiles[i].uuid);
                    destFile->setDirFlags(allFiles[i].name.c_str(), allFiles[i].flags);
                }
                else
                {
                    std::string file_ofs_path = allFiles[i].name;

                    try
                    {
                        OfsResult ret = openFile(in_handle, file_ofs_path.c_str());
                        if(ret != OFS_OK)
                            continue;

                        ofs64 total = allFiles[i].file_size;

                        bool dest_file_created = false;

                        while(total > 0)
                        {
                            if(total < MAX_BUFFER_SIZE)
                            {
                                read(in_handle, tmp_buffer, (unsigned int)total);
                                if(dest_file_created)
                                    destFile->write(out_handle, tmp_buffer, (unsigned int)total);
                                else
                                {
                                    destFile->createFileUUID(out_handle, file_ofs_path.c_str(), allFiles[i].uuid, total, (unsigned int)total, tmp_buffer);
                                    destFile->setFileFlags(out_handle, allFiles[i].flags);
                                }

                                output_amount += total;
                                total = 0;
                            }
                            else
                            {
                                read(in_handle, tmp_buffer, MAX_BUFFER_SIZE);
                                if(dest_file_created)
                                    destFile->write(out_handle, tmp_buffer, MAX_BUFFER_SIZE);
                                else
                                {
                                    destFile->createFileUUID(out_handle, file_ofs_path.c_str(), allFiles[i].uuid, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, tmp_buffer);
                                    destFile->setFileFlags(out_handle, allFiles[i].flags);
                                    dest_file_created = true;
                                }

                                total -= MAX_BUFFER_SIZE;
                                output_amount += MAX_BUFFER_SIZE;
                            }
                        }
                    }
                    catch(...)
                    {
                    }

                    if(out_handle._valid())
                        destFile->closeFile(out_handle);
                    if(in_handle._valid())
                        closeFile(in_handle);
                }
            }

            delete [] tmp_buffer;

            ret = switchFileSystemTo(dest);
        }
        
        return ret;
    }

//------------------------------------------------------------------------------------------

    OfsResult _OfsRfs::switchFileSystemTo(const char *dest)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::switchFileSystemTo, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsResult ret = OFS_INVALID_FILE;

        {
            STATIC_LOCK_AUTO_MUTEX

            NameOfsHandleMap::iterator it = mAllocatedHandles.find(std::string(dest));

            if(it != mAllocatedHandles.end())
                return ret;
        }

        if( OfsDirectoryExists(dest) )
        {
            STATIC_LOCK_AUTO_MUTEX
            mFileName = dest;

            if((mFileName[mFileName.size() - 1] == '/') || (mFileName[mFileName.size() - 1] == '/'))
                mFileName.erase(mFileName.size() - 1, 1);

             mAllocatedHandles.erase(mAllocatedHandles.find(mFileName));
             mActive = true;
             ret = OFS_OK;
             mAllocatedHandles.insert(NameOfsHandleMap::value_type(std::string(dest), this));
        }
        else
        {
             mActive = false;
             mFileName = "";
             ret = OFS_INVALID_FILE;
        }
        
        return ret;
    }

//------------------------------------------------------------------------------------------

    OfsResult _OfsRfs::defragFileSystemTo(const char *dest, LogCallBackFunction* logCallbackFunc)
    {
        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    ofs64 _OfsRfs::listFilesRecursive(const std::string& path, FileList& list)
    {
        if(list.empty())
            list = listFiles(path.c_str());

        unsigned int list_max = list.size();
        ofs64 file_size = 0;
        
        FileList subList;


        for(unsigned int i = 0;i < list_max;i++)
        {
            list[i].name = path + list[i].name;
            file_size += list[i].file_size;

            if(list[i].flags & OFS_DIR)
            {
                std::string newpath = list[i].name + "/";

                subList = listFiles(newpath.c_str());

                if(subList.size() > 0)
                {
                    file_size += listFilesRecursive(newpath, subList);

                    for(unsigned int z = 0;z < subList.size();z++)
                        list.push_back(subList[z]);
                }
            }
        }

        return file_size;
    }

//------------------------------------------------------------------------------------------
    OfsResult _OfsRfs::addTrigger(void *_owner, _OfsRfs::CallBackType _type, _OfsRfs::CallBackFunction _func, void *_data)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::addTrigger, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        _OfsRfs::CallBackData tdata;
        tdata.owner = _owner;
        tdata.type = _type;
        tdata.func = _func;
        tdata.data = _data;

        mTriggers.push_back(tdata);

        return OFS_OK;
    }
//------------------------------------------------------------------------------------------
    void _OfsRfs::removeTrigger(void *_owner, _OfsRfs::CallBackType _type)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::removeTrigger, Operation called on an unmounted file system.");
            return;
        }

        for(unsigned int i = 0;i < mTriggers.size();i++)
        {
            if(mTriggers[i].owner == _owner && mTriggers[i].type == _type)
            {
                mTriggers.erase(mTriggers.begin() + i);
                return;
            }
        }

        assert("removeTrigger: The trigger could not be found!!");
    }
//------------------------------------------------------------------------------------------
    OfsResult _OfsRfs::addFileTrigger(const char *filename, void *_owner, _OfsRfs::CallBackType _type, _OfsRfs::CallBackFunction _func, void *_data)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::addFileTrigger, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
            return OFS_FILE_NOT_FOUND;
        
        _OfsRfs::CallBackData tdata;
        tdata.owner = _owner;
        tdata.type = _type;
        tdata.func = _func;
        tdata.data = _data;

        fileDesc->Triggers.push_back(tdata);

        return OFS_OK;
    }
//------------------------------------------------------------------------------------------
    void _OfsRfs::removeFileTrigger(const char *filename, void *_owner, _OfsRfs::CallBackType _type)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_OfsRfs::removeFileTrigger, Operation called on an unmounted file system.");
            return;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
            return;

        for(unsigned int i = 0;i < fileDesc->Triggers.size();i++)
        {
            if(fileDesc->Triggers[i].owner == _owner && fileDesc->Triggers[i].type == _type)
            {
                fileDesc->Triggers.erase(fileDesc->Triggers.begin() + i);
                return;
            }
        }

        assert("removeFileTrigger: The trigger could not be found!!");
    }
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

}
