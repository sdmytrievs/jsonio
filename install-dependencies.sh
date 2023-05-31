#!/bin/bash
# Installing dependencies needed to build jsonio17

if [ "$(uname)" == "Darwin" ]; then
    # Do under Mac OS X platform
    #Needs Xcode and ArangoDB server locally installed
    #brew upgrade
    EXTN=dylib

elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    #Needs gcc v.5 or higher and ArangoDB server locally installed
    #sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev
    EXTN=so
fi

# Uncomment what is necessary to reinstall by force
#sudo rm -rf /usr/local/include/spdlog
#sudo rm -f /usr/local/lib/libvelocypack.a
#sudo rm -f /usr/local/lib/libjsonarango.$EXTN

threads=3

# spdlog
# if no spdlog installed in /usr/local/include/spdlog (copy only headers)
test -d /usr/local/include/spdlog || {

        # Building spdlog library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone https://github.com/gabime/spdlog -b v1.11.0  && \
                cd spdlog/include && \
                sudo cp -r spdlog /usr/local/include

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}

# Velocypack from ArangoDB (added for installing jsonArango database client)
# Installed as part jsonArango
# jsonArango database client
# if no jsonArango installed in /usr/local/lib/libjsonarango.a
test -f /usr/local/lib/libjsonarango.$EXTN || {

        # Building jsonio library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone  --recurse-submodules https://bitbucket.org/gems4/jsonarango.git && \
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
