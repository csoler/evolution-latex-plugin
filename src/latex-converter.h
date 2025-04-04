
#define LATEX_CONVERTER_ERROR_CODE_NO_ERROR  0x00
#define LATEX_CONVERTER_ERROR_CODE_ERROR     0x01

#ifdef __cplusplus
extern "C" {
#endif

bool convertText(const char *input,char **output,int *error_code);

#ifdef __cplusplus
}
#endif
