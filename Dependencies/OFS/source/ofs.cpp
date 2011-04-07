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

#include "ofs.h"
#include <algorithm>

using namespace std;

#if defined( __WIN32__ ) || defined( _WIN32 )
#define OPEN_STREAM(a, b, c) a.open(b, c, SH_DENYWR)
#else
#define OPEN_STREAM(a, b, c) a.open(b, c)
#endif


namespace OFS
{
    STATIC_AUTO_MUTEX_DECL(_Ofs)

    _Ofs::NameOfsHandleMap _Ofs::mAllocatedHandles;

    const UUID UUID_ZERO(0,0,0,0,0,0,0,0,0,0,0);

    const unsigned int MAX_BUFFER_SIZE = (16 * 1024 * 1024);

//------------------------------------------------------------------------------

    bool BlockCompare ( _Ofs::BlockData elem1, _Ofs::BlockData elem2 )
    {
        if(elem1.Length < elem2.Length)
            return true;
        else if(elem1.Length == elem2.Length)
            return (elem1.Start < elem2.Start);

        return false;
    }

//------------------------------------------------------------------------------

    void OFSHANDLE::_prepareReadWritePointers(bool append)
    {
        assert(mEntryDesc != NULL);

        mReadPos = 0;
        mReadBlock = 0;
        mReadBlockEnd = mEntryDesc->UsedBlocks[0].Start + mEntryDesc->UsedBlocks[0].Length;
        mRealReadPos = mEntryDesc->UsedBlocks[0].Start + sizeof(_Ofs::strMainEntryHeader);
        mWritePos = 0;
        mWriteBlock = 0;
        mWriteBlockEnd = mEntryDesc->UsedBlocks[0].Start + mEntryDesc->UsedBlocks[0].Length;
        mRealWritePos = mEntryDesc->UsedBlocks[0].Start + sizeof(_Ofs::strMainEntryHeader);

        if(append)
            _setWritePos(mEntryDesc->FileSize);
    }

//------------------------------------------------------------------------------

    void OFSHANDLE::_setWritePos(unsigned int value)
    {
        if(value > mEntryDesc->FileSize)
            value = mEntryDesc->FileSize;

        if(mWritePos != value)
        {
            mWritePos = value;
            unsigned int block_size = mEntryDesc->UsedBlocks[0].Length - sizeof(_Ofs::strMainEntryHeader);
            int i = 0;
            unsigned int max_i = mEntryDesc->UsedBlocks.size();
            while(block_size <= value)
            {
                value -= block_size;
                i++;

                if(i != max_i)
                {
                    block_size = mEntryDesc->UsedBlocks[i].Length - sizeof(_Ofs::strExtendedEntryHeader);
                    mWriteBlockEnd = mEntryDesc->UsedBlocks[i].Start + mEntryDesc->UsedBlocks[i].Length;
                }
            }

            mWriteBlock = i;
            if(i == 0)
                mRealWritePos = mEntryDesc->UsedBlocks[i].Start + sizeof(_Ofs::strMainEntryHeader) + value;
            else if(i == max_i)
                mRealWritePos = mEntryDesc->UsedBlocks[i - 1].Start + mEntryDesc->UsedBlocks[i - 1].Length;
            else
                mRealWritePos = mEntryDesc->UsedBlocks[i].Start + sizeof(_Ofs::strExtendedEntryHeader) + value;
        }
    }

//------------------------------------------------------------------------------

    void OFSHANDLE::_setReadPos(unsigned int value)
    {
        if(value > mEntryDesc->FileSize)
            value = mEntryDesc->FileSize;

        if(mReadPos != value)
        {
            mReadPos = value;
            unsigned int block_size = mEntryDesc->UsedBlocks[0].Length - sizeof(_Ofs::strMainEntryHeader);
            int i = 0;
            unsigned int max_i = mEntryDesc->UsedBlocks.size();
            while(block_size <= value)
            {
                value -= block_size;
                i++;

                if(i != max_i)
                {
                    block_size = mEntryDesc->UsedBlocks[i].Length - sizeof(_Ofs::strExtendedEntryHeader);
                    mReadBlockEnd = mEntryDesc->UsedBlocks[i].Start + mEntryDesc->UsedBlocks[i].Length;
                }
            }

            mReadBlock = i;
            if(i == 0)
                mRealReadPos = mEntryDesc->UsedBlocks[i].Start + sizeof(_Ofs::strMainEntryHeader) + value;
            else if(i == max_i)
                mRealReadPos = mEntryDesc->UsedBlocks[i - 1].Start + mEntryDesc->UsedBlocks[i - 1].Length;
            else
                mRealReadPos = mEntryDesc->UsedBlocks[i].Start + sizeof(_Ofs::strExtendedEntryHeader) + value;

        }
    }


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    _Ofs::_Ofs() : mActive(false), mUseCount(0), mRecoveryMode(false)
    {
        mFileName = "";
        mRootDir.Id = -1;
        mRootDir.ParentId = -1;
        mRootDir.Flags = OFS_DIR;
        mRootDir.FileSize = 0;
        mRootDir.Parent = NULL;
    }

//------------------------------------------------------------------------------

