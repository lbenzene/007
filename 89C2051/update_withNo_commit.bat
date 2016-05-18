@echo off
cls
call clean.bat
rem G:\Programe\Github_repiratory\Embedded\89C2051
rm -r ../../Github_repiratory/Embedded/89C2051/*
cp -r * ../../Github_repiratory/Embedded/89C2051/
cd ../../Github_repiratory/Embedded/89C2051/
git add *
git status
pause