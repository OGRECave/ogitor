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

#include "ofs14.h"
#include <algorithm>
#include <stdio.h>

using namespace std;

#if (defined( __WIN32__ ) || defined( _WIN32 )) && ! defined( __GNUC__ )
#define OPEN_STREAM(a, b, c) a.open(b, c, SH_DENYWR)
#else
#define OPEN_STREAM(a, b, c) a.open(b, c)
#endif


namespace OFS
{
//------------------------------------------------------------------------------

    bool BlockCompare ( _OfsBase::BlockData elem1, _OfsBase::BlockData elem2 )
    {
        if(elem1.Length < elem2.Length)
            return true;
        else if(elem1.Length == elem2.Length)
            return (elem1.Start < elem2.Start);

        return false;
    }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    _Ofs::_Ofs() : _OfsBase(OFS_PACKED)
    {
    }

//------------------------------------------------------------------------------

    _Ofs::~_Ofs()
    {
    }

//------------------------------------------------------------------------------
    
    OfsResult _Ofs::_rebuildUUIDMapRecursive(OfsEntryDesc *current)
    {
        if(current->Uuid != UUID_ZERO)
        {
             assert(mUuidMap.find(current->Uuid) == mUuidMap.end());
             mUuidMap.insert(UuidDescMap::value_type(current->Uuid, current));
        }

        for( unsigned int i = 0; i < current->Children.size(); i++ )
        {
            _rebuildUUIDMapRecursive( current->Children[i] );
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::rebuildUUIDMap()
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::rebuildUUIDMap, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        mUuidMap.clear();
        
        return _rebuildUUIDMapRecursive( &mRootDir );
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::linkFileSystem(const char *filename, const char *directory)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::linkFileSystem, Operation called on an already unmounted file system.");
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

    OfsResult _Ofs::unlinkFileSystem(const char *filename, const char *directory)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::unlinkFileSystem, Operation called on an already unmounted file system.");
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

    OfsResult _Ofs::getDirectoryLinks(const char *directory, NameOfsPtrMap& list)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getDirectoryLinks, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(directory);

        if(dirDesc == NULL)
            return OFS_INVALID_PATH;

        list = dirDesc->Links;

        return OFS_OK;
    }

//------------------------------------------------------------------------------
    
    void _Ofs::_getFileSystemStatsRecursive(OfsEntryDesc *desc, FileSystemStats& stats)
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

        ofs64 total_alloc = 0;
        
        for(unsigned int i = 0;i < desc->UsedBlocks.size();i++)
        {
            stats.ActualUsedSpace += desc->UsedBlocks[i].Length + sizeof(strBlockHeader);

            if(i == 0)
                total_alloc += desc->UsedBlocks[i].Length - sizeof(strMainEntryHeader);
            else
                total_alloc += desc->UsedBlocks[i].Length - sizeof(strExtendedEntryHeader);

            stats.UsedAllocations++;
        }

        stats.ActualFreeSpace += (total_alloc - desc->FileSize);
        stats.FreeSpace += (total_alloc - desc->FileSize);
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getFileSystemStats(FileSystemStats& stats)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileSystemStats, Operation called on an already unmounted file system.");
            return OFS_IO_ERROR;
        }

        memset(&stats, 0, sizeof(FileSystemStats));

        _getFileSystemStatsRecursive(&mRootDir, stats);

        stats.FreeAllocations = mFreeBlocks.size();

        for(unsigned int i = 0;i < mFreeBlocks.size();i++)
        {
            stats.FreeSpace += mFreeBlocks[i].Length + sizeof(strBlockHeader);
            stats.ActualFreeSpace += mFreeBlocks[i].Length;
        }

        mStream.seek(0, OFS_SEEK_END);

        stats.ActualUsedSpace += sizeof(strFileHeader);
        stats.TotalFileSize = mStream.tell();

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    void _Ofs::_deallocateChildren(OfsEntryDesc* parent)
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

    void _Ofs::_markUnused(BlockData data)
    {
        data.Type |= OFS_FREE_BLOCK;
        data.NextBlock = 0;
        mStream.seek(data.Start - sizeof(strBlockHeader) + offsetof(strBlockHeader, Type), OFS_SEEK_BEGIN);
        mStream.write((char*)&(data.Type), sizeof(unsigned int));
        mFreeBlocks.push_back(data);
    }

//------------------------------------------------------------------------------

    void _Ofs::_allocateFileBlock(OfsEntryDesc *desc, strMainEntryHeader& mainEntry, ofs64 block_size, unsigned int data_size, const char *data)
    {
        BlockData blockData;
        strBlockHeader blHeader;

        blHeader.Signature[0] = mHeader.BLOCK_HEADER_SIG[0];
        blHeader.Signature[1] = mHeader.BLOCK_HEADER_SIG[1];

        bool fill_needed = false;
        int alloc_pos = -1;

        for(unsigned int i = 0;i < mFreeBlocks.size();i++)
        {
            if(mFreeBlocks[i].Length >= block_size)
            {
                alloc_pos = i;
                break;
            }
        }

        if(alloc_pos != -1)
        {
           blockData = mFreeBlocks[alloc_pos];
           blockData.Type = OFS_MAIN_BLOCK;
           blockData.NextBlock = 0;
           
           if(blockData.Length >= (1024 + sizeof(strBlockHeader) + block_size))
           {
               mFreeBlocks[alloc_pos].Start += sizeof(strBlockHeader) + block_size;
               mFreeBlocks[alloc_pos].Length -= sizeof(strBlockHeader) + block_size;
               blockData.Length = block_size;

               blHeader.Type = OFS_MAIN_BLOCK;
               blHeader.Length = block_size;
               mStream.seek(blockData.Start - sizeof(strBlockHeader), OFS_SEEK_BEGIN);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&mainEntry, sizeof(strMainEntryHeader));
               
               blHeader.Type = OFS_FREE_BLOCK;
               blHeader.Length = mFreeBlocks[alloc_pos].Length;
               mStream.seek(block_size - sizeof(strMainEntryHeader), OFS_SEEK_CURRENT);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));

               desc->UsedBlocks.push_back(blockData);

               if(alloc_pos != 0)
                   std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);
           }
           else
           {
               mFreeBlocks.erase(mFreeBlocks.begin() + alloc_pos);
               blHeader.Type = OFS_MAIN_BLOCK;
               blHeader.Length = blockData.Length;
               mStream.seek(blockData.Start - sizeof(strBlockHeader), OFS_SEEK_BEGIN);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&mainEntry, sizeof(strMainEntryHeader));
               desc->UsedBlocks.push_back(blockData);
           }
        }
        else
        {
            mStream.seek(0, OFS_SEEK_END);
            blockData.Type = OFS_MAIN_BLOCK;
            blockData.NextBlock = 0;
            blockData.Length = block_size;
            blockData.Start = mStream.tell();
            blockData.Start += sizeof(strBlockHeader);
            
            blHeader.Length = blockData.Length;
            blHeader.Type = OFS_MAIN_BLOCK;
            
            mStream.write((char*)&blHeader, sizeof(strBlockHeader));
            mStream.write((char*)&mainEntry, sizeof(strMainEntryHeader));
            fill_needed = true;

            desc->UsedBlocks.push_back(blockData);
        }

        mStream.seek(blockData.Start + sizeof(strMainEntryHeader), OFS_SEEK_BEGIN);

        block_size -= sizeof(strMainEntryHeader); 
        
        if(data != NULL)
        {
            assert(block_size >= data_size);

            mStream.write(data, data_size);
            if(block_size > 0)
                block_size -= data_size; 
        }

        if(fill_needed && block_size > 0)
        {
            mStream.fill( block_size );
        }

        _writeHeader();
    }

