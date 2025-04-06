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
