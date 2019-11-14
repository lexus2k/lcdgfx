#!/bin/sh

if [ -d ~/esp/esp-idf ]; then
    echo "ESP IDF is already installed"
else
    # echo "Installing packages..."
    # sudo apt-get install libncurses-dev flex bison gperf python python-pip python-setuptools python-serial python-click python-cryptography python-future python-pyparsing python-pyelftools cmake ninja-build ccache
    #
    mkdir -p ~/esp
    git clone --recursive https://github.com/espressif/esp-idf.git ~/esp/esp-idf
fi

cd ~/esp/esp-idf
if [ "$1" = "travis" ]; then
    ./install.sh
fi
