#!/bin/bash
# New Runner @aug
seeds="100031 100032 100033 100034 100035 100036 100037 100038 100039 100040 100041 100042 100043 100044 100045 100046 100047 100048 100049 100050 100051 100052 100053 100054 100055 100056 100057 100058 100059 100060"

instances=(
    "test-Distancia_Euclidiana_10.in"
    "test-Distancia_euclidiana_20.in"
    "test-Distancia_euclidiana_40.in"
    "test-Distancia_euclidiana_80.in"
    "test-Produto_Interno_10.in"
    "test-Produto_Interno_20.in"
    "test-Produto_Interno_40.in"
    "test-Produto_Interno_80.in"
    "test-Densidade_probabi_10.in"
    "test-Densidade_probabi_20.in"
    "test-Densidade_probabi_40.in"
    "test-Densidade_probabi_80.in"    
) 

results_dir="results"
mkdir -p "$results_dir"

# Instâncias
for instance in "${instances[@]}"; do
    echo "Executando para a instância $instance..."
    
    # Seeds $seeds
    i=01
    for s in $seeds; do
        output_file="${results_dir}/${instance%.in}_${s}_$i.txt"
        echo "Executando ./bazel-bin/app_demo $s running_instances/$instance"
        
        # Verifica
        if [ -f "./bazel-bin/app_demo" ]; then
            ./bazel-bin/app_demo "$s" "running_instances/$instance" >> "$output_file"
            echo "Resultado salvo em $output_file"
        else
            echo "Erro: o binário app_demo não foi encontrado."
            exit 1
        fi
        
        i=$(printf "%02d" $((10#$i + 1)))
    done
done

echo "Done!"