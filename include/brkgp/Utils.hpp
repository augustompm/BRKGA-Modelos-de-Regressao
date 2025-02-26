// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>
#include <cassert>
#include <ginac.h>

using namespace GiNaC;

// Tipos Genéricos
template <typename T>
using Vec = std::vector<T>;

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;

typedef uint16_t chromosome;

// Dados do Problema de Regressão
struct RProblem {
    int nVars{-1};
    int tests{-1};
    int nConst{-1};
    Vec<Vec<double>> inputs;
    Vec<double> outputs;
    Vec<Pair<double, double>> vConst;
    // Unidades
    bool hasUnits{false};
    Vec<std::string> varUnits;
    std::string outUnit;
    Vec<std::string> constUnits;
    std::vector<std::string> allUnits;
    lst syms;  // GiNaC
    bool hasSolution{false};
    std::string solution;

    void setupUnits() {
        if (hasUnits) {
            for (const auto& u : allUnits) syms.append(symbol{u});
            // Remove duplicatas: sort + unique
            syms.sort();
            syms.unique();
        }
    }

    void make_squared() {
        squared = true;
        for (auto& output : outputs)
            output = output * output;
        if (hasUnits) {
            ex ex_out(outUnit, syms);
            ex_out = ex_out * ex_out;
            std::stringstream ss;
            ss << ex_out;
            outUnit = ss.str();
        }
    }

    bool isSquared() const { return squared; }

private:
    bool squared{false};
};

// Parâmetros do BRKGA
struct BRKGAParams {
    int populationLen = 30;
    int eliteSize = 25;
    int mutantSize = 5;
    uint16_t eliteBias = 70;
    int noImprovementMax = 10;
    // int restartMax = 1000;
    int iterationMax = 10;
    int moreDiversity = 75;
    int reset = 5;
};

// Cenário para Operações e Configurações
struct Scenario {
    // Operações binárias tipo 1 (+, -): não podem com unidades distintas
    std::vector<char> operationsBiT1;
    // Operações binárias tipo 2 (*, /): podem com unidades distintas
    std::vector<char> operationsBiT2;
    // Todas as operações binárias
    std::vector<char> operationsBi;
    //
    std::vector<char> operationsU;
    std::vector<char> operationsUT1;
    std::vector<char> operationsUT2;

    int maxConst{6}; // Definir valor padrão conforme necessário

    void separateT1T2() {
        // Deve garantir que T1 vem ANTES de T2
        bool isT2 = false;
        for(auto& op: operationsBi) {
            if((op == '+') || (op == '-')) {
                assert(!isT2 && "Operação T1 após T2");
                operationsBiT1.push_back(op);
            }
            else if((op == '*') || (op == '/')) {
                operationsBiT2.push_back(op);
                isT2 = true;
            }
            else {
                std::cerr << "ERROR: UNKNOWN OPERATION " << op << std::endl;
                assert(false);
            }
        }
        isT2 = false; // Reiniciar para operações unárias
        for(auto& op: operationsU) {
            if((op == 's') || (op == 'c')|| (op == 'e')|| (op == 'n')|| (op == 'p')|| (op == 'l')) {
                assert(!isT2 && "Operação UT1 após UT2");
                operationsUT1.push_back(op);
            }
            else if((op == 'a') || (op == 'i') || (op == 'r') || (op == 'v')) {
                operationsUT2.push_back(op);
                isT2 = true;
            }
            else {
                std::cerr << "ERROR: UNKNOWN OPERATION " << op << std::endl;
                assert(false);
            }
        }
    }

    void setStackLen(int _stackLen, int _stackLenMax,
                   double _stackLenFactor = 2.0) {
        assert(_stackLenFactor >= 1.0 && _stackLenFactor <= 3.0 && "stackLenFactor fora dos limites");
        stackLenIncreaseFactor = _stackLenFactor;
        stackLen = _stackLen;
        assert(_stackLenMax >= _stackLen && "stackLenMax menor que stackLen");
        stackLenMax = _stackLenMax;
        assert(maxConst > 0 && "maxConst deve ser > 0");
        individualLen = 3 * stackLen + maxConst + 1;
    }

    int getStackLen() const { return stackLen; }
    int getStackLenMax() const { return stackLenMax; }
    double getStackLenIncreaseFactor() const { return stackLenIncreaseFactor; }
    int getIndividualLen() const { return individualLen; }

    void setWeightPerUnusedVariable(double W) {
        assert(W >= 1.0 && "Weight por variável não utilizada deve ser >= 1.0");
        weightPerUnusedVariable = W;
    }
    double getWeightPerUnusedVariable() const { return weightPerUnusedVariable; }

private:
    int stackLen{30}; // Valor padrão
    int stackLenMax{30};
    double stackLenIncreaseFactor{1.5};
    int individualLen{3 * stackLen + maxConst + 1}; // 3 * 30 + 6 + 1 = 97
    // Multiplicar ERROR por este peso, para cada variável não utilizada (mínimo é UM)
    double weightPerUnusedVariable = 1.0;
};

// Cromossomo Avaliado com Valor
struct ValuedChromosome {
public:
    Vec<chromosome> randomKeys;
    double cost;
    int trueStackSize;
    int seed;

    void print() const {
        std::cout << "ValuedChromosome(cost=" << cost << "; seed=" << seed;
        std::cout << "; trueStackSize=" << trueStackSize
                  << "; size=" << randomKeys.size() << "): " << std::endl;
        for (auto rk : randomKeys) std::cout << rk << " ";
        std::cout << std::endl;
    }
};


inline void selectRandom10(RProblem &problem, int seed,
    const std::vector<std::vector<double>>& fullInputs,
    const std::vector<double>& fullOutputs) {

    std::cout << "skipping..." << std::endl;
    return;
}

/*

// Selecionar aleatoriamente 10 amostras
inline void selectRandom10(RProblem &problem, int seed,
      const std::vector<std::vector<double>>& fullInputs,
      const std::vector<double>& fullOutputs) {

    std::mt19937 gen(seed);
    std::uniform_int_distribution<> dis(0, 99); // Supondo 100 testes

    std::vector<int> idx(100);
    for (int i = 0; i < 100; i++) idx[i] = i;

    // Embaralha os índices
    std::shuffle(idx.begin(), idx.end(), gen);

    // idx[0..9] são os 10 selecionados
    problem.tests = 10;
    problem.inputs.clear();
    problem.inputs.resize(problem.tests, std::vector<double>(problem.nVars));
    problem.outputs.clear();
    problem.outputs.resize(problem.tests);

    for (int i = 0; i < 10; i++) {
        int sel = idx[i];
        for (int v = 0; v < problem.nVars; v++)
            problem.inputs[i][v] = fullInputs[sel][v];
        problem.outputs[i] = fullOutputs[sel];
    }

    std::cout << "SELECTED: [";
    for (int i = 0; i < 10; i++) std::cout << idx[i] << " ";
    std::cout << "]" << std::endl;
}
*/
