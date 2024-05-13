
rm -rf build

mkdir build

cd build

cmake ..

make

echo "compilation finished"

cd ..
./build/exceptions