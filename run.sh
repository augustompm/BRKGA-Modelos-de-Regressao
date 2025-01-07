#!/bin/bash
# Runner aug01

# seeds 
seeds="100001 100002 100003 100004 100005"

# instâncias 
instances=(
"Bose-einstein_100_III.4.32.in"
"Campo_eletrico_100_I.12.4.in"
"Campo_eletrico_gerado_por_dipolo_100_II.6.15a.in"
"Campo_magnetico_em_torno_de_um_fio_100_II.13.17.in"
"Densidade_de_energia_eletrica_100_II.8.31.in"
"Densidade_de_probabilidade_100_I.6.20a.in"
"Distancia_Euclidiana_100_I.8.14.in"
"Energia_cinetica_total_100_I.13.4.in"
"Energia_potencial_Elastica_mola_100_I.14.4.in"
"Energia_potencial_Gravitacional_100_I.14.3.in"
"Energia_potencial_gravitacional_com_constante_gravitacional_100_I.13.12.in"
"Energia_total_com_frequencia_100_l.24.6.in"
"Forca_gravitacional_100_I.9.18.in"
"Força_Atrito_100_I.12.1.in"
"Força_eletrica_100_I.12.5.in"
"Força_eletrica_entre_duas_cargas_100_I.12.2.in"
"Lentes_delgadas_100_I.27.6.in"
"Massa_relativistica_100_I.10.7.in"
"Momento_relativistico_100_I.15.10.in"
"Ondas_eletromagneticas_100_II.24.17.in"
"Periodo_orbital_100_Goldstein 3.74 (Kepler).in"
"Posição_centro_de_massa_100_I.18.4.in"
"Produto_interno_100_I.11.19.in"
"Transformação_Lorentz_100_I.15.3x.in"
"Transformação_Lorentz_tempo_100_I.15.3t.in"
)


# resultados
results_dir="GECCO_results"
mkdir -p "$results_dir"

# para cada instância
for instance in "${instances[@]}"; do
    echo "Executando para a instância $instance..."

    i=01
    # para cada seed
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
