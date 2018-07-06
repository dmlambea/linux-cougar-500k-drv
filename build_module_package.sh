#!/bin/sh

# This script runs the build container to create a DKMS .deb
# for the current module's source code. The container's image
# name must be given, since you can build your own with the
# provided Dockerfile.

if [ $# -ne 1 -a $# -ne 2 ]; then
	echo "Usage: $0 <imagename> [<linux-headers-dir>]"
	echo
	echo "Where <imagename> is the name you gave to the DKMS builder container,"
	echo "and <linux-headers-dir> is the root of your Linux kernel headers"
	echo "directory (defaults to /usr/src)."
	exit 1
fi

IMAGE=$1
HEADERSDIR=$2
if [ $# -eq 1 ]; then
	HEADERSDIR='/usr/src'
fi

SRC="$(pwd)/module"
TARGET="$(pwd)/build"

NAME=$(grep PACKAGE_NAME "$SRC/dkms.conf" | sed 's/^PACKAGE_NAME=//')
VER=$(grep PACKAGE_VERSION "$SRC/dkms.conf" | sed 's/^PACKAGE_VERSION=//')

mkdir -p "$TARGET"
docker run --rm \
	-v "$HEADERSDIR":/kernelsrc:ro \
	-v "/lib/modules":/lib/modules:ro \
	-v "$SRC":/src:ro \
	-v "$TARGET":/build:rw \
	$IMAGE /usr/local/bin/dkmsbuild.sh $NAME $VER

if [ $? -ne 0 ]; then
	echo "Build unsuccessful!"
	echo "Make sure the output directory is writable by container's 'nobody:nogroup' user."
fi
