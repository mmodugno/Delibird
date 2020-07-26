#!/bin/sh
unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

./gameboy BROKER NEW_POKEMON Pikachu 9 9 1
./gameboy BROKER NEW_POKEMON Squirtle 5 2 1

./gameboy BROKER NEW_POKEMON Jolteon 2 2 1
./gameboy BROKER NEW_POKEMON Flareon 4 6 1
