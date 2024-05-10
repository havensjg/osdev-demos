#!/bin/bash

# Create the directory for the iso files produced
mkdir -p "iso-files"

# Loop through each subdirectory in the current directory
for subdir in */; do

    # Remove the trailing slash from the directory name
    subdir="${subdir%/}"

    # Check if the entry is a directory
    if [ -d "$subdir" ] && [ "$subdir" != "iso-files" ]; then
        echo "Entering $subdir"

        # Navigate into the subdirectory
        cd "$subdir" || exit

        # Go to the build directory
        mkdir -p build
        cd build

        # Configure CMake
        cmake ..

        # Make the iso file
        make livecd

        # Put in a directory that contains all of the iso files
        cp myos.iso "../../iso-files/$subdir.iso"

        echo "Exiting $subdir"
        cd ../..
    fi
done