# CTRL

Low level-ish library for playing around in usermode on the 3DS.

## How-to

Download a [prebuilt](https://github.com/kynex7510/CTRL/releases) version, use as a CMake dependency, or build manually:

```
cmake -B Build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release
cmake --build Build --config Release
cmake --install Build --prefix Build/Release
```