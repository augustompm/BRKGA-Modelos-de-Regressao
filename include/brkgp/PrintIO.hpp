// PrintIO.hpp

#pragma once

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <cassert>
#include <random>
#include <brkgp/Utils.hpp>

// Funções auxiliares para executar operações binárias e unárias
inline std::string stringExecBinaryOp(int idOpBi, const std::string& v1, const std::string& v2, const std::vector<char>& operationsBi) {
    if (idOpBi >= 0 && idOpBi < static_cast<int>(operationsBi.size())) {
        if (operationsBi[idOpBi] == '/') {
#ifdef STRING_FRAC
            std::stringstream ss;
            ss << "\\frac{" << v1 << "}{" << v2 << "}";
            return ss.str();
#else
            return "(" + v1 + "/" + v2 + ")";
#endif
        }
        return "(" + v1 + " " + operationsBi[idOpBi] + " " + v2 + ")";
    }
    return "NOP";
}

inline std::string stringExecUnaryOp(int idOpU, const std::string& v1, const std::vector<char>& operationsU) {
    if (idOpU >= 0 && idOpU < static_cast<int>(operationsU.size())) {
        switch (operationsU[idOpU]) {
            case 's': return "\\sin(" + v1 + ")";
            case 'c': return "\\cos(" + v1 + ")";
            case 'i': return "(" + v1 + ")";
            case 'a': return "{" + v1 + "^2}";
            case 'v': return "{" + v1 + "^3}";
            case 'r': return "\\sqrt{" + v1 + "}";
            case 'e': return "\\exp(" + v1 + ")";
            case 'n': return "\\log(" + v1 + ")";
            case 'p': return "2^{" + v1 + "}";
            case 'l': return "\\log_2(" + v1 + ")";
            default: return "NOPU";
        }
    }
    return "NOPU";
}

// Função auxiliar para calcular stackLen baseado no tamanho do individual e maxConst
inline size_t calculateStackLen(const Vec<chromosome>& individual, int maxConst) {
    if (individual.size() < static_cast<size_t>(maxConst) + 1) {
        std::cerr << "Erro: O vetor 'individual' é muito pequeno para calcular stackLen." << std::endl;
        return 0;
    }
    size_t stackLen = (individual.size() - static_cast<size_t>(maxConst) - 1) / 3;
    return stackLen;
}

