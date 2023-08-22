// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

// C
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// C++
#include <stack>
#include <vector>
//
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <kahan-float/src/kahan.hpp>

using namespace kahan;

double execBinaryOp(int idop, double v1, double v2,
                    const std::vector<char>& operationsBi) {
  if (operationsBi[idop] == '+') {
    if (abs(v1) == INFINITY) return INFINITY;
    return v1 + v2;
  }
  if (operationsBi[idop] == '-') {
    if (abs(v1) == INFINITY) return INFINITY;
    return v1 - v2;
  }
  if (operationsBi[idop] == '*') {
    if ((abs(v1) == INFINITY && v2 == 0) || (v1 == 0 && abs(v2) == INFINITY))
      return INFINITY;
    return v1 * v2;
  }
  if (operationsBi[idop] == '/') {
    if (v2 < 0.000001) return INFINITY;
    if (abs(v2) == INFINITY) return 0;
    return v1 / v2;
  }
  return 0.0;
}

double execUnaryOp(int idop, double v1, const std::vector<char>& operationsU) {
  if (operationsU[idop] == 's') {
    if (v1 == INFINITY) return INFINITY;
    return sin(v1);
  }

  if (operationsU[idop] == 'c') {
    if (v1 == INFINITY) return INFINITY;
    return cos(v1);
  }
  if (operationsU[idop] == 'i') return v1;
  if (operationsU[idop] == 'a') return v1 * v1;
  if (operationsU[idop] == 'v') return v1 * v1 * v1;
  if (operationsU[idop] == 'r') {
    if (v1 < 0)
      return INFINITY;
    else
      return pow(v1, 1 / 2);
  }
  return 0.0;
}

// error between values v1 and v2 RMSE
double computeError(double v1, double v2) {
  // square
  return sqrt((v1 - v2) * (v1 - v2));
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
//
int stackAdjustment(Vec<chromosome>& individual, int stackLen, int nVars,
                    int nConst, int maxConst, int seed) {
  // 5303   358   3064   9156   4199   1320   636   7306   2445   6166   3572
  // 8249   4290   4389   4790   567   9692   202   3913   498 1      -1      0
  // 2     0      -1     -1     1     -1     1       0     2       0     0 0 -1
  // 2      -1     0     -1 1       1       0     2     0       -1     1     1
  // -1     1       0     2       0     0      0     -1     2      -1     0 1
  int somador = 0;
  int auxSomador = 0;
  int decodValue;
  int contConst = 0;
  int idConst = 0;
  int trueStackLen = 0;
  for (int i = 0; i < stackLen; i++) {
    // TODO(igormcoelho): verificar esse número 4!!!
    decodValue = floor((individual[i] / 10000.0) * 4) - 1;
    idConst =
        floor((individual[stackLen + i] / 10000.0) * (nVars + nConst)) - nConst;
    // TODO(igormcoelho): verificar esse número 2!!!
    if (decodValue != 2) {
      auxSomador += decodValue;
    }
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
    if ((decodValue == 1) && idConst < 0) {
      contConst++;
      if (contConst > maxConst) {
        srand(seed);
        // std::cout << "WARNING! ALTERANDO individual COD7!" << std::endl;
        individual[stackLen + i] =
            ((10000.0 / (nVars + nConst)) * nConst) +
            ((10000.0 / (nVars + nConst)) * (rand() % nVars) + 1);
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
                         const Scenario& other, int training) {
  // problem counters
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  // scenario
  const int stackLen = other.stackLen;
  // local variables
  kfloat64 sum_error = 0;
  //
  const int realTests = problem.tests - training;
  for (int i = 0; i < realTests; i++) {
    std::stack<double> stk;
    int contSeed = 0;

    for (int j = 0; j < stackLen; j++) {
      // cout << "begin var i=" << i << " / sol_size=" << rep.vstack.size() <<
      // endl;
      //  case: push
      //  -1 0 1 2
      //  2 3
      // push variable or constant
      if ((individual[j] < 7500) && (individual[j] >= 5000)) {
        int idVar =
            floor((individual[stackLen + j] / 10000.0) * (nVars + nConst)) -
            nConst;
        // printf("%d\n",idVar);
        double varValue = 0;
        // push variable
        if (idVar >= 0) {
          varValue = problem.inputs[i][idVar];
          stk.push(varValue);
        } else {
          // push constant
          idVar += nConst;
          if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
            varValue = problem.vConst[idVar].first;
            stk.push(varValue);
          } else {
            chromosome seedConst = individual[3 * stackLen + contSeed];
            srand(seedConst);
            varValue = rand() % (int)(problem.vConst[idVar].second -
                                      problem.vConst[idVar].first + 1) +
                       problem.vConst[idVar].first;
            stk.push(varValue);
            contSeed++;
          }
        }
      }

      // case: pop Binary Operation
      if (individual[j] < 2500) {
        // pop operation
        int idOpBi =
            floor((individual[2 * stackLen + j] / 10000.0) *
                  other.operationsBi.size());  // 4 is lenght of operationBi
        // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
        //
        double v1 = stk.top();
        stk.pop();
        double v2 = stk.top();
        stk.pop();
        double binaryProduct = execBinaryOp(idOpBi, v1, v2, other.operationsBi);
        // if for case: division by zero
        if (abs(binaryProduct) == INFINITY) {
          while (stk.size() > 0) stk.pop();
          stk.push(INFINITY);
          j = stackLen;
        } else {
          stk.push(binaryProduct);
        }
      }

      // pop: Unary Operation
      if ((individual[j] < 5000) && (individual[j] >= 2500)) {
        int idOpU =
            floor((individual[2 * stackLen + j] / 10000.0) *
                  other.operationsU.size());  // 3 is lenght of operationU
        // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
        //
        double v1 = stk.top();
        stk.pop();
        stk.push(execUnaryOp(idOpU, v1, other.operationsU));
      }

      if (individual[j] >= 7500) {
        //
        // cout << "i=" << i << " -> stack size = " << stk.size() << endl;
      }

      //
      // cout << "finished t= " << t << endl;
      // take value from stack
      // assert(stk.size() == 1);
    }

    double val = stk.top();
    stk.pop();
    //
    // compare with expected value

    sum_error += computeError(val, problem.outputs[i]);
    // printf("sum_error = %lf\n",sum_error);
  }
  // average erro
  double solutionValue = (double)sum_error / realTests;
  // printf("solutionValue = %lf\n",solutionValue);
  // printf("sum_error = %f solutionValue = %f\n",sum_error,solutionValue);
  // printf("%f\n",solutionValue);
  return solutionValue;
}
