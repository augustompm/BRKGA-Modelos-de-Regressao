// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

// C
#include <math.h>
#include <stdio.h>
#include <string.h>
// C++
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
//
#include <brkgp/Utils.hpp>

void printFile(const RProblem& problem) {
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
}

void printCodChromosome(const Vec<chromosome>& individual) {
  int len = individual.size();
  for (int i = 0; i < len; i++) {
    printf("%d   ", individual[i]);
  }
  printf("\n");
}

void printDecodChromosome(const Vec<chromosome>& individual,
                          const RProblem& problem, const Scenario& other) {
  // problem
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  // scenario
  const int stackLen = other.getStackLen();
  //
  int decodValue;
  for (int i = 0; i < stackLen; i++) {
    decodValue = floor((individual[i] / 10000.0) * 4) - 1;
    printf("%d   ", decodValue);
  }
  for (int i = stackLen; i < (2 * stackLen); i++) {
    decodValue = floor((individual[i] / 10000.0) * (nVars + nConst)) - nConst;
    printf("%d   ", decodValue);
  }
  for (int i = (2 * stackLen); i < (3 * stackLen); i++) {
    if (individual[i - 2 * stackLen] < 2500) {
      decodValue = floor((individual[i] / 10000.0) * other.operationsBi.size());
      printf("Bi:");
    } else if ((individual[i - 2 * stackLen] >= 2500) &&
               (individual[i - 2 * stackLen] < 5000)) {
      decodValue = floor((individual[i] / 10000.0) * other.operationsU.size());
      printf("U:");
    } else {
      decodValue = individual[i];
    }
    printf("%d   ", decodValue);
  }
  printf("\n");
}

void printSolution(const RProblem& problem, const Vec<chromosome>& individual,
                   const Scenario& other) {
  // problem
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  // scenario
  const int stackLen = other.getStackLen();
  //
  int decodValue;
  int cont = 0;
  double rangeConst = 0;
  std::cout << "stackLen: " << stackLen << " => ";
  for (int j = 0; j < stackLen; j++) {
    decodValue = floor((individual[j] / 10000.0) * 4) - 1;
    // printf("")
    if (decodValue == -1) {
      decodValue = floor((individual[j + 2 * stackLen] / 10000.0) *
                         other.operationsBi.size());
      printf("%c ", other.operationsBi[decodValue]);
    } else if (decodValue == 0) {
      decodValue = floor((individual[j + 2 * stackLen] / 10000.0) *
                         other.operationsU.size());
      printf("%c ", other.operationsU[decodValue]);
    } else if (decodValue == 1) {
      decodValue =
          floor((individual[j + stackLen] / 10000.0) * (nVars + nConst)) -
          nConst;
      if (decodValue < 0) {
        decodValue += nConst;
        if (problem.vConst[decodValue].second ==
            problem.vConst[decodValue].first) {
          printf("%f  ", problem.vConst[decodValue].first);
        } else {
          srand(individual[3 * stackLen + cont]);
          rangeConst = rand() % (int)(problem.vConst[decodValue].second -
                                      problem.vConst[decodValue].first + 1) +
                       problem.vConst[decodValue].first;
          cont++;
          printf("%f  ", rangeConst);
        }
      } else {
        printf("V%d  ", decodValue);
      }
    }
  }
  printf("\n");
}

// =================================================
//                  LaTeX printing
// =================================================

std::string stringExecBinaryOp(int idop, std::string v1, std::string v2,
                               const std::vector<char>& operationsBi) {
  if (operationsBi[idop] == '+') {
    std::stringstream ss;
    ss << "(" << v1 << " + " << v2 << ")";
    return ss.str();
  }
  if (operationsBi[idop] == '-') {
    std::stringstream ss;
    ss << "(" << v1 << " - " << v2 << ")";
    return ss.str();
  }
  if (operationsBi[idop] == '*') {
    std::stringstream ss;
    ss << "(" << v1 << " * " << v2 << ")";
    return ss.str();
  }
  if (operationsBi[idop] == '/') {
    std::stringstream ss;
#ifdef STRING_FRAC
    ss << "\\frac{" << v1 << "}{" << v2 << "}";
#else
    ss << "(" << v1 << "/" << v2 << ")";
#endif
    return ss.str();
  }
  // NO OPERATION BINARY
  return "NOP";
}

std::string stringExecUnaryOp(int idop, std::string v1,
                              const std::vector<char>& operationsU) {
  if (operationsU[idop] == 's') {
    std::stringstream ss;
    ss << "\\sin(" << v1 << ")";
    return ss.str();
  }

  if (operationsU[idop] == 'c') {
    std::stringstream ss;
    ss << "\\cos(" << v1 << ")";
    return ss.str();
  }
  if (operationsU[idop] == 'i') {
    std::stringstream ss;
    ss << "(" << v1 << ")";
    return ss.str();
  }
  if (operationsU[idop] == 'a') {
    std::stringstream ss;
    ss << "{" << v1 << "^2}";
    return ss.str();
  }
  if (operationsU[idop] == 'v') {
    std::stringstream ss;
    ss << "{" << v1 << "^3}";
    return ss.str();
  }
  if (operationsU[idop] == 'r') {
    std::stringstream ss;
    ss << "\\sqrt{" << v1 << "}";
    return ss.str();
  }
  // NOP UNARY
  return "NOPU";
}

std::string printSolution2(const RProblem& problem,
                           const Vec<chromosome>& individual,
                           const Scenario& other) {
  // problem counters
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  // scenario
  const int stackLen = other.getStackLen();

  std::stack<std::string> stk;
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
        // VARIABLE NAME!
        std::stringstream ss;
        ss << "v" << idVar;
        stk.push(ss.str());
      } else {
        // push constant
        idVar += nConst;
        if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
          varValue = problem.vConst[idVar].first;
          std::stringstream ss;
          ss << varValue;
          stk.push(ss.str());
        } else {
          chromosome seedConst = individual[3 * stackLen + contSeed];
          srand(seedConst);
          varValue = rand() % (int)(problem.vConst[idVar].second -
                                    problem.vConst[idVar].first + 1) +
                     problem.vConst[idVar].first;
          std::stringstream ss;
          ss << varValue;
          stk.push(ss.str());
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
      std::string v1 = stk.top();
      stk.pop();
      std::string v2 = stk.top();
      stk.pop();
      std::string binaryProduct =
          stringExecBinaryOp(idOpBi, v1, v2, other.operationsBi);
      stk.push(binaryProduct);
    }

    // pop: Unary Operation
    if ((individual[j] < 5000) && (individual[j] >= 2500)) {
      int idOpU = floor((individual[2 * stackLen + j] / 10000.0) *
                        other.operationsU.size());  // 3 is lenght of operationU
      // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
      //
      std::string v1 = stk.top();
      stk.pop();
      stk.push(stringExecUnaryOp(idOpU, v1, other.operationsU));
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

  std::string val = stk.top();
  stk.pop();
  //
  // compare with expected value

  return val;
}

void printPopulationCost(const Vec<ValuedChromosome>& mainPopulation,
                         int populationLen) {
  for (int i = 0; i < populationLen; i++) {
    printf("%lf  ", mainPopulation[i].cost);
  }
  printf("\n");
}
