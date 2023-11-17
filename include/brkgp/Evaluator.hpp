// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

// C
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// C++
#include <optional>
#include <stack>
#include <vector>
//
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <kahan-float/kahan.hpp>

template <class T>
using opt = std::optional<T>;

enum class OpType { BIN, UN, PUSH, SPECIAL, SP_NOP };

bool isOperation(int rk, OpType op) {
  // BIN:  [0, 2500)
  if ((op == OpType::BIN) && (rk < 2500) && (rk >= 0)) return true;
  // UN:  [2500, 5000)
  if ((op == OpType::UN) && (rk < 5000) && (rk >= 2500)) return true;
  // PUSH:  [5000, 7500)
  if ((op == OpType::PUSH) && (rk < 7500) && (rk >= 5000)) return true;
  // SPECIAL:  [7500, 10000)
  if ((op == OpType::SPECIAL) && (rk < 10000) && (rk >= 7500)) return true;
  return false;
}

opt<double> execBinaryOp(int idop, double v1, double v2,
                         const std::vector<char>& operationsBi) {
  if (operationsBi[idop] == '+') return std::make_optional<double>(v1 + v2);
  if (operationsBi[idop] == '-') return std::make_optional<double>(v1 - v2);
  if (operationsBi[idop] == '*') return std::make_optional<double>(v1 * v2);
  if (operationsBi[idop] == '/') {
    if (v2 < 0.000001)
      return std::nullopt;
    else
      return std::make_optional<double>(v1 / v2);
  }
  return std::nullopt;
}

opt<double> execUnaryOp(int idop, double v1,
                        const std::vector<char>& operationsU) {
  if (operationsU[idop] == 's') return std::make_optional<double>(::sin(v1));
  if (operationsU[idop] == 'c') return std::make_optional<double>(cos(v1));
  if (operationsU[idop] == 'i') return std::make_optional<double>(v1);
  if (operationsU[idop] == 'a') return std::make_optional<double>(v1 * v1);
  if (operationsU[idop] == 'v') return std::make_optional<double>(v1 * v1 * v1);
  if (operationsU[idop] == 'r') {
    if (v1 < 0)
      return std::nullopt;
    else
      // return std::make_optional<double>(::pow(v1, 1.0 / 2.0));
      return std::make_optional<double>(::sqrt(v1));
  }
  return std::nullopt;
}

// error between values v1 and v2 RMSE
double computeError(double v1, double v2) {
  // square
  return ::sqrt((v1 - v2) * (v1 - v2));
}

