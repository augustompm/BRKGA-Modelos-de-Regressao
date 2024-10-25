#!/bin/bash
# New Runner @aug
seeds="789123456 543210987 245678901 728462315 876543210 123456789 987654321 654321098 567890123 345678912"

instances=(
"Bose_einstein_10.in"
"Densidade_probabi_10.in"
"Forca_gravit_10.in"
"Massa_relativ_10.in"
"Produto_interno_10.in"
"Ondas_eletromag_10.in"
"Lentes_delgadas_10.in"
"Campo_eletrico_10.in"
"Distancia_euclidiana_10.in"
"Massa_relativ_10.in"
"Periodo_orbital_10.in"
"bose-einstein-new-test.in"
"distribuicao-probabilidade-new-test.in"
)


results_dir="results"
mkdir -p "$results_dir"

# Instâncias
for instance in "${instances[@]}"; do
    echo "Executando para a instância $instance..."
    
    # Seeds
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