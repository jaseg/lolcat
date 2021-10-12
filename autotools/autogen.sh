#!/bin/bash

auxdir=autoscripts

set -x

# aclocal generated files
rm -rf aclocal.m4 autom4te.cache

# autoheader generated files
rm -f config.h.in

# autoconf generated files
rm -f configure

# automake generated files
rm -f ${auxdir}/{config.guess,config.sub,depcomp,install-sh,missing}
rm -f Makefile.in

# configure generated files
rm -f config.h config.log config.status Makefile Makefile.in stamp-h1

rm -f $(find ${auxdir} -type l 2>/dev/null)
rmdir ${auxdir} 2>/dev/null

if [[ "$1" == clean ]]; then
	exit 0
fi

mkdir ${auxdir} 2>/dev/null

aclocal
autoheader
autoconf
automake --add-missing

