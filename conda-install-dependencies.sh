#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
    EXTN=dylib
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    EXTN=so
fi

# Uncomment what is necessary to reinstall by force 
# rm -f ${CONDA_PREFIX}/lib/libarango-cpp.$EXTN

# arango-cpp database client
# if no arango-cpp installed in ${CONDA_PREFIX}/lib/libarango-cpp.a
test -f ${CONDA_PREFIX}/lib/libarango-cpp.$EXTN || {

        # Building arango-cpp library
        mkdir -p ~/code && \
                cd ~/code && \
                git clone  --recurse-submodules https://github.com/sdmytrievs/arango-cpp.git && \
                cd arango-cpp && \
                mkdir -p build && \
                cd build && \
                cmake .. -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX} -DBUILD_EXAMPLES=OFF  -DBULID_LOCAL_TESTS=OFF -DBULID_REMOTE_TESTS=OFF && \
                make && \
                sudo make install

        # Removing generated build files
        cd ~ && \
                 rm -rf ~/code
}
