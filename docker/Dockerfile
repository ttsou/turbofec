# A docker image with prebuilt turbofec
# Ubuntu 18.04
FROM ubuntu:bionic
LABEL maintainer "tuan t. pham" <tuan@vt.edu>

ARG	GIT_URL=https://github.com/ttsou/turbofec
ARG	GIT_COMMIT=master

ENV	PKGS="build-essential autoconf libtool automake git" \
	DEBIAN_FRONTEND=noninteractive \
	SRC_DIR=turbofec

RUN	apt-get update && apt-get dist-upgrade -yq && \
	apt-get -yq install ${PKGS} && \
	mkdir /src && \
	cd /src && \
	git clone ${GIT_URL} ${SRC_DIR} && \
	cd ${SRC_DIR} && git checkout -b build ${GIT_COMMIT} && \
	autoreconf -i && ./configure && make && make check && \
	apt-get autoremove -yq && \
	apt-get autoclean && \
	rm -fr /tmp/* /var/lib/apt/lists/*

WORKDIR /src/${SRC_DIR}

# For an 8-core machine
# docker run -t --rm neofob/turbofec -j 8
ENTRYPOINT ["tests/turbo_test", "-b", "-i", "4", "-p", "10000"]
