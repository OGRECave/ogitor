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

#define BLOCK_SIGNATURE_OLD_0 0x89ABCDEF
#define BLOCK_SIGNATURE_OLD_1 0xFEDCBA98

namespace OFS
{

    /* OFS File Header */
    struct HeaderV10
    {
        unsigned char ID[4];      /* The file identifier */
        unsigned char VERSION[4]; /* Version of the OFS File */
        unsigned int  LAST_ID;    /* The Last ID used for any entry */
        unsigned int  RESERVED;   /* RESERVED */
    };

    /* Holds information about an allocated block (for writing to file as block header) */
    struct FileBlockDataV10
    {
        unsigned int Signature[2]; /* Block Signature */
        unsigned int Type;         /* Block Type */
        unsigned int Length;       /* Length of the block in file not including this header */
    };

    /* Short entry header for EXTENDED BLOCKS */
    struct EntryHeaderShortV10
    {
        int          Id;           /* Id of the Owner Entry */
        unsigned int NextBlock;    /* Position of Next Block in file (owned by the same entry) */
    };

    /* Full Entry Header, contains all information needed for entry */
    struct EntryHeaderV10
    {
        int          Id;           /* Id of the Owner Entry */
        int          ParentId;     /* Id of the Owner Entry's Parent Directory, -1 if root directory */
        unsigned int Flags;        /* File Flags */
        time_t       CreationTime; /* Entry's Creation Time */
        unsigned int FileSize;     /* Entry's File Size, 0 for Directories */
        unsigned int NextBlock;    /* File Position of Next Block owned by this entry */
        char         Name[252];    /* Entry's Name */
    };

    /* Full Entry Header, contains all information needed for entry */
    struct EntryHeaderV11
    {
        int          Id;           /* Id of the Owner Entry */
        int          ParentId;     /* Id of the Owner Entry's Parent Directory, -1 if root directory */
        unsigned int Flags;        /* File Flags */
        UUID         Uuid;         /* UUID of Entry */
        time_t       CreationTime; /* Entry's Creation Time */
        unsigned int FileSize;     /* Entry's File Size, 0 for Directories */
        unsigned int NextBlock;    /* File Position of Next Block owned by this entry */
        char         Name[252];    /* Entry's Name */
    };
    
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

    OfsConverter::OfsConverter()
    {
    }

//------------------------------------------------------------------------------

    OfsConverter::~OfsConverter()
    {
    }

//------------------------------------------------------------------------------

    void OfsConverter::_deallocateChildren(_Ofs::OfsEntryDesc* parent)
    {
        for(unsigned int i = 0;i < parent->Children.size();i++)
        {
            _deallocateChildren(parent->Children[i]);
            delete parent->Children[i];
        }

        parent->Children.clear();
    }

//------------------------------------------------------------------------------

