echo "Running Voxel Game!"

echo "Starting Server!"
start "Server Log" voxelserver.exe 8001

timeout 1

echo "Starting Client!"
start "Client Log" voxelclient.exe 127.0.0.1 8001