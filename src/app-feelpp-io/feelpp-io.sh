#/usr/bin/sh

DIM="$1"
ORDER="$2"
shift 2
./feelpp_app_io-${DIM}dP${ORDER} "$@"