// Função para imprimir a solução em LaTeX
inline std::string printSolution2(const RProblem& problem, const Vec<chromosome>& individual, const Scenario& other) {
    // Calcular stackLen dinamicamente
    size_t stackLen = calculateStackLen(individual, other.maxConst);
    if (stackLen == 0) {
        std::cerr << "Erro: stackLen calculado inválido." << std::endl;
        return "";
    }

    size_t individualLen = 3 * stackLen + static_cast<size_t>(other.maxConst) + 1;

    std::stack<std::string> stk;
    size_t contSeed = 0;

    // Verificação do tamanho do vetor
    if (individual.size() < individualLen) {
        std::cerr << "Aviso: O vetor 'individual' não possui elementos suficientes. Esperado: " << individualLen << ", Atual: " << individual.size() << std::endl;
        // Ajustar stackLen para o máximo possível
        stackLen = (individual.size() - static_cast<size_t>(other.maxConst) - 1) / 3;
        individualLen = 3 * stackLen + static_cast<size_t>(other.maxConst) + 1;
    }

    for (size_t j = 0; j < stackLen; j++) {
        // Verificar se j + stackLen está dentro dos limites
        if ((j + stackLen) >= individual.size()) {
            std::cerr << "Aviso: Índice j + stackLen = " << (j + stackLen) << " fora dos limites." << std::endl;
            break;
        }

        // Variável ou constante
        if ((individual[j] < 7500) && (individual[j] >= 5000)) {
            int idVar = static_cast<int>(floor((individual[stackLen + j] / 10000.0) * (problem.nVars + problem.nConst))) - problem.nConst;
            double varValue = 0.0;

            if (idVar >= 0) {
                // Variável
                if (idVar >= problem.nVars) {
                    std::cerr << "Erro: idVar (" << idVar << ") excede o número de variáveis (" << problem.nVars << ")." << std::endl;
                    continue;
                }
                std::stringstream ss;
                ss << "V" << idVar;
                stk.push(ss.str());
            } else {
                // Constante
                idVar += problem.nConst;
                if (idVar < 0 || idVar >= static_cast<int>(problem.vConst.size())) {
                    std::cerr << "Erro: idVar (" << idVar << ") está fora dos limites de vConst." << std::endl;
                    continue;
                }

                if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
                    varValue = problem.vConst[idVar].first;
                    std::stringstream ss;
                    ss << varValue;
                    stk.push(ss.str());
                } else {
                    if ((3 * stackLen + contSeed) >= individual.size()) {
                        std::cerr << "Aviso: Acessando individual[" << (3 * stackLen + contSeed) << "] fora dos limites." << std::endl;
                        break;
                    }
                    int seedConst = individual[3 * stackLen + contSeed];
                    std::mt19937 gen(seedConst);
                    std::uniform_int_distribution<> dis(static_cast<int>(problem.vConst[idVar].first), static_cast<int>(problem.vConst[idVar].second));
                    varValue = dis(gen);
                    std::stringstream ss;
                    ss << varValue;
                    stk.push(ss.str());
                    contSeed++;
                }
            }
        }

        // Operação binária
        if ((j < individual.size()) && (individual[j] < 2500)) {
            if ((2 * stackLen + j) >= individual.size()) {
                std::cerr << "Aviso: Acessando individual[" << (2 * stackLen + j) << "] fora dos limites para operação binária." << std::endl;
                continue;
            }
            int idOpBi = static_cast<int>(floor((individual[2 * stackLen + j] / 10000.0) * other.operationsBi.size()));
            if (idOpBi < 0 || idOpBi >= static_cast<int>(other.operationsBi.size())) {
                std::cerr << "Erro: idOpBi (" << idOpBi << ") inválido." << std::endl;
                continue;
            }

            // Verificar se há elementos suficientes na pilha
            if (stk.size() < 2) {
                std::cerr << "Erro: Pilha insuficiente para operação binária." << std::endl;
                continue;
            }

            std::string v1 = stk.top();
            stk.pop();
            std::string v2 = stk.top();
            stk.pop();
            std::string binaryProduct = stringExecBinaryOp(idOpBi, v1, v2, other.operationsBi);
            stk.push(binaryProduct);
        }

        // Operação unária
        if ((j < individual.size()) && (individual[j] < 5000) && (individual[j] >= 2500)) {
            if ((2 * stackLen + j) >= individual.size()) {
                std::cerr << "Aviso: Acessando individual[" << (2 * stackLen + j) << "] fora dos limites para operação unária." << std::endl;
                continue;
            }
            int idOpU = static_cast<int>(floor((individual[2 * stackLen + j] / 10000.0) * other.operationsU.size()));
            if (idOpU < 0 || idOpU >= static_cast<int>(other.operationsU.size())) {
                std::cerr << "Erro: idOpU (" << idOpU << ") inválido." << std::endl;
                continue;
            }

            // Verificar se há elementos suficientes na pilha
            if (stk.empty()) {
                std::cerr << "Erro: Pilha vazia para operação unária." << std::endl;
                continue;
            }

            std::string v1 = stk.top();
            stk.pop();
            std::string unaryProduct = stringExecUnaryOp(idOpU, v1, other.operationsU);
            stk.push(unaryProduct);
        }

        // Operação adicional ou futura (não implementada)
        if ((j < individual.size()) && (individual[j] >= 7500)) {
            // Implementar conforme necessário
        }
    }

    // Verificar se a pilha contém exatamente um elemento após o processamento
    if (stk.size() != 1) {
        std::cerr << "Aviso: Pilha após processamento contém " << stk.size() << " elementos, esperado 1." << std::endl;
        // Retornar a concatenação dos elementos restantes ou uma indicação de erro
        std::string partialSolution = "";
        while (!stk.empty()) {
            partialSolution = stk.top() + " " + partialSolution;
            stk.pop();
        }
        return partialSolution;
    }

    std::string val = stk.top();
    stk.pop();

    return val;
}

