/* Copyright (C) 2014 jaseg <github@jaseg.net>
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * Version 2, December 2004
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 * DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 * TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 * 0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <wchar.h>

int main(int argc, char** argv)
{
    char* foo[] = { "-" };
    char** inputs = argv + 1;
    char** inputs_end = argv + argc;
    if (inputs == inputs_end) {
        inputs = foo;
        inputs_end = inputs + 1;
    }

    setlocale(LC_ALL, "");

    int escape_state = 0;
    for (char** filename = inputs; filename < inputs_end; filename++) {
        FILE* f = stdin;

        if (strcmp(*filename, "-"))
            f = fopen(*filename, "r");

        if (!f) {
            fprintf(stderr, "Cannot open input file \"%s\": %s\n", *filename, strerror(errno));
            return 2;
        }

        int c;
        while ((c = fgetwc(f)) > 0) {
            if (!escape_state) {
                if (c == '\e') {
                    printf("%lc", c);
                    escape_state = 1;
                } else if (strchr("acegmnopqrsuvwxyz", c))
                    printf("▄");
                else if (strchr(".,:; \t\r\n", c))
                    printf("%lc", c);
                else
                    printf("█");
            } else {
                printf("%lc", c);
                if (!strchr("[0123456789;", c))
                    escape_state = 0;
            }
        }

        fclose(f);

        if (c != WEOF && c != 0) {
            fprintf(stderr, "Error reading input file \"%s\": %s\n", *filename, strerror(errno));
            return 2;
        }
    }
}
