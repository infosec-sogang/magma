#!/bin/bash
set -e

## ISLAB COMMENT ####
#### preinstall.sh : This file contains codes to install libraries for this fuzzer before build this fuzzer

apt-get update && \
    apt-get install -y make build-essential