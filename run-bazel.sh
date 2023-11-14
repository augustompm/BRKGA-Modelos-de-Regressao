#!/bin/bash

echo "REMEMBER TO RUN: vcpkg-install.sh"

bazel build ... --config linux

bazel run :refresh_compile_commands

./bazel-bin/app_demo
