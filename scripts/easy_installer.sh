#!/usr/bin/env bash
# rocksteady, turtlecoin developers 2017-2018
# pinkstarcoinv2 developers 2018
# Bittorium developers 2018
# Talleo developers 2019-2021
# use this installer to clone-and-compile Talleo in one line
# supports Ubuntu 18 LTS

sudo apt-get update
yes "" | sudo apt-get install build-essential python-dev gcc-6 g++-6 git cmake libboost-all-dev
export CXXFLAGS="-std=gnu++11"
git clone https://github.com/TalleoProject/Talleo/
cd Talleo
mkdir build && cd $_
cmake ..
make
