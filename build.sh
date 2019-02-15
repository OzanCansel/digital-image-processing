rm -rf build
mkdir build
cd build
if [[ "$OSTYPE" == "msys" ]]; then
	cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release .. 
else
	cmake -DCMAKE_BUILD_TYPE=Release ..
fi
cmake --build .