# CTRL

3DS library containing utilities for hacking around in usermode.

## How-to

Download a [prebuilt](https://github.com/kynex7510/CTRL/releases) version, use as a CMake dependency, or build manually:

```
cmake -B Build -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/cmake/3DS.cmake" -DCMAKE_BUILD_TYPE=Release -DCTRL_BUILD_TESTS=ON
cmake --build Build --config Release
cmake --install Build --prefix Build/Release
```