//------------------------------------------------------------------------------

    void _Ofs::_allocateExtendedFileBlock(OfsEntryDesc *desc, ofs64 block_size, unsigned int data_size, const char *data)
    {
        strExtendedEntryHeader extendedHeader;
        BlockData blockData;
        strBlockHeader blHeader;

        extendedHeader.Id = desc->Id;
        extendedHeader.ParentId = desc->ParentId;
        extendedHeader.NextBlock = 0;
        extendedHeader.Index = desc->UsedBlocks.size();

        blHeader.Signature[0] = mHeader.BLOCK_HEADER_SIG[0];
        blHeader.Signature[1] = mHeader.BLOCK_HEADER_SIG[1];

        bool fill_needed = false;
        int alloc_pos = -1;

        for(unsigned int i = 0;i < mFreeBlocks.size();i++)
        {
            if(mFreeBlocks[i].Length >= block_size)
            {
                alloc_pos = i;
                break;
            }
        }

        if(alloc_pos != -1)
        {
           blockData = mFreeBlocks[alloc_pos];
           blockData.Type = OFS_EXTENDED_BLOCK;
           blockData.NextBlock = 0;
           
           if(blockData.Length >= (1024 + sizeof(strBlockHeader) + block_size))
           {
               mFreeBlocks[alloc_pos].Start += sizeof(strBlockHeader) + block_size;
               mFreeBlocks[alloc_pos].Length -= sizeof(strBlockHeader) + block_size;
               blockData.Length = block_size;

               blHeader.Type = OFS_EXTENDED_BLOCK;
               blHeader.Length = block_size;
               mStream.seek(blockData.Start - sizeof(strBlockHeader), OFS_SEEK_BEGIN);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&extendedHeader, sizeof(strExtendedEntryHeader));
               
               blHeader.Type = OFS_FREE_BLOCK;
               blHeader.Length = mFreeBlocks[alloc_pos].Length;
               mStream.seek(block_size - sizeof(strExtendedEntryHeader), OFS_SEEK_CURRENT);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));

               desc->UsedBlocks.push_back(blockData);

               if(alloc_pos != 0)
                   std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);
           }
           else
           {
               mFreeBlocks.erase(mFreeBlocks.begin() + alloc_pos);
               blHeader.Type = OFS_EXTENDED_BLOCK;
               blHeader.Length = blockData.Length;
               mStream.seek(blockData.Start - sizeof(strBlockHeader), OFS_SEEK_BEGIN);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&extendedHeader, sizeof(strExtendedEntryHeader));
               desc->UsedBlocks.push_back(blockData);
           }
        }
        else
        {
            mStream.seek(0, OFS_SEEK_END);
            blockData.Type = OFS_EXTENDED_BLOCK;
            blockData.NextBlock = 0;
            blockData.Length = block_size;
            blockData.Start = mStream.tell();
            blockData.Start += sizeof(strBlockHeader);
            
            blHeader.Length = blockData.Length;
            blHeader.Type = OFS_EXTENDED_BLOCK;
            
            mStream.write((char*)&blHeader, sizeof(strBlockHeader));
            mStream.write((char*)&extendedHeader, sizeof(strExtendedEntryHeader));

            desc->UsedBlocks.push_back(blockData);

            fill_needed = true;
        }

        unsigned int prev_block = desc->UsedBlocks.size() - 2;
        
        desc->UsedBlocks[prev_block].NextBlock = blockData.Start;
        
        if(prev_block == 0)
            mStream.seek(desc->UsedBlocks[prev_block].Start + offsetof(strMainEntryHeader, NextBlock), OFS_SEEK_BEGIN);
        else
            mStream.seek(desc->UsedBlocks[prev_block].Start + offsetof(strExtendedEntryHeader, NextBlock), OFS_SEEK_BEGIN);

        mStream.write((char*)&(desc->UsedBlocks[prev_block].NextBlock), sizeof(ofs64));

        mStream.seek(blockData.Start + sizeof(strExtendedEntryHeader), OFS_SEEK_BEGIN);

        block_size -= sizeof(strExtendedEntryHeader); 
        
        if(data != NULL)
        {
            assert(block_size >= data_size);

            mStream.write(data, data_size);
            if(block_size > 0)
                block_size -= data_size; 
        }

        if(fill_needed && block_size > 0)
        {
            mStream.fill( block_size );
        }

        _writeHeader();
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::_mount(const char *file, unsigned int op)
    {
        LOCK_AUTO_MUTEX

        if(mActive)
        {
            OFS_EXCEPT("_Ofs::mount, Operation called on an already mounted file system.");
            return OFS_IO_ERROR;
        }
        
        mFileName = file;

        OfsResult ret = OFS_INVALID_FILE;

        if(op & OFS_MOUNT_RECOVER)
            mRecoveryMode = true;
        
        if(op & OFS_MOUNT_LINK)
            mLinkMode = true;

        if(op & OFS_MOUNT_OPEN)
        {
            OPEN_STREAM(mStream, mFileName.c_str(), "rb+");
            if(!mStream.fail())
            {
                mActive = true;
                ret = _readHeader();
            }
        }
        else
        {
            OPEN_STREAM(mStream, mFileName.c_str(), "wb+");
            if(!mStream.fail())
            {
                mActive = true;
                memset(&mHeader, 0, sizeof(_Ofs::strFileHeader));

                time_t signature_time = time(NULL);
                unsigned int * signature = (unsigned int *)(&signature_time);

                mHeader.ID[0] = 'O';mHeader.ID[1] = 'F';mHeader.ID[2] = 'S';mHeader.ID[3] = '1';
                
                mHeader.VERSION[0] = VERSION_MAJOR_0;
                mHeader.VERSION[1] = VERSION_MAJOR_1;
                mHeader.VERSION[2] = VERSION_MINOR;
                mHeader.VERSION[3] = VERSION_FIX;
                mHeader.BLOCK_HEADER_SIG[0] = signature[0];
                mHeader.BLOCK_HEADER_SIG[1] = signature[0] ^ 0xFFFFFFFF;

                mHeader.LAST_ID = 0;
                mRootDir.CreationTime = time( NULL );
                ret = _writeHeader();
            }
        }

        if(ret != OFS_OK)
        {
            _clear();
        }
        
        return ret;
    }

//------------------------------------------------------------------------------

    void _Ofs::_unmount()
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::unmount, Operation called on an already unmounted file system.");
            return;
        }

        _clear();
    }

