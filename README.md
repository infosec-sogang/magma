# ISLAB-MAGMA

## 1. Getting started
### 1.1 Build Dockerfile
Run
```
$ ./build-docker.sh
```
or 
```
$ docker build -t magma-clean -f docker/Dockerfile .
```
### 1.2 Run Docker Container
```
$ ./run-container.sh
``` 
or
 ```
 $ docker run -it magma-clean /bin/bash
 ```

## 2. Directory Structure
### 2.1 Local framework structure
```
|- README.md     <- The top-level README (this file)
|
|- docs
|  |
|  └─ magma-official-docs   <- Directory to store magma's official docs
|  
|- fuzzers
|  |
|  |- afl <- Directory for afl fuzzer
|  |
|  |- vanilla    <- Directory for vanilla fuzzer. In other words, it just builds the target library.
|  |
|  └─ template   <- Template for fuzzer. (This directory is currently the same as the vanilla directory. )
|     | 
|     |_ preinstall.sh  <- This file contains codes to install libraries for this fuzzer before build this fuzzer
|     |
|     |- fetch.sh   <- This file contains codes to get the source-file of this fuzzer by using wget or git clone
|     |
|     |- build.sh   <- This file contains codes to build this fuzzer
|     |
|     └─ instrument.sh  <- This file contains codes to build magma.o and to build target library
|  
|- magma  <- Directory for magma
|
|- targets  <-Directory for target libraries
|
|- build-docker.sh
|
└─ run-container.sh
```

### 2.2 Docker directory structure
```
|- magma_out  <- Directory in which executable files for compiled target libraries are stored
|
|- magma     
|  |
|  |- targets <- Directory that contains target libraries
|  |  |
|  |  |_ libpng
|  |  |  |
|  |  |  |_ ... 
|  |  |  |
|  |  |  └─ repo  <- Directory for total source codes of this library 
|  |  |
|  |  |_ libtiff
|  |  |  |
|  |  |  |_ ... 
|  |  |  |
|  |  |  └─ repo  <- Directory for total source codes of this library 
|  |  |
|  |  |_ libxml2
|  |  |  |
|  |  |  |_ ... 
|  |  |  |
|  |  |  └─ repo  <- Directory for total source codes of this library 
|  |  |
|  |  |_ openssl
|  |  |  |
|  |  |  |_ ... 
|  |  |  |
|  |  |  └─ repo  <- Directory for total source codes of this library 
|  |  |
|  |  |_ php
|  |  |  |
|  |  |  |_ ... 
|  |  |  |
|  |  |  └─ repo  <- Directory for total source codes of this library 
|  |  |
|  |  |_ sqlite3
|  |     |
|  |     |_ ... 
|  |     |
|  |     └─ repo  <- Directory for total source codes of this library 
|  |
|  |_ fuzzers <- Directory that contains fuzzers
|  |
|  |- vanilla <- Directory for vanilla fuzzer. In other words, it just builds the target library.
|  |  |
|  |  |- ...
|  |  |
|  |  └─ instrument.sh <- To rebuild target library with this fuzzer
|  |
|  └─ magma   <- Directory that contains magma
|  
└─ magma_shared   <- Legacy directory (TODO: DELETE IT)
```

## 3. Usage
**(TODO : Update It)**
### 3.1 Rebuild all targets libraries with vanilla in docker container
```
$ /magma/fuzzers/vanilla/instrument.sh
```
### 3.2 Setup new fuzzer
**(Todo : make it sophisticated, comfortable, friendly)**  

Now, In order to add new fuzzer in this framework  
We should
1. Add the fuzzer at LocalDirecrtory/fuzzer by using and manipulate template
2. Edit Dockerfile  