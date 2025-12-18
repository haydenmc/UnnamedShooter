#!/bin/bash

# Parse command line arguments
BUILDTYPE="debug"
RECONFIGURE=""

for arg in "$@"; do
    case $arg in
        --release|-r)
            BUILDTYPE="release"
            shift
            ;;
        --debug|-d)
            BUILDTYPE="debug"
            shift
            ;;
        --reconfigure)
            RECONFIGURE="--reconfigure"
            shift
            ;;
        --help|-h)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --release, -r       Build in release mode (optimized, no debug info)"
            echo "  --debug, -d         Build in debug mode (default)"
            echo "  --reconfigure       Force meson reconfiguration"
            echo "  --help, -h          Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $arg"
            echo "Run '$0 --help' for usage information"
            exit 1
            ;;
    esac
done

# Build the Docker image if it doesn't exist
IMAGE_NAME="unnamed-shooter-build"

if [[ "$(docker images -q $IMAGE_NAME 2> /dev/null)" == "" ]]; then
    echo "Building Docker image '$IMAGE_NAME'..."
    docker build -t $IMAGE_NAME .
fi

# Run the build using volume mount
echo "Running build in Docker container (buildtype: $BUILDTYPE)..."

BUILD_CMD="meson setup builddir --buildtype=$BUILDTYPE $RECONFIGURE && meson compile -C builddir"

# Detect if we're using podman or docker
if command -v podman &> /dev/null && [[ "$(docker --version 2>&1)" == *"podman"* ]]; then
    # Podman: use --userns=keep-id for proper user mapping
    docker run --rm \
        --userns=keep-id \
        -v "$(pwd)":/workspace:Z \
        -w /workspace \
        $IMAGE_NAME \
        bash -c "$BUILD_CMD"
else
    # Docker: use -u flag
    docker run --rm \
        -u $(id -u):$(id -g) \
        -v "$(pwd)":/workspace \
        -w /workspace \
        $IMAGE_NAME \
        bash -c "$BUILD_CMD"
fi

echo "Build complete! Binary is in builddir/"
