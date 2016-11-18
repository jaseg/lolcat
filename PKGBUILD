# Maintainer: Ricardo (XenGi) Band <email@ricardo.band>

pkgname=c-lolcat
pkgver=r31.f585abd
pkgrel=1
pkgdesc="High-performance implementation of lolcat"
arch=('i686' 'x86_64')
url="https://github.com/jaseg/lolcat"
license=('WTFPL')
depends=()
makedepends=()
conflicts=('lolcat')
source=("$pkgname"::'git+https://github.com/jaseg/lolcat.git'
  'musl'::'git://git.musl-libc.org/musl'
  'memorymapping'::'git+https://github.com/NimbusKit/memorymapping.git')
sha256sums=('SKIP' 'SKIP' 'SKIP')

pkgver() {
  cd "$srcdir/$pkgname"
  printf 'r%s.%s' "$(git rev-list --count HEAD)" "$(git describe --always)"
}

prepare() {
  cd "$srcdir/$pkgname"
  git submodule init
  git config submodule.musl.url $srcdir/musl
  git config submodule.memorymapping.url $srcdir/memorymapping
  git submodule update
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
