#!/bin/bash
set -e
bazel --version | grep "7.4.1" || { echo "Versão incorreta do Bazel. Saindo."; exit 1; }
bazel build //:app_demo --config=linux
