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
#include "file_ops.h"
#include <boost/filesystem.hpp>

bool OfsDirectoryExists(const char *path)
{
    return boost::filesystem::is_directory(path);
}

bool OfsCreateDirectory(const char *path)
{
    bool exists = boost::filesystem::is_directory(path);
    if( !exists )
        exists = boost::filesystem::create_directory(path);

    return exists;
}

bool OfsDeleteFile(const char *path)
{
    return boost::filesystem::remove(path);
}

bool OfsDeleteDirectory(const char *path)
{
    return boost::filesystem::remove_all(path) > 0;
}

bool OfsRenameFile(const char *path, const char *new_path)
{
    boost::filesystem::rename(path, new_path);
    return true;
}


void OfsListContentsRecursive(OFS::_OfsBase *owner, OFS::_OfsBase::OfsEntryDesc *desc, int &id, const char *name, bool linkmode)
{
      OFS::_OfsBase::OfsEntryDesc *newdesc;

      if (boost::filesystem::is_regular_file(name))        // is name a regular file?
      {
          newdesc = new OFS::_OfsBase::OfsEntryDesc();

          newdesc->Owner = owner;
          newdesc->Id = id;
          newdesc->ParentId = desc->Id;
          newdesc->Parent = desc;
          newdesc->Flags = OFS::OFS_FILE;
          newdesc->Name = boost::filesystem::path(name).filename().string();
          newdesc->FileSize = boost::filesystem::file_size(name);
          newdesc->OldParentId = desc->Id;
          newdesc->UseCount = 0;
          newdesc->WriteLocked = false;
          newdesc->Uuid = OFS::UUID_ZERO;
          desc->Children.push_back(newdesc);

          if(linkmode)
              newdesc->Flags |= OFS::OFS_LINK;
      }
      else if (boost::filesystem::is_directory(name))      // is p a directory?
      {
          newdesc = new OFS::_OfsBase::OfsEntryDesc();

          newdesc->Owner = owner;
          newdesc->Id = id;
          newdesc->ParentId = desc->Id;
          newdesc->Parent = desc;
          newdesc->Flags = OFS::OFS_DIR;
          newdesc->Name = boost::filesystem::path(name).filename().string();
          newdesc->FileSize = 0;
          newdesc->OldParentId = desc->Id;
          newdesc->UseCount = 0;
          newdesc->WriteLocked = false;
          newdesc->Uuid = OFS::UUID_ZERO;
          desc->Children.push_back(newdesc);

          if(linkmode)
              newdesc->Flags |= OFS::OFS_LINK;
          
          boost::filesystem::directory_iterator it(name);
          boost::filesystem::directory_iterator end;

          while( it != end )
          {
              OfsListContentsRecursive(owner, newdesc, ++id, it->path().filename().string().c_str(), linkmode);
              ++it;
          }
      }
}

void OfsListContents(OFS::_OfsBase *owner, OFS::_OfsBase::OfsEntryDesc *desc, int &id, const char *path, bool linkmode)
{
      if (boost::filesystem::is_directory(path))      // is p a directory?
      {
          boost::filesystem::directory_iterator it(path);
          boost::filesystem::directory_iterator end;

          while( it != end )
          {
              OfsListContentsRecursive(owner, desc, id, it->path().filename().string().c_str(), linkmode);
              ++it;
          }
      }
}
