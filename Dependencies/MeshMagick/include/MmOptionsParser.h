/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2009 Daniel Wickert

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef __MM_OPTIONSPARSER_H__
#define __MM_OPTIONSPARSER_H__

#include "MeshMagickPrerequisites.h"

#ifdef __APPLE__
#	include <Ogre/OgreString.h>
#	include <Ogre/OgreAny.h>
#else
#	include <OgreString.h>
#	include <OgreAny.h>
#endif


#include <vector>
#include <set>

namespace meshmagick
{
    typedef std::pair<Ogre::String, Ogre::Any> Option;
    typedef std::vector<Option> OptionList;

    /// Enum of valid command option types, that the parser is able to process
    typedef enum {OT_BOOL, OT_INT, OT_REAL, OT_VECTOR3, OT_QUATERNION, OT_STRING,
        OT_SELECTION} OptionType;

    /// This struct defines the syntax and parts of the semantics of a command line option,
    /// so that the OptionsParser is able to parse the option.
    struct _MeshMagickExport OptionDefinition
    {
        /// Name of the option as written on the command line
        Ogre::String name;
        /// Expected type
        OptionType type;
        /// Whether user has to set this option
        bool isMandatory;
        /// Whether this option can occur more than once.
        bool allowMultiple;
        /// Default value, if not given by user. If it is empty, then no default.
        Ogre::Any defaultValue;
        /// Contains the selection options. First character determines the delimiter
        /// between the options. Example: "/one/two/three", means valid options are
        /// one two and three, '/' is used as delimiter.
        /// this value is only used for OT_SELECTION as type.
        Ogre::String selection;

        /**
         *  Constructs and completely initialises an OptionDefinition.
         *  @param n Name of the option.
         *  @param t Type of the option.
         *  @param im Whether the option is mandatory.
         *  @param am Whether multiple occurrences of the option are allowed.
         *  @param dv default value returned, if option is not set.
         *  @param s if Type t is OT_SELECTION, this string contains the valid options.
         *  @see OptionDefinition#selection
         */
        OptionDefinition(const Ogre::String& n,
                         OptionType t = OT_BOOL,
                         bool im = false,
                         bool am = false,
                         Ogre::Any dv = Ogre::Any(),
                         const Ogre::String s = Ogre::StringUtil::BLANK)
            : name(n),
              type(t),
              isMandatory(im),
              allowMultiple(am),
              defaultValue(dv),
              selection(s) {}

        OptionDefinition(const OptionDefinition& rhs)
        {
            name          = rhs.name;
            type          = rhs.type;
            isMandatory   = rhs.isMandatory;
            allowMultiple = rhs.allowMultiple;
            defaultValue  = rhs.defaultValue;
            selection     = rhs.selection;
        }

        OptionDefinition& operator=(const OptionDefinition& rhs)
        {
            if (this != &rhs)
            {
                name          = rhs.name;
                type          = rhs.type;
                isMandatory   = rhs.isMandatory;
                allowMultiple = rhs.allowMultiple;
                defaultValue  = rhs.defaultValue;
                selection     = rhs.selection;
            }
            return *this;
        }

        bool operator<(const OptionDefinition& rhs) const
        {
            return name < rhs.name;
        }

        bool operator==(const OptionDefinition& rhs) const
        {
            return name == rhs.name;
        }

        bool operator==(const Ogre::String& rhs) const
        {
            return name == rhs;
        }
    };

    typedef std::set<OptionDefinition> OptionDefinitionSet;

    class _MeshMagickExport OptionsParser
    {
    public:
        static OptionList parseOptions(int argc, const char** argv,
            const OptionDefinitionSet& optionDefs);
    };

    /// Simple helper class to work with options.
    class OptionsUtil
    {
    public:
        /// Returns true, if named bool option is set.
        static bool isOptionSet(const OptionList& options, const Ogre::String& name);
        /// Returns the String value of named option or def, if option not set.
        static Ogre::String getStringOption(const OptionList& options, const Ogre::String& name,
            const Ogre::String& def=Ogre::StringUtil::BLANK);
        /// Returns the display string of an OptionType enum value.
        static Ogre::String getTypeName(OptionType type);
    };
}

#endif