    _Ofs::~_Ofs()
    {
    }

//------------------------------------------------------------------------------

    void _Ofs::_incUseCount()
    {
        LOCK_AUTO_MUTEX

        ++mUseCount;
    }

//------------------------------------------------------------------------------

    void _Ofs::_decUseCount()
    {
        std::string name;
        bool doDelete = false;

        //This scope is needed to make sure deletion occurs after
        //SCOPED_LOCK terminates
        {
            LOCK_AUTO_MUTEX

            --mUseCount;
            if(mUseCount < 1)
            {
                name = mFileName;
                doDelete = true;

                if(mActive)
                    _unmount();
            }
        }

        if(doDelete)
        {
            STATIC_LOCK_AUTO_MUTEX
            
            delete this;

            NameOfsHandleMap::iterator it = mAllocatedHandles.find(name);
            
            if(it != mAllocatedHandles.end())
                mAllocatedHandles.erase(it);
        }
    }

//------------------------------------------------------------------------------

    bool _Ofs::isActive()
    {
        LOCK_AUTO_MUTEX

        return mActive; 
    }

//------------------------------------------------------------------------------

    void _Ofs::_getFileSystemStatsRecursive(OfsEntryDesc *desc, FileSystemStats& stats)
    {
        if(desc->Flags & OFS_FILE)
            stats.NumFiles++;
        else
        {
            stats.NumDirectories++;

            for(unsigned int c = 0;c < desc->Children.size();c++)
                _getFileSystemStatsRecursive(desc->Children[c], stats);
        }

        stats.UsedSpace += desc->FileSize;

        unsigned int total_alloc = 0;
        
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

        mStream.clear();
        mStream.seekg(0, fstream::end);

        stats.ActualUsedSpace += sizeof(strFileHeader);
        stats.TotalFileSize = mStream.tellg();

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    void _Ofs::_deallocateChildren(OfsEntryDesc* parent)
    {
        for(unsigned int i = 0;i < parent->Children.size();i++)
        {
            _deallocateChildren(parent->Children[i]);
            delete parent->Children[i];
        }

        parent->Children.clear();
    }

//------------------------------------------------------------------------------

    void _Ofs::_markUnused(BlockData data)
    {
        data.Type |= OFS_FREE_BLOCK;
        data.NextBlock = 0;
        mStream.clear();
        mStream.seekp(data.Start - sizeof(strBlockHeader) + offsetof(strBlockHeader, Type), fstream::beg);
        mStream.write((char*)&(data.Type), sizeof(unsigned int));
        mFreeBlocks.push_back(data);
    }

//------------------------------------------------------------------------------

    void _Ofs::_allocateFileBlock(OfsEntryDesc *desc, strMainEntryHeader& mainEntry, unsigned int block_size, unsigned int data_size, const char *data)
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
               mStream.clear();
               mStream.seekp(blockData.Start - sizeof(strBlockHeader), fstream::beg);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&mainEntry, sizeof(strMainEntryHeader));
               
               blHeader.Type = OFS_FREE_BLOCK;
               blHeader.Length = mFreeBlocks[alloc_pos].Length;
               mStream.seekp(block_size - sizeof(strMainEntryHeader), fstream::cur);
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
               mStream.clear();
               mStream.seekp(blockData.Start - sizeof(strBlockHeader), fstream::beg);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&mainEntry, sizeof(strMainEntryHeader));
               desc->UsedBlocks.push_back(blockData);
           }
        }
        else
        {
            mStream.clear();
            mStream.seekp(0, fstream::end);
            blockData.Type = OFS_MAIN_BLOCK;
            blockData.NextBlock = 0;
            blockData.Length = block_size;
            blockData.Start = mStream.tellp();
            blockData.Start += sizeof(strBlockHeader);
            
            blHeader.Length = blockData.Length;
            blHeader.Type = OFS_MAIN_BLOCK;
            
            mStream.write((char*)&blHeader, sizeof(strBlockHeader));
            mStream.write((char*)&mainEntry, sizeof(strMainEntryHeader));
            fill_needed = true;

            desc->UsedBlocks.push_back(blockData);
        }

        mStream.clear();
        mStream.seekp(blockData.Start + sizeof(strMainEntryHeader), fstream::beg);

        block_size -= sizeof(strMainEntryHeader); 
        
        if(data != NULL)
        {
            assert(block_size >= data_size);

            mStream.write(data, data_size);
            if(block_size > 0)
                block_size -= data_size; 
        }

        if(fill_needed)
        {
            char dummy[] = {0};
            for(unsigned int i = 0;i < block_size;i++)
            {
                mStream.write(dummy, 1);
            }
        }

        _writeHeader();
    }

