#!/bin/bash

echo "ON WINDOWS (Visual Studio), USE: Developer Command Prompt"
echo "REMEMBER TO RUN: ./script-deps.sh"

# Seeds desejadas
seeds="789123456 543210987 245678901 728462315 876543210"

# Arquivo para salvar os resultados
output_file="DistrProba10.txt"

# Remover o arquivo de resultados existente, se houver
rm -f "$output_file"

# Loop sobre as seeds
for s in $seeds; do
    echo "Compilando e executando com a seed $s..."
    
    # Definir variáveis necessárias para substituições do template
    bazel build --cxxopt="-DGMP_TYPE_L='#define GMP_DEMANDS_UINTD_LONG'" \
                --cxxopt="-DGMP_TYPE_LL='/* #undef GMP_DEMANDS_UINTD_LONG_LONG */'" \
                --define=TIME_UNIX_WIN='#define HAVE_GETTIMEOFDAY' \
                --define=GMP_TYPE_L='#define GMP_DEMANDS_UINTD_LONG' \
                --define=GMP_TYPE_LL='/* #undef GMP_DEMANDS_UINTD_LONG_LONG */' \
                //:app_demo  

    # Verificar se a compilação foi bem-sucedida antes de executar o binário
    if [ -f "./bazel-bin/app_demo" ]; then
        ./bazel-bin/app_demo $s >> "$output_file"
    else
        echo "Erro: o binário app_demo não foi encontrado após a compilação."
    fi
done

echo "Resultados salvos em $output_file"
echo "Concluído!"
