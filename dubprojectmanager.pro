TARGET = DubProjectManager
TEMPLATE = lib

DEFINES += DUBPROJECTMANAGER_LIBRARY

# DubProjectManager files

SOURCES += dubprojectmanagerplugin.cpp \
    dubmanager.cpp \
    dubproject.cpp \
    dubfile.cpp \
    dubprojectnode.cpp \
    dubexception.cpp

HEADERS += dubprojectmanagerplugin.h \
        dubprojectmanager_global.h \
        dubprojectmanagerconstants.h \
    dubmanager.h \
    dubproject.h \
    dubfile.h \
    dubprojectnode.h \

# Qt Creator linking

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/usr/src/qtcreator

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/usr/lib/x86_64-linux-gnu/qtcreator

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\QtProject\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/data/QtProject/qtcreator" or "~/.local/share/data/QtProject/qtcreator" on Linux
##    "~/Library/Application Support/QtProject/Qt Creator" on Mac
USE_USER_DESTDIR = yes

PROVIDER = cleem

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/coreplugin/coreplugin.pri)
include($$QTCREATOR_SOURCES/src/plugins/projectexplorer/projectexplorer.pri)

LIBS += -L$$IDE_PLUGIN_PATH/QtProject \
        -L$$IDE_BUILD_TREE \
        -L$$IDE_BUILD_TREE/plugins/QtProject

OTHER_FILES += \
    DubProject.mimetypes.xml

