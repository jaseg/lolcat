# Maintainer: Ricardo (XenGi) Band <email@ricardo.band>

pkgname=c-lolcat
pkgver=v1.3
pkgrel=1
pkgdesc="High-performance implementation of lolcat"
arch=('i686' 'x86_64')
url="https://github.com/jaseg/lolcat"
license=('WTFPL')
depends=('musl')
makedepends=()
conflicts=('lolcat' 'python-lolcat')
source=("$pkgname"::'git+https://github.com/jaseg/lolcat.git')
sha256sums=(SKIP)

pkgver() {
    cd "$srcdir/$pkgname"
    printf 'r%s.%s' "$(git rev-list --count HEAD)" "$(git describe --always)"
}

build() {
  cd "$srcdir/$pkgname"
  make
}

package() {
  cd "$srcdir/$pkgname"
  mkdir -p "${pkgdir}/usr/bin"
  make DESTDIR="$pkgdir/usr/bin" install
}
