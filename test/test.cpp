/* latex-converter plugin for Gnome Evolution
 * Copyright (C) 2025 Cyril Soler.
 *
 * Authors and maintainers:
 *     April 2025:  Cyril Soler <cyril.soler@inria.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <fstream>
#include "argstream.h"

extern bool convertText_cpp(const std::string& input,std::string& output,std::string& error);

void showHelp(const char *name)
{
	std::cerr << "Testing code for evolution LaTeX plugin.\nUsage:\n   "  
		<< name << " -i [input file] [-o output]" 
		<< std::endl
		<< std::endl;
}

int main(int argc,char *argv[])
{
	try 
	{
		argstream as(argc,argv);

		std::string input_file;
		std::string output_file;

		as >> parameter('i',"input",input_file,"input file with LaTeX equations",false)
			>> parameter('o',"output",output_file,"output HTML file",false)
			>> help() ;

		as.defaultErrorHandling();

		if(input_file.empty())
		{
			showHelp(argv[0]);
			return 0;
		}
		// read the file

		std::string input_text ;
		{
			std::ifstream i(input_file.c_str());
			if(!i)
				throw std::runtime_error("Cannot read input file \"" + input_file + "\"");

			std::ostringstream buffer;
			buffer << i.rdbuf();
			input_text = buffer.str();
		}

		// convert it
		//
		std::string output_text;
		std::string error_string;

        if(!convertText_cpp(input_text,output_text,error_string))
			throw std::runtime_error("Error while converting file: " + error_string);

		// save the file
		//
		if(!output_file.empty())
		{
			std::ofstream o(output_file.c_str());

			o << "<html><body>";
			o << output_text;
			o << "</body></html>";
			o.close();
		}
		else
			std::cout << output_text;

		return 0;
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception raised: " << e.what() << std::endl;
		return false;
	}
}