//------------------------------------------------------------------------------

    void _Ofs::_clear()
    {
        mActive = false;

        if(!mStream.fail())
        {
            mStream.flush();
            mStream.close();
        }

        memset(&mHeader, 0, sizeof(strFileHeader));

        mFileName = "";
        mRootDir.UsedBlocks.clear();
        _deallocateChildren(&mRootDir);
        _deallocateChildren(&mRecycleBinRoot);

        mFreeBlocks.clear();
        mActiveFiles.clear();
        mUuidMap.clear();
        mTriggers.clear();
        mRecoveryMode = false;
        mLinkMode = false;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::_readHeader()
    {
        assert(mActive);

        mStream.seek(0, OFS_SEEK_END);
        ofs64 file_size = mStream.tell();

        int HeaderSize = sizeof(strFileHeader);
        mStream.seek(0, OFS_SEEK_BEGIN);
        mStream.read((char *)&mHeader, HeaderSize);

        if(mHeader.ID[0] != 'O' || mHeader.ID[1] != 'F' || mHeader.ID[2] != 'S' || mHeader.ID[3] != '1')
            return OFS_INVALID_FILE;

        int current_version = ((VERSION_MAJOR_0 - '0') * 1000) + ((VERSION_MAJOR_1 - '0') * 100) + ((VERSION_MINOR - '0') * 10);
        int file_version = ((mHeader.VERSION[0] - '0') * 1000) + ((mHeader.VERSION[1] - '0') * 100) + ((mHeader.VERSION[2] - '0') * 10);

        // Temporary hack for initial conversion, will be removed later
        if(file_version == 1000)
            file_version = 100;

        int version_diff = (current_version - file_version) / 10;

        if(version_diff > 0)
            return OFS_PREVIOUS_VERSION;
        else if(version_diff < 0)
            return OFS_UNKNOWN_VERSION;

        ofs64 current_loc = mStream.tell();

        if(current_loc == file_size)
            return OFS_OK;

        strBlockHeader blHeader;
        BlockData blockData;
        strMainEntryHeader mainEntry;
        strExtendedEntryHeader extendedEntry;
        IdDescMap::const_iterator it;
        IdDescMap::iterator dait;
        IdDescMap DirMap;
        IdDescMap FileMap;

        DirMap.insert(IdDescMap::value_type(mRootDir.Id, &mRootDir));
        DirMap.insert(IdDescMap::value_type(mRecycleBinRoot.Id, &mRecycleBinRoot));

        ofs64 skipped = 0;

        while((current_loc = mStream.tell()) < file_size)
        {
            mStream.read((char*)&blHeader, sizeof(strBlockHeader));
            if(blHeader.Signature[0] != mHeader.BLOCK_HEADER_SIG[0] || blHeader.Signature[1] != mHeader.BLOCK_HEADER_SIG[1])
            {
                if(mRecoveryMode)
                {
                    ofs64 pos = mStream.tell();
                    mStream.seek( pos + 1 - sizeof(strBlockHeader));
                    skipped++;
                    continue;
                }
                else
                    return OFS_FILE_CORRUPT;
            }
            else
            {
                if(skipped > sizeof(strBlockHeader))
                {
                    ofs64 pos = mStream.tell();

                    //TODO : We may want to do some corrections here, like creating a free block signature

                    mStream.seek( pos );
                }

                skipped = 0;
            }

            if(blHeader.Type & OFS_FREE_BLOCK)
            {
                blockData.Type = blHeader.Type;
                blockData.Start = mStream.tell();
                blockData.Length = blHeader.Length;
                blockData.NextBlock = 0;

                mFreeBlocks.push_back(blockData);
                mStream.seek(blockData.Length, OFS_SEEK_CURRENT);
            }
            else if(blHeader.Type == OFS_MAIN_BLOCK)
            {
                mStream.read((char*)&mainEntry, sizeof(strMainEntryHeader));
                
                if(mLinkMode)
                    mainEntry.Flags |= OFS_LINK;

                blockData.Type = blHeader.Type;
                blockData.Start = mStream.tell();
                blockData.Start -= sizeof(strMainEntryHeader);
                blockData.Length = blHeader.Length;
                blockData.NextBlock = mainEntry.NextBlock;

                OfsEntryDesc *entryDesc = new OfsEntryDesc();

                if(mainEntry.Flags & OFS_DIR)
                {
                    dait = DirMap.find(mainEntry.Id);
                    if(dait == DirMap.end())
                        DirMap.insert(IdDescMap::value_type(mainEntry.Id, entryDesc));
                    else
                    {
                        delete entryDesc;
                        entryDesc = dait->second;
                    }
                }
                else
                {
                    dait = FileMap.find(mainEntry.Id);
                    if(dait == FileMap.end())
                        FileMap.insert(IdDescMap::value_type(mainEntry.Id, entryDesc));
                    else
                    {
                        delete entryDesc;
                        entryDesc = dait->second;
                    }
                }
                    
                entryDesc->Owner = this;
                entryDesc->Id = mainEntry.Id;
                entryDesc->ParentId = mainEntry.ParentId;
                entryDesc->Flags = mainEntry.Flags;
                entryDesc->Name = mainEntry.Name;
                entryDesc->FileSize = mainEntry.FileSize;
                entryDesc->CreationTime = mainEntry.CreationTime;
                entryDesc->UseCount = 0;
                entryDesc->WriteLocked = false;
                entryDesc->Uuid = mainEntry.Uuid;

                if((mainEntry.ParentId != RECYCLEBIN_DIRECTORY_ID) && (mainEntry.Uuid != UUID_ZERO))
                {
                    assert(mUuidMap.find(mainEntry.Uuid) == mUuidMap.end());
                    mUuidMap.insert(UuidDescMap::value_type(mainEntry.Uuid, entryDesc));
                }

                it = DirMap.find(entryDesc->ParentId);
                if(it == DirMap.end())
                {
                    OfsEntryDesc *dirDesc = new OfsEntryDesc();
                    dirDesc->Id = entryDesc->ParentId;
                    dirDesc->Name = "Error_NoName";
                    dirDesc->Parent = 0;

                    it = DirMap.insert(IdDescMap::value_type(dirDesc->Id, dirDesc)).first;
                }

                if(entryDesc->UsedBlocks.size() == 0)
                    entryDesc->UsedBlocks.push_back(blockData);
                else
                    entryDesc->UsedBlocks[0] = blockData;

                if(entryDesc->ParentId == RECYCLEBIN_DIRECTORY_ID)
                    entryDesc->OldParentId = mainEntry.OldParentId;
                else
                    entryDesc->OldParentId = ROOT_DIRECTORY_ID;
                
                entryDesc->Parent = it->second;
                it->second->Children.push_back(entryDesc);

                mStream.seek(blockData.Length - sizeof(strMainEntryHeader), OFS_SEEK_CURRENT);
            }
            else if(blHeader.Type == OFS_EXTENDED_BLOCK)
            {
                mStream.read((char*)&extendedEntry, sizeof(strExtendedEntryHeader));
 
                blockData.Type = blHeader.Type;
                blockData.Start = mStream.tell();
                blockData.Start -= sizeof(strExtendedEntryHeader);
                blockData.Length = blHeader.Length;
                blockData.NextBlock = extendedEntry.NextBlock;

                it = FileMap.find(extendedEntry.Id);
                if(it == FileMap.end())
                {
                    OfsEntryDesc *fileDesc = new OfsEntryDesc();
                    fileDesc->Id = extendedEntry.Id;
                    fileDesc->ParentId = extendedEntry.ParentId;
                    fileDesc->Name = "Error_NoName";
                    fileDesc->Parent = 0;

                    it = FileMap.insert(IdDescMap::value_type(fileDesc->Id, fileDesc)).first;
                }

                while(it->second->UsedBlocks.size() <= extendedEntry.Index)
                    it->second->UsedBlocks.push_back(blockData);

                it->second->UsedBlocks[extendedEntry.Index] = blockData;

                mStream.seek(blockData.Length - sizeof(strExtendedEntryHeader), OFS_SEEK_CURRENT);
            }
            else
                return OFS_FILE_CORRUPT;
        }

        std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::_writeHeader()
    {
        assert(mActive);

        mStream.seek(0, OFS_SEEK_BEGIN);
        
        mStream.write((char *)&mHeader, sizeof(strFileHeader));
        
        mStream.flush();

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    _Ofs::OfsEntryDesc* _Ofs::_getDirectoryDesc(const char *filename)
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

    _Ofs::OfsEntryDesc* _Ofs::_getFileDesc(OfsEntryDesc *dir_desc, std::string filename)
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
    
    _Ofs::OfsEntryDesc* _Ofs::_findChild(OfsEntryDesc *dir_desc, std::string child_name)
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

    std::string   _Ofs::_extractFileName(const char *filename)
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

    _Ofs::OfsEntryDesc* _Ofs::_createDirectory(OfsEntryDesc *parent, const std::string& name, const UUID& uuid)
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

                OFS_EXCEPT("_Ofs::_createDirectory, The UUID supplied already exists.");

                return NULL;
            }
            else
            {
                mUuidMap.insert(UuidDescMap::value_type(uuid, dir));
            }
        }


        dir->Owner = this;
        dir->Id = mHeader.LAST_ID++;
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

        BlockData dirData;
        strBlockHeader fileData;
        strMainEntryHeader fileHeader;

        fileHeader.Id = dir->Id;
        fileHeader.ParentId = dir->ParentId;
        fileHeader.Flags = dir->Flags;
        fileHeader.OldParentId = ROOT_DIRECTORY_ID;
        fileHeader.NextBlock = 0;
        fileHeader.FileSize = 0;
        fileHeader.CreationTime = dir->CreationTime;
        int sz = dir->Name.length();
        if(sz > 251)
            sz = 251;
        memcpy(fileHeader.Name, dir->Name.c_str(), sz);
        fileHeader.Name[sz] = 0;
        fileHeader.Uuid = dir->Uuid;

        fileData.Signature[0] = mHeader.BLOCK_HEADER_SIG[0];
        fileData.Signature[1] = mHeader.BLOCK_HEADER_SIG[1];

        if(mFreeBlocks.size() > 0)
        {
           dirData = mFreeBlocks[0];
           dirData.Type = OFS_MAIN_BLOCK;
           dirData.NextBlock = 0;
           if(dirData.Length >= (1024 + sizeof(strBlockHeader) + sizeof(strMainEntryHeader)))
           {
               mFreeBlocks[0].Start += sizeof(strBlockHeader) + sizeof(strMainEntryHeader);
               mFreeBlocks[0].Length -= sizeof(strBlockHeader) + sizeof(strMainEntryHeader);
               dirData.Length = sizeof(strMainEntryHeader);

               fileData.Type = OFS_MAIN_BLOCK;
               fileData.Length = sizeof(strMainEntryHeader);
               mStream.seek(dirData.Start - sizeof(strBlockHeader), OFS_SEEK_BEGIN);
               mStream.write((char*)&fileData, sizeof(strBlockHeader));
               mStream.write((char*)&fileHeader, sizeof(strMainEntryHeader));
               
               fileData.Type = OFS_FREE_BLOCK;
               fileData.Length = mFreeBlocks[0].Length;
               mStream.write((char*)&fileData, sizeof(strBlockHeader));

               dir->UsedBlocks.push_back(dirData);
           }
           else
           {
               mFreeBlocks.erase(mFreeBlocks.begin());
               fileData.Type = OFS_MAIN_BLOCK;
               fileData.Length = dirData.Length;
               mStream.seek(dirData.Start - sizeof(strBlockHeader), OFS_SEEK_BEGIN);
               mStream.write((char*)&fileData, sizeof(strBlockHeader));
               mStream.write((char*)&fileHeader, sizeof(strMainEntryHeader));
               dir->UsedBlocks.push_back(dirData);
           }
        }
        else
        {
            mStream.seek(0, OFS_SEEK_END);
            dirData.Type = OFS_MAIN_BLOCK;
            dirData.NextBlock = 0;
            dirData.Length = sizeof(strMainEntryHeader);
            dirData.Start = mStream.tell();
            dirData.Start += sizeof(strBlockHeader);
            fileData.Length = dirData.Length;
            fileData.Type = OFS_MAIN_BLOCK;
            mStream.write((char*)&fileData, sizeof(strBlockHeader));
            mStream.write((char*)&fileHeader, sizeof(strMainEntryHeader));
            dir->UsedBlocks.push_back(dirData);
        }

        _writeHeader();

        return dir;
    }