//------------------------------------------------------------------------------

    void _Ofs::_allocateExtendedFileBlock(OfsEntryDesc *desc, unsigned int block_size, unsigned int data_size, const char *data)
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
               mStream.clear();
               mStream.seekp(blockData.Start - sizeof(strBlockHeader), fstream::beg);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&extendedHeader, sizeof(strExtendedEntryHeader));
               
               blHeader.Type = OFS_FREE_BLOCK;
               blHeader.Length = mFreeBlocks[alloc_pos].Length;
               mStream.seekp(block_size - sizeof(strExtendedEntryHeader), fstream::cur);
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
               mStream.clear();
               mStream.seekp(blockData.Start - sizeof(strBlockHeader), fstream::beg);
               mStream.write((char*)&blHeader, sizeof(strBlockHeader));
               mStream.write((char*)&extendedHeader, sizeof(strExtendedEntryHeader));
               desc->UsedBlocks.push_back(blockData);
           }
        }
        else
        {
            mStream.clear();
            mStream.seekp(0, fstream::end);
            blockData.Type = OFS_EXTENDED_BLOCK;
            blockData.NextBlock = 0;
            blockData.Length = block_size;
            blockData.Start = mStream.tellp();
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
        
        mStream.clear();
        if(prev_block == 0)
            mStream.seekp(desc->UsedBlocks[prev_block].Start + offsetof(strMainEntryHeader, NextBlock), fstream::beg);
        else
            mStream.seekp(desc->UsedBlocks[prev_block].Start + offsetof(strExtendedEntryHeader, NextBlock), fstream::beg);

        mStream.write((char*)&(desc->UsedBlocks[prev_block].NextBlock), sizeof(unsigned int));

        mStream.clear();
        mStream.seekp(blockData.Start + sizeof(strExtendedEntryHeader), fstream::beg);

        block_size -= sizeof(strExtendedEntryHeader); 
        
        if(data != NULL)
        {
            assert(block_size >= data_size);

            mStream.write(data, data_size);
            if(block_size > 0)
                block_size -= data_size; 
        }

        if(fill_needed)
        {
            char dummy[] = {0};
            for(unsigned int i = 0;i < block_size;i++)
            {
                mStream.write(dummy, 1);
            }
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
        
        if(op & OFS_MOUNT_OPEN)
        {
            OPEN_STREAM(mStream, mFileName.c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
            if(!mStream.fail() && mStream.is_open())
            {
                mActive = true;
                ret = _readHeader();
            }
        }
        else
        {
            OPEN_STREAM(mStream, mFileName.c_str(), fstream::in | fstream::out | fstream::binary | fstream::trunc);
            if(!mStream.fail())
            {
                mActive = true;
                memset(&mHeader, 0, sizeof(_Ofs::strFileHeader));

                __time64_t signature_time = _time64(NULL);
                unsigned int * signature = (unsigned int *)(&signature_time);

                mHeader.ID[0] = 'O';mHeader.ID[1] = 'F';mHeader.ID[2] = 'S';mHeader.ID[3] = '1';
                
                mHeader.VERSION[0] = VERSION_MAJOR_0;
                mHeader.VERSION[1] = VERSION_MAJOR_1;
                mHeader.VERSION[2] = VERSION_MINOR;
                mHeader.VERSION[3] = VERSION_FIX;
                mHeader.BLOCK_HEADER_SIG[0] = signature[0];
                mHeader.BLOCK_HEADER_SIG[1] = signature[0] ^ 0xFFFFFFFF;

                mHeader.LAST_ID = 0;
                mRootDir.CreationTime = _time64( NULL );
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

        if(mStream.is_open())
        {
            mStream.flush();
            mStream.close();
        }

        memset(&mHeader, 0, sizeof(strFileHeader));

        mFileName = "";
        mRootDir.UsedBlocks.clear();
        _deallocateChildren(&mRootDir);

        mFreeBlocks.clear();
        mActiveFiles.clear();
        mUuidMap.clear();
        mTriggers.clear();
        mRecoveryMode = false;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::_readHeader()
    {
        assert(mActive);

        mStream.seekg(0, fstream::end);
        unsigned int file_size = mStream.tellg();

        int HeaderSize = sizeof(strFileHeader);
        mStream.seekg(0, fstream::beg);
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

        unsigned int current_loc = mStream.tellg();

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

        DirMap.insert(IdDescMap::value_type(-1, &mRootDir));

        unsigned int skipped = 0;

        while((current_loc = mStream.tellg()) < file_size)
        {
            mStream.read((char*)&blHeader, sizeof(strBlockHeader));
            if(blHeader.Signature[0] != mHeader.BLOCK_HEADER_SIG[0] || blHeader.Signature[1] != mHeader.BLOCK_HEADER_SIG[1])
            {
                if(mRecoveryMode)
                {
                    unsigned int pos = mStream.tellg();
                    mStream.clear();
                    mStream.seekg( pos + 1 - sizeof(strBlockHeader));
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
                    unsigned int pos = mStream.tellg();
                    mStream.clear();

                    //TODO : We may want to do some corrections here, like creating a free block signature

                    mStream.seekg( pos );
                }

                skipped = 0;
            }

            if(blHeader.Type & OFS_FREE_BLOCK)
            {
                blockData.Type = blHeader.Type;
                blockData.Start = mStream.tellg();
                blockData.Length = blHeader.Length;
                blockData.NextBlock = 0;

                mFreeBlocks.push_back(blockData);
                mStream.seekg(blockData.Length, fstream::cur);
            }
            else if(blHeader.Type == OFS_MAIN_BLOCK)
            {
                mStream.read((char*)&mainEntry, sizeof(strMainEntryHeader));
                blockData.Type = blHeader.Type;
                blockData.Start = mStream.tellg();
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
                    
                entryDesc->Id = mainEntry.Id;
                entryDesc->ParentId = mainEntry.ParentId;
                entryDesc->Flags = mainEntry.Flags;
                entryDesc->Name = mainEntry.Name;
                entryDesc->FileSize = mainEntry.FileSize;
                entryDesc->CreationTime = mainEntry.CreationTime;
                entryDesc->UseCount = 0;
                entryDesc->WriteLocked = false;
                entryDesc->Uuid = mainEntry.Uuid;

                if(mainEntry.Uuid != UUID_ZERO)
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

                entryDesc->Parent = it->second;
                it->second->Children.push_back(entryDesc);

                mStream.seekg(blockData.Length - sizeof(strMainEntryHeader), fstream::cur);
            }
            else if(blHeader.Type == OFS_EXTENDED_BLOCK)
            {
                mStream.read((char*)&extendedEntry, sizeof(strExtendedEntryHeader));
 
                blockData.Type = blHeader.Type;
                blockData.Start = mStream.tellg();
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

                mStream.seekg(blockData.Length - sizeof(strExtendedEntryHeader), fstream::cur);
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

        mStream.clear();
        mStream.seekp(0, fstream::beg);
        
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


        dir->Id = mHeader.LAST_ID++;
        dir->ParentId = parent->Id;
        dir->Flags = OFS_DIR;
        dir->Name = name;
        dir->FileSize = 0;
        dir->Parent = parent;
        dir->CreationTime = _time64( NULL );
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
               mStream.clear();
               mStream.seekp(dirData.Start - sizeof(strBlockHeader), fstream::beg);
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
               mStream.clear();
               mStream.seekp(dirData.Start - sizeof(strBlockHeader), fstream::beg);
               mStream.write((char*)&fileData, sizeof(strBlockHeader));
               mStream.write((char*)&fileHeader, sizeof(strMainEntryHeader));
               dir->UsedBlocks.push_back(dirData);
           }
        }
        else
        {
            mStream.clear();
            mStream.seekp(0, fstream::end);
            dirData.Type = OFS_MAIN_BLOCK;
            dirData.NextBlock = 0;
            dirData.Length = sizeof(strMainEntryHeader);
            dirData.Start = mStream.tellp();
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

    _Ofs::OfsEntryDesc* _Ofs::_createFile(OfsEntryDesc *parent, const std::string& name, unsigned int file_size, const UUID& uuid, unsigned int data_size, const char *data)
    {
        assert(parent != NULL);
        
        OfsEntryDesc *file = new OfsEntryDesc();

        file->Id = mHeader.LAST_ID++;
        file->ParentId = parent->Id;
        file->Flags = OFS_FILE;
        file->Name = name;
        file->FileSize = file_size;
        file->Parent = parent;
        file->CreationTime = _time64( NULL );
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
        fileHeader.NextBlock = 0;
        fileHeader.FileSize = file->FileSize;
        fileHeader.CreationTime = file->CreationTime;

        int sz = file->Name.length();
        if(sz > 251)
            sz = 251;
        memcpy(fileHeader.Name, file->Name.c_str(), sz);
        fileHeader.Name[sz] = 0;
        fileHeader.Uuid = file->Uuid;

        unsigned int blockToAlloc = file_size;

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

        if(dirDesc->Children.size() > 0 && !force)
            return OFS_DIRECTORY_NOT_EMPTY;

        if(dirDesc->Parent != NULL)
        {
            OfsResult ret = _deleteDirectory(dirDesc);

            mStream.flush();

            std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);

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
            _createDirectory(curDesc, dir, uuid);

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

        std::string nName = _extractFileName(newname);

        int sz = nName.length();
        if(sz > 251)
            nName.erase(251, sz - 251);

        if(_getFileDesc(dirDesc, nName) != NULL || fileDesc->WriteLocked || (fileDesc->Flags & OFS_READONLY))
            return OFS_ACCESS_DENIED;

        fileDesc->Name = nName;

        mStream.clear();
        mStream.seekp(fileDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Name), fstream::beg);
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

            mStream.clear();
            mStream.seekp(dirDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Name), fstream::beg);
            mStream.write(dirDesc->Name.c_str(), dirDesc->Name.length() + 1);

            mStream.flush();

            return OFS_OK;
        }
        else
            return OFS_INVALID_PATH;
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
                if((fileDesc->UseCount > 0) || (fileDesc->Flags & OFS_READONLY))
                    return OFS_ACCESS_DENIED;

                fileDesc->WriteLocked = true;

                if(!(open_mode & OFS_APPEND))
                {
                    for(unsigned int i = 1;i < fileDesc->UsedBlocks.size();i++)
                    {
                        _markUnused(fileDesc->UsedBlocks[i]);
                    }

                    for(unsigned int i = 1;i < fileDesc->UsedBlocks.size();i++)
                    {
                        fileDesc->UsedBlocks.erase(fileDesc->UsedBlocks.begin() + 1);
                    }

                    fileDesc->UsedBlocks[0].NextBlock = 0;
                    fileDesc->FileSize = 0;

                    strMainEntryHeader fileHeader;

                    fileHeader.Id = fileDesc->Id;
                    fileHeader.ParentId = fileDesc->ParentId;
                    fileHeader.Flags = fileDesc->Flags;
                    fileHeader.NextBlock = 0;
                    fileHeader.FileSize = 0;
                    fileHeader.CreationTime = fileDesc->CreationTime;
                    int sz = fileDesc->Name.length();
                    if(sz > 251)
                        sz = 251;
                    memcpy(fileHeader.Name, fileDesc->Name.c_str(), sz);
                    fileHeader.Name[sz] = 0;
                    fileHeader.Uuid = fileDesc->Uuid;

                    mStream.clear();
                    mStream.seekp(fileDesc->UsedBlocks[0].Start, fstream::beg);
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
        handle._prepareReadWritePointers((open_mode & OFS_APPEND) != 0);

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
            if((fileDesc->UseCount > 0) || (fileDesc->Flags & OFS_READONLY))
                return OFS_ACCESS_DENIED;

            fileDesc->WriteLocked = true;

            if(!(open_mode & OFS_APPEND))
            {
                for(unsigned int i = 1;i < fileDesc->UsedBlocks.size();i++)
                {
                    _markUnused(fileDesc->UsedBlocks[i]);
                }

                for(unsigned int i = 1;i < fileDesc->UsedBlocks.size();i++)
                {
                    fileDesc->UsedBlocks.erase(fileDesc->UsedBlocks.begin() + 1);
                }

                fileDesc->UsedBlocks[0].NextBlock = 0;
                fileDesc->FileSize = 0;

                strMainEntryHeader fileHeader;

                fileHeader.Id = fileDesc->Id;
                fileHeader.ParentId = fileDesc->ParentId;
                fileHeader.Flags = fileDesc->Flags;
                fileHeader.NextBlock = 0;
                fileHeader.FileSize = 0;
                fileHeader.CreationTime = fileDesc->CreationTime;
                int sz = fileDesc->Name.length();
                if(sz > 251)
                    sz = 251;
                memcpy(fileHeader.Name, fileDesc->Name.c_str(), sz);
                fileHeader.Name[sz] = 0;
                fileHeader.Uuid = fileDesc->Uuid;

                mStream.clear();
                mStream.seekp(fileDesc->UsedBlocks[0].Start, fstream::beg);
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
        handle._prepareReadWritePointers((open_mode & OFS_APPEND) != 0);

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::createFileUUID(OFSHANDLE& handle, const char *filename, const UUID& uuid, unsigned int file_size, unsigned int data_size, const char *data)
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
        handle._prepareReadWritePointers(true);

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

    OfsResult _Ofs::truncateFile(OFSHANDLE& handle, int file_size)
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

        if(!(handle.mAccessFlags & OFS_WRITE))
            return OFS_ACCESS_DENIED;

        unsigned int trunc_pos = file_size;

        if(file_size < 0)
            trunc_pos = handle.mWritePos;

        if(trunc_pos < handle.mEntryDesc->FileSize)
        {
            OfsEntryDesc *desc = handle.mEntryDesc;
            desc->FileSize = 0;
            handle._setWritePos(trunc_pos);

            for(unsigned int i = handle.mWriteBlock + 1;i < desc->UsedBlocks.size();i++)
            {
                _markUnused(desc->UsedBlocks[i]);
            }

            while((handle.mWriteBlock + 1) < desc->UsedBlocks.size())
                desc->UsedBlocks.erase(desc->UsedBlocks.begin() + (handle.mWriteBlock + 1));

            std::sort(mFreeBlocks.begin(), mFreeBlocks.end(), BlockCompare);

            mStream.clear();
            mStream.seekp(desc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, FileSize), fstream::beg);
            mStream.write((char*)&trunc_pos, sizeof(unsigned int)); 

            mStream.clear();
            if(handle.mWriteBlock == 0)
                mStream.seekp(desc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, NextBlock), fstream::beg);
            else
                mStream.seekp(desc->UsedBlocks[handle.mWriteBlock].Start + offsetof(strExtendedEntryHeader, NextBlock), fstream::beg);

            trunc_pos = 0;
            mStream.write((char*)&trunc_pos, sizeof(unsigned int)); 
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

        mStream.clear();
        mStream.seekp(file->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Flags), fstream::beg);
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

        if(!(handle.mAccessFlags & OFS_READ))
            return OFS_ACCESS_DENIED;

        OfsEntryDesc *desc = handle.mEntryDesc;

        if(desc->FileSize < (handle.mReadPos + length))
            length = desc->FileSize - handle.mReadPos;

        if(length > 0)
        {
            unsigned int can_read = handle.mReadBlockEnd - handle.mRealReadPos;
            unsigned int tmp_len = length;

            mStream.clear();

            bool done = false;
            while(!done)
            {
                if(can_read >= tmp_len)
                {
                    mStream.seekg(handle.mRealReadPos, fstream::beg);
                    mStream.read(dest, tmp_len);
                    handle._setReadPos(handle.mReadPos + tmp_len);
                    done = true;
                }
                else
                {
                    mStream.seekg(handle.mRealReadPos, fstream::beg);
                    mStream.read(dest, can_read);
                    handle._setReadPos(handle.mReadPos + can_read);
                    tmp_len -= can_read;
                    dest += can_read;

                    can_read = handle.mReadBlockEnd - handle.mRealReadPos;
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

        mStream.clear();

        OfsEntryDesc *desc = handle.mEntryDesc;
        if(length > 0)
        {
            unsigned int total_alloc = desc->UsedBlocks[0].Length - sizeof(strMainEntryHeader);
            for(unsigned int i = 1;i < desc->UsedBlocks.size();i++)
                total_alloc += desc->UsedBlocks[i].Length - sizeof(strExtendedEntryHeader);

            total_alloc -= handle.mWritePos;
            unsigned int output_amount = length;

            if(total_alloc < length)
            {
                unsigned int space_needed = length - total_alloc;
                unsigned int alloc_size =  sizeof(strExtendedEntryHeader);
                
                if(space_needed < 1024)
                    alloc_size += 1024;
                else
                    alloc_size += space_needed;

                _allocateExtendedFileBlock(desc, alloc_size, length - total_alloc, (src + total_alloc));

                output_amount = total_alloc; 
            }

            unsigned int write_pos_save = handle.mWritePos;

            unsigned int can_write = handle.mWriteBlockEnd - handle.mRealWritePos;

            mStream.clear();

            bool done = false;
            while(!done)
            {
                if(can_write >= output_amount)
                {
                    mStream.seekp(handle.mRealWritePos, fstream::beg);
                    mStream.write(src, output_amount);
                    handle._setWritePos(handle.mWritePos + output_amount);
                    done = true;
                }
                else
                {
                    mStream.seekp(handle.mRealWritePos, fstream::beg);
                    mStream.write(src, can_write);
                    handle._setWritePos(handle.mWritePos + can_write);
                    output_amount -= can_write;
                    src += can_write;

                    can_write = handle.mWriteBlockEnd - handle.mRealWritePos;
                }
            }

            desc->FileSize += length;
            mStream.seekp(desc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, FileSize), fstream::beg);
            mStream.write((char*)&(desc->FileSize), sizeof(unsigned int));
            mStream.flush();

            handle._setWritePos(write_pos_save + length);
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    unsigned int _Ofs::seekr(OFSHANDLE& handle, int pos, SeekDirection dir)
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
        case OFS_SEEK_CURRENT:pos += handle.mReadPos;
                              if(pos < 0) pos = 0;
                              break;
        case OFS_SEEK_END:pos += handle.mEntryDesc->FileSize;
                          if(pos < 0) pos = 0;
                          break;
        }

        handle._setReadPos(pos);
        return handle.mReadPos;
    }

//------------------------------------------------------------------------------

    unsigned int _Ofs::seekw(OFSHANDLE& handle, int pos, SeekDirection dir)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::seekw, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::seekw, Supplied OfsHandle is not valid.");
            return 0;
        }

        switch(dir)
        {
        case OFS_SEEK_BEGIN:if(pos < 0) pos = 0;
                            break;
        case OFS_SEEK_CURRENT:pos += handle.mWritePos;
                              if(pos < 0) pos = 0;
                              break;
        case OFS_SEEK_END:pos += handle.mEntryDesc->FileSize;
                          if(pos < 0) pos = 0;
                          break;
        }

        handle._setWritePos(pos);
        return handle.mWritePos;
    }

//------------------------------------------------------------------------------

    unsigned int _Ofs::tellr(OFSHANDLE& handle)
    {
        LOCK_AUTO_MUTEX

        if(!mActive)
        {
            OFS_EXCEPT("_Ofs::tellr, Operation called on an unmounted file system.");
            return 0;
        }

        if(!handle._valid())
        {
            OFS_EXCEPT("_Ofs::tellr, Supplied OfsHandle is not valid.");
            return 0;
        }

        return handle.mReadPos;
    }

//------------------------------------------------------------------------------

    unsigned int _Ofs::tellw(OFSHANDLE& handle)
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

        return handle.mWritePos;
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

        return (handle.mReadPos >= handle.mEntryDesc->FileSize);
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

        entry.name = handle.mEntryDesc->Name;
        entry.flags = handle.mEntryDesc->Flags;
        entry.uuid = handle.mEntryDesc->Uuid;
        entry.file_size = handle.mEntryDesc->FileSize;
        entry.create_time = handle.mEntryDesc->CreationTime;
        entry.modified_time = handle.mEntryDesc->CreationTime;

        return OFS_OK;
    }

//------------------------------------------------------------------------------

    OfsResult _Ofs::getCreationTime(const char *filename, __time64_t& creation_time)
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

    OfsResult _Ofs::getModificationTime(const char *filename, __time64_t& mod_time)
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

    OfsResult _Ofs::getFileSize(const char *filename, unsigned int& size)
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

        if(fileDesc != NULL)
        {
            if(fileDesc->WriteLocked)
                return OFS_ACCESS_DENIED;

            _setFileFlags(fileDesc, flags);
            mStream.flush();
            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
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

        if(fileDesc != NULL)
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
                mStream.seekp(fileDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Uuid), fstream::beg);
                mStream.write((char*)&(fileDesc->Uuid), sizeof(UUID));
                mStream.flush();
            }
            
            return OFS_OK;
        }
        else
            return OFS_FILE_NOT_FOUND;
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
            mStream.seekp(handle.mEntryDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Uuid), fstream::beg);
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
                mStream.seekp(dirDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, Uuid), fstream::beg);
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

    OfsResult _Ofs::getCreationTime(OFSHANDLE& handle, __time64_t& creation_time)
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

    OfsResult _Ofs::getModificationTime(OFSHANDLE& handle, __time64_t& mod_time) 
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

    OfsResult _Ofs::getFileSize(OFSHANDLE& handle, unsigned int& size)
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

    OfsResult _Ofs::_mount(_Ofs **_ptr, const char *file, unsigned int op)
    {
        STATIC_LOCK_AUTO_MUTEX

        *_ptr = 0;

        NameOfsHandleMap::iterator it = mAllocatedHandles.find(std::string(file));

        if(it == mAllocatedHandles.end())
        {
            *_ptr = new _Ofs();
            (*_ptr)->_incUseCount();
            
            OfsResult ret = (*_ptr)->_mount(file, op);

            if(ret != OFS_OK)
            {
                (*_ptr)->_decUseCount();
                *_ptr = 0;
                return ret;
            }

            mAllocatedHandles.insert(NameOfsHandleMap::value_type(std::string(file), *_ptr));
        }
        else
        {
            if(op == OFS_MOUNT_CREATE)
            {
                OFS_EXCEPT("_Ofs::mount, Cannot overwrite an archive in use.");

                return OFS_ACCESS_DENIED;
            }

            *_ptr = it->second;
            (*_ptr)->_incUseCount();
        }

        return OFS_OK;
    }

