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

# ../8086.c \
# ../display.c \
# ../decode.c \
# ../decode_format.c \
files="\
../*.c \
$core/core.c \
"

pushd build
cl -nologo -Zi $files -I$include -wd4047 -wd4024 -wd4133 -link user32.lib -out:8086.exe
# -Fe: build/8086.exe
popd

# gcc 8086.c display.c decode.c decode_format.c -o build/8086.exe $core/core.c -I$include $warnings -g -O0
