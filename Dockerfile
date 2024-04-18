FROM --platform=linux/amd64 ubuntu:22.04

ARG TOOLS_PATH=/opt/gcc-arm-none-linux
ARG ARM_VERSION=13.2.rel1
ENV ARM_ARCH=aarch64

USER root

WORKDIR /home/app
COPY . .


# Install Mantatory tools (curl git python3) and optional tools (vim sudo)
RUN apt-get update && \
    apt-get install -y apparmor \
		automake \
		build-essential \
		cmake \
		curl \
		file \
		g++ \
		gcc \
		gcc-arm-linux-gnueabihf \
		git \
		gnupg \
		gnupg2 \
		jq \
		libapparmor-dev \
		lsb-release \
		make \
		net-tools \
		ninja-build \
		pkg-config \
		python-is-python3 \
		python-pkg-resources \
		python3 \
		python3-pip \
		qemu-efi-aarch64 \
		qemu-system-arm \
		qemu-system-mips \
		stlink-tools \
		vim \
		wget \
		xz-utils


# Get ARM Toolchain
RUN echo "Downloading ARM GNU GCC for Platform" \
	&& mkdir ${TOOLS_PATH} \
	&& curl -Lo gcc-arm-none-linux.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-aarch64-none-linux-gnu.tar.xz" \
	&& tar xf gcc-arm-none-linux.tar.xz --strip-components=1 -C ${TOOLS_PATH} \
	&& rm gcc-arm-none-linux.tar.xz \
	&& rm ${TOOLS_PATH}/*.txt \
	&& rm -rf ${TOOLS_PATH}/share/doc

# Configure git for safe.directory
RUN git config --global --add safe.directory /depot_tools
# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
RUN pip install --upgrade setuptools

RUN curl -sL https://deb.nodesource.com/setup_20.x  | bash -
RUN apt-get -y install nodejs

RUN node -p "process.arch"

ENV SKIP_DOWNLOAD=true

RUN npm install

RUN TARGET_ARCH=arm64 npm run build

CMD ["sleep","3600"]
