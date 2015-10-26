#!/usr/bin/env sh

mkdir -p images/collected
mkdir images/generated
npm install -g phantomjs
bower install
cd face-detect
node-gyp rebuild
cd ..