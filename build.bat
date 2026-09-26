@echo off
rem 把 w64devkit 工具链加入本次会话 PATH，然后执行 make
set PATH=D:\VScode\w64devkit\bin;%PATH%
make %*
