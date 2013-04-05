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

#include "ofs13.h"
#include "ofs14.h"
#include "ofs_converter.h"
#include <algorithm>

using namespace std;

#if defined( __WIN32__ ) || defined( _WIN32 )
#define OPEN_STREAM(a, b, c) a.open(b, c, SH_DENYWR)
#else
#define OPEN_STREAM(a, b, c) a.open(b, c)
#endif

namespace OFS
{

    
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

    bool OfsConverter::convert(std::string infile, std::string outfile)
    {
        OPEN_STREAM(mInStream, infile.c_str(), fstream::in | fstream::out | fstream::binary | fstream::ate);
        if(mInStream.fail() || !mInStream.is_open())
            return false;

        _Ofs::strFileHeader fsHeader;

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
            case 13:return _convertv13_v14(infile, outfile);
            };
        }

        return false;
    }

//------------------------------------------------------------------------------

    bool OfsConverter::_convertv13_v14(std::string infile, std::string outfile)
    {
        OFS13::OfsPtr    srcFile;
        OfsPtr           destFile;
        OFS13::OfsResult ret1;
        OfsResult        ret2;

        ret1 = srcFile.mount(infile.c_str(), OFS13::OFS_MOUNT_OPEN);

        if( ret1 != OFS13::OFS_OK )
            return false;

        ret2 = destFile.mount(outfile.c_str(), OFS_MOUNT_CREATE);

        if( ret2 != OFS_OK )
            return false;

        
        OFS13::FileList allFiles;

        srcFile->listFilesRecursive("/", allFiles);

        std::sort(allFiles.begin(), allFiles.end(), OFS13::FileEntry::Compare);

        if(ret2 == OFS_OK)
        {
            OFS13::OFSHANDLE in_handle;
            OFSHANDLE        out_handle;

            unsigned int output_amount = 0;

            char *tmp_buffer = new char[MAX_BUFFER_SIZE];

            for(unsigned int i = 0;i < allFiles.size();i++)
            {
                if(allFiles[i].flags & OFS13::OFS_DIR)
                {
                    destFile->createDirectoryUUID(allFiles[i].name.c_str(), *((OFS::UUID*)&allFiles[i].uuid));
                    destFile->setDirFlags(allFiles[i].name.c_str(), allFiles[i].flags);
                }
                else
                {
                    std::string file_ofs_path = allFiles[i].name;

                    try
                    {
                        ret1 = srcFile->openFile(in_handle, file_ofs_path.c_str());
                        if(ret1 != OFS13::OFS_OK)
                            continue;

                        unsigned int total = allFiles[i].file_size;

                        bool dest_file_created = false;

                        while(total > 0)
                        {
                            if(total < MAX_BUFFER_SIZE)
                            {
                                srcFile->read(in_handle, tmp_buffer, total);
                                if(dest_file_created)
                                    destFile->write(out_handle, tmp_buffer, total);
                                else
                                {
                                    destFile->createFileUUID(out_handle, file_ofs_path.c_str(), *((OFS::UUID*)&allFiles[i].uuid), total, total, tmp_buffer);
                                    destFile->setFileFlags(out_handle, allFiles[i].flags);
                                }

                                output_amount += total;
                                total = 0;
                            }
                            else
                            {
                                srcFile->read(in_handle, tmp_buffer, MAX_BUFFER_SIZE);
                                if(dest_file_created)
                                    destFile->write(out_handle, tmp_buffer, MAX_BUFFER_SIZE);
                                else
                                {
                                    destFile->createFileUUID(out_handle, file_ofs_path.c_str(), *((OFS::UUID*)&allFiles[i].uuid), MAX_BUFFER_SIZE, MAX_BUFFER_SIZE, tmp_buffer);
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
                        srcFile->closeFile(in_handle);
                }
            }

            delete [] tmp_buffer;

            srcFile.unmount();
            destFile.unmount();
        }

        return true;
    }

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

}