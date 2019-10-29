#!/bin/bash
# Installing dependencies needed to build jsonio14

if [ "$(uname)" == "Darwin" ]; then

    # Do under Mac OS X platform
    #Needs Xcode and ArangoDB server locally installed
    brew upgrade
    brew install cmake
    
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then

    #Needs gcc v.5 or higher and ArangoDB server locally installed
    sudo apt-get update
fi

# Uncomment what is necessary to reinstall by force 


threads=3


if [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
   sudo ldconfig
fi
