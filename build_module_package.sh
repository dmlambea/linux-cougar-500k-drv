#!/bin/sh

# This script runs the build container to create a DKMS .deb
# for the current module's source code. The container's image
# name must be given, since you can build your own with the
# provided Dockerfile.

if [ $# -ne 1 ]; then
	echo "Usage: $0 <imagename>"
	exit 1
fi

IMAGE=$1
SRC="$(pwd)/module"
TARGET="$(pwd)/build"

NAME=$(grep PACKAGE_NAME "$SRC/dkms.conf" | sed 's/^PACKAGE_NAME=//')
VER=$(grep PACKAGE_VERSION "$SRC/dkms.conf" | sed 's/^PACKAGE_VERSION=//')

mkdir -p "$TARGET"
docker run --rm -v "$SRC":/src:ro -v "$TARGET":/build:rw $IMAGE /usr/local/bin/dkmsbuild.sh $NAME $VER

if [ $? -ne 0 ]; then
	echo "Build unsuccessful!"
	echo "Make sure the output directory is writable by container's 'nobody:nogroup' user."
fi
