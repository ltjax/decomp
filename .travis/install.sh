#!/bin/bash

set -e
set -x

if [[ "$(uname -s)" == 'Linux' ]]; then
    sudo apt-get install g++-${GCC_VERSION}
    sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} 60 --slave /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION}
    sudo update-alternatives --config gcc
    python3 --version
    sudo pip3 install cmake==3.13.3
    pip3 install --upgrade pip --user
    pip --version
    pip install conan==1.24.1 --upgrade --user
    pip install conan_package_tools==0.32.2 --user
    cmake --version
    conan --version
    conan profile new default --detect --force
    conan profile update settings.compiler.libcxx=libstdc++11 default
fi

if [[ "$(uname -s)" == 'Darwin' ]]; then  
    python3 --version
    
    # Create virtualenv called venv
    python3 -m venv venv
    
    # Activate it..
    source venv/bin/activate
    
    # Install conan and the package tools there
    pip install conan conan_package_tools --upgrade
    conan --version
    conan user
fi
