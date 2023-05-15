#! /bin/bash
[ -z $1 ] || TARGET=$1
[ -z $TARGET ] && TARGET=debug
for i in [0123456789]* ; do
  echo "Building $i";
  pushd $i && make clean $TARGET;
  popd;
done
