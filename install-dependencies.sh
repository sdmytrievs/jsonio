#!/bin/bash
# Installing dependencies needed to build jsonio17

if [ "$(uname)" == "Darwin" ]; then

    # Do under Mac OS X platform
    #Needs Xcode and ArangoDB server locally installed
    brew upgrade
    brew install cmake

    CXXSTANDARD=17

elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then

    #Needs gcc v.5 or higher and ArangoDB server locally installed
    sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev

    CXXSTANDARD=11
fi

# Uncomment what is necessary to reinstall by force
#sudo rm -f /usr/local/lib/libvelocypack.a
#sudo rm -f /usr/local/lib/libjsonarango.a


threads=3


# Velocypack from ArangoDB (added for installing JSONIO database client)
# if no VPack installed in /usr/local/lib/libvelocypack.a (/usr/local/include/velocypack)
test -f /usr/local/lib/libvelocypack.a || {

        # Building velocypack library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/arangodb/velocypack.git && \
                cd velocypack && \
                mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF -DCMAKE_CXX_STANDARD=$CXXSTANDARD && \
                make -j $threads && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

# JSONIO database client (added for building ThermoMatch code)
# if no JSONIO installed in /usr/local/lib/libjsonarango.a
test -f /usr/local/lib/libjsonarango.a || {

        # Building jsonio library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://bitbucket.org/gems4/jsonarango.git && \
                cd jsonarango && \
                mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release && \
                make -j $threads && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}



if [ `uname -s` == Linux* ];  then
   sudo ldconfig
fi
