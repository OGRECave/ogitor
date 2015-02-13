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

/*////////////////////////////////////////////////////////////////////////////////
//This Header is used to define any library wide structs and enums and defines
////////////////////////////////////////////////////////////////////////////////*/

#pragma once

#include "OgrePrerequisites.h"

#include "OgreArchive.h"
#include "OgreArchiveFactory.h"
#include "Threading/OgreThreadHeaders.h"

#include "ofs.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   #ifdef PLUGIN_EXPORT
     #define PluginExport __declspec (dllexport)
   #else
     #define PluginExport __declspec (dllimport)
   #endif
#else
   #define PluginExport
#endif

namespace Ogre {

	/** Specialisation of the Archive class to allow reading of files from 
        OFS filesystem folders / directories.
    */
    class PluginExport OFSArchive : public Archive 
    {
    protected:
        mutable OFS::OfsPtr mOfs;
        String              mFileSystemName;
        String              mDir;
        
        /** Utility method to retrieve all files in a directory matching pattern.
        @param pattern File pattern
        @param recursive Whether to cascade down directories
        @param dirs Set to true if you want the directories to be listed
            instead of files
        @param simpleList Populated if retrieving a simple list
        @param detailList Populated if retrieving a detailed list
        @param currentDir The current directory relative to the base of the 
            archive, for file naming
        */
        void findFiles(const String& pattern, bool recursive, bool dirs,
            StringVector* simpleList, FileInfoList* detailList) const;

        OGRE_AUTO_MUTEX;

    public:
        OFSArchive(const String& name, const String& archType );
        ~OFSArchive();

        /// @copydoc Archive::isCaseSensitive
        bool isCaseSensitive(void) const;

        /// @copydoc Archive::load
        void load();
        /// @copydoc Archive::unload
        void unload();

        /// @copydoc Archive::open
#if OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR <= 9
        DataStreamPtr open(const String& filename, bool readOnly = true) const;
#else
        virtual DataStreamPtr open(const String& filename, bool readOnly = true);
#endif

		/// @copydoc Archive::create
		DataStreamPtr create(const String& filename) const;

		/// @copydoc Archive::delete
		void remove(const String& filename) const;

		/// @copydoc Archive::list
        StringVectorPtr list(bool recursive = true, bool dirs = false);

        /// @copydoc Archive::listFileInfo
        FileInfoListPtr listFileInfo(bool recursive = true, bool dirs = false);

        /// @copydoc Archive::find
        StringVectorPtr find(const String& pattern, bool recursive = true,
            bool dirs = false);

        /// @copydoc Archive::findFileInfo
#if OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR <= 9
        FileInfoListPtr findFileInfo(const String& pattern, bool recursive = true,
            bool dirs = false) const;
#else
        virtual FileInfoListPtr findFileInfo(const String& pattern, 
            bool recursive = true, bool dirs = false);
#endif

        /// @copydoc Archive::exists
        bool exists(const String& filename);

		/// @copydoc Archive::getModifiedTime
		time_t getModifiedTime(const String& filename);

		/// Set whether filesystem enumeration will include hidden files or not.
		/// This should be called prior to declaring and/or initializing filesystem
		/// resource locations. The default is true (ignore hidden files).
		static void setIgnoreHidden(bool ignore)
		{
			ms_IgnoreHidden = ignore;
		}

		/// Get whether hidden files are ignored during filesystem enumeration.
		static bool getIgnoreHidden()
		{
			return ms_IgnoreHidden;
		}

		static bool ms_IgnoreHidden;
    };

    /** Specialisation of ArchiveFactory for FileSystem files. */
    //class _OgrePrivate FileSystemArchiveFactory : public ArchiveFactory
    class PluginExport OFSArchiveFactory : public ArchiveFactory
    {
    public:
        virtual ~OFSArchiveFactory() {}
        /// @copydoc FactoryObj::getType
        const String& getType(void) const;
        /// @copydoc FactoryObj::createInstance
        

#if OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR <= 8
	Archive *createInstance(const String& name) { return OGRE_NEW OFSArchive(name, "Ofs"); } 
#else
	Archive *createInstance(const String& name, bool readOnly ) { return OGRE_NEW OFSArchive(name, "Ofs"); }
#endif            
    
        /// @copydoc FactoryObj::destroyInstance
        void destroyInstance( Archive* arch) { delete arch; }
    };

    /** Specialisation of DataStream to handle streaming data from ofs archives. */
    class PluginExport OfsDataStream : public DataStream
    {
    protected:
        OFS::OfsPtr     mOfs;
        OFS::OFSHANDLE *mFileHandle;

    public:
        /// Constructor
        OfsDataStream(OFS::OfsPtr _ofsptr, OFS::OFSHANDLE *_handle);

		~OfsDataStream();
        /// @copydoc DataStream::read
        size_t read(void* buf, size_t count);
		/// @copydoc DataStream::write
		size_t write(const void* buf, size_t count);
        /// @copydoc DataStream::skip
        void skip(long count);
        /// @copydoc DataStream::seek
        void seek( size_t pos );
        /// @copydoc DataStream::seek
        size_t tell(void) const;
        /// @copydoc DataStream::eof
        bool eof(void) const;
        /// @copydoc DataStream::close
        void close(void);
    };
}




