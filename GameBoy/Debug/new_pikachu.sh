#!/bin/sh
unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

./gameboy GAMECARD NEW_POKEMON Pikachu 2 5 10 9