//------------------------------------------------------------------------------

    _Ofs::OfsEntryDesc* _Ofs::_createFile(OfsEntryDesc *parent, const std::string& name, ofs64 file_size, const UUID& uuid, unsigned int data_size, const char *data)
    {
        assert(parent != NULL);
        
        OfsEntryDesc *file = new OfsEntryDesc();

        file->Owner = this;
        file->Id = mHeader.LAST_ID++;
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
            mUuidMap.insert(UuidDescMap::value_type(uuid, file));

        strMainEntryHeader fileHeader;

        fileHeader.Id = file->Id;
        fileHeader.ParentId = file->ParentId;
        fileHeader.Flags = file->Flags;
        fileHeader.OldParentId = ROOT_DIRECTORY_ID;
        fileHeader.NextBlock = 0;
        fileHeader.FileSize = file->FileSize;
        fileHeader.CreationTime = file->CreationTime;

        int sz = file->Name.length();
        if(sz > 251)
            sz = 251;
        memcpy(fileHeader.Name, file->Name.c_str(), sz);
        fileHeader.Name[sz] = 0;
        fileHeader.Uuid = file->Uuid;

        ofs64 blockToAlloc = file_size;

        if(blockToAlloc == 0)
            blockToAlloc = 1024;

        blockToAlloc += sizeof(strMainEntryHeader);

        _allocateFileBlock(file, fileHeader, blockToAlloc, data_size, data);

        return file;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::deleteDirectory(const char *path, bool force)
    {
        assert(path != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::deleteDirectory, Operation called on an unmounted file system.");
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

            mStream.flush();

            std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);

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

    OfsResult _Ofs::_deleteDirectory(OfsEntryDesc *dir)
    {
        assert(dir != NULL);

        if(dir->Flags & OFS_READONLY)
        {
            return OFS_ACCESS_DENIED;
        }

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

        _markUnused(dir->UsedBlocks[0]);

        if( dir->Uuid != UUID_ZERO )
        {
            UuidDescMap::iterator uit = mUuidMap.find(dir->Uuid);

            if( uit != mUuidMap.end() )
               mUuidMap.erase( uit );
        }

        delete dir;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::deleteFile(const char *filename)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::deleteFile, Operation called on an unmounted file system.");
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

        std::vector<_Ofs::CallBackData> saveTrigs = fileDesc->Triggers;

        OfsResult ret = _deleteFile(fileDesc);

        mStream.flush();

        if(ret == OFS_OK)
        {
            std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);

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

    OfsResult _Ofs::_deleteFile(OfsEntryDesc *file)
    {
        assert(file != NULL);

        IdHandleMap::const_iterator it = mActiveFiles.find(file->Id);

        if(it != mActiveFiles.end() || (file->Flags & OFS_READONLY))
        {
            return OFS_ACCESS_DENIED;
        }

        for(unsigned int i = 0;i < file->Parent->Children.size();i++)
        {
            if(file->Parent->Children[i]->Id == file->Id)
            {
                file->Parent->Children.erase(file->Parent->Children.begin() + i);
                break;
            }
        }

        for(unsigned int i = 0;i < file->UsedBlocks.size();i++)
        {
            _markUnused(file->UsedBlocks[i]);
        }

        if( file->Uuid != UUID_ZERO )
        {
            UuidDescMap::iterator uit = mUuidMap.find(file->Uuid);

            if( uit != mUuidMap.end() )
               mUuidMap.erase( uit );
        }

        delete file;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::createDirectoryUUID(const char *filename, const UUID& uuid, bool force)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::createDirectory, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::renameFile(const char *filename, const char *newname)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::renameFile, Operation called on an unmounted file system.");
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

        fileDesc->Name = nName;

        mStream.seek(fileDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Name), OFS_SEEK_BEGIN);
        mStream.write(fileDesc->Name.c_str(), fileDesc->Name.length() + 1);

        mStream.flush();

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

    OfsResult _Ofs::renameDirectory(const char *dirname, const char *newname)
    {
        assert(dirname != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::renameDirectory, Operation called on an unmounted file system.");
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

            dirDesc->Name = nName;

            mStream.seek(dirDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Name), OFS_SEEK_BEGIN);
            mStream.write(dirDesc->Name.c_str(), dirDesc->Name.length() + 1);

            mStream.flush();

            return OFS_OK;
        }
        else
            return OFS_INVALID_PATH;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::moveDirectory(const char *dirname, const char *dest)
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
            } else {
                ret = moveFile(currentLoc.c_str(), destLoc.c_str());
            }

            if (ret != OFS_OK)
                return ret;
        }
        
        /* Delete the directory only if it is empty as it might contain our new folder */
        deleteDirectory(dirname, false);

        return ret;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::openFile(OFSHANDLE& handle, const char *filename, unsigned int open_mode)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::openFile, Operation called on an unmounted file system.");
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

            if(open_mode & OFS_WRITE)
            {
                if(((fileDesc->UseCount > 0) && !(open_mode & OFS_FORCE)) || (fileDesc->Flags & OFS_READONLY) || (fileDesc->Flags & OFS_LINK))
                    return OFS_ACCESS_DENIED;

                fileDesc->WriteLocked = true;

                if(!(open_mode & OFS_APPEND))
                {
                    for(unsigned int i = 1;i < fileDesc->UsedBlocks.size();i++)
                    {
                        _markUnused(fileDesc->UsedBlocks[i]);
                    }

                    if(fileDesc->UsedBlocks.size() > 1)
                        fileDesc->UsedBlocks.erase(fileDesc->UsedBlocks.begin() + 1, fileDesc->UsedBlocks.end());

                    fileDesc->UsedBlocks[0].NextBlock = 0;
                    fileDesc->FileSize = 0;

                    strMainEntryHeader fileHeader;

                    fileHeader.Id = fileDesc->Id;
                    fileHeader.ParentId = fileDesc->ParentId;
                    fileHeader.Flags = fileDesc->Flags;
                    fileHeader.OldParentId = ROOT_DIRECTORY_ID;
                    fileHeader.NextBlock = 0;
                    fileHeader.FileSize = 0;
                    fileHeader.CreationTime = fileDesc->CreationTime;
                    int sz = fileDesc->Name.length();
                    if(sz > 251)
                        sz = 251;
                    memcpy(fileHeader.Name, fileDesc->Name.c_str(), sz);
                    fileHeader.Name[sz] = 0;
                    fileHeader.Uuid = fileDesc->Uuid;

                    mStream.seek(fileDesc->UsedBlocks[0].Start, OFS_SEEK_BEGIN);
                    mStream.write((char*)&fileHeader, sizeof(strMainEntryHeader)); 

                    mStream.flush();

                    std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);
                }
            }
        } 

        if(fileDesc->UseCount == 0)
            mActiveFiles.insert(IdHandleMap::value_type(fileDesc->Id, &handle));

        fileDesc->UseCount++;
        handle.mEntryDesc = fileDesc;
        handle.mAccessFlags = open_mode;
        handle._preparePointers((open_mode & OFS_APPEND) != 0);

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::openFile(OFSHANDLE& handle, const UUID& uuid, unsigned int open_mode)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::openFile, Operation called on an unmounted file system.");
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

        if(open_mode & OFS_WRITE)
        {
            if((fileDesc->UseCount > 0) || (fileDesc->Flags & OFS_READONLY) || (fileDesc->Flags & OFS_LINK))
                return OFS_ACCESS_DENIED;

            fileDesc->WriteLocked = true;

            if(!(open_mode & OFS_APPEND))
            {
                for(unsigned int i = 1;i < fileDesc->UsedBlocks.size();i++)
                {
                    _markUnused(fileDesc->UsedBlocks[i]);
                }

                if(fileDesc->UsedBlocks.size() > 1)
                    fileDesc->UsedBlocks.erase(fileDesc->UsedBlocks.begin() + 1, fileDesc->UsedBlocks.end());

                fileDesc->UsedBlocks[0].NextBlock = 0;
                fileDesc->FileSize = 0;

                strMainEntryHeader fileHeader;

                fileHeader.Id = fileDesc->Id;
                fileHeader.ParentId = fileDesc->ParentId;
                fileHeader.Flags = fileDesc->Flags;
                fileHeader.OldParentId = ROOT_DIRECTORY_ID;
                fileHeader.NextBlock = 0;
                fileHeader.FileSize = 0;
                fileHeader.CreationTime = fileDesc->CreationTime;
                int sz = fileDesc->Name.length();
                if(sz > 251)
                    sz = 251;
                memcpy(fileHeader.Name, fileDesc->Name.c_str(), sz);
                fileHeader.Name[sz] = 0;
                fileHeader.Uuid = fileDesc->Uuid;

                mStream.seek(fileDesc->UsedBlocks[0].Start, OFS_SEEK_BEGIN);
                mStream.write((char*)&fileHeader, sizeof(strMainEntryHeader)); 

                mStream.flush();

                std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);
            }
        }

        if(fileDesc->UseCount == 0)
            mActiveFiles.insert(IdHandleMap::value_type(fileDesc->Id, &handle));

        fileDesc->UseCount++;
        handle.mEntryDesc = fileDesc;
        handle.mAccessFlags = open_mode;
        handle._preparePointers((open_mode & OFS_APPEND) != 0);

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::createFileUUID(OFSHANDLE& handle, const char *filename, const UUID& uuid, ofs64 file_size, unsigned int data_size, const char *data)
    {
        assert(filename != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::openFile, Operation called on an unmounted file system.");
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
                    OFS_EXCEPT("_Ofs::createFileUUID, The UUID supplied already exists.");
    
                    return OFS_INVALID_UUID;
                }
            }

            fileDesc = _createFile(dirDesc, fName, file_size, uuid, data_size, data);

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
        handle._preparePointers(true);

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

    OfsResult _Ofs::closeFile(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::closeFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::closeFile, Supplied OfsHandle is not valid.");
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

    OfsResult _Ofs::truncateFile(OFSHANDLE& handle, ofs64 file_size)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::closeFile, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::truncateFile, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if(!(handle.mAccessFlags & OFS_WRITE) || (handle.mAccessFlags & OFS_LINK))
            return OFS_ACCESS_DENIED;

        ofs64 trunc_pos = file_size;

        if(file_size < 0)
            trunc_pos = handle.mPos;

        if(trunc_pos < handle.mEntryDesc->FileSize)
        {
            OfsEntryDesc *desc = handle.mEntryDesc;
            desc->FileSize = trunc_pos;
            handle._setPos(trunc_pos);

            for(unsigned int i = handle.mBlock + 1;i < desc->UsedBlocks.size();i++)
            {
                _markUnused(desc->UsedBlocks[i]);
            }

            while((handle.mBlock + 1) < desc->UsedBlocks.size())
                desc->UsedBlocks.erase(desc->UsedBlocks.begin() + (handle.mBlock + 1));

            std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);

            mStream.seek(desc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, FileSize), OFS_SEEK_BEGIN);
            mStream.write((char*)&trunc_pos, sizeof(ofs64)); 

            if(handle.mBlock == 0)
                mStream.seek(desc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, NextBlock), OFS_SEEK_BEGIN);
            else
                mStream.seek(desc->UsedBlocks[handle.mBlock].Start + offsetof(strExtendedEntryHeader, NextBlock), OFS_SEEK_BEGIN);

            trunc_pos = 0;
            mStream.write((char*)&trunc_pos, sizeof(ofs64)); 
            mStream.flush();
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    void _Ofs::_setFileFlags(OfsEntryDesc *file, unsigned int flags)
    {
        assert(file != NULL);

        flags &= (OFS_READONLY | OFS_HIDDEN);
        file->Flags = (file->Flags & (OFS_FILE | OFS_DIR)) | flags;

        mStream.seek(file->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Flags), OFS_SEEK_BEGIN);
        mStream.write((char *)&(file->Flags), sizeof(unsigned int));

        if(file->Flags & OFS_DIR)
        {
            for(unsigned int i = 0;i < file->Children.size();i++)
            {
                _setFileFlags(file->Children[i], flags);
            }
        }
    }

