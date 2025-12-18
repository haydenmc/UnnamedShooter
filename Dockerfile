FROM fedora:43

# Install build dependencies
RUN dnf install -y \
    gcc \
    gcc-c++ \
    meson \
    ninja-build \
    pkg-config \
    SDL2-devel \
    python3 \
    python3-pip \
    git \
    && dnf clean all

WORKDIR /workspace

CMD ["bash"]
