#include "shell_parse.h"

#include <stddef.h>
#define isblank(c) ((c) == ' ' || (c) == '\t')

int shell_parse_line(char* input, char* argv[], const int maxArgc)
{
    char tmp;
    int  nargc = 0;

    while (nargc < maxArgc) {
        while (isblank(*input)) {
            input++;
        }
        if (*input == '\0') {  // end of input
            argv[nargc] = NULL;
            break;
        }

        tmp = *input;
        // single quotes ('') and double quotes ("")
        if (tmp == '\'' || tmp == '"') {
            argv[nargc] = ++input;
            while (*input && (*input != tmp)) {
                input++;
            }
        } else {  // normal character
            argv[nargc] = input++;
            while (*input && !isblank(*input)) {
                input++;
            }
        }

        nargc++;
        if (*input) {
            *input++ = '\0'; /* terminate current arg */
        }
    }

    return nargc;
}
