# voxels
This is basically my attempts to learn OpenGL and C.

### Build
You need to initialize the git submodules
```shell
git submodule init cglm
git submodule init RGFW
git submodule update
```
Compile [nob](https://github.com/tsoding/nob.h) once
```shell
cc -o nob.c nob
```
And then every time you want to rebuild Voxels you should
```shell
./nob
```
