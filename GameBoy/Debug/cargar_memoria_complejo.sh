#!/bin/sh
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Pikachu 9 3 #19b
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Squirtle 9 3 #20b

LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CAUGHT_POKEMON 10 OK
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CAUGHT_POKEMON 11 FAIL

LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Bulbasaur 1 7 #21
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Charmander 1 7 #22

LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER GET_POKEMON Pichu #9
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER GET_POKEMON Raichu #10
