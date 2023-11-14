:: @echo off

echo "ON WINDOWS (Visual Studio), USE: Developer Command Prompt"
echo "REMEMBER TO RUN: .\script-deps.bat"

bazel build ... --config windows_mt

.\bazel-bin\app_demo.exe
