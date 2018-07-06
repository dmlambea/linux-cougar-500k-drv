#!/bin/sh

SRC=/src
TARGET=/build
KERNELSRC=/kernelsrc

# This script creates the .deb file for a DKMS module,
# given its name and version. The source code must
# reside in $SRC and the resulting .deb file will be
# copied to $TARGET

if [ $# -ne 2 ]; then
	echo "Usage: $0 <modulename> <modulever>"
	exit 1
fi

NAME=$1
VER=$2
BUILDDIR="/usr/src/$NAME-$VER"

echo "Building driver $NAME-$VER from $SRC onto $BUILDDIR"

mkdir -p "$BUILDDIR"
cp -r "$SRC"/* "$BUILDDIR/"

dkms add -m $NAME -v $VER
dkms build -m $NAME -v $VER --kernelsourcedir "$KERNELSRC"/linux-headers-$(uname -r)
dkms mkdsc -m $NAME -v $VER --source-only
dkms mkdeb -m $NAME -v $VER --source-only

cp "/var/lib/dkms/$NAME/$VER/deb/"*.deb "$TARGET/"

echo "Build successful!"
