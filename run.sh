#!/bin/bash
# Runner aug01

# seeds 
seeds="100006 100007 100008 100009 100010 100011 100012 100013 100014 100015 100016 100017 100018 100019 100020 100021 100022 100023 100024 100025 100026 100027 100028 100029 100030"


# instâncias 
instances=(
"descent.in"
)


# resultados
results_dir="descent"
mkdir -p "$results_dir"

# para cada instância
for instance in "${instances[@]}"; do
    echo "Executando para a instância $instance..."

    i=01
    # para cada seed
    for s in $seeds; do
        
        output_file="${results_dir}/${instance%.in}_${s}_$i.txt"
        echo "Executando ./bazel-bin/app_demo $s descent/$instance 125 25 15 85 100 50 5 5"
        
        if [ -f "./bazel-bin/app_demo" ]; then
            ./bazel-bin/app_demo "$s" "descent/$instance" 125 25 15 85 100 50 5 5 >> "$output_file"
            echo "Resultado salvo em $output_file"
        else
            echo "Erro: o binário app_demo não foi encontrado."
            exit 1
        fi
        
        i=$(printf "%02d" $((10#$i + 1)))
    done
done

echo "Done!"
