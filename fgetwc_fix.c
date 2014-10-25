
#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "fgetwc_fix.h"

/* code blatantly ripped from newlib. If you are from newlib: newlib rocks, keep going! */
wint_t _fgetwc_fixed(FILE *fp) {
	wchar_t wc;
	size_t res;
	size_t nconv = 0;
	char buf[MB_CUR_MAX];
	mbstate_t mbstate;
	memset(&mbstate, 0, sizeof(mbstate));

	while((buf[nconv++] = fgetc(fp)) != EOF){
		res = mbrtowc(&wc, buf, nconv, &mbstate);
		if (res == (size_t)-1) /* invalid sequence */
			break;
		else if (res == (size_t)-2) /* incomplete sequence */
			continue;
		else if (res == 0)
			return L'\0';
		else
			return wc;
	}

	errno = EILSEQ;
	return WEOF;
}
