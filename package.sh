#!/bin/sh -e

version=v0.3-$(git rev-parse --short HEAD)

## linux package
mkdir -p linux/bin
cp tophat linux/bin/tophat-linux
cp tophat.exe linux/bin/tophat-win.exe
cp -r examples/preset/* linux
echo $version > linux/version
zip -r tophat-linux.zip linux/*
rm -r linux

## windows package
mkdir -p windows/bin
cp -r examples/* windows
cp cmd/*.bat windows
cp tophat.exe windows/bin/tophat-win.exe
cp tophat windows/bin/tophat
zip -r -0 tophat-win.zip windows/* # windows doesn't like linux's zip compression
