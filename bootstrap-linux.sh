#!/bin/sh

echo "Running Voxel Game!"

./voxelserver 8001&
sleep 1
./voxelclient 127.0.0.1 8001
