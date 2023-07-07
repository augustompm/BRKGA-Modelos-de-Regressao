#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
#include <iostream>
#include <utility>
#include <vector>
//
// #include <Scanner/File.h>
#include <Scanner/Scanner.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <brkgp/ReadIO.hpp>
#include <brkgp/Utils.hpp>

using namespace scannerpp;  // NOLINT

void readIO(double*** inputs, double** outputs, Scanner& scanner, int* nVars,
            int* tests, int* nConst, Vec<Pair<double, double>>& vConst);

void changeIO(double*** inputs, double** outputs, int training, int nVars,
              int tests, int nConst);

// using namespace std;
// using namespace scannerpp;
// typedef unsigned short chromosome;

void readIO(double*** inputs, double** outputs, Scanner& scanner, int& nVars,
            int& tests, int& nConst, vector<pair<double, double>>& vConst) {
  nVars = scanner.nextInt();
  tests = scanner.nextInt();
  nConst = scanner.nextInt();

  // for each test...
  (*inputs) = (double**)malloc(tests * sizeof(double*));
  (*outputs) = (double*)malloc(tests * sizeof(double));
  vConst.resize(nConst);

  for (int t = 0; t < tests; t++) {
    (*inputs)[t] = (double*)malloc(nVars * sizeof(double));
    for (unsigned i = 0; i < nVars; i++) (*inputs)[t][i] = scanner.nextDouble();
    (*outputs)[t] = scanner.nextDouble();
  }
  for (int i = 0; i < nConst; i++) {
    vConst[i].first = scanner.nextDouble();
    vConst[i].second = scanner.nextDouble();
  }
}

void changeIO(double*** inputs, double** outputs, int training, int nVars,
              int tests, int nConst) {
  double** auxInputs;
  double* auxOutputs;

  auxInputs = (double**)malloc((tests) * sizeof(double*));
  auxOutputs = (double*)malloc((tests) * sizeof(double));

  for (int t = 0; t < (tests); t++) {
    auxInputs[t] = (double*)malloc((nVars) * sizeof(double));
    memcpy(auxInputs[t], (*inputs)[t], sizeof(double) * nVars);
  }
  memcpy(auxOutputs, (*outputs), sizeof(double) * tests);
  for (int i = 0; i < tests; i++) {
    if (i < training) {
      memcpy((*inputs)[i], auxInputs[i + (tests - training)],
             sizeof(double) * nVars);
      (*outputs)[i] = auxOutputs[i + (tests - training)];
    } else {
      memcpy((*inputs)[i], auxInputs[i - training], sizeof(double) * nVars);
      (*outputs)[i] = auxOutputs[i - training];
    }
  }
  for (int t = 0; t < (tests); t++) {
    free(auxInputs[t]);
  }
  free(auxInputs);
  free(auxOutputs);
}