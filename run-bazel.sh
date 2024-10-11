#!/bin/bash
set -e

echo "LEMBRE-SE DE EXECUTAR: vcpkg-install.sh"

OUTPUT_USER_ROOT="/dados/home/tesla-dados/.cache/bazel"

mkdir -p "$OUTPUT_USER_ROOT"

bazel --output_user_root="$OUTPUT_USER_ROOT" build //... --config=linux

bazel --output_user_root="$OUTPUT_USER_ROOT" run :refresh_compile_commands

bazel --output_user_root="$OUTPUT_USER_ROOT" test //... --config=linux

./bazel-bin/app_demo
