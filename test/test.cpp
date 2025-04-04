#include <iostream>
#include <fstream>
#include "argstream.h"

//-===============================================================================================-//
//-====================================  Code to export ==========================================-//
//-===============================================================================================-//

#include <vector>

struct Equation {
	std::string latex_code;
	int start;
	int end;
	std::string base64_code;
};
bool parseFile(const std::string& file,std::vector<Equation>& latex_equations,std::string& err_str)
{
	return true;
}

bool convertEquation(const std::string& latex_code,std::string& base64_image_code,std::string& err_str)
{
	return true;
}

bool replaceEquations(const std::string& input_file,const std::vector<Equation>& eqns,std::string& output_file,std::string& err_str)
{
	return true;
}

bool convertText(const std::string& input,std::string& output,std::string& error)
{
	std::vector<Equation> eqns;
	std::string err_str;

	if(!parseFile(input,eqns,err_str))
	{
		error = "Error while parsing file: " + err_str ;
		return false;
	}
	std::cerr << "Read " << eqns.size() << " equations." << std::endl;

	for(uint32_t i=0;i<eqns.size();++i)
		if(!convertEquation(eqns[i].latex_code,eqns[i].base64_code,err_str))
		{
			error = "Error while converting equation \"" + eqns[i].latex_code + "\": " + err_str ;
			return false;
		}

	if(!replaceEquations(input,eqns,output,err_str))
	{
		error = "Error: " + err_str;
		return false;
	}

	return true;
}
//-===============================================================================================-//
//-====================================  END code to export ======================================-//
//-===============================================================================================-//

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

		if(!convertText(input_text,output_text,error_string))
			throw std::runtime_error("Error while converting file: " + error_string);

		// save the file
		//
		if(!output_file.empty())
		{
			std::ofstream o(output_file.c_str());
			o << output_text;
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

