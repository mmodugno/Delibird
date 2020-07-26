#!/bin/sh
unset LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/home/utnso/tp-2020-1c-The-X-Team/utils/Debug"

./gameboy TEAM APPEARED_POKEMON Pikachu 9 9
./gameboy TEAM APPEARED_POKEMON Squirtle 6 7
./gameboy TEAM APPEARED_POKEMON Onix 2 8
./gameboy TEAM APPEARED_POKEMON Gengar 6 6
./gameboy TEAM APPEARED_POKEMON Squirtle 5 5
