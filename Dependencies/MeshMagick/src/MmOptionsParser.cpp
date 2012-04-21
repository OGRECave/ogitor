/*
This file is part of MeshMagick - An Ogre mesh file manipulation tool.
Copyright (C) 2007-2010 Daniel Wickert

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "MmOptionsParser.h"

#include <OgreStringConverter.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>

#include <algorithm>
#include <stdexcept>

using namespace Ogre;

namespace meshmagick
{
    OptionList OptionsParser::parseOptions(int argc, const char** argv,
        const OptionDefinitionSet& optionDefs)
    {
        OptionList options;
        for (int i = 0; i < argc; ++i)
        {
            String arg = argv[i];

            // find the name part, therefore delimit it at pos of '=', or the end of the string,
            // if no '=' found.
            size_t dPos = arg.find('=');

            // first char is '-', so ommit it.
            String name = arg.substr(1, dPos - 1);
            
            // Get the option definition for this arg.
            // Medium ugly hack. OptionDefinitions are equal, when their names are equal..
            OptionDefinitionSet::const_iterator it = optionDefs.find(OptionDefinition(name));
            if (it == optionDefs.end())
            {
                throw std::logic_error("unrecognised option: " + name);
            }
            OptionDefinition optionDef = *it;

            if (dPos == Ogre::String::npos)
            {
                if (optionDef.type == OT_BOOL)
                {
                    // Set it to true
                    options.push_back(Option(name, Any(true)));
                }
                else
                {
                    // Different type, see if there is a default value.
                    if (!optionDef.defaultValue.isEmpty())
                    {
                        options.push_back(Option(name, optionDef.defaultValue));
                    }
                    else
                    {
                        // No default value, then option is misused.
                        throw std::logic_error("option " + name + " expects an parameter of type "
                            + OptionsUtil::getTypeName(optionDef.type));
                    }
                }
            }
            else
            {
                // We have a parameter for this option
                String param = arg.substr(dPos + 1);
                Any value;

                // Parse it, depending on the type of this option
                if (optionDef.type == OT_INT)
                {
                    value = Any(StringConverter::parseInt(param));
                }
                else if (optionDef.type == OT_REAL)
                {
                    value = Any(StringConverter::parseReal(param));
                }
                else if (optionDef.type == OT_STRING)
                {
                    value = Any(param);
                }
                else if (optionDef.type == OT_VECTOR3)
                {
                    // We expect three Real components delimited by '/'
                    StringVector components = StringUtil::split(param, "/");
                    if (components.size() != 3)
                    {
                        throw std::logic_error("misformed parameter for option " + name);
                    }
                    value = Any(Vector3(
                        StringConverter::parseReal(components[0]),
                        StringConverter::parseReal(components[1]),
                        StringConverter::parseReal(components[2])));
                }
                else if (optionDef.type == OT_QUATERNION)
                {
                    // We expect four Real components delimited by '/'
                    // First is the angle in degree, the three following are the axis
                    StringVector components = StringUtil::split(param, "/");
                    if (components.size() != 4)
                    {
                        throw std::logic_error("misformed parameter for option " + name);
                    }
                    value = Any(Quaternion(
                        Degree(StringConverter::parseReal(components[0])),
                        Vector3(StringConverter::parseReal(components[1]),
                            StringConverter::parseReal(components[2]),
                            StringConverter::parseReal(components[3])).normalisedCopy()));
                }
                else if (optionDef.type == OT_SELECTION)
                {
                    // Valid parameters are delimited by the first char of the selection string
                    StringVector components = StringUtil::split(optionDef.selection,
                        optionDef.selection.substr(0, 1));

                    // Is the param among those?
                    StringVector::const_iterator it =
                        std::find(components.begin(), components.end(), param);
                    if (it != components.end())
                    {
                        value = Any(param);
                    }
                    else
                    {
                        throw std::logic_error("misformed parameter for option " + name);
                    }
                }
                else
                {
                    throw std::logic_error("unrecognised type");
                }
                options.push_back(Option(name, value));
            }
        }
        return options;
    }

    //////////////////////////////////////////////////////////////////////////

    struct find_option_by_name : public std::binary_function<Option, String, bool>
    {
        bool operator()(const Option& option, const String& name) const
        {
            return option.first == name;
        }
    };

    bool OptionsUtil::isOptionSet(const OptionList& options, const String& name)
    {
        OptionList::const_iterator it = std::find_if(options.begin(), options.end(),
            std::bind2nd(find_option_by_name(), name));
        return it != options.end();
    }

    String OptionsUtil::getStringOption(const OptionList& options, const Ogre::String& name, 
        const String& def)
    {
        OptionList::const_iterator it = std::find_if(options.begin(), options.end(),
            std::bind2nd(find_option_by_name(), name));
        if (it != options.end())
        {
            try
            {
                return any_cast<String>((*it).second);
            }
            catch (...)
            {
            	return def;
            }
        }
        else
        {
            return def;
        }
    }

    String OptionsUtil::getTypeName(OptionType type)
    {
        if (type == OT_BOOL)
        {
            return "bool";
        }
        else if (type == OT_INT)
        {
            return "real";
        }
        else
        {
            throw std::logic_error("no such option type.");
        }
    }
}
