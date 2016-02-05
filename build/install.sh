#!/bin/bash

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then
  brew install qt5
  brew link --force qt5
else
  sudo apt-get install qt55-meta-full
  sudo apt-get install -qq gcc-4.8 g++-4.8 
  sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90 

  . /opt/qt55/bin/qt55-env.sh
fi
