# Projeto Final

## Recursing submodules

```
git submodule update --init --recursive
git pull --recurse-submodules
```

## Using Conan
```
python3 -m pip install conan
conan profile detect
```

On CMakeProject2, load conan auto-generated cmake files into build/ folder:

```
conan install conanfile.txt --output-folder=build-conan --build=missing
```

Update your CMakePresets.json to include conan:

```{.json}
"cacheVariables": {
    "CMAKE_BUILD_TYPE": "Release",
    "CMAKE_TOOLCHAIN_FILE": "build-conan/conan_toolchain.cmake"
},
```
or, to add vcpkg:

```{.json}
"toolchainFile": "${sourceDir}/../deps/vcpkg/scripts/buildsystems/vcpkg.cmake",
```

Read more on vcpkg integration: https://learn.microsoft.com/pt-br/vcpkg/users/buildsystems/cmake-integration

