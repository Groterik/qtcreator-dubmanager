[![Build Status](https://travis-ci.org/Groterik/qtcreator-dubmanager.svg?branch=master)](https://travis-ci.org/Groterik/qtcreator-dubmanager)
[![Build status](https://ci.appveyor.com/api/projects/status/0clgytjpyoqqx2ew?svg=true)](https://ci.appveyor.com/project/Groterik/qtcreator-dubmanager)
[![Bintray](https://img.shields.io/bintray/v/groterik/qtcreator-dlang-tools/qtcreator-dubmanager.svg)](https://dl.bintray.com/groterik/qtcreator-dlang-tools/)
#About
The Qt Creator plugin for basic support of [DUB package manager](https://github.com/D-Programming-Language/dub).

The plugin provides most standard features of project management for DUB projects such as building, running, visual source tree, etc.

#Installation
Build from the sources or download precompiled version from [Bintray](https://bintray.com): [project page](https://bintray.com/groterik/qtcreator-dlang-tools/qtcreator-dubmanager) or [downloads directory](https://dl.bintray.com/groterik/qtcreator-dlang-tools).

Note: since [Travis-CI](https://travis-ci.org) is used to build binaries for linux and [Appveyor](https://www.appveyor.com)  -- for windows, build numbers for same revision but different platforms can differ. 


Copy the plugin shared library and pluginspec file into local Qt Creator plugin directory:
* $HOME/.local/share/data/QtProject/qtcreator/plugins/$QTCREATOR_VERSION/ on Linux.
* %LOCALAPPDATA%\QtProject\qtcreator\plugins\$QTCREATOR_VERSION\ for windows.
* ~/Library/Application Support/QtProject/Qt Creator/plugins/$QTCREATOR_VERSION/ on Mac.

You can also set the plugin directory for Qt Creator using command line argument ```-pluginpath <dir>```.

Run Qt Creator and enable the plugin on ```Help -> About Plugins``` page. Restart Qt Creator.

#TODO

#Building
1. clone the project.
1. cd to the project directory
1. run ```qmake && make``` or open dubprojectmanager.pro in the Qt Creator and build the project from IDE. May be you should set QTC_SOURCE and/or QTC_BUILD variables for qmake for QtCreator's sources and libraries correspondingly. It can be done with the following syntax: ```qmake "QTC_SOURCE=$MY_QTCREATOR_SOURCE_PATH" "QTC_BUILD=$MY_QTCREATOR_LIB_PATH"```
1. if successful, the plugin shared library will appear in local plugin directory.
1. on Mac OS X: run ```fix_mac.sh $PATH_TO_PLUGIN/libDubProjectManager.dylib``` to fix rpath constants (mess with Qt libraries and QtC's Qt libraries). 

#Build requirements
1. Qt >= 5.4.0
1. Qt Creator >= 3.3.0 (with sources for plugins building, of course)

#Implementation notes
The plugin uses ```dub``` executable for generating correct project information. The plugin parses ```dub``` output for describing any projects but the file ```dub.json``` (or ```dub.sdl```) itself is not parsed by the plugin.