    bool OfsConverter::convert(std::string infile, std::string outfile)
    {
        unsigned int ehV10 = sizeof(EntryHeaderV10);
        unsigned int ehV11 = sizeof(EntryHeaderV11);
        unsigned int ehV12 = sizeof(_Ofs::strMainEntryHeader);
        
        OPEN_STREAM(mInStream, infile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
        if(mInStream.fail() || !mInStream.is_open())
            return false;

        HeaderV10 fsHeader;

        mInStream.seekg(0, fstream::beg);
        mInStream.read((char *)&fsHeader, sizeof(char) * 8);
        mInStream.close();


        if(fsHeader.ID[0] != 'O' || fsHeader.ID[1] != 'F' || fsHeader.ID[2] != 'S' || fsHeader.ID[3] != '1')
            return false;

        unsigned int current_version = ((VERSION_MAJOR_0 - '0') * 1000) + ((VERSION_MAJOR_1 - '0') * 100) + ((VERSION_MINOR - '0') * 10);
        unsigned int file_version = ((fsHeader.VERSION[0] - '0') * 1000) + ((fsHeader.VERSION[1] - '0') * 100) + ((fsHeader.VERSION[2] - '0') * 10);
        
        if(file_version == 1000)
            file_version = 100;

        file_version /= 10;
        current_version /= 10;

        if(file_version > current_version)
            return false;
        else
        {
            switch(file_version)
            {
            case 10:return _convertv10_v12(infile, outfile);
            case 11:return _convertv11_v12(infile, outfile);
            };
        }

        return false;
    }

//------------------------------------------------------------------------------

    bool OfsConverter::_convertv10_v12(std::string infile, std::string outfile)
    {
        OPEN_STREAM(mInStream, infile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
        if(mInStream.fail() || !mInStream.is_open())
            return false;

        HeaderV10 fsHeader;

        mInStream.seekg(0, fstream::end);
        unsigned int file_size = mInStream.tellg();

        int HeaderSize = sizeof(HeaderV10);
        mInStream.seekg(0, fstream::beg);
        mInStream.read((char *)&fsHeader, HeaderSize);

        unsigned int current_loc = mInStream.tellg();

        int max_id = -1000;

        FileBlockDataV10    fileBlockData;
        EntryHeaderV10      entryHeaderOld;
        EntryHeaderShortV10 entryHeaderShort;

        _Ofs::BlockData          blockData;
        _Ofs::strMainEntryHeader entryHeaderNew;
        _Ofs::IdDescMap::const_iterator it;
        _Ofs::IdDescMap::iterator dait;
        _Ofs::IdDescMap DirMap;
        _Ofs::IdDescMap FileMap;
        _Ofs::PosBlockDataMap extendedEntries;
        _Ofs::OfsEntryDesc RootDir;

        RootDir.Id = -1;
        RootDir.ParentId = -1;

        DirMap.insert(_Ofs::IdDescMap::value_type(-1, &RootDir));

        while((current_loc = mInStream.tellg()) < file_size)
        {
            mInStream.read((char*)&fileBlockData, sizeof(FileBlockDataV10));
            if(fileBlockData.Signature[0] != BLOCK_SIGNATURE_OLD_0 || fileBlockData.Signature[1] != BLOCK_SIGNATURE_OLD_1)
            {
                unsigned int pos = mInStream.tellg();
                mInStream.clear();
                mInStream.seekg( pos + 1 - sizeof(FileBlockDataV10));
                continue;
            }

            if(fileBlockData.Type & _Ofs::OFS_FREE_BLOCK)
            {
                mInStream.seekg(fileBlockData.Length, fstream::cur);
            }
            else if(fileBlockData.Type == _Ofs::OFS_MAIN_BLOCK)
            {
                mInStream.read((char*)&entryHeaderOld, sizeof(EntryHeaderV10));
                blockData.Type = fileBlockData.Type;
                blockData.Start = mInStream.tellg();
                blockData.Start -= sizeof(EntryHeaderV10);
                blockData.Length = fileBlockData.Length;
                blockData.NextBlock = entryHeaderOld.NextBlock;

                _Ofs::OfsEntryDesc *entryDesc = new _Ofs::OfsEntryDesc();

                if(entryHeaderOld.Flags & OFS_DIR)
                {
                    dait = DirMap.find(entryHeaderOld.Id);
                    if(dait == DirMap.end())
                        DirMap.insert(_Ofs::IdDescMap::value_type(entryHeaderOld.Id, entryDesc));
                    else
                    {
                        delete entryDesc;
                        entryDesc = dait->second;
                    }
                }
                else
                    FileMap.insert(_Ofs::IdDescMap::value_type(entryHeaderOld.Id, entryDesc));
 
                entryDesc->Id = entryHeaderOld.Id;
                entryDesc->ParentId = entryHeaderOld.ParentId;
                entryDesc->Flags = entryHeaderOld.Flags;
                entryDesc->Name = entryHeaderOld.Name;
                entryDesc->FileSize = entryHeaderOld.FileSize;
                entryDesc->CreationTime = entryHeaderOld.CreationTime;
                entryDesc->UseCount = 0;
                entryDesc->WriteLocked = false;
                entryDesc->Uuid = UUID_ZERO;

                it = DirMap.find(entryDesc->ParentId);
                if(it == DirMap.end())
                {
                    _Ofs::OfsEntryDesc *dirDesc = new _Ofs::OfsEntryDesc();
                    dirDesc->Id = entryDesc->ParentId;
                    dirDesc->Name = "Error_NoName";
                    dirDesc->Parent = 0;

                    it = DirMap.insert(_Ofs::IdDescMap::value_type(dirDesc->Id, dirDesc)).first;
                }

                entryDesc->UsedBlocks.push_back(blockData);
                entryDesc->Parent = it->second;
                it->second->Children.push_back(entryDesc);

                mInStream.seekg(blockData.Length - sizeof(EntryHeaderV10), fstream::cur);

                if(max_id < entryHeaderOld.Id)
                    max_id = entryHeaderOld.Id;
            }
            else if(fileBlockData.Type == _Ofs::OFS_EXTENDED_BLOCK)
            {
                mInStream.read((char*)&entryHeaderShort, sizeof(EntryHeaderShortV10));
 
                blockData.Type = fileBlockData.Type;
                blockData.Start = mInStream.tellg();
                blockData.Start -= sizeof(EntryHeaderShortV10);
                blockData.Length = fileBlockData.Length;
                blockData.NextBlock = entryHeaderShort.NextBlock;

                extendedEntries.insert(_Ofs::PosBlockDataMap::value_type(blockData.Start, blockData));
                mInStream.seekg(blockData.Length - sizeof(EntryHeaderShortV10), fstream::cur);
            }
            else
            {
                mInStream.close();
                return false;
            }
        }

        _Ofs::PosBlockDataMap::iterator pit;
        _Ofs::IdDescMap::const_iterator dit = FileMap.begin();

        while(dit != FileMap.end())
        {
            unsigned int next_block = dit->second->UsedBlocks[0].NextBlock;
            while(next_block != 0)
            {
                pit = extendedEntries.find(next_block);
                if(pit == extendedEntries.end())
                {
                    break;
                }

                dit->second->UsedBlocks.push_back(pit->second);

                next_block = pit->second.NextBlock;

                extendedEntries.erase(pit);
            }

            dit++;
        }

        OPEN_STREAM(mOutStream, outfile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::trunc);
        if(mOutStream.fail() || !mOutStream.is_open())
        {
            mInStream.close();
            return false;
        }

        _Ofs::strFileHeader newHeader;

        memset(&newHeader, 0, sizeof(_Ofs::strFileHeader));

        time_t signature_time = time(NULL);
        unsigned int * signature = (unsigned int *)(&signature_time);

        newHeader.ID[0] = 'O';newHeader.ID[1] = 'F';newHeader.ID[2] = 'S';newHeader.ID[3] = '1';
        newHeader.VERSION[0] = VERSION_MAJOR_0;
        newHeader.VERSION[1] = VERSION_MAJOR_1;
        newHeader.VERSION[2] = VERSION_MINOR;
        newHeader.VERSION[3] = VERSION_FIX;
        newHeader.BLOCK_HEADER_SIG[0] = signature[0];
        newHeader.BLOCK_HEADER_SIG[1] = signature[0] ^ 0xFFFFFFFF;
        newHeader.LAST_ID = ++max_id;

        
        mOutStream.write((char *)&newHeader, sizeof(_Ofs::strFileHeader));

        dit = DirMap.begin();

        _Ofs::strBlockHeader blHeader;
        memset(&blHeader, 0 , sizeof(_Ofs::strBlockHeader));
        memset(&entryHeaderNew, 0 , sizeof(_Ofs::strMainEntryHeader));

        blHeader.Signature[0] = newHeader.BLOCK_HEADER_SIG[0];
        blHeader.Signature[1] = newHeader.BLOCK_HEADER_SIG[1];
        blHeader.Type = _Ofs::OFS_MAIN_BLOCK;

        while(dit != DirMap.end())
        {
            if(dit->second->Id == -1)
            {
                dit++;
                continue;
            }

            blHeader.Length = sizeof(_Ofs::strMainEntryHeader);
            mOutStream.write((char *)&blHeader, sizeof(_Ofs::strBlockHeader));

            entryHeaderNew.Id = dit->second->Id;
            entryHeaderNew.ParentId = dit->second->ParentId;
            entryHeaderNew.Flags = dit->second->Flags;
            entryHeaderNew.NextBlock = 0;
            entryHeaderNew.FileSize = dit->second->FileSize;
            entryHeaderNew.CreationTime = dit->second->CreationTime;

            int sz = dit->second->Name.length();
            if(sz > 255)
                sz = 255;
            memcpy(entryHeaderNew.Name, dit->second->Name.c_str(), sz);
            entryHeaderNew.Name[sz] = 0;
            entryHeaderNew.Uuid = dit->second->Uuid;

            mOutStream.write((char *)&entryHeaderNew, sizeof(_Ofs::strMainEntryHeader));
            
            dit++;
        }

        dit = FileMap.begin();

        char *buffer = new char[2 * MAX_BUFFER_SIZE];

        while(dit != FileMap.end())
        {
            blHeader.Length = sizeof(_Ofs::strMainEntryHeader) + dit->second->FileSize;
            mOutStream.write((char *)&blHeader, sizeof(_Ofs::strBlockHeader));

            entryHeaderNew.Id = dit->second->Id;
            entryHeaderNew.ParentId = dit->second->ParentId;
            entryHeaderNew.Flags = dit->second->Flags;
            entryHeaderNew.NextBlock = 0;
            entryHeaderNew.FileSize = dit->second->FileSize;
            entryHeaderNew.CreationTime = dit->second->CreationTime;

            int sz = dit->second->Name.length();
            if(sz > 255)
                sz = 255;
            memcpy(entryHeaderNew.Name, dit->second->Name.c_str(), sz);
            entryHeaderNew.Name[sz] = 0;
            entryHeaderNew.Uuid = dit->second->Uuid;

            mOutStream.write((char *)&entryHeaderNew, sizeof(_Ofs::strMainEntryHeader));

            unsigned int total = dit->second->FileSize;
            unsigned int curBlock = 0;

            unsigned int can_read = dit->second->UsedBlocks[0].Length - sizeof(EntryHeaderV10);

            while(total > 0)
            {
                if(can_read > total)
                {
                    mInStream.clear();
                    mInStream.seekg(dit->second->UsedBlocks[0].Start + sizeof(EntryHeaderV10), fstream::beg);
                    mInStream.read(buffer, total);
                    mOutStream.write(buffer, total);
                    total = 0;
                }
                else
                {
                    mInStream.clear();
                    mInStream.seekg(dit->second->UsedBlocks[0].Start + sizeof(EntryHeaderV10), fstream::beg);
                    mInStream.read(buffer, can_read);
                    mOutStream.write(buffer, can_read);
                    total -= can_read;
                    
                    if(total > 0)
                    {
                        curBlock++;
                        can_read = dit->second->UsedBlocks[curBlock].Length - sizeof(EntryHeaderShortV10);
                    }
                }
            }
            
            dit++;
        }

        delete [] buffer;

        _deallocateChildren(&RootDir);

        mInStream.close();
        mOutStream.close();

        return true;
    }

//------------------------------------------------------------------------------

    bool OfsConverter::_convertv11_v12(std::string infile, std::string outfile)
    {
        OPEN_STREAM(mInStream, infile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
        if(mInStream.fail() || !mInStream.is_open())
            return false;

        HeaderV10 fsHeader;

        mInStream.seekg(0, fstream::end);
        unsigned int file_size = mInStream.tellg();

        int HeaderSize = sizeof(HeaderV10);
        mInStream.seekg(0, fstream::beg);
        mInStream.read((char *)&fsHeader, HeaderSize);

        unsigned int current_loc = mInStream.tellg();

        int max_id = -1000;

        FileBlockDataV10    fileBlockData;
        EntryHeaderV11      entryHeaderOld;
        EntryHeaderShortV10 entryHeaderShort;

        _Ofs::BlockData          blockData;
        _Ofs::strMainEntryHeader entryHeaderNew;
        _Ofs::IdDescMap::const_iterator it;
        _Ofs::IdDescMap::iterator dait;
        _Ofs::IdDescMap DirMap;
        _Ofs::IdDescMap FileMap;
        _Ofs::PosBlockDataMap extendedEntries;
        _Ofs::OfsEntryDesc RootDir;

        RootDir.Id = -1;
        RootDir.ParentId = -1;

        DirMap.insert(_Ofs::IdDescMap::value_type(-1, &RootDir));

        while((current_loc = mInStream.tellg()) < file_size)
        {
            mInStream.read((char*)&fileBlockData, sizeof(FileBlockDataV10));
            if(fileBlockData.Signature[0] != BLOCK_SIGNATURE_OLD_0 || fileBlockData.Signature[1] != BLOCK_SIGNATURE_OLD_1)
            {
                unsigned int pos = mInStream.tellg();
                mInStream.clear();
                mInStream.seekg( pos + 1 - sizeof(FileBlockDataV10));
                continue;
            }

            if(fileBlockData.Type & _Ofs::OFS_FREE_BLOCK)
            {
                mInStream.seekg(fileBlockData.Length, fstream::cur);
            }
            else if(fileBlockData.Type == _Ofs::OFS_MAIN_BLOCK)
            {
                mInStream.read((char*)&entryHeaderOld, sizeof(EntryHeaderV11));
                blockData.Type = fileBlockData.Type;
                blockData.Start = mInStream.tellg();
                blockData.Start -= sizeof(EntryHeaderV11);
                blockData.Length = fileBlockData.Length;
                blockData.NextBlock = entryHeaderOld.NextBlock;

                _Ofs::OfsEntryDesc *entryDesc = new _Ofs::OfsEntryDesc();

                if(entryHeaderOld.Flags & OFS_DIR)
                {
                    dait = DirMap.find(entryHeaderOld.Id);
                    if(dait == DirMap.end())
                        DirMap.insert(_Ofs::IdDescMap::value_type(entryHeaderOld.Id, entryDesc));
                    else
                    {
                        delete entryDesc;
                        entryDesc = dait->second;
                    }
                }
                else
                    FileMap.insert(_Ofs::IdDescMap::value_type(entryHeaderOld.Id, entryDesc));
 
                entryDesc->Id = entryHeaderOld.Id;
                entryDesc->ParentId = entryHeaderOld.ParentId;
                entryDesc->Flags = entryHeaderOld.Flags;
                entryDesc->Name = entryHeaderOld.Name;
                entryDesc->FileSize = entryHeaderOld.FileSize;
                entryDesc->CreationTime = entryHeaderOld.CreationTime;
                entryDesc->UseCount = 0;
                entryDesc->WriteLocked = false;
                entryDesc->Uuid = entryHeaderOld.Uuid;

                it = DirMap.find(entryDesc->ParentId);
                if(it == DirMap.end())
                {
                    _Ofs::OfsEntryDesc *dirDesc = new _Ofs::OfsEntryDesc();
                    dirDesc->Id = entryDesc->ParentId;
                    dirDesc->Name = "Error_NoName";
                    dirDesc->Parent = 0;

                    it = DirMap.insert(_Ofs::IdDescMap::value_type(dirDesc->Id, dirDesc)).first;
                }

                entryDesc->UsedBlocks.push_back(blockData);
                entryDesc->Parent = it->second;
                it->second->Children.push_back(entryDesc);

                mInStream.seekg(blockData.Length - sizeof(EntryHeaderV11), fstream::cur);

                if(max_id < entryHeaderOld.Id)
                    max_id = entryHeaderOld.Id;
            }
            else if(fileBlockData.Type == _Ofs::OFS_EXTENDED_BLOCK)
            {
                mInStream.read((char*)&entryHeaderShort, sizeof(EntryHeaderShortV10));
 
                blockData.Type = fileBlockData.Type;
                blockData.Start = mInStream.tellg();
                blockData.Start -= sizeof(EntryHeaderShortV10);
                blockData.Length = fileBlockData.Length;
                blockData.NextBlock = entryHeaderShort.NextBlock;

                extendedEntries.insert(_Ofs::PosBlockDataMap::value_type(blockData.Start, blockData));
                mInStream.seekg(blockData.Length - sizeof(EntryHeaderShortV10), fstream::cur);
            }
            else
            {
                mInStream.close();
                return false;
            }
        }

        _Ofs::PosBlockDataMap::iterator pit;
        _Ofs::IdDescMap::const_iterator dit = FileMap.begin();

        while(dit != FileMap.end())
        {
            unsigned int next_block = dit->second->UsedBlocks[0].NextBlock;
            while(next_block != 0)
            {
                pit = extendedEntries.find(next_block);
                if(pit == extendedEntries.end())
                {
                    break;
                }

                dit->second->UsedBlocks.push_back(pit->second);

                next_block = pit->second.NextBlock;

                extendedEntries.erase(pit);
            }

            dit++;
        }

        OPEN_STREAM(mOutStream, outfile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::trunc);
        if(mOutStream.fail() || !mOutStream.is_open())
        {
            mInStream.close();
            return false;
        }

        _Ofs::strFileHeader newHeader;

        memset(&newHeader, 0, sizeof(_Ofs::strFileHeader));

        time_t signature_time = time(NULL);
        unsigned int * signature = (unsigned int *)(&signature_time);

        newHeader.ID[0] = 'O';newHeader.ID[1] = 'F';newHeader.ID[2] = 'S';newHeader.ID[3] = '1';
        newHeader.VERSION[0] = VERSION_MAJOR_0;
        newHeader.VERSION[1] = VERSION_MAJOR_1;
        newHeader.VERSION[2] = VERSION_MINOR;
        newHeader.VERSION[3] = VERSION_FIX;
        newHeader.BLOCK_HEADER_SIG[0] = signature[0];
        newHeader.BLOCK_HEADER_SIG[1] = signature[0] ^ 0xFFFFFFFF;
        newHeader.LAST_ID = ++max_id;

        
        mOutStream.write((char *)&newHeader, sizeof(_Ofs::strFileHeader));

        dit = DirMap.begin();

        _Ofs::strBlockHeader blHeader;
        memset(&blHeader, 0 , sizeof(_Ofs::strBlockHeader));
        memset(&entryHeaderNew, 0 , sizeof(_Ofs::strMainEntryHeader));

        blHeader.Signature[0] = newHeader.BLOCK_HEADER_SIG[0];
        blHeader.Signature[1] = newHeader.BLOCK_HEADER_SIG[1];
        blHeader.Type = _Ofs::OFS_MAIN_BLOCK;

        while(dit != DirMap.end())
        {
            if(dit->second->Id == -1)
            {
                dit++;
                continue;
            }

            blHeader.Length = sizeof(_Ofs::strMainEntryHeader);
            mOutStream.write((char *)&blHeader, sizeof(_Ofs::strBlockHeader));

            entryHeaderNew.Id = dit->second->Id;
            entryHeaderNew.ParentId = dit->second->ParentId;
            entryHeaderNew.Flags = dit->second->Flags;
            entryHeaderNew.NextBlock = 0;
            entryHeaderNew.FileSize = dit->second->FileSize;
            entryHeaderNew.CreationTime = dit->second->CreationTime;

            int sz = dit->second->Name.length();
            if(sz > 255)
                sz = 255;
            memcpy(entryHeaderNew.Name, dit->second->Name.c_str(), sz);
            entryHeaderNew.Name[sz] = 0;
            entryHeaderNew.Uuid = dit->second->Uuid;

            mOutStream.write((char *)&entryHeaderNew, sizeof(_Ofs::strMainEntryHeader));
            
            dit++;
        }

        dit = FileMap.begin();

        char *buffer = new char[2 * MAX_BUFFER_SIZE];

        while(dit != FileMap.end())
        {
            blHeader.Length = sizeof(_Ofs::strMainEntryHeader) + dit->second->FileSize;
            mOutStream.write((char *)&blHeader, sizeof(_Ofs::strBlockHeader));

            entryHeaderNew.Id = dit->second->Id;
            entryHeaderNew.ParentId = dit->second->ParentId;
            entryHeaderNew.Flags = dit->second->Flags;
            entryHeaderNew.NextBlock = 0;
            entryHeaderNew.FileSize = dit->second->FileSize;
            entryHeaderNew.CreationTime = dit->second->CreationTime;

            int sz = dit->second->Name.length();
            if(sz > 255)
                sz = 255;
            memcpy(entryHeaderNew.Name, dit->second->Name.c_str(), sz);
            entryHeaderNew.Name[sz] = 0;
            entryHeaderNew.Uuid = dit->second->Uuid;

            mOutStream.write((char *)&entryHeaderNew, sizeof(_Ofs::strMainEntryHeader));

            unsigned int total = dit->second->FileSize;
            unsigned int curBlock = 0;

            unsigned int can_read = dit->second->UsedBlocks[0].Length - sizeof(EntryHeaderV11);

            while(total > 0)
            {
                if(can_read > total)
                {
                    mInStream.clear();
                    mInStream.seekg(dit->second->UsedBlocks[0].Start + sizeof(EntryHeaderV11), fstream::beg);
                    mInStream.read(buffer, total);
                    mOutStream.write(buffer, total);
                    total = 0;
                }
                else
                {
                    mInStream.clear();
                    mInStream.seekg(dit->second->UsedBlocks[0].Start + sizeof(EntryHeaderV11), fstream::beg);
                    mInStream.read(buffer, can_read);
                    mOutStream.write(buffer, can_read);
                    total -= can_read;
                    
                    if(total > 0)
                    {
                        curBlock++;
                        can_read = dit->second->UsedBlocks[curBlock].Length - sizeof(EntryHeaderShortV10);
                    }
                }
            }
            
            dit++;
        }

        delete [] buffer;

        _deallocateChildren(&RootDir);

        mInStream.close();
        mOutStream.close();

        return true;
    }

//------------------------------------------------------------------------------

}