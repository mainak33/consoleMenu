cmake -S . -B build
cmake --build build --config Release
cmake --install build --prefix .
cd build && ctest