//------------------------------------------------------------------------------

    FileList _Ofs::listFiles(const char *path, unsigned int file_flags)
    {
        assert(path != NULL);

        LOCK_AUTO_MUTEX

        FileList output;

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::listFiles, Operation called on an unmounted file system.");
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

    FileList _Ofs::listRecycleBinFiles()
    {
        LOCK_AUTO_MUTEX

        FileList output;

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::listRecycleBinFiles, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::read(OFSHANDLE& handle, char *dest, unsigned int length, unsigned int *actual_read)
    {
        assert(dest != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::read, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::read, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if(handle.mEntryDesc->Owner != this)
            return handle.mEntryDesc->Owner->read(handle, dest, length, actual_read);

        if(!(handle.mAccessFlags & OFS_READ))
            return OFS_ACCESS_DENIED;

        OfsEntryDesc *desc = handle.mEntryDesc;

        if(desc->FileSize < (handle.mPos + length))
            length = (unsigned int)(desc->FileSize - handle.mPos);

        if(length > 0)
        {
            ofs64 can_read = handle.mBlockEnd - handle.mRealPos;
            unsigned int tmp_len = length;

            bool done = false;
            while(!done && (can_read > 0))
            {
                if(can_read >= tmp_len)
                {
                    mStream.seek(handle.mRealPos, OFS_SEEK_BEGIN);
                    mStream.read(dest, tmp_len);
                    handle._setPos(handle.mPos + tmp_len);
                    done = true;
                }
                else
                {
                    mStream.seek(handle.mRealPos, OFS_SEEK_BEGIN);
                    mStream.read(dest, (unsigned int)can_read);
                    handle._setPos(handle.mPos + can_read);
                    tmp_len -= (unsigned int)can_read;
                    dest += can_read;

                    can_read = handle.mBlockEnd - handle.mRealPos;
                }
            }
        }

        if(actual_read != NULL)
            *actual_read = length;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::write(OFSHANDLE& handle, const char *src, unsigned int length)
    {
        assert(src != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::write, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::write, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if(!(handle.mAccessFlags & OFS_WRITE))
            return OFS_ACCESS_DENIED;

        OfsEntryDesc *desc = handle.mEntryDesc;
        if(length > 0)
        {
            ofs64 total_alloc = desc->UsedBlocks[0].Length - sizeof(strMainEntryHeader);
            for(unsigned int i = 1;i < desc->UsedBlocks.size();i++)
                total_alloc += desc->UsedBlocks[i].Length - sizeof(strExtendedEntryHeader);

            total_alloc -= handle.mPos;
            unsigned int output_amount = length;

            if(total_alloc < length)
            {
                unsigned int space_needed = length - (unsigned int)total_alloc;
                unsigned int alloc_size =  sizeof(strExtendedEntryHeader);
                
                if(space_needed < 1024)
                    alloc_size += 1024;
                else
                    alloc_size += space_needed;

                _allocateExtendedFileBlock(desc, alloc_size, space_needed, (src + total_alloc));

                output_amount = (unsigned int)total_alloc; 
            }

            ofs64 write_pos_save = handle.mPos;

            ofs64 can_write = handle.mBlockEnd - handle.mRealPos;

            bool done = false;
            while(!done)
            {
                if(can_write >= output_amount)
                {
                    mStream.seek(handle.mRealPos, OFS_SEEK_BEGIN);
                    mStream.write(src, output_amount);
                    handle._setPos(handle.mPos + output_amount);
                    done = true;
                }
                else
                {
                    mStream.seek(handle.mRealPos, OFS_SEEK_BEGIN);
                    mStream.write(src, (unsigned int)can_write);
                    handle._setPos(handle.mPos + can_write);
                    output_amount -= (unsigned int)can_write;
                    src += can_write;

                    can_write = handle.mBlockEnd - handle.mRealPos;
                }
            }

            if(write_pos_save + length > desc->FileSize)
            {
                desc->FileSize = write_pos_save + length;
                mStream.seek(desc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, FileSize), OFS_SEEK_BEGIN);
                mStream.write((char*)&(desc->FileSize), sizeof(ofs64));
                mStream.flush();
            }

            handle._setPos(write_pos_save + length);
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    ofs64 _Ofs::seek(OFSHANDLE& handle, ofs64 pos, SeekDirection dir)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::seekr, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::seekr, Supplied OfsHandle is not valid.");
            return 0;
        }

        switch(dir)
        {
        case OFS_SEEK_BEGIN:if(pos < 0) pos = 0;
                            break;
        case OFS_SEEK_CURRENT:pos += handle.mPos;
                              if(pos < 0) pos = 0;
                              break;
        case OFS_SEEK_END:pos += handle.mEntryDesc->FileSize;
                          if(pos < 0) pos = 0;
                          break;
        }

        handle._setPos(pos);
        return handle.mPos;
    }

//------------------------------------------------------------------------------

    ofs64 _Ofs::tell(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::tellw, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::tellw, Supplied OfsHandle is not valid.");
            return 0;
        }

        return handle.mPos;
    }

//------------------------------------------------------------------------------

    bool _Ofs::eof(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::eof, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::eof, Supplied OfsHandle is not valid.");
            return 0;
        }

        return (handle.mPos >= handle.mEntryDesc->FileSize);
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getFileName(OFSHANDLE& handle, std::string& filename)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileName, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::getFileName, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        filename = handle.mEntryDesc->Name;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getDirEntry(const char *path, FileEntry& entry)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getDirEntry, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::getFileEntry(const char *filename, FileEntry& entry)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileEntry, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::getFileEntry(OFSHANDLE& handle, FileEntry& entry)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileEntry, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::getFileEntry, Supplied OfsHandle is not valid.");
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

    OfsResult _Ofs::getCreationTime(const char *filename, time_t& creation_time)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getCreationTime, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::getModificationTime(const char *filename, time_t& mod_time)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getModificationTime, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::getFileSize(const char *filename, ofs64& size)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileSize, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::setFileFlags(const char *filename, unsigned int flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::setFileFlags, Operation called on an unmounted file system.");
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
            mStream.flush();
            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::setFileFlags(OFSHANDLE& handle, unsigned int flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::setFileFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::setFileFlags, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        if( handle.mEntryDesc->Flags & OFS_LINK )
            return OFS_ACCESS_DENIED;

        _setFileFlags(handle.mEntryDesc, flags);
        mStream.flush();

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getFileFlags(const char *filename, unsigned int& flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileFlags, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::getFileFlags(OFSHANDLE& handle, unsigned int& flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileFlags, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::getFileFlags, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        flags = handle.mEntryDesc->Flags;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::setFileUUID(const char *filename, const UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::setFileUUID, Operation called on an unmounted file system.");
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
                mStream.seek(fileDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Uuid), OFS_SEEK_BEGIN);
                mStream.write((char*)&(fileDesc->Uuid), sizeof(UUID));
                mStream.flush();
            }
            
            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::setFileUUID(OFSHANDLE& handle, const UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::setFileUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::setFileUUID, Supplied OfsHandle is not valid.");
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
            mStream.seek(handle.mEntryDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Uuid), OFS_SEEK_BEGIN);
            mStream.write((char*)&(handle.mEntryDesc->Uuid), sizeof(UUID));
            mStream.flush();
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getFileUUID(const char *filename, UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileUUID, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::getFileUUID(OFSHANDLE& handle, UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileUUID, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::getFileUUID, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        uuid = handle.mEntryDesc->Uuid;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::setDirUUID(const char *dirpath, const UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::setDirUUID, Operation called on an unmounted file system.");
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
                mStream.seek(dirDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Uuid), OFS_SEEK_BEGIN);
                mStream.write((char*)&(dirDesc->Uuid), sizeof(UUID));
                mStream.flush();
            }
            
            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getDirUUID(const char *dirpath, UUID& uuid)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getDirUUID, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::setDirFlags(const char *dirpath, unsigned int flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::setDirFlags, Operation called on an unmounted file system.");
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
            mStream.flush();

            return OFS_OK;
        }
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getDirFlags(const char *dirpath, unsigned int& flags)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getDirFlags, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::getCreationTime(OFSHANDLE& handle, time_t& creation_time)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getCreationTime, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::getCreationTime, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        creation_time = handle.mEntryDesc->CreationTime;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getModificationTime(OFSHANDLE& handle, time_t& mod_time) 
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getModificationTime, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::getModificationTime, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        mod_time = handle.mEntryDesc->CreationTime;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getFileSize(OFSHANDLE& handle, ofs64& size)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::getFileSize, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::getFileSize, Supplied OfsHandle is not valid.");
            return OFS_INVALID_FILE;
        }

        size = handle.mEntryDesc->FileSize;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    bool _Ofs::exists(const char *filename)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::exists, Operation called on an unmounted file system.");
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

    OfsResult _Ofs::copyFile(const char *src, const char *dest)
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

    OfsResult _Ofs::moveFile(const char *src, const char *dest)
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
            for(unsigned int i = 0;i < srcDesc->Parent->Children.size();i++)
            {
                if(srcDesc->Parent->Children[i] == srcDesc)
                {
                    srcDesc->Parent->Children.erase(srcDesc->Parent->Children.begin() + i);
                    break;
                }
            }

            dirDesc->Children.push_back(srcDesc);

            srcDesc->ParentId = dirDesc->Id;
            srcDesc->Parent = dirDesc;

            std::string nName = _extractFileName(dest);
            if( nName == "" || srcDesc->Name == nName )
            {
                mStream.seek(srcDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, ParentId), OFS_SEEK_BEGIN);
                mStream.write((char*)&(srcDesc->ParentId), sizeof(unsigned int));
                mStream.flush();
            }
            else
            {
                int sz = nName.length();
                if(sz > 251)
                    nName.erase(251, sz - 251);

                srcDesc->Name = nName;

                mStream.seek(srcDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Name), OFS_SEEK_BEGIN);
                mStream.write(srcDesc->Name.c_str(), srcDesc->Name.length() + 1);
                mStream.flush();

                mStream.seek(srcDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, ParentId), OFS_SEEK_BEGIN);
                mStream.write((char*)&(srcDesc->ParentId), sizeof(unsigned int));
                mStream.flush();
            }

            ret = OFS_OK;
        }

        return ret;
    }

//------------------------------------------------------------------------------------------
    
    OfsResult _Ofs::moveToRecycleBin(const char *path)
    {
        assert(path != NULL);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::moveToRecycleBin, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(path);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(path);

        if( fName.size() > 0 )
        {
            OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

            if(fileDesc == NULL)
                return OFS_FILE_NOT_FOUND;

            dirDesc = fileDesc;
        }
            
        if(dirDesc->Flags & OFS_LINK)
            return OFS_ACCESS_DENIED;

        std::vector<_Ofs::CallBackData> saveTrigs = dirDesc->Triggers;

        IdHandleMap::const_iterator it = mActiveFiles.find(dirDesc->Id);

        if(it != mActiveFiles.end())
            return OFS_ACCESS_DENIED;

        for(unsigned int i = 0;i < dirDesc->Parent->Children.size();i++)
        {
            if(dirDesc->Parent->Children[i]->Id == dirDesc->Id)
            {
                dirDesc->Parent->Children.erase(dirDesc->Parent->Children.begin() + i);
                break;
            }
        }

        dirDesc->OldParentId = dirDesc->ParentId;
        dirDesc->Parent = &mRecycleBinRoot;
        dirDesc->ParentId = mRecycleBinRoot.Id;
        mRecycleBinRoot.Children.push_back( dirDesc );

        mStream.seek(dirDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, ParentId), OFS_SEEK_BEGIN);
        mStream.write((char*)&(dirDesc->ParentId), sizeof(unsigned int));
        mStream.seek(dirDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, OldParentId), OFS_SEEK_BEGIN);
        mStream.write((char*)&(dirDesc->OldParentId), sizeof(unsigned int));
        mStream.flush();

        for(unsigned int i = 0;i < saveTrigs.size();i++)
        {
            if(saveTrigs[i].type == CLBK_DELETE)
            {
                saveTrigs[i].func(saveTrigs[i].data, 0, path);
            }
        }

        for(unsigned int i = 0;i < mTriggers.size();i++)
        {
            if(mTriggers[i].type == CLBK_DELETE)
            {
                mTriggers[i].func(mTriggers[i].data, 0, path);
            }
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    _Ofs::OfsEntryDesc* _Ofs::_findDescById( _Ofs::OfsEntryDesc* base, int id )
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

    OfsResult _Ofs::restoreFromRecycleBin(int id)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::restoreFromRecycleBin, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *sourceDesc = NULL, *destDesc = NULL;

        for( unsigned int i = 0; i < mRecycleBinRoot.Children.size(); i++)
        {
            if( mRecycleBinRoot.Children[i]->Id == id )
            {
                sourceDesc = mRecycleBinRoot.Children[i];
                break;
            }
        }

        if( sourceDesc == NULL )
            return OFS_FILE_NOT_FOUND;

        destDesc = _findDescById( &mRootDir, sourceDesc->OldParentId );

        if( destDesc == NULL )
            destDesc = &mRootDir;

        for( unsigned int i = 0; i < destDesc->Children.size(); i++)
        {
            if( destDesc->Children[i]->Name == sourceDesc->Name )
                return OFS_ACCESS_DENIED;
        }

        for( unsigned int i = 0; i < mRecycleBinRoot.Children.size(); i++)
        {
            if( mRecycleBinRoot.Children[i]->Id == id )
            {
                mRecycleBinRoot.Children.erase(mRecycleBinRoot.Children.begin() + i);
                break;
            }
        }

        sourceDesc->OldParentId = ROOT_DIRECTORY_ID;
        sourceDesc->Parent = destDesc;
        sourceDesc->ParentId = destDesc->Id;
        destDesc->Children.push_back( sourceDesc );

        mStream.seek(sourceDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, ParentId), OFS_SEEK_BEGIN);
        mStream.write((char*)&(sourceDesc->ParentId), sizeof(unsigned int));
        mStream.seek(sourceDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, OldParentId), OFS_SEEK_BEGIN);
        mStream.write((char*)&(sourceDesc->OldParentId), sizeof(unsigned int));
        mStream.flush();

        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    void _Ofs::_deleteRecycleBinDesc(OfsEntryDesc *desc)
    {
        unsigned int i;

        for(i = 0;i < desc->UsedBlocks.size();i++)
        {
            _markUnused(desc->UsedBlocks[i]);
        }


        for(i = 0; i < desc->Children.size(); i++)
        {
            _deleteRecycleBinDesc(desc->Children[i]);
        }
    }

