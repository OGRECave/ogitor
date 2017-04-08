/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2015 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
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

#include "OgreStableHeaders.h"
#include "OfsFileSystem.h"
#include "OgreLogManager.h"
#include "OgreException.h"
#include "OgreStringVector.h"
#include "OgreRoot.h"

#include <boost/regex.hpp>

namespace Ogre {

	bool OFSArchive::ms_IgnoreHidden = true;

    //-----------------------------------------------------------------------
    OFSArchive::OFSArchive(const String& name, const String& archType)
        : Archive(name, archType)
    {
        int pos = name.find("::");
        mName = name;
        mFileSystemName = name.substr(0, pos);
        mDir = name;
        mDir.erase(0, pos + 2);
        if(!mDir.empty() && mDir[mDir.length() - 1] == '/')
            mDir.erase(mDir.length() - 1, 1);
    }
    //-----------------------------------------------------------------------
    bool OFSArchive::isCaseSensitive(void) const
    {
        return true;
    }
    //-----------------------------------------------------------------------
    static bool is_absolute_path(const char* path)
    {
        return path[0] == '/' || path[0] == '\\';
    }
    //-----------------------------------------------------------------------
    static String concatenate_path(const String& base, const String& name)
    {
        if (base.empty() || is_absolute_path(name.c_str()))
            return name;
        else
            return base + '/' + name;
    }
    //-----------------------------------------------------------------------
    void OFSArchive::findFiles(const String& pattern, bool recursive, 
        bool dirs, StringVector* simpleList, FileInfoList* detailList) const
    {
        // pattern can contain a directory name, separate it from mask
        size_t pos1 = pattern.rfind ('/');
        size_t pos2 = pattern.rfind ('\\');
        if (pos1 == pattern.npos || ((pos2 != pattern.npos) && (pos1 < pos2)))
            pos1 = pos2;
        String directory;
        if (pos1 != pattern.npos)
            directory = pattern.substr (0, pos1 + 1);

        String full_pattern = concatenate_path(mDir, pattern);
        String mod_pattern = pattern;

        OFS::FileList retList = mOfs->listFiles(mDir.c_str(), OFS::OFS_FILE);

        if(pattern == "*")
        {
            for(unsigned int i = 0;i < retList.size();i++)
            {
                if (simpleList)
                {
                    simpleList->push_back(directory + retList[i].name);
                }
                else if (detailList)
                {
                    FileInfo fi;
                    fi.archive = (Archive*)this;
                    fi.filename = directory + retList[i].name;
                    fi.basename = retList[i].name;
                    fi.path = directory;
                    fi.compressedSize = retList[i].file_size;
                    fi.uncompressedSize = retList[i].file_size;
                    detailList->push_back(fi);
                }
            }
        }
        else
        {
            if(mod_pattern[0] == '*')
                mod_pattern = "." + mod_pattern;

            try
            {
                const boost::regex e(mod_pattern.c_str());

                for(unsigned int i = 0;i < retList.size();i++)   
                {
                    if(regex_match(retList[i].name.c_str(), e))
                    {
                        if (simpleList)
                        {
                            simpleList->push_back(directory + retList[i].name);
                        }
                        else if (detailList)
                        {
                            FileInfo fi;
                            fi.archive = (Archive*)this;
                            fi.filename = directory + retList[i].name;
                            fi.basename = retList[i].name;
                            fi.path = directory;
                            fi.compressedSize = retList[i].file_size;
                            fi.uncompressedSize = retList[i].file_size;
                            detailList->push_back(fi);
                        }
                    }
                }
            }
            catch(...)
            {
            }
        }
/*
        lHandle = _findfirst(full_pattern.c_str(), &tagData);
        res = 0;
        while (lHandle != -1 && res != -1)
        {
            if ((dirs == ((tagData.attrib & _A_SUBDIR) != 0)) &&
				( !ms_IgnoreHidden || (tagData.attrib & _A_HIDDEN) == 0 ) &&
                (!dirs || !is_reserved_dir (tagData.name)))
            {
                if (simpleList)
                {
                    simpleList->push_back(directory + tagData.name);
                }
                else if (detailList)
                {
                    FileInfo fi;
                    fi.archive = this;
                    fi.filename = directory + tagData.name;
                    fi.basename = tagData.name;
                    fi.path = directory;
                    fi.compressedSize = tagData.size;
                    fi.uncompressedSize = tagData.size;
                    detailList->push_back(fi);
                }
            }
            res = _findnext( lHandle, &tagData );
        }
        // Close if we found any files
        if(lHandle != -1)
            _findclose(lHandle);

        // Now find directories
        if (recursive)
        {
            String base_dir = mName;
            if (!directory.empty ())
            {
                base_dir = concatenate_path(mName, directory);
                // Remove the last '/'
                base_dir.erase (base_dir.length () - 1);
            }
            base_dir.append ("/*");

            // Remove directory name from pattern
            String mask ("/");
            if (pos1 != pattern.npos)
                mask.append (pattern.substr (pos1 + 1));
            else
                mask.append (pattern);

            lHandle = _findfirst(base_dir.c_str (), &tagData);
            res = 0;
            while (lHandle != -1 && res != -1)
            {
                if ((tagData.attrib & _A_SUBDIR) &&
					( !ms_IgnoreHidden || (tagData.attrib & _A_HIDDEN) == 0 ) &&
                    !is_reserved_dir (tagData.name))
                {
                    // recurse
                    base_dir = directory;
                    base_dir.append (tagData.name).append (mask);
                    findFiles(base_dir, recursive, dirs, simpleList, detailList);
                }
                res = _findnext( lHandle, &tagData );
            }
            // Close if we found any files
            if(lHandle != -1)
                _findclose(lHandle);
        }
*/
    }
    //-----------------------------------------------------------------------
    OFSArchive::~OFSArchive()
    {
        unload();
    }
    //-----------------------------------------------------------------------
    void OFSArchive::load()
    {
		OGRE_LOCK_AUTO_MUTEX;

        // test to see if this folder is writeable
		String testPath = mFileSystemName + "__testwrite.ogre";
		std::ofstream writeStream;
		writeStream.open(testPath.c_str());
		if (writeStream.fail())
			mReadOnly = true;
		else
		{
			mReadOnly = false;
			writeStream.close();
			::remove(testPath.c_str());
		}

        mOfs.mount(mFileSystemName.c_str(), OFS::OFS_MOUNT_OPEN);
    }
    //-----------------------------------------------------------------------
    void OFSArchive::unload()
    {
        OGRE_LOCK_AUTO_MUTEX;

        mOfs.unmount();
    }
    //-----------------------------------------------------------------------
    DataStreamPtr OFSArchive::open(const String& filename, bool readOnly) const
    {
        String name = concatenate_path(mDir, filename);

        std::replace(name.begin(), name.end(), '\\', '/');

        unsigned int mode = OFS::OFS_READ;

		if (!readOnly && !isReadOnly())
            mode |= OFS::OFS_WRITE;

        OFS::OFSHANDLE *handle = new OFS::OFSHANDLE();
        OFS::OfsResult ret = mOfs->openFile(*handle, name.c_str(), mode);

        if(ret != OFS::OFS_OK)
        {
			delete handle;

            OGRE_EXCEPT(Exception::ERR_FILE_NOT_FOUND,
				"Cannot open file: " + name,
				"OFSArchive::open");
        }

		/// Construct return stream, tell it to delete on destroy
		OfsDataStream* stream = OGRE_NEW OfsDataStream(mOfs, handle);

        return DataStreamPtr(stream);
    }
	//---------------------------------------------------------------------
	DataStreamPtr OFSArchive::create(const String& filename)
	{
		if (isReadOnly())
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Cannot create a file in a read-only archive", 
				"OFSArchive::create");
		}

        String name = concatenate_path(mDir, filename);

        std::replace(name.begin(), name.end(), '\\', '/');

        OFS::OFSHANDLE *handle = new OFS::OFSHANDLE();
        OFS::OfsResult ret = mOfs->createFile(*handle, name.c_str());

        if(ret != OFS::OFS_OK)
        {
			delete handle;

            OGRE_EXCEPT(Exception::ERR_FILE_NOT_FOUND,
				"Cannot create file: " + name,
				"OFSArchive::create");
        }

		/// Construct return stream, tell it to delete on destroy
		OfsDataStream* stream = OGRE_NEW OfsDataStream(mOfs, handle);

        return DataStreamPtr(stream);
	}
	//---------------------------------------------------------------------
	void OFSArchive::remove(const String& filename)
	{
		if (isReadOnly())
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, 
				"Cannot remove a file from a read-only archive", 
				"OFSArchive::remove");
		}

		String name = concatenate_path(mDir, filename);

        std::replace(name.begin(), name.end(), '\\', '/');

        mOfs->deleteFile(name.c_str());
	}
    //-----------------------------------------------------------------------
    StringVectorPtr OFSArchive::list(bool recursive, bool dirs) const
    {
		// directory change requires locking due to saved returns
		// Note that we have to tell the SharedPtr to use OGRE_DELETE_T not OGRE_DELETE by passing category
		StringVectorPtr ret(OGRE_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);

        findFiles("*", recursive, dirs, ret.getPointer(), 0);

        return ret;
    }
    //-----------------------------------------------------------------------
    FileInfoListPtr OFSArchive::listFileInfo(bool recursive, bool dirs) const
    {
		// Note that we have to tell the SharedPtr to use OGRE_DELETE_T not OGRE_DELETE by passing category
        FileInfoListPtr ret(OGRE_NEW_T(FileInfoList, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);

        findFiles("*", recursive, dirs, 0, ret.getPointer());

        return ret;
    }
    //-----------------------------------------------------------------------
    StringVectorPtr OFSArchive::find(const String& pattern,
                                            bool recursive, bool dirs) const
    {
		// Note that we have to tell the SharedPtr to use OGRE_DELETE_T not OGRE_DELETE by passing category
		StringVectorPtr ret(OGRE_NEW_T(StringVector, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);

        findFiles(pattern, recursive, dirs, ret.getPointer(), 0);

        return ret;

    }
    //-----------------------------------------------------------------------
    FileInfoListPtr OFSArchive::findFileInfo(const String& pattern, 
        bool recursive, bool dirs) const
    {
		// Note that we have to tell the SharedPtr to use OGRE_DELETE_T not OGRE_DELETE by passing category
		FileInfoListPtr ret(OGRE_NEW_T(FileInfoList, MEMCATEGORY_GENERAL)(), SPFM_DELETE_T);

        findFiles(pattern, recursive, dirs, 0, ret.getPointer());

        return ret;
    }
    //-----------------------------------------------------------------------
	bool OFSArchive::exists(const String& filename) const
	{
		String name = concatenate_path(mDir, filename);

        std::replace(name.begin(), name.end(), '\\', '/');

        return mOfs->exists(name.c_str());
	}
	//---------------------------------------------------------------------
	time_t OFSArchive::getModifiedTime(const String& filename) const
	{
		String name = concatenate_path(mDir, filename);

        std::replace(name.begin(), name.end(), '\\', '/');

        time_t mod_time = 0;

        mOfs->getModificationTime(name.c_str(), mod_time);

	    return mod_time;
	}
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    OfsDataStream::OfsDataStream(OFS::OfsPtr _ofsptr, OFS::OFSHANDLE *_handle)
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
	OfsDataStream::~OfsDataStream()
	{
		close();
	}
    //-----------------------------------------------------------------------
    size_t OfsDataStream::read(void* buf, size_t count)
    {
        unsigned int read_amount = 0;

        mOfs->read(*mFileHandle, (char*)buf, count, &read_amount);

        return read_amount;
    }
	//---------------------------------------------------------------------
	size_t OfsDataStream::write(const void* buf, size_t count)
	{
        if(mOfs->write(*mFileHandle, (const char*)buf, (unsigned int)count) == OFS::OFS_OK)
            return count;
        else
            return 0;
	}
    //-----------------------------------------------------------------------
    void OfsDataStream::skip(long count)
    {
        mOfs->seek(*mFileHandle, count, OFS::OFS_SEEK_CURRENT);
    }
    //-----------------------------------------------------------------------
    void OfsDataStream::seek(size_t pos)
    {
        mOfs->seek(*mFileHandle, pos, OFS::OFS_SEEK_BEGIN);
    }
    //-----------------------------------------------------------------------
    size_t OfsDataStream::tell(void) const
    {
        return mOfs->tell(*mFileHandle);
    }
    //-----------------------------------------------------------------------
    bool OfsDataStream::eof(void) const
    {
        return mOfs->eof(*mFileHandle);
    }
    //-----------------------------------------------------------------------
    void OfsDataStream::close(void)
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
    const String& OFSArchiveFactory::getType(void) const
    {
        static String name = "Ofs";
        return name;
    }
    //-----------------------------------------------------------------------
}