// stackAdjustment: ajusta chaves aleatórias do indivíduo, caso seja
// impossível decodificá-lo! Por exemplo, operações binárias seguidas
// sem operandos disponíveis na pilha!
// IDEIA: usar realStackValue para representar o fim do indivíduo, ao
// invés de ajustá-lo!
// EXEMPLO: fazer vetor simples para x² (só precisa de stackLen=2)!
// IDEIA 2: QUANDO PARAR DECODIFICAÇÃO?
//  - Hipótese: quando stack size = 1?
//  - TALVEZ... Marcador de chave aleatória que só é ativado se stack size = 1.
// IDEIA 3: INVOCAR 'mathomatic' para simplificar equações... Fazer só no Best?
// IDEIA 4: USAR UNIDADE SIMBOLICA COMO CRITERIO DE SIMPLICIDADE AO LONGO DA
// EXECUCAO...
//
int stackAdjustment(Vec<chromosome>& individual, int stackLen, int nVars,
                    int nConst, int maxConst, int seed) {
  int somador = 0;
  int auxSomador = 0;
  int contConst = 0;
  int idConst = 0;
  int trueStackLen = 0;
  for (int i = 0; i < stackLen; i++) {
    int stackDiff = 2;  // DEFAULT = 2 (why?)
    // Binary: remove two elements and add one
    if (isOperation(individual[i], OpType::BIN)) stackDiff = -1;
    // Unary: remove one element and add one
    if (isOperation(individual[i], OpType::UN)) stackDiff = 0;
    // Push: add one
    if (isOperation(individual[i], OpType::PUSH)) stackDiff = 1;
    //
    int my_floor2 =
        ::floor((individual[stackLen + i] / 10000.0) * (nVars + nConst));
    idConst = my_floor2 - nConst;
    // TODO(igormcoelho): verificar esse número 2!!!
    // [-1,0,1] ?
    if (stackDiff != 2) {
      auxSomador += stackDiff;
    }
    // se == 2, faz nada???
    // ???
    if (auxSomador < 1) {
      // ALTERANDO OPERAÇÕES!
      if (individual[i] < 2500) {
        // std::cout << "WARNING! ALTERANDO individual COD0!" << std::endl;
        individual[i] += 5000;
      } else if (individual[i] >= 7500) {  // se der testar depois, no caso i=0
        // std::cout << "WARNING! ALTERANDO individual COD1!" << std::endl;
        individual[i] -= 2500;
      } else {  // se der testar depois, no caso i=0
        // std::cout << "WARNING! ALTERANDO individual COD2!" << std::endl;
        individual[i] += 2500;
      }
      auxSomador = somador + 1;
    }
    if (auxSomador > (stackLen - i)) {
      if (somador == 1) {
        // std::cout << "WARNING! ALTERANDO individual COD3!" << std::endl;
        individual[i] += 2500;
        auxSomador = 1;
      } else if (somador > 1) {
        if ((individual[i] >= 2500) && (individual[i] < 5000)) {
          // std::cout << "WARNING! ALTERANDO individual COD4!" << std::endl;
          individual[i] -= 2500;
        } else if (individual[i] >=
                   7500) {  // se der testar depois, no caso i=0
          // std::cout << "WARNING! ALTERANDO individual COD5!" << std::endl;
          individual[i] -= 7500;
        } else {  // se der testar depois, no caso i=0
          // std::cout << "WARNING! ALTERANDO individual COD6!" << std::endl;
          individual[i] -= 5000;
        }
        auxSomador = somador - 1;
      }
    }
    if ((stackDiff == 1) && idConst < 0) {
      contConst++;
      if (contConst > maxConst) {
        srand(seed);
        // std::cout << "WARNING! ALTERANDO individual COD7!" << std::endl;
        individual[stackLen + i] =
            ((10000.0 / (nVars + nConst)) * nConst) +
            ((10000.0 / (nVars + nConst)) * (rand() % nVars) + 1);  // NOLINT
        seed++;
      }
    }
    somador = auxSomador;
    if (individual[i] < 7500) trueStackLen++;
  }
  return trueStackLen;
}

