btlauncher plugin using FireBreath

usage
see test/fblauncher_test.html


build instructions (windows):  
- install [cmake](http://www.cmake.org/files/v2.8/cmake-2.8.8-win32-x86.exe)
- install wix installer toolkit
- clone FireBreath (currently pinned to stable version 1.6)
```
git clone https://github.com/firebreath/FireBreath.git
git checkout firebreath-1.6
```
- follow FireBreath docs to run prep2008.cmd on this project
- build generated visual studio project

debug instructions (windows):
- go to build\bin\SoShare\Debug folder and run "regsvr32.exe npSoShare.dll"
- open browser, use visual studio to attach to browser process (make sure you're attaching to "Native" and not "Script" code)

build instructions (osx):
- install [cmake](http://www.cmake.org/files/v2.8/cmake-2.8.8-Darwin64-universal.dmg)