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

bool convertEquation(const std::string& latex_code,std::string& base64_image_code,int& width,int& height,std::string& err_str)
{
    //char template_c[500] = "/tmp/tmp_file_evolution_latex_plugin_XXXXXX" ;
    char template_c[500] = "tmp_file_evolution_latex_plugin_XXXXXX" ;
    mkstemp(template_c);

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

    std::string command_latex    = "latex " + template_str + ".tex";
    std::string command_dvips    = "dvips -E -x 16000 " + template_str + ".dvi";
    std::string command_ps2pdf   = "ps2pdf -dEPSCrop " + template_str + ".ps " + template_str + ".pdf";
    std::string command_pdftoppm = "pdftocairo -singlefile -transp -png " + template_str + ".pdf " + template_str ;
    std::string command_base64   = "base64 " + template_str + ".png > " + template_str + ".b64";

    // compile the files
    //
    //
    system(command_latex.c_str());
    system(command_dvips.c_str());
    system(command_ps2pdf.c_str());
    system(command_pdftoppm.c_str());
    system(command_base64.c_str());

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
    //system( ("rm -f " + template_str + ".png").c_str() );
    system( ("rm -f " + template_str + ".b64").c_str() );
    system( ("rm -f " + template_str + ".log").c_str() );
    system( ("rm -f " + template_str + ".aux").c_str() );
    system( ("rm -f " + template_str).c_str() );

    return true;
}

bool replaceEquations(const std::string& input_file,const std::vector<Equation>& eqns,std::string& output_file,std::string& err_str)
{
    int last_end = 0;

    for(int i=0;i<eqns.size();++i)
    {
        output_file.append(input_file.substr(last_end,eqns[i].start-last_end)); // concats whatever is before the eq
        output_file.append("<IMG src=\"data:image/png;base64,");
        output_file.append(eqns[i].base64_code);

        char s[100];
        int size = eqns[i].height/271.0 * 16.0;
        sprintf(s,"%dpx",(int)rint(size));

        output_file.append("\" alt=\"" + eqns[i].latex_code + "\" style=\"height:"+s+"; vertical-align:middle;\">");
        last_end = eqns[i].end+1;
    }
    output_file.append(input_file.substr(last_end,input_file.size()));

    return true;
}

bool convertText_cpp(const std::string& input,std::string& output,std::string& error)
{
    std::cerr << "Converting a file of " << input.size() << " characters." << std::endl;

    std::vector<Equation> eqns;
    std::string err_str;

    if(!parseFile(input,eqns,err_str))
    {
        error = "Error while parsing file: " + err_str ;
        return false;
    }
    std::cerr << "Read " << eqns.size() << " equations." << std::endl;

    for(uint32_t i=0;i<eqns.size();++i)
        if(!convertEquation(eqns[i].latex_code,eqns[i].base64_code,eqns[i].width,eqns[i].height,err_str))
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

extern "C" {

bool convertText(const char *input,char **output,int *error_code)
{
    std::string result;
    std::string err_str;

    if(convertText_cpp(std::string(input),result,err_str))
    {
        *output = (char*)malloc(result.size()+1);
        memcpy(*output,result.c_str(),result.size());
        (*output)[result.size()] = 0;

        *error_code = LATEX_CONVERTER_ERROR_CODE_NO_ERROR ;
        return true;
    }
    else
    {
        *error_code = LATEX_CONVERTER_ERROR_CODE_ERROR ;
        return false;
    }
}

}
