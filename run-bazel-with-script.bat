:: @echo off

echo "ON WINDOWS (Visual Studio), USE: Developer Command Prompt"
echo "REMEMBER TO RUN: .\script-deps.bat"

rem Seeds desejadas, 789123456 543210987 245678901 728462315 876543210
set seeds=789123456 543210987 245678901 728462315 876543210

rem Arquivo para salvar os resultados
set output_file=DistrProba10.txt

rem Remover o arquivo de resultados existente, se houver
del %output_file% 2>nul

rem Loop sobre as seeds
for %%s in (%seeds%) do (
    echo Compilando e executando com a seed %%s...
    bazel build ... --config windows_mt
    .\bazel-bin\app_demo.exe %%s >> %output_file%
)

echo Resultados salvos em %output_file%
echo Concluído!