double solutionEvaluator(const RProblem& problem,
                         const Vec<chromosome>& individual,
                         const Scenario& other, int training, int idSol) {
  // idSol: Identifier of Solution in Population (Good for DEBUG!)
  // problem counters
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  // scenario
  const int stackLen = other.stackLen;
  // local variables
  kahan::kfloat64 sum_error = 0;
  if (idSol == 0) {
    std::cout << "DEBUG[idSol=0] BEGIN sum_error:" << sum_error << std::endl;
  }
  //
  const int realTests = problem.tests - training;
  for (int t = 0; t < realTests; t++) {
    std::stack<double> stk;
    int contSeed = 0;

    for (int j = 0; j < stackLen; j++) {
      //
      if (isOperation(individual[j], OpType::BIN)) {
        // case: pop Binary Operation [0, 2500)
        // pop operation
        int idOpBi = floor(
            (individual[2 * stackLen + j] / 10000.0) *
            (double)other.operationsBi.size());  // 4 is lenght of operationBi
        // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
        //
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] idOpBi=" << idOpBi << std::endl;
        }
        double v1 = stk.top();
        stk.pop();
        double v2 = stk.top();
        stk.pop();
        // RETORNAR opcional VAZIO!
        opt<double> binResult =
            execBinaryOp(idOpBi, v1, v2, other.operationsBi);
        // error in binary operation
        if (!binResult) {
          // LIMPA PILHA E ABORTA TESTE!
          while (stk.size() > 0) stk.pop();
          stk.push(INFINITY);
          break;
        } else {
          if (idSol == 0 && t == 0)
            std::cout << "DEBUG[idSol=0] bin result: " << *binResult
                      << std::endl;
          stk.push(*binResult);
        }
      } else if (isOperation(individual[j], OpType::UN)) {
        // pop: Unary Operation [2500, 5000)
        int idOpU = floor(
            (individual[2 * stackLen + j] / 10000.0) *
            (double)other.operationsU.size());  // 3 is lenght of operationU
        // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
        //
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] idOpU=" << idOpU << std::endl;
        }
        double v1 = stk.top();
        stk.pop();

        opt<double> unResult = execUnaryOp(idOpU, v1, other.operationsU);

        // error in unary operation
        if (!unResult) {
          // LIMPA PILHA E ABORTA TESTE!
          while (stk.size() > 0) stk.pop();
          stk.push(INFINITY);
          break;
        } else {
          if (idSol == 0 && t == 0)
            std::cout << "DEBUG[idSol=0] un result: " << *unResult << std::endl;

          stk.push(*unResult);
        }
      } else if (isOperation(individual[j], OpType::PUSH)) {
        // push variable or constant [5000,7500)
        int my_floor =
            ::floor((individual[stackLen + j] / 10000.0) * (nVars + nConst));
        int idVar = my_floor - nConst;
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] idVar=" << idVar << std::endl;
        }
        // printf("%d\n",idVar);
        double varValue = 0;
        // push variable
        if (idVar >= 0) {
          varValue = problem.inputs[t][idVar];
          stk.push(varValue);
          if (idSol == 0 && t == 0) {
            std::cout << "DEBUG[idSol=0] pushvar: " << varValue << std::endl;
          }
        } else {
          // push constant
          idVar += nConst;
          if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
            varValue = problem.vConst[idVar].first;
            stk.push(varValue);
          } else {
            chromosome seedConst = individual[3 * stackLen + contSeed];
            srand(seedConst);
            // NOLINTNEXTLINE
            varValue = rand() % (int)(problem.vConst[idVar].second -
                                      problem.vConst[idVar].first + 1) +
                       problem.vConst[idVar].first;
            stk.push(varValue);
            contSeed++;
          }
        }
      } else if (isOperation(individual[j], OpType::SPECIAL)) {
        // special (currently: NOP): [7500, 10000)
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] NOP" << std::endl;
        }
        //
        // cout << "i=" << i << " -> stack size = " << stk.size() << endl;
      } else {
        std::cout << "ERROR! UNKNOWN OPERATION " << individual[j] << std::endl;
        return INFINITY;
      }

      //
      // cout << "finished t= " << t << endl;
      // take value from stack
      // assert(stk.size() == 1);
      //
    }  // for j < stackLen

    double val = stk.top();
    stk.pop();
    //
    // compare with expected value

    if (idSol == 0 && t == 0) {
      std::cout << "DEBUG[idSol=0] val:" << val << std::endl;
      std::cout << "DEBUG[idSol=0] expected:" << problem.outputs[t]
                << std::endl;
      std::cout << "DEBUG[idSol=0] BEFORE sum_error:" << sum_error << std::endl;
    }

    sum_error += computeError(val, problem.outputs[t]);

    if (idSol == 0 && t == 0) {
      std::cout << "DEBUG[idSol=0] AFTER sum_error:" << sum_error << std::endl;
    }
    // printf("sum_error = %lf\n",sum_error);
  }
  // average erro
  double solutionValue = (double)sum_error / realTests;

  if (idSol == 0) {
    std::cout << "DEBUG[idSol=0] Final sum_error:" << sum_error << std::endl;
    std::cout << "DEBUG[idSol=0] Final solutionValue:" << solutionValue
              << std::endl;
  }
  // printf("solutionValue = %lf\n",solutionValue);
  // printf("sum_error = %f solutionValue = %f\n",sum_error,solutionValue);
  // printf("%f\n",solutionValue);
  return solutionValue;
}
