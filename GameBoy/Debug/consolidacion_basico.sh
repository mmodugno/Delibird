#!/bin/sh
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CAUGHT_POKEMON 1 OK
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CAUGHT_POKEMON 2 FAIL

LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Pikachu 2 3
LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Squirtle 5 2

LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Onyx 4 5

LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy SUSCRIPTOR CAUGHT_POKEMON 10

LD_LIBRARY_PATH="/home/utnso/tp-2020-1c-The-X-Team/utils/Debug" ./gameboy BROKER CATCH_POKEMON Charmander 4 5
