#pragma once

// C
#include <math.h>
#include <stdio.h>
#include <string.h>
// C++
#include <utility>
//
#include <brkgp/Utils.hpp>

void printFile(const RProblem& problem) {
  int nVars = problem.nVars;
  int tests = problem.tests;
  int nConst = problem.nConst;
  const Vec<Vec<double>>& inputs = problem.inputs;
  const Vec<double>& outputs = problem.outputs;
  const Vec<Pair<double, double>>& vConst = problem.vConst;

  printf("%d %d %d\n", nVars, tests, nConst);
  for (int i = 0; i < tests; i++) {
    for (int j = 0; j < nVars; j++) printf("%.2f\t", inputs[i][j]);
    printf("%.2f\n", outputs[i]);
  }
  for (int i = 0; i < nConst; i++) {
    printf("%.4f\t%.4f\n", vConst[i].first, vConst[i].second);
  }
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
  int nVars = problem.nVars;
  int nConst = problem.nConst;
  // scenario
  int stackLen = other.stackLen;
  int operationsBiLen = other.operationsBi.size();
  int operationsULen = other.operationsU.size();
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
      decodValue = floor((individual[i] / 10000.0) * operationsBiLen);
      printf("Bi:");
    } else if ((individual[i - 2 * stackLen] >= 2500) &&
               (individual[i - 2 * stackLen] < 5000)) {
      decodValue = floor((individual[i] / 10000.0) * operationsULen);
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
  const Vec<Pair<double, double>>& vConst = problem.vConst;
  int nVars = problem.nVars;
  int nConst = problem.nConst;
  // scenario
  int stackLen = other.stackLen;
  auto& operationsBi = other.operationsBi;
  auto& operationsU = other.operationsU;
  int operationsBiLen = other.operationsBi.size();
  int operationsULen = other.operationsU.size();
  //
  int decodValue;
  int cont = 0;
  double rangeConst = 0;
  for (int i = 0; i < stackLen; i++) {
    decodValue = floor((individual[i] / 10000.0) * 4) - 1;
    // printf("")
    if (decodValue == -1) {
      decodValue =
          floor((individual[i + 2 * stackLen] / 10000.0) * operationsBiLen);
      printf("%c ", operationsBi[decodValue]);
    } else if (decodValue == 0) {
      decodValue =
          floor((individual[i + 2 * stackLen] / 10000.0) * operationsULen);
      printf("%c ", operationsU[decodValue]);
    } else if (decodValue == 1) {
      decodValue =
          floor((individual[i + stackLen] / 10000.0) * (nVars + nConst)) -
          nConst;
      if (decodValue < 0) {
        decodValue += nConst;
        if (vConst[decodValue].second == vConst[decodValue].first) {
          printf("%f  ", vConst[decodValue].first);
        } else {
          srand(individual[3 * stackLen + cont]);
          rangeConst = rand() % (int)(vConst[decodValue].second -
                                      vConst[decodValue].first + 1) +
                       vConst[decodValue].first;
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

void printPopulationCost(const Vec<ValuedChromosome>& mainPopulation,
                         int populationLen) {
  for (int i = 0; i < populationLen; i++) {
    printf("%lf  ", mainPopulation[i].cost);
  }
  printf("\n");
}
