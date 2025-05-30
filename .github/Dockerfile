FROM ubuntu:22.04

# set noninteractive installation
ENV DEBIAN_FRONTEND=noninteractive

# install host packages for Zephyr builds
RUN apt-get update && apt-get install -y --no-install-recommends \
    ccache \
    cmake \
    file \
    g++-multilib \
    gcc \
    gcc-multilib \
    git \
    gperf \
    make \
    ninja-build \
    python3-venv \
    wget \
    xz-utils \
    tree \
    && rm -rf /var/lib/apt/lists/*

# install the Zephyr toolchain
ARG ZEPHYR_SDK_VERSION=0.17.0
RUN wget -q https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz \
    && mkdir -p /opt/toolchains \
    && tar xf zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz -C /opt/toolchains \
    && /opt/toolchains/zephyr-sdk-${ZEPHYR_SDK_VERSION}/setup.sh -c -t arm-zephyr-eabi \
    && rm zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64.tar.xz

# tell cmake where to find the Zephyr SDK cmake packages
ENV CMAKE_PREFIX_PATH=/opt/toolchains

# create a Python virtual environment and install Python packages
RUN python3 -m venv /venv \
    && . /venv/bin/activate \
    && pip install --no-cache-dir \
    west==1.2.0

# Set up workspace
WORKDIR /workspace

# Copy your west.yml to a cache directory
COPY west.yml /zephyr-workspace-cache/.manifest/west.yml

# retrieve a lighter version of the Zephyr workspace, which is enough to
# populate a cache for use when building. we'll need to re-generate this image
# if the manifest changes, to keep the benefit.
RUN . /venv/bin/activate \
  && cd /zephyr-workspace-cache \
  && west init -l .manifest \
  && west update --narrow --fetch-opt=--filter=tree:0

# install Python dependencies for the Zephyr workspace
RUN . /venv/bin/activate \
  && cd /zephyr-workspace-cache \
  && pip install --no-cache-dir -r zephyr/scripts/requirements.txt

# Set up environment variables
ENV ZEPHYR_BASE=/zephyr-workspace-cache/zephyr
ENV PATH="/venv/bin:${PATH}"

# Set the working directory
WORKDIR /workspace