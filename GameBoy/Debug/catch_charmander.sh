#!/bin/sh
unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" 

./gameboy GAMECARD CATCH_POKEMON Charmander 413 17 1
