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
#include "OgitorsRoot.h"
#include "BaseEditor.h"

#include <boost/regex.hpp>

using namespace Ogre;

namespace Ogitors
{

//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegExpByName(const Ogre::String& nameregexp, bool inverse, const ObjectVector& list_in, ObjectVector& list_out)
{
    list_out.clear();
    
    try
    {
        const boost::regex e(nameregexp.c_str());

        for(unsigned int i = 0;i < list_in.size();i++)   
        {
            if(regex_match(list_in[i]->getName().c_str(), e) != inverse)
            {
                list_out.push_back(list_in[i]);
            }
        }
    }
    catch(...)
    {
        list_out.clear();
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegExpByProperty(const Ogre::String& nameregexp, bool inverse, const ObjectVector& list_in, ObjectVector& list_out)
{
    list_out.clear();

    try
    {
        const boost::regex e(nameregexp.c_str());

        OgitorsPropertyVector pvec;

        for(unsigned int i = 0;i < list_in.size();i++)   
        {
            pvec = list_in[i]->getProperties()->getPropertyVector();

            bool add_list = false;
            
            for(unsigned int k = 0;k < pvec.size();k++)
            {
                if(regex_match(pvec[k]->getName().c_str(), e))
                {
                    add_list = true;
                    break;
                }
            }
            
            if(add_list != inverse)
                list_out.push_back(list_in[i]);
        }
    }
    catch(...)
    {
        list_out.clear();
    }
}    
//-----------------------------------------------------------------------------------------
void OgitorsRoot::RegExpByCustomProperty(const Ogre::String& nameregexp, bool inverse, const ObjectVector& list_in, ObjectVector& list_out)
{
    list_out.clear();

    try
    {
        const boost::regex e(nameregexp.c_str());

        OgitorsPropertyVector pvec;

        for(unsigned int i = 0;i < list_in.size();i++)   
        {
            pvec = list_in[i]->getCustomProperties()->getPropertyVector();

            bool add_list = false;
            
            for(unsigned int k = 0;k < pvec.size();k++)
            {
                if(regex_match(pvec[k]->getName().c_str(), e))
                {
                    add_list = true;
                    break;
                }
            }
            
            if(add_list != inverse)
                list_out.push_back(list_in[i]);
        }
    }
    catch(...)
    {
        list_out.clear();
    }
}    
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetObjectListByName(unsigned int type, const Ogre::String& nameregexp, bool inverse, ObjectVector& list)
{
    list.clear();

    try
    {
        const boost::regex e(nameregexp.c_str());

        NameObjectPairList::iterator list_st, list_ed;

        if(type == 0)
        {
            list_st = mNameList.begin();
            list_ed = mNameList.end();
        }
        else
        {
            list_st = mNamesByType[type].begin();
            list_ed = mNamesByType[type].end();
        }

        while(list_st != list_ed)
        {
            if(regex_match(list_st->first.c_str(), e) != inverse)
            {
                list.push_back(list_st->second);
            }
            list_st++;
        }
    }
    catch(...)
    {
        list.clear();
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetObjectListByProperty(unsigned int type, const Ogre::String& nameregexp, bool inverse, ObjectVector& list)
{
    list.clear();

    try
    {
        const boost::regex e(nameregexp.c_str());

        NameObjectPairList::iterator list_st, list_ed;

        if(type == 0)
        {
            list_st = mNameList.begin();
            list_ed = mNameList.end();
        }
        else
        {
            list_st = mNamesByType[type].begin();
            list_ed = mNamesByType[type].end();
        }

        OgitorsPropertyVector pvec;

        while(list_st != list_ed)
        {
            pvec = list_st->second->getProperties()->getPropertyVector();
            bool add_list = false;
            for(unsigned int k = 0;k < pvec.size();k++)
            {
                if(regex_match(pvec[k]->getName().c_str(), e))
                {
                    add_list = true;
                    break;
                }
            }
            
            if(add_list != inverse)
                list.push_back(list_st->second);
            
            list_st++;
        }
    }
    catch(...)
    {
        list.clear();
    }
}
//-----------------------------------------------------------------------------------------
void OgitorsRoot::GetObjectListByCustomProperty(unsigned int type, const Ogre::String& nameregexp, bool inverse, ObjectVector& list)
{
    list.clear();

    try
    {
        const boost::regex e(nameregexp.c_str());

        NameObjectPairList::iterator list_st, list_ed;

        if(type == 0)
        {
            list_st = mNameList.begin();
            list_ed = mNameList.end();
        }
        else
        {
            list_st = mNamesByType[type].begin();
            list_ed = mNamesByType[type].end();
        }

        OgitorsPropertyVector pvec;

        while(list_st != list_ed)
        {
            pvec = list_st->second->getCustomProperties()->getPropertyVector();
            bool add_list = false;
            for(unsigned int k = 0;k < pvec.size();k++)
            {
                if(regex_match(pvec[k]->getName().c_str(), e))
                {
                    add_list = true;
                    break;
                }
            }
            
            if(add_list != inverse)
                list.push_back(list_st->second);
            
            list_st++;
        }
    }
    catch(...)
    {
        list.clear();
    }
}
//-----------------------------------------------------------------------------------------

}