// Função para imprimir os dados do problema
inline void printFile(const RProblem& problem) {
    int nVars = problem.nVars;
    int tests = problem.tests;
    int nConst = problem.nConst;
    const Vec<Vec<double>>& inputs = problem.inputs;
    const Vec<double>& outputs = problem.outputs;

    printf("%d %d %d\n", nVars, tests, nConst);
    for (int i = 0; i < tests; i++) {
        for (int j = 0; j < nVars; j++) printf("%.2f\t", inputs[i][j]);
        printf("%.2f\n", outputs[i]);
    }
    for (int i = 0; i < nConst; i++) {
        printf("%.4f\t%.4f\n", problem.vConst[i].first, problem.vConst[i].second);
    }
    std::cout << "isSquared? " << problem.isSquared() << std::endl;
    if (problem.hasUnits) {
        std::cout << "varUnits: ";
        for (const auto& unit : problem.varUnits) std::cout << unit << " ";
        std::cout << std::endl;
        std::cout << "outUnit: " << problem.outUnit << std::endl;
    }
}

// Função para imprimir os cromossomos codificados
inline void printCodChromosome(const Vec<chromosome>& individual) {
    for (const auto& gene : individual) {
        printf("%d   ", gene);
    }
    printf("\n");
}

// Função para imprimir os cromossomos decodificados
inline void printDecodChromosome(const Vec<chromosome>& individual, const RProblem& problem, const Scenario& other) {
    // Contadores do problema
    int nVars = problem.nVars;
    int nConst = problem.nConst;

    // Cenário
    size_t stackLen = calculateStackLen(individual, other.maxConst);
    if (stackLen == 0) {
        std::cerr << "Erro: stackLen calculado inválido." << std::endl;
        return;
    }
    size_t individualLen = 3 * stackLen + static_cast<size_t>(other.maxConst) + 1;

    int decodValue;
    for (size_t i = 0; i < stackLen; i++) {
        if (i >= individual.size()) {
            std::cerr << "Aviso: Índice i=" << i << " excede o tamanho de 'individual'=" << individual.size() << std::endl;
            break;
        }
        decodValue = floor((individual[i] / 10000.0) * 4) - 1;
        printf("%d   ", decodValue);
    }
    for (size_t i = stackLen; i < (2 * stackLen); i++) {
        if (i >= individual.size()) {
            std::cerr << "Aviso: Índice i=" << i << " excede o tamanho de 'individual'=" << individual.size() << std::endl;
            break;
        }
        decodValue = floor((individual[i] / 10000.0) * (nVars + nConst)) - nConst;
        printf("%d   ", decodValue);
    }
    for (size_t i = (2 * stackLen); i < (3 * stackLen); i++) {
        if (i >= individual.size()) {
            std::cerr << "Aviso: Índice i=" << i << " excede o tamanho de 'individual'=" << individual.size() << std::endl;
            break;
        }
        if (individual[i - 2 * stackLen] < 2500) {
            decodValue = floor((individual[i] / 10000.0) * other.operationsBi.size());
            printf("Bi:");
        } else if ((individual[i - 2 * stackLen] >= 2500) && (individual[i - 2 * stackLen] < 5000)) {
            decodValue = floor((individual[i] / 10000.0) * other.operationsU.size());
            printf("U:");
        } else {
            decodValue = individual[i];
        }
        printf("%d   ", decodValue);
    }
    printf("\n");
}

