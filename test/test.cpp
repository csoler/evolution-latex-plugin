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

