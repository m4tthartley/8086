#
#  build.sh
#  8086
#
#  Created by Matt Hartley on 16/11/2023.
#  Copyright 2023 GiantJelly. All rights reserved.
#

mkdir -p build

core=$USERPROFILE/code/core/core

include=$USERPROFILE/code/core
warnings="-Wno-incompatible-pointer-types"
gcc 8086.c -o build/8086.exe $core/core.c -I$include $warnings -g
