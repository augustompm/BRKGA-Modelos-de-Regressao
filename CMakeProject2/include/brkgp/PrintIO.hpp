// SPDX-License-Identifier: MIT
// Copyright (C) 2023

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

  printf("%d %d %d\n", nVars, tests, nConst);
  for (int i = 0; i < tests; i++) {
    for (int j = 0; j < nVars; j++) printf("%.2f\t", inputs[i][j]);
    printf("%.2f\n", outputs[i]);
  }
  for (int i = 0; i < nConst; i++) {
    printf("%.4f\t%.4f\n", problem.vConst[i].first, problem.vConst[i].second);
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
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  // scenario
  const int stackLen = other.stackLen;
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
  const int stackLen = other.stackLen;
  //
  int decodValue;
  int cont = 0;
  double rangeConst = 0;
  for (int i = 0; i < stackLen; i++) {
    decodValue = floor((individual[i] / 10000.0) * 4) - 1;
    // printf("")
    if (decodValue == -1) {
      decodValue = floor((individual[i + 2 * stackLen] / 10000.0) *
                         other.operationsBi.size());
      printf("%c ", other.operationsBi[decodValue]);
    } else if (decodValue == 0) {
      decodValue = floor((individual[i + 2 * stackLen] / 10000.0) *
                         other.operationsU.size());
      printf("%c ", other.operationsU[decodValue]);
    } else if (decodValue == 1) {
      decodValue =
          floor((individual[i + stackLen] / 10000.0) * (nVars + nConst)) -
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

void printPopulationCost(const Vec<ValuedChromosome>& mainPopulation,
                         int populationLen) {
  for (int i = 0; i < populationLen; i++) {
    printf("%lf  ", mainPopulation[i].cost);
  }
  printf("\n");
}
