# server side

cd server
make
cd build
./server portno

# client side

cd client
make
cd build
./client host portno

# cleanup

cd server
make clean

cd client
make clean