// Função para imprimir a solução final
inline void printSolution(const RProblem& problem, const Vec<chromosome>& individual, const Scenario& other) {
    // Calcular stackLen dinamicamente
    size_t stackLen = calculateStackLen(individual, other.maxConst);
    if (stackLen == 0) {
        std::cerr << "Erro: stackLen calculado inválido." << std::endl;
        return;
    }

    size_t individualLen = 3 * stackLen + static_cast<size_t>(other.maxConst) + 1;

    int decodValue;
    size_t cont = 0;
    double rangeConst = 0.0;
    std::vector<bool> empty(stackLen, false);
    std::cout << "stackLen: " << stackLen << " => ";

    for (size_t j = 0; j < stackLen; j++) {
        if (j >= individual.size()) {
            std::cerr << "Aviso: Índice j=" << j << " excede o tamanho de 'individual'=" << individual.size() << std::endl;
            break; // Sair do loop se j exceder
        }

        if (individual[j] > (10000.0 / 4) * 3) empty[j] = true;
        decodValue = floor((individual[j] / 10000.0) * 4) - 1;

        if (decodValue == -1) {
            if ((j + 2 * stackLen) >= individual.size()) {
                std::cerr << "Aviso: Acessando individual[" << (j + 2 * stackLen) << "] fora dos limites." << std::endl;
                break;
            }
            decodValue = floor((individual[j + 2 * stackLen] / 10000.0) * other.operationsBi.size());
            if (decodValue < 0 || decodValue >= static_cast<int>(other.operationsBi.size())) {
                std::cerr << "Erro: decodValue (" << decodValue << ") inválido para operação binária." << std::endl;
                continue;
            }
            printf("%c ", other.operationsBi[decodValue]);
        } else if (decodValue == 0) {
            if ((j + 2 * stackLen) >= individual.size()) {
                std::cerr << "Aviso: Acessando individual[" << (j + 2 * stackLen) << "] fora dos limites." << std::endl;
                break;
            }
            decodValue = floor((individual[j + 2 * stackLen] / 10000.0) * other.operationsU.size());
            if (decodValue < 0 || decodValue >= static_cast<int>(other.operationsU.size())) {
                std::cerr << "Erro: decodValue (" << decodValue << ") inválido para operação unária." << std::endl;
                continue;
            }
            printf("%c ", other.operationsU[decodValue]);
        } else if (decodValue == 1) {
            if ((j + stackLen) >= individual.size()) {
                std::cerr << "Aviso: Acessando individual[" << (j + stackLen) << "] fora dos limites." << std::endl;
                break;
            }
            decodValue = floor((individual[j + stackLen] / 10000.0) * (problem.nVars + problem.nConst)) - problem.nConst;
            if (decodValue < 0) {
                decodValue += problem.nConst;
                if (problem.vConst[decodValue].second == problem.vConst[decodValue].first) {
                    printf("%.2f  ", problem.vConst[decodValue].first);
                } else {
                    if ((3 * stackLen + cont) >= individual.size()) {
                        std::cerr << "Aviso: Acessando individual[" << (3 * stackLen + cont) << "] fora dos limites." << std::endl;
                        break;
                    }
                    int seedConst = individual[3 * stackLen + cont];
                    std::mt19937 gen(seedConst);
                    std::uniform_int_distribution<> dis(static_cast<int>(problem.vConst[decodValue].first), static_cast<int>(problem.vConst[decodValue].second));
                    rangeConst = dis(gen);
                    cont++;
                    printf("%.2f  ", rangeConst);
                }
            } else {
                printf("V%d  ", decodValue);
            }
        }
    }
    printf("empty: ");
    for (size_t i = 0; i < stackLen; i++) {
        if (i >= empty.size()) {
            std::cerr << "Aviso: Índice i=" << i << " excede o tamanho de 'empty'=" << empty.size() << std::endl;
            break;
        }
        printf("%d ", static_cast<int>(empty[i]));
    }
    printf("\n");
}

// Função para imprimir os custos da população
inline void printPopulationCost(const Vec<ValuedChromosome>& mainPopulation, int populationLen) {
    for (int i = 0; i < populationLen; i++) {
        printf("%.4lf  ", mainPopulation[i].cost);
    }
    printf("\n");
}
