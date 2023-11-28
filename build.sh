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

# pushd build
# cl -nologo 8086.c display.c decode.c decode_format.c $core/core.c -Fe: build/8086.exe -I$include \
# -wd4047 -wd4024 -wd4133 user32.lib -Zi
# popd

gcc 8086.c display.c decode.c decode_format.c -o build/8086.exe $core/core.c -I$include $warnings -g -O0
