@echo off
cls
rm -r ../Embedded/89C2051/*
cp -r * ../Embedded/89C2051/
cd ../Embedded/89C2051/
git add *
git status
pause