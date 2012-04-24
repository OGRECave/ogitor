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

#include "MeshMagickPrerequisites.h"

#include "MmMeshMergeToolFactory.h"
#include "MmInfoToolFactory.h"
#include "MmOgreEnvironment.h"
#include "MmOptimiseToolFactory.h"
#include "MmOptionsParser.h"
#include "MmRenameToolFactory.h"
#include "MmTool.h"
#include "MmToolManager.h"
#include "MmTransformToolFactory.h"

using namespace Ogre;
using namespace meshmagick;

void printHelp(void)
{
    // Print help message
    std::cout << std::endl << "MeshMagick "
		<< MESHMAGICK_VERSION_MAJOR << "."
		<< MESHMAGICK_VERSION_MINOR << "."
		<< MESHMAGICK_VERSION_PATCH << " - versatile Ogre mesh manipulation tool." << std::endl;
    std::cout << "Copyright 2007-2008 by Daniel Wickert" << std::endl << std::endl;
    std::cout << "Usage: MeshMagick [global_options] toolname [tool_options] infile(s) -- [outfile(s)]" << std::endl;
    std::cout << "Global options:" << std::endl;
    std::cout << "    -help               = Prints this help text" << std::endl;
    std::cout << "    -help=toolname      = Prints help for the specified tool" << std::endl;
    std::cout << "    -list               = Lists available tools" << std::endl;
    std::cout << "    -no-follow-skeleton = Do not follow Skeleton-Link (if applicable)" << std::endl;
    std::cout << "    -quiet              = Supress all messages to cout." << std::endl;
    std::cout << "    -verbose            = Print more detailed messages." << std::endl;
    std::cout << "    -version            = Print meshmagick version." << std::endl;
    std::cout << std::endl;
    std::cout << "If no outfile is specified, the infile is overwritten. (if applicable)" << std::endl;
    std::cout << std::endl;
}

struct CommandLine
{
    String commandName;
    int globalArgc;
    const char** globalArgv;
    String toolName;
    int toolArgc;
    const char** toolArgv;
    StringVector inFileNames;
    StringVector outFileNames;
};

CommandLine parseCommandLine(int argc, const char** argv)
{
    // Parse command line ...

    CommandLine cmdLine;
    cmdLine.commandName = argv[0];

    int idx = 1; // Current argv-index for the following for-loops to start with
    // Determine number of global arguments
    int numGlobalArgs = 0;
    for (int i = idx; i < argc; ++i)
    {
        String arg = argv[i];
        if (!arg.empty() && arg.at(0) != '-')
        {
            // This item has to be the toolname
            cmdLine.toolName = arg;

            // We are done with counting
            numGlobalArgs = i - 1;

            idx = i + 1;

            break;
        }
        else if (i == argc - 1)
        {
            // There is no toolname
            numGlobalArgs = i;
        }
    }

    cmdLine.globalArgc = numGlobalArgs;
    cmdLine.globalArgv = numGlobalArgs > 0 ? argv + 1 : NULL;

    if (!cmdLine.toolName.empty())
    {
        // determine number of tool arguments
        int numToolArgs = 0;
        for (int i = idx; i < argc; ++i)
        {
            String arg = argv[i];
            if (!arg.empty() && arg.at(0) != '-')
            {
                // this item is the infile
                cmdLine.inFileNames.push_back(arg);

                // We are done with counting
                numToolArgs = i - numGlobalArgs - 2;

                idx = i + 1;

                break;
            }
        }

        cmdLine.toolArgc = numToolArgs;
        cmdLine.toolArgv = numToolArgs > 0 ? argv + numGlobalArgs + 2 : NULL;

        // Are there further input files?
        for (int i = idx; i < argc; ++i)
        {
            String arg = argv[i];
            if (arg == "--")
            {
                // No more input files
                idx = i + 1;
                break;
            }
            else
            {
                // this item is the infile
                cmdLine.inFileNames.push_back(arg);
            }
        }


        // If there are elements left in the argv, these have to be output files
        for (int i = idx; i < argc; ++i)
        {
            String arg = argv[i];
            cmdLine.outFileNames.push_back(arg);
        }
    }

    return cmdLine;
}

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        printHelp();
        return -1;
    }

    ToolManager manager;
    manager.registerToolFactory(new TransformToolFactory());
    manager.registerToolFactory(new InfoToolFactory());
    manager.registerToolFactory(new MeshMergeToolFactory());
    manager.registerToolFactory(new RenameToolFactory());
	manager.registerToolFactory(new OptimiseToolFactory());

    OgreEnvironment* ogreEnv = new OgreEnvironment();
	ogreEnv->initialize();

    CommandLine cmdLine = parseCommandLine(argc, argv);

    // Define allowed global arguments
    OptionDefinitionSet globalOptionDefs = OptionDefinitionSet();
    globalOptionDefs.insert(OptionDefinition("help", OT_STRING, false, false, Any(String())));
    globalOptionDefs.insert(OptionDefinition("list"));
    globalOptionDefs.insert(OptionDefinition("no-follow-skeleton"));
    globalOptionDefs.insert(OptionDefinition("version"));
    globalOptionDefs.insert(OptionDefinition("quiet"));
    globalOptionDefs.insert(OptionDefinition("verbose"));

	OptionList globalOptions;
	try
	{
        globalOptions = OptionsParser::parseOptions(
            cmdLine.globalArgc, cmdLine.globalArgv, globalOptionDefs);
	}
    catch (std::exception& se)
    {
		std::cout << "Parsing global options failed:" << std::endl;
        std::cout << se.what() << std::endl;
        return -1;
    }
    catch (...)
    {
		std::cout << "Parsing global options failed." << std::endl;
        return -1;
    }

    // Evaluate global options (as far as they are of interest here...)
    for (OptionList::const_iterator it = globalOptions.begin(); it != globalOptions.end(); ++it)
    {
        if (it->first == "version")
        {
			std::cout << "MeshMagick version "
                << MESHMAGICK_VERSION_MAJOR << "."
                << MESHMAGICK_VERSION_MINOR << "."
                << MESHMAGICK_VERSION_PATCH << std::endl;
			std::cout << "using Ogre version "
                << OGRE_VERSION_MAJOR << "."
                << OGRE_VERSION_MINOR << "."
                << OGRE_VERSION_PATCH
                << " (" << OGRE_VERSION_NAME ")"
                << std::endl;
            return 0;
        }
        else if (it->first == "help")
        {
            String toolName = any_cast<String>(it->second);
            if (!toolName.empty())
            {
                // toolhelp
				try
				{
	                manager.printToolHelp(toolName, std::cout);
				}
				catch (std::exception& se)
				{
					std::cout << se.what() << std::endl << std::endl;
					// global help
					printHelp();
				}
                return 0;
            }
            else
            {
                // global help
                printHelp();
                return 0;
            }
        }
        else if (it->first == "list")
        {
            manager.printToolList(std::cout);
            return 0;
        }
    }

    if (cmdLine.toolName.empty())
    {
        std::cout << "No tool given." << std::endl << "call \""
            << argv[0] << "\" -h for help" << std::endl;
        return -1;
    }

    // create and invoke tool
    try
    {
        manager.invokeTool(cmdLine.toolName, globalOptions, cmdLine.toolArgc, cmdLine.toolArgv,
            cmdLine.inFileNames, cmdLine.outFileNames);
    }
    catch (std::exception& se)
    {
        std::cout << "Invocation of tool " << cmdLine.toolName << " failed:" << std::endl;
        std::cout << se.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cout << "Invocation of tool " << cmdLine.toolName << " failed." << std::endl;
        return -1;
    }

    return 0;
}
