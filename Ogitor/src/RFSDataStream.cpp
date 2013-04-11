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
////////////////////////////////////////////////////////////////////////////////*/

#include "OgitorsPrerequisites.h"
#include "ofs.h"
#include "RFSDataStream.h"


namespace Ogitors 
{

    //-----------------------------------------------------------------------
    RfsDataStream::RfsDataStream(OFS::OfsPtr _ofsptr, OFS::OFSHANDLE *_handle)
        : mOfs(_ofsptr), mFileHandle(_handle)
    {
        assert(_handle != NULL);
        OFS::ofs64 sz = 0; 

        _ofsptr->getFileSize(*_handle, sz);
        _ofsptr->getFileName(*_handle, mName);

        mSize = sz;

        mAccess = 0;

        if(_handle->getAcessFlags() & OFS::OFS_READ)
            mAccess |= DataStream::READ;

        if(_handle->getAcessFlags() & OFS::OFS_WRITE)
            mAccess |= DataStream::WRITE;
    }
    //-----------------------------------------------------------------------
    RfsDataStream::~RfsDataStream()
	{
		close();
	}
    //-----------------------------------------------------------------------
    size_t RfsDataStream::read(void* buf, size_t count)
    {
        unsigned int read_amount = 0;

        mOfs->read(*mFileHandle, (char*)buf, count, &read_amount);

        return read_amount;
    }
	//---------------------------------------------------------------------
    size_t RfsDataStream::write(const void* buf, size_t count)
	{
        if(mOfs->write(*mFileHandle, (const char*)buf, (unsigned int)count) == OFS::OFS_OK)
            return count;
        else
            return 0;
	}
    //-----------------------------------------------------------------------
    void RfsDataStream::skip(long count)
    {
        mOfs->seekr(*mFileHandle, count, OFS::OFS_SEEK_CURRENT);
        mOfs->seekw(*mFileHandle, count, OFS::OFS_SEEK_CURRENT);
    }
    //-----------------------------------------------------------------------
    void RfsDataStream::seek( size_t pos )
    {
        mOfs->seekr(*mFileHandle, pos, OFS::OFS_SEEK_BEGIN);
        mOfs->seekw(*mFileHandle, pos, OFS::OFS_SEEK_BEGIN);
    }
    //-----------------------------------------------------------------------
    size_t RfsDataStream::tell(void) const
    {
        return mOfs->tellr(*mFileHandle);
    }
    //-----------------------------------------------------------------------
    bool RfsDataStream::eof(void) const
    {
        return mOfs->eof(*mFileHandle);
    }
    //-----------------------------------------------------------------------
    void RfsDataStream::close(void)
    {
        if (mFileHandle != 0)
		{
            mOfs->closeFile(*mFileHandle);

            delete mFileHandle;
            mFileHandle = 0;
		}

        mOfs.unmount();
    }
    //-----------------------------------------------------------------------

}
