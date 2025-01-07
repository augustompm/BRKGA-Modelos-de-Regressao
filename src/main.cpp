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

// Função auxiliar para debug
void debugScanner(Scanner& scanner) {
    try {
        std::string linha = scanner.nextLine();
        std::cout << "Próxima linha do scanner: " << linha << std::endl;
    } catch (...) {
        std::cout << "Erro ao ler próxima linha" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
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
        other.setStackLen(30, 30, 1.5);

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

        if (argc > 3) params.populationLen = atoi(argv[3]);
        if (argc > 4) params.eliteSize = atoi(argv[4]);
        if (argc > 5) params.mutantSize = atoi(argv[5]);
        if (argc > 6) params.eliteBias = atoi(argv[6]);
        if (argc > 7) params.iterationMax = atoi(argv[7]);
        if (argc > 8) params.noImprovementMax = atoi(argv[8]);
        if (argc > 9) params.reset = atoi(argv[9]);
        if (argc > 10) training = atoi(argv[10]);

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

        ValuedChromosome bestFoundSolution;
        bestFoundSolution.cost = INFINITY;
        bestFoundSolution.randomKeys = Vec<chromosome>(other.getIndividualLen(), 0);
        bestFoundSolution.trueStackSize = 0.0;

        std::cout << "Debug: Iniciando leitura do arquivo..." << std::endl;
        Scanner scanner(new File(instance));
        RProblem problem;
        try {
            readIO(problem, scanner);
            std::cout << "Leitura feita com sucesso" << std::endl;
        } catch (...) {
            std::cerr << "Erro durante a leitura do arquivo" << std::endl;
            std::cout << "Tentando debug do scanner..." << std::endl;
            debugScanner(scanner);
            throw;
        }

        Vec<Vec<double>> fullInputs = problem.inputs;
        Vec<double> fullOutputs = problem.outputs;

        if (false) problem.make_squared();

        other.setWeightPerUnusedVariable(1.0);

        printFile(problem);

        training = percentToInt(training, problem.tests);
        std::cout << "training=" << training << std::endl;

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

        run_brkga(problem, params, seed, bestFoundSolution, other, training, opInitialSol, fullInputs, fullOutputs);

        std::cout << "best=" << bestFoundSolution.cost << std::endl;
        printSolution(problem, bestFoundSolution.randomKeys, other);
        std::cout << "trueStackSize=" << bestFoundSolution.trueStackSize << std::endl;
        std::string eq = printSolution2(problem, bestFoundSolution.randomKeys, other);
        std::cout << "printSolution2: " << eq << std::endl;

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        double executionTime = duration.count();
        std::cout << "Tempo de execucao: " << executionTime << " segundos" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Erro fatal durante execução" << std::endl;
        return 1;
    }

    std::cout << "FIM" << std::endl;
    return 0;
}