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

#include "ofs_base.h"
#include "ofs14.h"
#include "ofs_rfs.h"
#include <algorithm>
#include <stdio.h>

using namespace std;

namespace OFS
{
    STATIC_AUTO_MUTEX_DECL(_OfsBase)

    _OfsBase::NameOfsHandleMap _OfsBase::mAllocatedHandles;

    const UUID UUID_ZERO(0,0,0,0,0,0,0,0,0,0,0);

    const unsigned int MAX_BUFFER_SIZE = (16 * 1024 * 1024);

//------------------------------------------------------------------------------

    size_t FileStream::write( const void *data, size_t size )
    {
        assert( m_pFile != NULL );

        size_t actual_len =  fwrite( data, 1, size, m_pFile );
        
        assert(actual_len == size);
        
        return actual_len;
    }


    void FileStream::close()
    {
        if( m_pFile != NULL )
        {
            fflush( m_pFile );
            fclose( m_pFile );
            m_pFile = NULL;
        }
    }

//------------------------------------------------------------------------------

    void FileStream::open( const char *file, const char *mode, int flag )
    {
        close();

#if (defined( __WIN32__ ) || defined( _WIN32 )) && ! defined( __GNUC__ )
		if( flag == 0)
            flag = _SH_DENYNO;

        m_pFile = _fsopen( file, mode, flag );
#else
        m_pFile = fopen( file, mode );
#endif
		
	}

//------------------------------------------------------------------------------
	char fl_4k[ 4096 ];
	char fl_32[ 32 ];
	char fl_b = 0;


    void FileStream::fill( ofs64 len )
    {
		ofs64 i;

		ofs64 fl4k = len >> 12;
		ofs64 fl32 = ( len & 0xFFF ) >> 5; 
		len = len & 0x1F;

        assert( m_pFile != NULL );
         
	    for( i = 0; i < fl4k; i++ )
		    fwrite( &fl_4k, 4096, 1, m_pFile );    

	    for( i = 0; i < fl32; i++ )
		    fwrite( &fl_32, 32, 1, m_pFile );    

		for( i = 0; i < len; i++ )
		    fwrite( &fl_b, 1, 1, m_pFile );    
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

    void OFSHANDLE::_setWritePos(ofs64 value)
    {
        if(value > mEntryDesc->FileSize)
            value = mEntryDesc->FileSize;

        if(mWritePos != value)
        {
            mWritePos = value;
            mWriteBlockEnd = mEntryDesc->UsedBlocks[0].Start + mEntryDesc->UsedBlocks[0].Length;

            ofs64 block_size = mEntryDesc->UsedBlocks[0].Length - sizeof(_Ofs::strMainEntryHeader);
            int i = 0;
            ofs64 max_i = mEntryDesc->UsedBlocks.size();
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

    void OFSHANDLE::_setReadPos(ofs64 value)
    {
        if(value > mEntryDesc->FileSize)
            value = mEntryDesc->FileSize;

        if(mReadPos != value)
        {
            mReadPos = value;
            mReadBlockEnd = mEntryDesc->UsedBlocks[0].Start + mEntryDesc->UsedBlocks[0].Length;

            ofs64 block_size = mEntryDesc->UsedBlocks[0].Length - sizeof(_Ofs::strMainEntryHeader);
            int i = 0;
            ofs64 max_i = mEntryDesc->UsedBlocks.size();
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

    _OfsBase::_OfsBase( FileSystemType type ) : mFileSystemType(type), mActive(false), mUseCount(0), mRecoveryMode(false), mLinkMode(false)
    {
        mFileName           = "";
        mRootDir.Owner     = this;
        mRootDir.Id         = ROOT_DIRECTORY_ID;
        mRootDir.ParentId   = ROOT_DIRECTORY_ID;
        mRootDir.Flags      = OFS_DIR;
        mRootDir.FileSize   = 0;
        mRootDir.Parent     = NULL;

        mRecycleBinRoot.Owner      = this;
        mRecycleBinRoot.Id         = RECYCLEBIN_DIRECTORY_ID;
        mRecycleBinRoot.ParentId   = RECYCLEBIN_DIRECTORY_ID;
        mRecycleBinRoot.Flags      = OFS_DIR;
        mRecycleBinRoot.FileSize   = 0;
        mRecycleBinRoot.Parent     = NULL;
    }

//------------------------------------------------------------------------------

    _OfsBase::~_OfsBase()
    {
    }

//------------------------------------------------------------------------------

    void _OfsBase::_incUseCount()
    {
        LOCK_AUTO_MUTEX

        ++mUseCount;
    }

//------------------------------------------------------------------------------

    void _OfsBase::_decUseCount()
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

    bool _OfsBase::isActive()
    {
        LOCK_AUTO_MUTEX

        return mActive; 
    }


//------------------------------------------------------------------------------

    std::string _OfsBase::getFileSystemDirectory()
    {
        if(mFileSystemType == OFS_RFS )
            return mFileName;
        else
        {
            int pos1 = mFileName.find_last_of("/");
            int pos2 = mFileName.find_last_of("\\");

            if( pos2 > pos1 ) pos1 = pos2;

            return mFileName.substr( 0, pos1 );
        }
    }

//------------------------------------------------------------------------------

    OfsResult _OfsBase::mount(_OfsBase** ptr, const char *file, unsigned int op)
    {
        assert(ptr != 0);

        STATIC_LOCK_AUTO_MUTEX

        std::string file_name = file;

        if((file_name[file_name.size() - 1] == '/') || (file_name[file_name.size() - 1] == '/'))
            file_name.erase(file_name.size() - 1, 1);

        NameOfsHandleMap::iterator it = mAllocatedHandles.find(file_name);

        if(it == mAllocatedHandles.end())
        {
            std::string extension = file_name.substr(file_name.size() - 4);
#ifdef linux
	        int result = strcasecmp(extension.c_str(), ".ofs");
#else
	        int result = _strcmpi(extension.c_str(), ".ofs");
#endif            
            if(result == 0)
                *ptr = new _Ofs();
            else
                *ptr = new _OfsRfs();

            (*ptr)->_incUseCount();
            
            OfsResult ret = (*ptr)->_mount(file_name.c_str(), op);

            if(ret != OFS_OK)
            {
                (*ptr)->_decUseCount();
                *ptr = 0;
                return ret;
            }

            mAllocatedHandles.insert(NameOfsHandleMap::value_type(file_name, *ptr));
        }
        else
        {
            if((op & OFS_MOUNT_LINK) && (it->second->mLinkMode == false))
                return OFS_ACCESS_DENIED;

            if(op == OFS_MOUNT_CREATE)
            {
                OFS_EXCEPT("_Ofs::mount, Cannot overwrite an archive in use.");

                return OFS_ACCESS_DENIED;
            }

            *ptr = it->second;
            (*ptr)->_incUseCount();
        }

        return OFS_OK;
    }


//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------

    OfsPtr& OfsPtr::operator=(OfsPtr& other)
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


    OfsResult OfsPtr::mount(const char *file, unsigned int op)
    {
        assert(mPtr == 0);

        return _OfsBase::mount(&mPtr, file, op);
    }

//------------------------------------------------------------------------------

    void OfsPtr::unmount()
    {
        if(mPtr != 0)
            mPtr->_decUseCount();
        
        mPtr = 0;
    }

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------


}
