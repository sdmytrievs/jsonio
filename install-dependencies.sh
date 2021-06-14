#!/bin/bash
# Installing dependencies needed to build jsonio17

if [ "$(uname)" == "Darwin" ]; then

    # Do under Mac OS X platform
    #Needs Xcode and ArangoDB server locally installed
    brew upgrade
    brew install cmake

    EXTN=dylib

elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then

    #Needs gcc v.5 or higher and ArangoDB server locally installed
    sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev

    EXTN=so

fi

# Uncomment what is necessary to reinstall by force
#sudo rm -f /usr/local/lib/libvelocypack.a
#sudo rm -f /usr/local/lib/libjsonarango.$EXTN


threads=3


# Velocypack from ArangoDB (added for installing jsonArango database client)
# if no VPack installed in /usr/local/lib/libvelocypack.a (/usr/local/include/velocypack)
test -f /usr/local/lib/libvelocypack.a || {

        # Building velocypack library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/arangodb/velocypack.git && \
                cd velocypack && \
                mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DBuildVelocyPackExamples=OFF && \
                make -j $threads && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

# jsonArango database client
# if no jsonArango installed in /usr/local/lib/libjsonarango.a
test -f /usr/local/lib/libjsonarango.$EXTN || {

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
