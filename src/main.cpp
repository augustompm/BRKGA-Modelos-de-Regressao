// main.cpp

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ginac.h>
#include <gmp.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <Scanner/Scanner.hpp>
#include <brkgp/BRKGA.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <brkgp/ReadIO.hpp>
#include <brkgp/Utils.hpp>

using namespace GiNaC;

// Definição da classe my_visitor (omitida para brevidade)

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <seed> <instance_file>" << std::endl;
        return 1;
    }
    int seed = atoi(argv[1]);
    std::string instance = argv[2];
    Scenario other;
    other.operationsBi = {'+', '-', '*', '/'};
    other.operationsU = {'e', 'i', 'r', 'a'};
    other.separateT1T2();

    std::cout << "Bi T1: " << other.operationsBiT1.size() << ":";
    for(auto& op: other.operationsBiT1) std::cout << op;
    std::cout << std::endl;

    std::cout << "Bi T2: " << other.operationsBiT2.size() << ":";
    for(auto& op: other.operationsBiT2) std::cout << op;
    std::cout << std::endl;

    std::cout << "U T1: " << other.operationsUT1.size() << ":";
    for(auto& op: other.operationsUT1) std::cout << op;
    std::cout << std::endl;

    std::cout << "U T2: " << other.operationsUT2.size() << ":";
    for(auto& op: other.operationsUT2) std::cout << op;
    std::cout << std::endl;

    other.maxConst = 6;
    other.setStackLen(30, 30, 1.5); // Definir stackLen=30, stackLenMax=30, factor=1.5

    std::cout << "|ARGS| = " << argc << std::endl;
    std::cout << "individualLen:" << other.getIndividualLen() << std::endl;

    BRKGAParams params;
    params.populationLen = 100;
    params.eliteSize = 25;
    params.mutantSize = 10;
    params.eliteBias = 85;
    params.noImprovementMax = 50;
    params.iterationMax = 50;
    params.moreDiversity = 20;
    params.reset = 5;
    int training = 100;

    // Atualizar parâmetros adicionais se fornecidos
    if (argc > 3) params.populationLen = atoi(argv[3]);
    if (argc > 4) params.eliteSize = atoi(argv[4]);
    if (argc > 5) params.mutantSize = atoi(argv[5]);
    if (argc > 6) params.eliteBias = atoi(argv[6]);
    if (argc > 7) params.iterationMax = atoi(argv[7]);
    if (argc > 8) params.noImprovementMax = atoi(argv[8]);
    if (argc > 9) params.reset = atoi(argv[9]);
    if (argc > 10) training = atoi(argv[10]);

    // Adicionar operações unárias adicionais se fornecidas
    if (argc > 11) {
        if (argv[11][0] == 't') {
            other.operationsU.push_back('s');
            other.operationsU.push_back('c');
        } else if (argv[11][0] == 'e') {
            other.operationsU.push_back('a');
            other.operationsU.push_back('v');
            other.operationsU.push_back('r');
        }
    }
    if (argc > 12) {
        if (argv[12][0] == 't') {
            other.operationsU.push_back('s');
            other.operationsU.push_back('c');
        } else if (argv[12][0] == 'e') {
            other.operationsU.push_back('a');
            other.operationsU.push_back('v');
            other.operationsU.push_back('r');
        }
    }

    // Inicializar bestFoundSolution com individualLen correto
    ValuedChromosome bestFoundSolution;
    bestFoundSolution.cost = INFINITY;
    bestFoundSolution.randomKeys = Vec<chromosome>(other.getIndividualLen(), 0); // Deve ser 3 * stackLen + maxConst + 1
    bestFoundSolution.trueStackSize = 0.0;

    // Ler o problema
    Scanner scanner(new File(instance));
    RProblem problem;
    readIO(problem, scanner);

    Vec<Vec<double>> fullInputs = problem.inputs;
    Vec<double> fullOutputs = problem.outputs;

    // Opcional: modificar o problema para ser quadrado
    if (false) problem.make_squared();

    // Definir peso para variáveis não usadas
    other.setWeightPerUnusedVariable(1.0);

    // Imprimir os dados do problema
    printFile(problem);

    // Ajustar o treinamento com base no número de testes
    training = percentToInt(training, problem.tests);
    std::cout << "training=" << training << std::endl;

    // Configurar o gerador de números aleatórios
    RKGenerator rkg;
    rkg.nVars = problem.nVars;
    rkg.nConst = problem.nConst;
    rkg.operationsBi = other.operationsBi;
    rkg.operationsU = other.operationsU;
    rkg.stackLen = other.getStackLen();
    rkg.maxConst = other.maxConst;

    std::optional<Vec<chromosome>> opInitialSol = std::nullopt;

    std::cout << "run_brkga:" << std::endl;
    std::cout << "best=" << bestFoundSolution.cost << std::endl;

    // Executar o BRKGA
    run_brkga(problem, params, seed, bestFoundSolution, other, training, opInitialSol, fullInputs, fullOutputs);

    // Imprimir os resultados
    std::cout << "best=" << bestFoundSolution.cost << std::endl;
    printSolution(problem, bestFoundSolution.randomKeys, other);
    std::cout << "trueStackSize=" << bestFoundSolution.trueStackSize << std::endl;
    std::string eq = printSolution2(problem, bestFoundSolution.randomKeys, other);
    std::cout << "printSolution2: " << eq << std::endl;

    double auxBestFoundSolutionCost = bestFoundSolution.cost;

    // Calcular o tempo de execução
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double executionTime = duration.count();
    std::cout << "Tempo de execucao: " << executionTime << " segundos" << std::endl;

    // Código opcional para testes adicionais
    if (false) {
        std::cout << "BEGIN GINAC TESTS!" << std::endl;
    }

    std::cout << "FIM" << std::endl;
    return 0;
}
