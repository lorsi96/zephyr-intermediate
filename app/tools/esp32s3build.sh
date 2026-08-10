#!/usr/bin/bash
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)

pushd $SCRIPT_DIR/..
west build -p always -b esp32s3_devkitc/esp32s3/procpu 
popd
