#!/usr/bin/env zsh

clang \
    -O3 \
    --target=wasm32 \
    --no-standard-libraries \
    -Wl,--no-entry \
    -Wl,--allow-undefined \
    -Wl,--export=engine_update \
    -Wl,--export=engine_init \
    -Wl,--export=engine_key_down\
    -Wl,--export=engine_key_up\
    -o dist/src.wasm \
    src.c

pushd dist
python3 -m http.server 8001
