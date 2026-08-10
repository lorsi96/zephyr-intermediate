#!/usr/bin/bash
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

pushd $SCRIPT_DIR/..
west build -p always -b native_sim
popd
