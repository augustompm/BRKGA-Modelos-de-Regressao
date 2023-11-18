#!/bin/bash
set -e

echo "REMEMBER TO RUN: vcpkg-install.sh"

bazel build ... --config linux

bazel run :refresh_compile_commands

bazel test ... --config linux

./bazel-bin/app_demo
