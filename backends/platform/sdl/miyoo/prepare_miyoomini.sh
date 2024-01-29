#!/bin/bash

JPEG_VERSION=3.0.1
OGG_VERSION=1.3.5
VORBIS_VERSION=1.3.7
THEORA_VERSION=1.1.1
LIBMAD_VERSION=0.15.1b
GIFLIB_VERSION=5.2.1
LIBMPEG2_VERSION=0.5.1
FLAC_VERSION=1.4.3
LIBVPX_VERSION=1.14.0
A52DEC_VERSION=0.7.4
FRIBIDI_VERSION=1.0.13
FAAD2_VERSION=2.11.1

MIYOO_SYSROOT="/opt/miyoomini-toolchain/arm-linux-gnueabihf/sysroot"
MIYOO_PREFIX="/opt/miyoomini-toolchain/usr/bin/arm-linux-gnueabihf-"
BUILD_DIR="$(mktemp -d)/toolchain-extra"
CMAKE_FLAGS=" -DCMAKE_C_COMPILER=${MIYOO_PREFIX}gcc -DCMAKE_CXX_COMPILER=${MIYOO_PREFIX}g++ -DCMAKE_INSTALL_PREFIX=/usr/local"
MIYOO_HOST=arm-linux-gnueabihf
CONFIGURE_FLAGS="CFLAGS=-fPIC CC=${MIYOO_PREFIX}gcc AR=${MIYOO_PREFIX}ar --host=$MIYOO_HOST --disable-shared"

mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR"
git clone --depth=5 -b $JPEG_VERSION https://github.com/libjpeg-turbo/libjpeg-turbo
cd libjpeg-turbo
cmake -B build $CMAKE_FLAGS
cd build
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://github.com/xiph/ogg/releases/download/v${OGG_VERSION}/libogg-${OGG_VERSION}.tar.xz
tar xvf libogg-${OGG_VERSION}.tar.xz
cd libogg-${OGG_VERSION}
./configure $CONFIGURE_FLAGS
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://github.com/xiph/vorbis/releases/download/v${VORBIS_VERSION}/libvorbis-${VORBIS_VERSION}.tar.xz
tar xvf libvorbis-${VORBIS_VERSION}.tar.xz
cd libvorbis-${VORBIS_VERSION}
./configure $CONFIGURE_FLAGS
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget http://downloads.xiph.org/releases/theora/libtheora-${THEORA_VERSION}.tar.bz2
tar xvf libtheora-${THEORA_VERSION}.tar.bz2
cd libtheora-${THEORA_VERSION}
./configure $CONFIGURE_FLAGS --disable-examples --disable-encode
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://altushost-swe.dl.sourceforge.net/project/mad/libmad/${LIBMAD_VERSION}/libmad-${LIBMAD_VERSION}.tar.gz
tar xvf libmad-${LIBMAD_VERSION}.tar.gz
cd libmad-${LIBMAD_VERSION}
./configure $CONFIGURE_FLAGS
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://deac-riga.dl.sourceforge.net/project/giflib/giflib-${GIFLIB_VERSION}.tar.gz
tar xvf giflib-${GIFLIB_VERSION}.tar.gz
cd giflib-${GIFLIB_VERSION}
make CFLAGS=-fPIC CC=${MIYOO_PREFIX}gcc AR=${MIYOO_PREFIX}ar
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://libmpeg2.sourceforge.io/files/libmpeg2-${LIBMPEG2_VERSION}.tar.gz
tar xvf libmpeg2-${LIBMPEG2_VERSION}.tar.gz
cd libmpeg2-${LIBMPEG2_VERSION}
./configure $CONFIGURE_FLAGS
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://ftp.osuosl.org/pub/xiph/releases/flac/flac-${FLAC_VERSION}.tar.xz
tar xvf flac-${FLAC_VERSION}.tar.xz
cd flac-${FLAC_VERSION}
./configure $CONFIGURE_FLAGS --enable-static
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://liba52.sourceforge.io/files/a52dec-${A52DEC_VERSION}.tar.gz
tar xvf a52dec-${A52DEC_VERSION}.tar.gz
cd a52dec-${A52DEC_VERSION}
./configure $CONFIGURE_FLAGS
make
make install DESTDIR="$MIYOO_SYSROOT"

cd "$BUILD_DIR"
wget https://github.com/fribidi/fribidi/releases/download/v${FRIBIDI_VERSION}/fribidi-${FRIBIDI_VERSION}.tar.xz
tar xvf fribidi-${FRIBIDI_VERSION}.tar.xz
cd fribidi-${FRIBIDI_VERSION}
./configure $CONFIGURE_FLAGS
make
make install DESTDIR="$MIYOO_SYSROOT"

ls "$MIYOO_SYSROOT/usr/local/lib/"*.so*
rm "$MIYOO_SYSROOT/usr/local/lib/"*.so*
