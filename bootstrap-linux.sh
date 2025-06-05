echo "Running Voxel Game(Make sure valid binaries are built)!"

echo "Starting Server!"
./voxelserver/target/debug/voxelserver&

sleep 1

echo "Starting Client!"
./main

clear