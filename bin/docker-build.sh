#!/bin/sh

# this script builds an MPQC docker image
#
# to run bash in the image: docker run --privileged -i -t mpqc-dev:latest /sbin/my_init -- bash -l
# locations:
#   - source dir: /usr/local/src/mpqc
#   - build dir: /usr/local/src/mpqc-build
#   - installed executable: /usr/local/bin/mpqc
#   - installed headers dir: /usr/local/include/mpqc
#   - installed libraries dir: /usr/local/lib, e.g. /usr/local/lib/libMPQCmpqc.a

# update these before rebuilding
LIBINT_VERSION=2.5.0-beta.1

##############################################################
# make a script to disable ASLR to make MADWorld happy
disable_aslr=disable_aslr.sh
cat > $disable_aslr << END
#!/bin/sh
echo 0 > /proc/sys/kernel/randomize_va_space
END
chmod +x $disable_aslr

##############################################################
# make Dockerfile, if missing
cat > Dockerfile << END
# Use phusion/baseimage as base image. To make your builds
# reproducible, make sure you lock down to a specific version, not
# to 'latest'! See
# https://github.com/phusion/baseimage-docker/blob/master/Changelog.md
# for a list of version numbers.
FROM phusion/baseimage:0.9.22

# Use baseimage-docker's init system.
CMD ["/sbin/my_init"]

# build MPQC4
# 1. basic prereqs
RUN apt-get update && apt-get install -y cmake liblapack-dev mpich libboost-dev libeigen3-dev git wget libboost-serialization-dev libtbb-dev && apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*
# 2. libint
RUN cd /usr/local/src && wget https://github.com/evaleev/libint/releases/download/v${LIBINT_VERSION}/libint-${LIBINT_VERSION}.tgz && tar -xvzf libint-${LIBINT_VERSION}.tgz && cd libint-${LIBINT_VERSION} && ./configure --prefix=/usr/local --with-incdirs="-I/usr/include/eigen3" && make -j2 && make install && make clean
# 3. tiledarray
RUN cd /usr/local/src && mkdir tiledarray && cd tiledarray && git clone --depth=1 https://github.com/ValeevGroup/tiledarray.git tiledarray_src && cmake tiledarray_src -DCMAKE_INSTALL_PREFIX=/usr/local -DMPI_CXX_COMPILER=mpicxx -DMPI_C_COMPILER=mpicc -DCMAKE_BUILD_TYPE=Release && make && make install && make clean
# 4. mpqc
RUN cd /usr/local/src && git clone --depth=1 https://github.com/ValeevGroup/mpqc.git && mkdir mpqc-build && cd mpqc-build && cmake ../mpqc -DCMAKE_INSTALL_PREFIX=/usr/local -DTiledArray_DIR="/usr/local/lib/cmake/tiledarray" -DCMAKE_CXX_FLAGS="-ftemplate-depth=1024 -Wno-unused-command-line-argument" -DLIBINT2_INSTALL_DIR=/usr/local -DCMAKE_BUILD_TYPE=Release && make mpqc && make install

# Clean up APT when done.
RUN apt-get clean && rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# disable ASLR to make MADWorld happy
RUN mkdir -p /etc/my_init.d
ADD $disable_aslr /etc/my_init.d/disable_aslr.sh
END

function clean_up {
  rm -f $disable_aslr Dockerfile
  exit
}

trap clean_up SIGHUP SIGINT SIGTERM

##############################################################
# build a dev image
docker build -t mpqc-dev .

##############################################################
# extra admin tasks, uncomment as needed
# docker save mpqc-dev | bzip2 > mpqc-dev.docker.tar.bz2

##############################################################
# done
clean_up
