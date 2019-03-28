#!/bin/bash

if [ ! -d wrapper/build ]; then
    mkdir -p wrapper/build
fi

cd wrapper/build

cmake .. -DTTK_SIM=1
make
mv libelev_wrapper.a ../..