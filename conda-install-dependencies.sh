#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
    EXTN=dylib
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    EXTN=so
fi

# Uncomment what is necessary to reinstall by force 
# rm -f ${CONDA_PREFIX}/lib/libjsonarango.$EXTN

# jsonArango database client
# if no jsonArango installed in ${CONDA_PREFIX}/lib/libjsonarango.a
test -f ${CONDA_PREFIX}/lib/libjsonarango.$EXTN || {

        # Building jsonio library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone  --recurse-submodules https://bitbucket.org/gems4/jsonarango.git && \
                cd jsonarango && \
                mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} -DBUILD_EXAMPLES=OFF  -DBULID_LOCAL_TESTS=OFF -DBULID_REMOTE_TESTS=OFF && \
                make && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}
