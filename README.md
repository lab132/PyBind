# Linux compilation

Fastbuild is configured to use clang.
Make sure you have clang installed and is available in your path as `clang` and `clang++`.
On Ubuntu 14.04 official clang installation are available for 3.3, 3.4 and 3.5. To create `clang` and `clang++` links you can write:
```
sudo ln -s /usr/bin/clang-3.5 /usr/bin/clang
sudo ln -s /usr/bin/clang++-3.5 /usr/bin/clang++
```
and replace `3.5` with your version of clang.

Compilation on linux is tested on Ubuntu 14.04 x64 using Clang 3.5.

### Dependencies

The only dependency for this is obviously Python in version 3.5.x.
Make sure you have the developer version of Python 3.5.x installed.
