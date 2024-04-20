FROM ubuntu:20.04

RUN apt-get update && apt-get install -y sudo
RUN sudo apt-get install -y gdb

## Magma directory hierarchy
# magma_root is relative to the docker-build's working directory
# The Docker image must be built in the root of the magma directory
ARG magma_root=./

## Path variables inside the container
ENV MAGMA_R /magma
ENV OUT     /magma_out
ENV SHARED  /magma_shared

ENV CC  /usr/bin/gcc
ENV CXX /usr/bin/g++
ENV LD /usr/bin/ld
ENV AR /usr/bin/ar
ENV AS /usr/bin/as
ENV NM /usr/bin/nm
ENV RANLIB /usr/bin/ranlib

RUN mkdir -p ${SHARED} ${OUT} && \
    chmod 744 ${SHARED} ${OUT}

ARG magma_path=magma
ENV MAGMA    ${MAGMA_R}/${magma_path}
USER root:root
RUN mkdir -p ${MAGMA}
COPY --chown=root:root ${magma_root}/${magma_path} ${MAGMA}/
RUN ${MAGMA}/preinstall.sh

# FUZZER SETUP ########
ARG fuzzer_name=vanilla
ARG fuzzer_path=fuzzers/${fuzzer_name}
ENV FUZZER   ${MAGMA_R}/${fuzzer_path}
USER root:root
RUN mkdir -p ${FUZZER}
COPY --chown=root:root ${magma_root}/${fuzzer_path} ${FUZZER}/
RUN ${FUZZER}/preinstall.sh
RUN ${FUZZER}/fetch.sh
RUN ${FUZZER}/build.sh

#### TARGETS ####
# libpng
ENV target_path=targets/libpng
ENV TARGET  ${MAGMA_R}/${target_path}
USER root:root
RUN mkdir -p ${TARGET}
COPY --chown=root:root ${magma_root}/${target_path} ${TARGET}/
RUN ${TARGET}/preinstall.sh
RUN ${TARGET}/fetch.sh
RUN ${MAGMA}/apply_patches.sh
ENV TARGET_libpng   ${TARGET}
# libtiff
ENV target_path=targets/libtiff
ENV TARGET  ${MAGMA_R}/${target_path}
USER root:root
RUN mkdir -p ${TARGET}
COPY --chown=root:root ${magma_root}/${target_path} ${TARGET}/
RUN ${TARGET}/preinstall.sh
RUN ${TARGET}/fetch.sh
RUN ${MAGMA}/apply_patches.sh
ENV TARGET_libtiff   ${TARGET}
# libxml2
ENV target_path=targets/libxml2
ENV TARGET  ${MAGMA_R}/${target_path}
USER root:root
RUN mkdir -p ${TARGET}
COPY --chown=root:root ${magma_root}/${target_path} ${TARGET}/
RUN ${TARGET}/preinstall.sh
RUN ${TARGET}/fetch.sh
RUN ${MAGMA}/apply_patches.sh
ENV TARGET_libxml2   ${TARGET}
# openssl
ENV target_path=targets/openssl
ENV TARGET  ${MAGMA_R}/${target_path}
USER root:root
RUN mkdir -p ${TARGET}
COPY --chown=root:root ${magma_root}/${target_path} ${TARGET}/
RUN ${TARGET}/preinstall.sh
RUN ${TARGET}/fetch.sh
RUN ${MAGMA}/apply_patches.sh
ENV TARGET_openssl   ${TARGET}
# sqlite
ENV target_path=targets/sqlite3
ENV TARGET  ${MAGMA_R}/${target_path}
USER root:root
RUN mkdir -p ${TARGET}
COPY --chown=root:root ${magma_root}/${target_path} ${TARGET}/
RUN ${TARGET}/preinstall.sh
RUN ${TARGET}/fetch.sh
RUN ${MAGMA}/apply_patches.sh
ENV TARGET_sqlite   ${TARGET}
# php
ENV target_path=targets/php
ENV TARGET  ${MAGMA_R}/${target_path}
USER root:root
RUN mkdir -p ${TARGET}
COPY --chown=root:root ${magma_root}/${target_path} ${TARGET}/
RUN ${TARGET}/preinstall.sh
RUN ${TARGET}/fetch.sh
RUN ${MAGMA}/apply_patches.sh
ENV TARGET_php   ${TARGET}

## Configuration parameters
# ARG isan
# ARG harden
# ARG canaries
# ARG fixes

# ARG ISAN_FLAG=${isan:+-DMAGMA_FATAL_CANARIES}
# ARG HARDEN_FLAG=${harden:+-DMAGMA_HARDEN_CANARIES}
# ARG CANARIES_FLAG=${canaries:+-DMAGMA_ENABLE_CANARIES}
# ARG FIXES_FLAG=${fixes:+-DMAGMA_ENABLE_FIXES}
# ARG BUILD_FLAGS="-include ${MAGMA}/src/canary.h ${CANARIES_FLAG} ${FIXES_FLAG} ${ISAN_FLAG} ${HARDEN_FLAG} -g -O0"

ARG BUILD_FLAGS="-include ${MAGMA}/src/canary.h -DMAGMA_ENABLE_CANARIES -DMAGMA_FATAL_CANARIES -g -O0"

ENV CFLAGS ${BUILD_FLAGS}
ENV CXXFLAGS ${BUILD_FLAGS}
ENV LIBS -l:magma.o -lrt
ENV LDFLAGS -L"${OUT}" -g

RUN ${FUZZER}/instrument.sh

# ENTRYPOINT "${MAGMA}/run.sh"