//------------------------------------------------------------------------------------------

    OfsResult _Ofs::emptyRecycleBin()
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::emptyRecycleBin, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        for( unsigned int i = 0; i < mRecycleBinRoot.Children.size(); i++)
        {
            _deleteRecycleBinDesc(mRecycleBinRoot.Children[i]);
        }

        _deallocateChildren(&mRecycleBinRoot);

        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    OfsResult _Ofs::moveFileSystemTo(const char *dest)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::moveFileSystemTo, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsResult ret = OFS_INVALID_FILE;

        {
            STATIC_LOCK_AUTO_MUTEX

            NameOfsHandleMap::iterator it = mAllocatedHandles.find(std::string(dest));

            if(it != mAllocatedHandles.end())
                return ret;
        }

        FileStream destStream;
        OPEN_STREAM(destStream, dest, "wb+");

        if(!destStream.fail())
        {
            mStream.seek(0, OFS_SEEK_END);
            ofs64 total_amount = mStream.tell();

            mStream.seek(0, OFS_SEEK_BEGIN);

            char *buffer = new char[65536];

            while(total_amount > 0)
            {
                if(total_amount > 65536)
                {
                    mStream.read(buffer, 65536);
                    destStream.write(buffer, 65536);
                    total_amount -= 65536;
                }
                else
                {
                    mStream.read(buffer, (unsigned int)total_amount);
                    destStream.write(buffer, (unsigned int)total_amount);
                    total_amount = 0;
                }
            }

            delete [] buffer;

            destStream.close();
            mStream.close();

            OPEN_STREAM(mStream, dest, "rb+");
            if(!mStream.fail())
            {
                STATIC_LOCK_AUTO_MUTEX
                mAllocatedHandles.erase(mAllocatedHandles.find(mFileName));
                mActive = true;
                mFileName = dest;
                ret = OFS_OK;
                mAllocatedHandles.insert(NameOfsHandleMap::value_type(std::string(dest), this));
            }
            else
            {
                mActive = false;
                mFileName = "";
                ret = OFS_INVALID_FILE;
            }
        }
        
        return ret;
    }

