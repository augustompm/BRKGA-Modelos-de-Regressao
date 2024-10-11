#!/bin/bash

echo "ON LINUX, MAKE SURE YOU HAVE THE NECESSARY TOOLS INSTALLED"

echo "REMEMBER TO RUN: ./tools/vcpkg/bootstrap-vcpkg.sh"

export BAZEL_OUTPUT_USER_ROOT="/dados/home/tesla-dados/.cache/bazel"

mkdir -p "$BAZEL_OUTPUT_USER_ROOT"

./tools/vcpkg/bootstrap-vcpkg.sh

./tools/vcpkg/vcpkg integrate install

./tools/vcpkg/vcpkg --vcpkg-root ./tools/vcpkg install --triplet x64-linux


# Run your Bazel build script
./run-bazel.sh
