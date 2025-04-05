
#define LATEX_CONVERTER_ERROR_CODE_NONE      0x00
#define LATEX_CONVERTER_ERROR_CODE_PARSING   0x01
#define LATEX_CONVERTER_ERROR_CODE_CONVERT   0x02
#define LATEX_CONVERTER_ERROR_CODE_SUBSTIT   0x03
#define LATEX_CONVERTER_ERROR_CODE_LATEX     0x04
#define LATEX_CONVERTER_ERROR_CODE_DVIPS     0x05
#define LATEX_CONVERTER_ERROR_CODE_PS2PDF    0x06
#define LATEX_CONVERTER_ERROR_CODE_PDF2PNG   0x07
#define LATEX_CONVERTER_ERROR_CODE_BASE64    0x08
#define LATEX_CONVERTER_ERROR_CODE_UNKNOWN   0x09

#ifdef __cplusplus
extern "C" {
#endif

bool convertText(const char *input, char **output, int *error_code, char **error_info);

#ifdef __cplusplus
}
#endif
