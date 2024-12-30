#!/bin/bash
# New Runner @aug

# seeds 
seeds="100001"

# instâncias 
instances=(
"bose-einstein_100_III.4.32.in"
"Campo_eletrico_100_I.12.4.in"
"Densidade_de_probabilidade_100_I.6.20a.in"
"Distancia_Euclidiana_100_I.8.14.in"
"Energia_cinetica_total_100_I.13.4.in"
"Energia_potencial_Elastica_mola_100_I.14.4.in"
"Energia_potencial_Gravitacional_100_I.14.3.in"
"Energia_potencial_gravitacional_com_constante_gravitacional_100_I.13.12.in"
"Energia_total_com_frequencia_100_l.24.6.in"
"Força_Atrito_100_I.12.1.in"
"Força_eletrica_100_I.12.5.in"
"Força_eletrica_entre_duas_cargas_100_I.12.2.in"
"Forca_gravitacional_100_I.9.18.in"
"Lentes_delgadas_100_I.27.6.in"
"Massa_relativistica_100_I.10.7.in"
"Ondas_eletromagneticas_100_II.24.17.in"
"Periodo_orbital_100_Goldstein 3.74 (Kepler).in"
"Posição_centro_de_massa_100_I.18.4.in"
"Produto_interno_100_I.11.19.in"
"transformação_Lorentz_100_I.15.3x.in"
"transformação_Lorentz_tempo_100_I.15.3t.in"
)

# Diretório de resultados
results_dir="GECCO_results"
mkdir -p "$results_dir"

# Executa para cada instância
for instance in "${instances[@]}"; do
    echo "Executando para a instância $instance..."

    i=01
    for s in $seeds; do
        
        output_file="${results_dir}/${instance%.in}_${s}_$i.txt"
        echo "Executando ./bazel-bin/app_demo $s instances_GECCO/$instance 125 25 15 85 100 50 5 5"
        
        if [ -f "./bazel-bin/app_demo" ]; then
            ./bazel-bin/app_demo "$s" "instances_GECCO/$instance" 125 25 15 85 100 50 5 5 >> "$output_file"
            echo "Resultado salvo em $output_file"
        else
            echo "Erro: o binário app_demo não foi encontrado."
            exit 1
        fi
        
        i=$(printf "%02d" $((10#$i + 1)))
    done
done

echo "Done!"
