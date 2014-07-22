#About
The Qt Creator plugin for basic support of [DUB package manager](https://github.com/D-Programming-Language/dub).

The plugin provides most standard features of project management for DUB projects such as building, running, visual source tree, etc.

#Installation
Copy the plugin shared library and pluginspec file into local Qt Creator plugin directory:
* $HOME/.local/share/data/QtProject/qtcreator/plugins/$QTCREATOR_VERSION/cleem/ on Linux
* %LOCALAPPDATA%\QtProject\qtcreator\plugins\$QTCREATOR_VERSION\cleem\
* ~/Library/Application Support/QtProject/Qt Creator/plugins/$QTCREATOR_VERSION/cleem/ on Mac

#TODO

#Building
1. Clone the project.
1. cd to the project directory
1. run ```qmake && make``` or open dubprojectmanager.pro in the Qt Creator and build the project from IDE.
1. if successful, the plugin shared library will appear in local plugin directory.

#Build requirements
1. Qt >= 5.0.0
1. Qt Creator >= 2.7.0 (with sources for plugins building, of course)

#Implementation notes
The plugin uses ```dub``` executable for generating correct project information. The plugin parses ```dub``` output for describing any projects but the file ```dub.json``` itself is not parsed by the plugin.
