argosd
====

Open source reimplimentation of Samsung's "Adaptive Resource Governor for Operating Systems" daemon for Android.

Building (out of tree)
====

mkdir build

cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=33 -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-33 ..

License
====

GPLv2
