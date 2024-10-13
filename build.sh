#!/bin/bash
set -e
bazel build //:app_demo --config=linux
