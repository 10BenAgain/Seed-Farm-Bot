#!/usr/bin/bash
# This just builds the Go binaries and puts them in a bin directory

ROOT=$(pwd)
BIN_DIR="bin"

SANITY="sanity"
SAVS="savs"

if [ ! -d "$BIN_DIR" ]; then
    mkdir -v $BIN_DIR
fi

echo "Building sanity script..."
    go -C $ROOT/$SANITY build $SANITY.go
    sanity_bin=$ROOT/$SANITY/$SANITY

cp $sanity_bin $ROOT/$BIN_DIR

echo "Building pull script..."
    go -C $ROOT/$SAVS build pull.go
    pull_bin=$ROOT/$SAVS/pull

cp $pull_bin $ROOT/$BIN_DIR

rm $pull_bin $sanity_bin

for FILE in $ROOT/$BIN_DIR/*; do 
    echo " ->  $FILE"
done

echo "Done."