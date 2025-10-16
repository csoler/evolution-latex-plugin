/* latex-converter plugin for Gnome Evolution
 * Copyright (C) 2016 Red Hat, Inc. (www.redhat.com)
 * Copyright (C) 2025 Cyril Soler.
 *
 * Authors and maintainers:
 *           2016: Red Hat, Inc. (evolution plugin example code)
 *     April 2025: Cyril Soler <cyril.soler@inria.fr>
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

#include <math.h>
#include <string.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#include <libpng/png.h>
#include "latex-converter.h"

int get_png_size(const std::string& filename, int& width, int& height)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) return -1;

    // Read and validate the PNG signature (first 8 bytes)
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fclose(fp);
        return -2;  // Not a PNG
    }

    // Initialize png structs
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return -3;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return -4;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        // libpng will jump here if there's an error
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return -5;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8); // We already read the first 8 bytes

    png_read_info(png_ptr, info_ptr);

    width  = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);

    // Cleanup
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return 0;
}

struct Equation {
    std::string latex_code;
    int start;
    int end;
    int width;
    int height;
    std::string base64_code;
};
bool parseFile(const std::string& file,std::vector<Equation>& eqns,std::string& err_str)
{
    int i=0;
    int current_eqn = 0;
    bool inside_equation = false;
    int last_start = 0;

    while(i < file.length())
    {
        if(file[i] == '$')
        {
            if(inside_equation)
            {
                Equation eq;
                eq.start = last_start;
                eq.end = i;
                eq.latex_code = file.substr(last_start+1,i-last_start-1);

                std::cerr << "Found new equation \"" << eq.latex_code << "\"" << std::endl;
                eqns.push_back(eq);
                inside_equation = false;
            }
            else
            {
                last_start = i;
                inside_equation = true;
            }
        }
        ++i;
    }
    return true;
}

bool convertEquation(const std::string& latex_code,std::string& base64_image_code,int& width,int& height,int& err_code,std::string& error_details)
{
    //char template_c[500] = "/tmp/tmp_file_evolution_latex_plugin_XXXXXX" ;
    char template_c[500] = "tmp_file_evolution_latex_plugin_XXXXXX" ;
    mkstemp(template_c);

    err_code = LATEX_CONVERTER_ERROR_CODE_NONE;
    error_details.clear();

    std::string template_str(template_c) ;

    //template_str = "tmp" ;// debug

    // create a temporary LaTeX file
    //
    std::ofstream tmp_latex_file(template_str + ".tex");

    tmp_latex_file << "\\documentclass{article}\n" ;
    tmp_latex_file << "\\usepackage{times}\n" ;
    tmp_latex_file << "\\usepackage{graphicx}\n" ;
    tmp_latex_file << "\\pagestyle{empty}\n" ;
    tmp_latex_file << "\\begin{document}\n" ;
    tmp_latex_file << "\\begin{center}\n";
    tmp_latex_file << "$" + latex_code + "$\n";
    tmp_latex_file << "\\end{center}\n";
    tmp_latex_file << "\\end{document}\n";
    tmp_latex_file.close();

    // make a temporary file

    std::string command_latex    = "latex -interaction=nonstopmode " + template_str + ".tex";
    std::string command_dvips    = "dvips -E -x 16000 " + template_str + ".dvi";
    std::string command_ps2pdf   = "ps2pdf -dEPSCrop " + template_str + ".ps " + template_str + ".pdf";
    std::string command_pdftoppm = "pdftocairo -singlefile -transp -png " + template_str + ".pdf " + template_str ;
    std::string command_base64   = "base64 " + template_str + ".png > " + template_str + ".b64";

    // compile the files
    //
    //
    if(system(command_latex.c_str()))
    {
        std::ostringstream buffer;
        buffer << std::ifstream(template_str+".log").rdbuf();
        error_details = buffer.str();
        err_code = LATEX_CONVERTER_ERROR_CODE_LATEX;
        return false;
    }
    if(system(command_dvips.c_str()))    { err_code = LATEX_CONVERTER_ERROR_CODE_DVIPS; return false; }
    if(system(command_ps2pdf.c_str()))   { err_code = LATEX_CONVERTER_ERROR_CODE_PS2PDF; return false; }
    if(system(command_pdftoppm.c_str())) { err_code = LATEX_CONVERTER_ERROR_CODE_PDF2PNG; return false; }
    if(system(command_base64.c_str()))   { err_code = LATEX_CONVERTER_ERROR_CODE_BASE64; return false; }

    get_png_size(template_str + ".png",width,height);

    // read the base64 file
    //
    std::ostringstream buffer;
    buffer << std::ifstream(template_str+".b64").rdbuf();
    base64_image_code = buffer.str();

    system( ("rm -f " + template_str + ".tex").c_str() );
    system( ("rm -f " + template_str + ".dvi").c_str() );
    system( ("rm -f " + template_str + ".ps" ).c_str() );
    system( ("rm -f " + template_str + ".pdf").c_str() );
    system( ("rm -f " + template_str + ".png").c_str() );
    system( ("rm -f " + template_str + ".b64").c_str() );
    system( ("rm -f " + template_str + ".log").c_str() );
    system( ("rm -f " + template_str + ".aux").c_str() );
    system( ("rm -f " + template_str).c_str() );

    return true;
}

// replace all \n into a <br/> statement

std::string substituteEndl(const std::string& input)
{
    std::string output;

    for(uint i=0;i<input.size();++i)
        if(input[i] == '\n')
            output.append("<BR/>");
        else
            output.push_back(input[i]);

    return output;
}
bool replaceEquations(const std::string& input_file,const std::vector<Equation>& eqns,std::string& output_file,std::string& /*err_str*/)
{
    int last_end = 0;

    for(size_t i=0;i<eqns.size();++i)
    {
        output_file.append(substituteEndl(input_file.substr(last_end,eqns[i].start-last_end))); // concats whatever is before the eq
        output_file.append("<IMG src=\"data:image/png;base64,");
        output_file.append(eqns[i].base64_code);

        char s[100];
        int size = eqns[i].height/271.0 * 16.0;
        sprintf(s,"%dpx",(int)rint(size));

        output_file.append("\" alt=\"$" + eqns[i].latex_code + "$\" style=\"height:"+s+"; vertical-align:middle;\"> ");
        last_end = eqns[i].end+1;
    }
    output_file.append(substituteEndl(input_file.substr(last_end,input_file.size())));

    return true;
}

