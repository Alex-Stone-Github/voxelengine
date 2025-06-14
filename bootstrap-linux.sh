#!/bin/sh

echo "Running Voxel Game(Make sure valid binaries are built)!"

echo "Starting Server!"
./voxelserver 8001&

sleep 1

echo "Starting Client!"
./voxelclient 127.0.0.1 8001
