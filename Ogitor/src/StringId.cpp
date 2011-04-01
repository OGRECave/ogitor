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
//
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

#if( OGRE_DEBUG_MODE == 1)
    #define STRINGID_COLLISION_CHECK 1
#else
    #define STRINGID_COLLISION_CHECK 0
#endif

namespace Ogitors
{

    namespace HASH
    {

    #if (defined(__GNUC__) && defined(__i386__)) || defined(_MSC_VER)
    #define get16bits(d) (*((const Ogre::uint16 *) (d)))
    #endif

    #if !defined (get16bits)
    #define get16bits(d) ((((Ogre::uint32)(((const Ogre::uint8 *)(d))[1])) << 8)\
                           +(Ogre::uint32)(((const Ogre::uint8 *)(d))[0]) )
    #endif

        Ogre::uint32 SuperFastHash (const char * data, int len) 
        {
            Ogre::uint32 hash = len, tmp;
            int rem;

            if (len <= 0 || data == NULL) return 0;

            rem = len & 3;
            len >>= 2;

            /* Main loop */
            for (;len > 0; len--) {
                hash  += get16bits (data);
                tmp    = (get16bits (data+2) << 11) ^ hash;
                hash   = (hash << 16) ^ tmp;
                data  += 2*sizeof (Ogre::uint16);
                hash  += hash >> 11;
            }

            /* Handle end cases */
            switch (rem) {
                case 3: hash += get16bits (data);
                        hash ^= hash << 16;
                        hash ^= data[sizeof (Ogre::uint16)] << 18;
                        hash += hash >> 11;
                        break;
                case 2: hash += get16bits (data);
                        hash ^= hash << 11;
                        hash += hash >> 17;
                        break;
                case 1: hash += *data;
                        hash ^= hash << 10;
                        hash += hash >> 1;
            }

            /* Force "avalanching" of final 127 bits */
            hash ^= hash << 3;
            hash += hash >> 5;
            hash ^= hash << 4;
            hash += hash >> 17;
            hash ^= hash << 25;
            hash += hash >> 6;

            return hash;
        }
    }

    StringId::stringid_string_map* StringId::stringid_assert_map = 0;

    StringId::StringId(const char *str)
    {
        unsigned int len = strlen(str);
        mID = HASH::SuperFastHash(str, len);

#if( STRINGID_COLLISION_CHECK == 1 )
        if(stringid_assert_map == 0)
            stringid_assert_map = new stringid_string_map();

        stringid_string_map::iterator it = stringid_assert_map->find(mID);
        if(it != stringid_assert_map->end())
        {
            assert(it->second == std::string(str));
        }
        else
            (*stringid_assert_map)[mID] = std::string(str);
#endif 
    }

    StringId::StringId(const std::string& str)
    {
        unsigned int len = str.length();
        mID = HASH::SuperFastHash(str.c_str(), len);

#if( STRINGID_COLLISION_CHECK == 1 )
        if(stringid_assert_map == 0)
            stringid_assert_map = new stringid_string_map();

        stringid_string_map::iterator it = stringid_assert_map->find(mID);
        if(it != stringid_assert_map->end())
        {
            assert(it->second == str);
        }
        else
            (*stringid_assert_map)[mID] = str;
#endif
    }

    bool StringId::operator==(const StringId& strID) const
    {
        return (mID == strID.mID);
    }

    bool StringId::operator==(const std::string& str) const
    {
        unsigned int len = str.length();
        return (mID == HASH::SuperFastHash(str.c_str(), len));
    }

    bool StringId::operator==(const char *str) const
    {
        unsigned int len = strlen(str);
        return (mID == HASH::SuperFastHash(str, len));
    }

    bool StringId::operator<(const StringId& strID) const
    {
        return (mID < strID.mID);
    }
 }