//------------------------------------------------------------------------------------------

    OfsResult _Ofs::copyFile(const char *src, const char *dest)
    {
        LOCK_AUTO_MUTEX

        OFSHANDLE srcHandle, destHandle;
        OfsResult ret = OFS_OK;

        if((ret = openFile(srcHandle, src)) != OFS_OK)
            return ret;

        unsigned int file_size = 0;

        getFileSize(srcHandle, file_size);

        if(file_size < 1024 * 1024 * 64)
        {
            UUID uuid;

            char *buffer = new char[file_size];
            read(srcHandle, buffer, file_size);
            closeFile(srcHandle);

            ret = createFileUUID(destHandle, dest, UUID_ZERO, file_size, file_size, buffer);
            
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
            return ret;

        OfsEntryDesc *srcDesc = srcHandle.mEntryDesc;

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
            mStream.clear();
            mStream.seekp(srcDesc->UsedBlocks[0].Start + offsetof(strMainEntryHeader, ParentId), fstream::beg);
            mStream.write((char*)&(srcDesc->ParentId), sizeof(unsigned int));
            mStream.flush();

            ret = OFS_OK;
        }

        return ret;
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

        std::fstream destStream;
        OPEN_STREAM(destStream, dest, fstream::in | fstream::out | fstream::binary | fstream::trunc);

        if(!destStream.fail())
        {
            mStream.clear();
            mStream.seekg(0, fstream::end);
            unsigned int total_amount = mStream.tellg();

            mStream.seekg(0, fstream::beg);

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
                    mStream.read(buffer, total_amount);
                    destStream.write(buffer, total_amount);
                    total_amount = 0;
                }
            }

            delete [] buffer;

            destStream.close();
            mStream.close();

            OPEN_STREAM(mStream, dest, fstream::in | fstream::out | fstream::binary | fstream::ate);
            if(!mStream.fail() && mStream.is_open())
            {
                mActive = true;
                mFileName = dest;
                ret = OFS_OK;
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

        std::fstream destStream;
        OPEN_STREAM(destStream, dest, fstream::in | fstream::out | fstream::binary | fstream::ate);

        if(!destStream.fail())
        {
            destStream.close();
            mStream.close();

            OPEN_STREAM(mStream, dest, fstream::in | fstream::out | fstream::binary | fstream::ate);
            if(!mStream.fail() && mStream.is_open())
            {
                mActive = true;
                mFileName = dest;
                mStream.seekg(0, fstream::beg);
                mStream.read((char*)&mHeader, sizeof(_Ofs::strFileHeader));
                ret = OFS_OK;
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

    OfsResult _Ofs::defragFileSystemTo(const char *dest)
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

            unsigned int output_amount = 0;

            char *tmp_buffer = new char[MAX_BUFFER_SIZE];

            for(unsigned int i = 0;i < allFiles.size();i++)
            {
                if(allFiles[i].flags & OFS::OFS_DIR)
                {
                    destFile->createDirectoryUUID(allFiles[i].name.c_str(), allFiles[i].uuid);
                }
                else
                {
                    std::string file_ofs_path = allFiles[i].name;

                    try
                    {
                        OFS::OfsResult ret = openFile(in_handle, file_ofs_path.c_str());
                        if(ret != OFS::OFS_OK)
                            continue;

                        unsigned int total = allFiles[i].file_size;

                        bool dest_file_created = false;

                        while(total > 0)
                        {
                            if(total < MAX_BUFFER_SIZE)
                            {
                                read(in_handle, tmp_buffer, total);
                                if(dest_file_created)
                                    destFile->write(out_handle, tmp_buffer, total);
                                else
                                    destFile->createFileUUID(out_handle, file_ofs_path.c_str(), allFiles[i].uuid, total, total, tmp_buffer);

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

            ret = OFS_OK;
        }

        return ret;
    }

//------------------------------------------------------------------------------------------

    unsigned int _Ofs::listFilesRecursive(const std::string& path, FileList& list)
    {
        if(list.empty())
            list = listFiles(path.c_str());

        unsigned int list_max = list.size();
        unsigned int file_size = 0;
        
        OFS::FileList subList;


        for(unsigned int i = 0;i < list_max;i++)
        {
            list[i].name = path + list[i].name;
            file_size += list[i].file_size;

            if(list[i].flags & OFS::OFS_DIR)
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
    OfsPtr::~OfsPtr()
    {
        if(mPtr != 0)
            mPtr->_decUseCount();
        
        mPtr = 0;
    }

//------------------------------------------------------------------------------

    OfsResult OfsPtr::mount(const char *file, unsigned int op)
    {
        assert(mPtr == 0);

        return _Ofs::_mount(&mPtr, file, op);
    }

//------------------------------------------------------------------------------

    void OfsPtr::unmount()
    {
        if(mPtr != 0)
            mPtr->_decUseCount();
        
        mPtr = 0;
    }

//------------------------------------------------------------------------------

}