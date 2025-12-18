#!/bin/bash

# Build the Docker image if it doesn't exist
IMAGE_NAME="unnamed-shooter-build"

if [[ "$(docker images -q $IMAGE_NAME 2> /dev/null)" == "" ]]; then
    echo "Building Docker image '$IMAGE_NAME'..."
    docker build -t $IMAGE_NAME .
fi

# Run the build using volume mount
echo "Running build in Docker container..."

# Detect if we're using podman or docker
if command -v podman &> /dev/null && [[ "$(docker --version 2>&1)" == *"podman"* ]]; then
    # Podman: use --userns=keep-id for proper user mapping
    docker run --rm \
        --userns=keep-id \
        -v "$(pwd)":/workspace:Z \
        -w /workspace \
        $IMAGE_NAME \
        bash -c "meson setup builddir && meson compile -C builddir"
else
    # Docker: use -u flag
    docker run --rm \
        -u $(id -u):$(id -g) \
        -v "$(pwd)":/workspace \
        -w /workspace \
        $IMAGE_NAME \
        bash -c "meson setup builddir && meson compile -C builddir"
fi

echo "Build complete! Binary is in builddir/"
