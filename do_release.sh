#!/bin/sh

set -e

if [ $# -lt 1 ]; then
    echo "do_release.sh must be called with a version number as its first argument."
    exit 1
fi

if [ -n "$(git status --porcelain|grep -v '^??')" ]; then
    echo "do_release.sh must be called from a clean working directory."
    exit 2
fi

VER="$1"

if echo "$VER" | grep -v '^[0-9]\+\.[0-9]\+$'; then
    echo "do_release.sh must be called with a version number formatted like "1.23" as its first argument, with no leading \"v\"."
    exit 1
fi

echo "Updating files for version v$VER"

sed -i "/L\"lolcat version [0-9.]\+, (c) [0-9]\+ jaseg\\\\n\"/s/version [0-9.]\+/version $VER/" lolcat.c
sed -i "s/^pkgver=v[0-9.]\+/pkgver=v$VER/" PKGBUILD
sed -i "/^AC_INIT/s/\[[0-9.]\+\]/[$VER]/" autotools/configure.ac
git add lolcat.c PKGBUILD autotools/configure.ac
git commit -m 'Bump version to v$VER'
git tag "v$VER"
echo "Success."

