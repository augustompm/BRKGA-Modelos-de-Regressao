#!/bin/bash
# New Runner @aug
seeds="100001 100002 100003 100004 100005 100006 100007 100008 100009 100010 100011 100012 100013 100014 100015 100016 100017 100018 100019 100020 100021 100022 100023 100024 100025 100026 100027 100028 100029 100030 100031 100032 100033 100034 100035 100036 100037 100038 100039 100040 100041 100042 100043 100044 100045 100046 100047 100048 100049 100050 100051 100052 100053 100054 100055 100056 100057 100058 100059 100060"

instances=(
    "Produto_interno_100.in" 
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