//------------------------------------------------------------------------------------------

    OfsResult _Ofs::switchFileSystemTo(const char *dest)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::switchFileSystemTo, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsResult ret = OFS_INVALID_FILE;

        {
            STATIC_LOCK_AUTO_MUTEX

            NameOfsHandleMap::iterator it = mAllocatedHandles.find(std::string(dest));

            if(it != mAllocatedHandles.end())
                return ret;
        }

        FileStream destStream;
        OPEN_STREAM(destStream, dest, "rb+");

        if(!destStream.fail())
        {
            destStream.close();
            mStream.close();

            OPEN_STREAM(mStream, dest, "rb+");
            if(!mStream.fail())
            {
                STATIC_LOCK_AUTO_MUTEX
                mAllocatedHandles.erase(mAllocatedHandles.find(mFileName));
                mActive = true;
                mFileName = dest;
                mStream.seek(0, OFS_SEEK_BEGIN);
                mStream.read((char*)&mHeader, sizeof(_Ofs::strFileHeader));
                ret = OFS_OK;
                mAllocatedHandles.insert(NameOfsHandleMap::value_type(std::string(dest), this));
            }
            else
            {
                mActive = false;
                mFileName = "";
                ret = OFS_INVALID_FILE;
            }
        }
        
        return ret;
    }

