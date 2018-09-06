# Quick Start Recipes

* If you build the environment directly on the local computer, please start directly from step: [Checking out the source code](#checking-out-the-source-code)
* We provide an image of Elastos.RT for using. If you have an Amazon account, you can purchase the corresponding server and choose to install the image. After installation, you can build the environment of Elastos.RT.

  you can refer it: [How_To_Use_The_Image_Of_Elastos.RT](./How_To_Use_The_Image_Of_Elastos.RT.md)

## Checking out the source code

The Elastos.RT Git repository is located
at: <https://github.com/elastos/Elastos.RT>

To clone the repository
in your environment:

``` shell
cd ~

git clone --recurse-submodules https://github.com/elastos/Elastos.RT
```

We use `--recurse-submodules` here because we need to download the submodules as well.
If you forgot to use the argument, you could download the submodules later by typing:

``` shell
git submodule init
git submodule update
```

## Preparing the build environment

Please select the appropriate compilation environment as needed:

* ### Use dockerized Untuntu_64bit build environment

  1. Install `docker` and `docker-compose`

     Follow the official instructions to install Docker and Docker Compose.

     Install Docker: <https://docs.docker.com/install/>

     Install Docker Compose: <https://docs.docker.com/compose/install/>

  2. Build the docker image

     ``` shell
     $ cd ~/Elastos.RT/docker
     ~/Elastos.RT/docker$ docker-compose build --build-arg HOST_USER_UID=`id -u` --build-arg HOST_USER_GID=`id -g` build-env
     ```

  3. Enter the dockerized build environment:

     ``` shell
     $ cd ~/Elastos.RT/docker
     ~/Elastos.RT/docker$ docker-compose run --rm build-env
     ```
     NOTES: Please do NOT store any important files outside of the `Elastos.RT` folder as the docker container will be removed after you exit the building environment. And only the changes in the `Elastos.RT` folder will be retained as it has defined as a VOLUME in the Dockerfile.

     Type `exit` if you want to exit the docker build environment. And type `docker-compose run --rm build-env` if you want to re-enter it.

* ### Ubuntu_32bit

  On Ubuntu this should obtain the necessary pre-reqs:

  ``` shell
  sudo dpkg --add-architecture i386
  sudo apt-get update
  sudo apt-get install texinfo libglib2.0-dev autoconf libtool libsdl-dev build-essential tofrodos bison g++-multilib git gperf
  sudo apt-get install libxml2-utils make bc zip zlib1g-dev zlib1g-dev:i386 libicu-dev libicu-dev:i386 libssl-dev libssl-dev:i386 libuv1-dev libuv1-dev:i386
  ```

  or

  copy the libs under folder Build/Prebuilt/Linux/usr/lib to /lib/i386-linux-gnu/

  ``` shell
  sudo cp ~/Elastos.RT/Build/Prebuilt/Linux/usr/lib/lib* /lib/i386-linux-gnu/

  cd /lib/i386-linux-gnu/
  sudo ln -s libicuuc.so.52 libicuuc.so
  sudo ln -s libcrypto.so.1.0.0 libcrypto.so
  ```

  [Install cmake](#install-cmake) if needed.

* ### Ubuntu_64bit

  On Ubuntu this should obtain the necessary pre-reqs:

  ``` shell
  sudo apt-get install texinfo libglib2.0-dev autoconf libtool libsdl-dev build-essential tofrodos zlib1g-dev:amd64 zlib1g-dev
  sudo apt-get install bison g++-multilib git gperf libxml2-utils make zlib1g-dev:i386 zip libsqlite3-dev libicu-dev libssl-dev libuv1-dev
  sudo apt-get install autopoint flex libncurses5-dev
  ```

  If you can't find the package "zlib1g-dev:i386", try installing the package "lib32z1".

  [Install cmake](#install-cmake) if needed.

* ### Install CMake

  To use the new CMake building environment, you will need to have `cmake` installed.

  To build for the Android by using `clang` compiler, you should have CMake 3.6.0 or newer version.

  The Ubuntu 16.04 come with CMake 3.5.0 if you install it by using `apt-get`.

  Check the cmake version if you already installed it.

  ``` shell
  cmake --version
  ```

  Remove the old version

  ``` shell
  sudo apt-get purge cmake
  ```

  Download and install the newer version:

  ``` shell
  CMAKE_VERSION=3.11

  CMAKE_BUILD=0

  wget https://cmake.org/files/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.${CMAKE_BUILD}-Linux-x86_64.tar.gz

  mkdir ~/cmake

  tar xzvf cmake-${CMAKE_VERSION}.${CMAKE_BUILD}-Linux-x86_64.tar.gz --strip-components=1 -C ~/cmake

  export PATH=~/cmake/bin:$PATH
  ```

  You may want to append the `export PATH=~/cmake/bin:$PATH` to your `~/.bashrc` file.

* ### MacOS (for IOS build)

  1. Install [homebrew](https://brew.sh)

     ``` shell
     /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
     ```

  2. Install cmake

     ``` shell
     brew install cmake
     ```

  3. Install Xcode and Check Xcode path

     Please search Xcode in AppStore and install it.

     Run this command to check the active developer directory path

     ``` shell
     /usr/bin/xcode-select -print-path
     ```

     If the result is not `/Applications/Xcode.app/Contents/Developer`，then swtich the active developer directory

     ``` shell
     sudo xcode-select --switch /Applications/Xcode.app
     ```

  4. Install simulator in Xcode

     If you just wanted to build for the "ios_device", you can ignore this step.

     Xcode->Preferences->Components, install iOS 11.1 Simulator or newer version.

## Build Elastos.RT

### Build Android on Mac or Linux

1. Download the proper Android SDK NDK and JDK for your OS. (Currently support Mac and Linux)

   ***Download the lastest version [Android SDK](https://developer.android.com/studio/)***

   You may choose to install Android Studio and sdk manager or install sdk tools only.

   ***Download the lastest version [Android NDK](https://developer.android.com/ndk/downloads/index.html)***

   You will need to have Android NDK r15c or newer version.

   ***Download [JDK](http://www.oracle.com/technetwork/java/javase/downloads/index.html)***

   Please select jdk1.8.0.

2. Set `ANDROID_NDK` and `ANDROID_HOME` environment variable

   ``` shell
   export ANDROID_NDK=~/my-android-ndk
   export ANDROID_HOME=~/my-android-sdk
   ```

   NOTE: If you are using the dockerized build environment. Make sure set the ANDROID_NDK to a Linux version of the Android NDK before entering the dockerized build environment.

   [Install cmake](#install-cmake) if needed.

3. Enter the build environment:

   ``` shell
   ~/Elastos.RT$ source Setup/SetEnv.sh arm_android_32
   ```

   or

   ``` shell
   ~/Elastos.RT$ source Setup/SetEnv.sh
   ```

   and then choose the index number of the "arm_android_32" item from the numbered list

4. emake

   ``` shell
   ~/Elastos.RT/Sources$ emake
   ```

### Build Linux

1. Enter the build environment:

   ``` shell
   ~/Elastos.RT$ source Setup/SetEnv.sh linux_64
   ```

   or

   ``` shell
   ~/Elastos.RT$ source Setup/SetEnv.sh
   ```

   and then choose the index number of the "linux_64" item from the numbered list

2. emake

   ``` shell
   ~/Elastos.RT/Sources$ emake
   ```

### Build IOS

1. Enter the build environment:

   * ios_simulator : Simulator version
     ``` shell
     ~/Elastos.RT$ source Setup/SetEnv.sh ios_simulator
     ```
     or
     ``` shell
     ~/Elastos.RT$ source Setup/SetEnv.sh
     ```
     and then choose the index number of the "ios_simulator" item from the numbered list

   * ios_device : Real machine version
     ``` shell
     ~/Elastos.RT$ source Setup/SetEnv.sh ios_device
     ```
     or
     ``` shell
     ~/Elastos.RT$ source Setup/SetEnv.sh
     ```
     and then choose the index number of the "ios_device" item from the numbered list

2. emake

   ``` shell
   ~/Elastos.RT/Sources$ emake
   ```

NOTES: No matter which IOS version is compiled, you should register Apple developer ID before compiling; but before compiling ios_device version, you also should set DEVELOPMENT_TEAM_ID and you may need to modify APP_BUNDLE_IDENTIFIER in CMakeLists.txt
