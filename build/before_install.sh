#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
  brew install p7zip
  brew install wget

  QT_BIN_LINK="http://download.qt.io/official_releases/qtcreator/3.6/3.6.0/installer_source/mac_x64/qtcreator.7z"

else
  sudo add-apt-repository -y ppa:beineri/opt-qt551
  # GCC 4.8
  sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
  sudo apt-get update -qq

  sudo apt-get install p7zip
  sudo apt-get install zip

  QT_BIN_LINK="http://download.qt.io/official_releases/qtcreator/3.6/3.6.0/installer_source/linux_gcc_64_rhel66/qtcreator.7z"

fi


wget "http://download.qt-project.org/official_releases/qtcreator/3.6/3.6.0/qt-creator-opensource-src-3.6.0.tar.gz"
tar xzf qt-creator-opensource-src-3.6.0.tar.gz
wget "${QT_BIN_LINK}"
7zr x -o./qtc_build/3.6.0/ qtcreator.7z