//------------------------------------------------------------------------------------------

    OfsResult _Ofs::defragFileSystemTo(const char *dest, LogCallBackFunction* logCallbackFunc)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::defragFileSystemTo, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsResult ret = OFS_INVALID_FILE;

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

                if(logCallbackFunc)
                    (*logCallbackFunc)(std::string("Defragmenting ").append(allFiles[i].name));

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

            if(logCallbackFunc)
                (*logCallbackFunc)(std::string("Defragmenting completed."));

            delete [] tmp_buffer;

            ret = OFS_OK;
        }

        return ret;
    }

//------------------------------------------------------------------------------------------

    ofs64 _Ofs::listFilesRecursive(const std::string& path, FileList& list)
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
    OfsResult _Ofs::addTrigger(void *_owner, _Ofs::CallBackType _type, _Ofs::CallBackFunction _func, void *_data)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::addTrigger, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        _Ofs::CallBackData tdata;
        tdata.owner = _owner;
        tdata.type = _type;
        tdata.func = _func;
        tdata.data = _data;

        mTriggers.push_back(tdata);

        return OFS_OK;
    }
//------------------------------------------------------------------------------------------
    void _Ofs::removeTrigger(void *_owner, _Ofs::CallBackType _type)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::removeTrigger, Operation called on an unmounted file system.");
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
    OfsResult _Ofs::addFileTrigger(const char *filename, void *_owner, _Ofs::CallBackType _type, _Ofs::CallBackFunction _func, void *_data)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::addFileTrigger, Operation called on an unmounted file system.");
            return OFS_IO_ERROR;
        }

        OfsEntryDesc *dirDesc = _getDirectoryDesc(filename);

        if(dirDesc == NULL)
            return OFS_FILE_NOT_FOUND;

        std::string fName = _extractFileName(filename);

        OfsEntryDesc *fileDesc = _getFileDesc(dirDesc, fName);

        if(fileDesc == NULL)
            return OFS_FILE_NOT_FOUND;
        
        _Ofs::CallBackData tdata;
        tdata.owner = _owner;
        tdata.type = _type;
        tdata.func = _func;
        tdata.data = _data;

        fileDesc->Triggers.push_back(tdata);

        return OFS_OK;
    }
//------------------------------------------------------------------------------------------
    void _Ofs::removeFileTrigger(const char *filename, void *_owner, _Ofs::CallBackType _type)
    {
        assert(_owner != 0);

        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::removeFileTrigger, Operation called on an unmounted file system.");
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