bool convertText_cpp(const std::string& input,std::string& output,int& error_code,std::string& error_details)
{
    std::cerr << "Converting a file of " << input.size() << " characters." << std::endl
              << std::endl << "===========INPUT TEXT================" << std::endl
              << input << std::endl << "=====================================" << std::endl ;

    std::vector<Equation> eqns;
    std::string err_str;

    if(!parseFile(input,eqns,err_str))
    {
        error_code = LATEX_CONVERTER_ERROR_CODE_PARSING;
        return false;
    }
    std::cerr << "Read " << eqns.size() << " equations." << std::endl;

    for(uint32_t i=0;i<eqns.size();++i)
        if(!convertEquation(eqns[i].latex_code,eqns[i].base64_code,eqns[i].width,eqns[i].height,error_code,error_details))
            return false;

    if(!replaceEquations(input,eqns,output,err_str))
    {
        error_code = LATEX_CONVERTER_ERROR_CODE_SUBSTIT;
        return false;
    }

    std::cerr << "Converted text: " << std::endl << "==========OUTPUT TEXT================" << std::endl
              << output << std::endl << "=====================================" << std::endl ;
    return true;
}

static char *duplicate_string(const std::string& s)
{
    char *res = (char*)malloc(s.size()+1);
    memcpy(res,s.c_str(),s.size());
    res[s.size()] = 0;
    return res;
}

extern "C" {

bool convertText(const char *input,char **output,int *error_code,char **error_info)
{
    std::string result;
    int err_code;
    std::string err_details;
    *error_info = NULL;

    if(convertText_cpp(std::string(input),result,err_code,err_details))
    {
        *output = duplicate_string(result);
        *error_code = LATEX_CONVERTER_ERROR_CODE_NONE ;
        return true;
    }
    else
    {
        *error_info = duplicate_string(err_details);
        *error_code = err_code;
        return false;
    }
}

}
