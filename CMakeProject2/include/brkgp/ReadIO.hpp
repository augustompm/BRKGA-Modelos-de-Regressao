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

// void readIO(double*** inputs, double** outputs, Scanner& scanner, int* nVars,
//             int* tests, int* nConst, Vec<Pair<double, double>>& vConst);

// void changeIO(double*** inputs, double** outputs, int training, int nVars,
//               int tests, int nConst);

// using namespace std;
// using namespace scannerpp;
// typedef unsigned short chromosome;

// void readIO(double*** inputs, double** outputs, Scanner& scanner, int& nVars,
// void readIO(Vec<Vec<double>>& inputs, Vec<double>& outputs, Scanner& scanner,
//            int& nVars, int& tests, int& nConst,
//            Vec<Pair<double, double>>& vConst) {

void readIO(RProblem& problem, Scanner& scanner) {
  Vec<Vec<double>>& inputs = problem.inputs;
  Vec<double>& outputs = problem.outputs;
  int& nVars = problem.nVars;
  int& tests = problem.tests;
  int& nConst = problem.nConst;
  Vec<Pair<double, double>>& vConst = problem.vConst;

  nVars = scanner.nextInt();
  tests = scanner.nextInt();
  nConst = scanner.nextInt();

  // for each test...
  // (*inputs) = (double**)malloc(tests * sizeof(double*));
  // (*outputs) = (double*)malloc(tests * sizeof(double));
  inputs = Vec<Vec<double>>(tests);
  outputs = Vec<double>(tests);
  vConst.resize(nConst);

  for (int t = 0; t < tests; t++) {
    // (*inputs)[t] = (double*)malloc(nVars * sizeof(double));
    inputs[t] = Vec<double>(nVars);
    for (unsigned i = 0; i < nVars; i++) inputs[t][i] = scanner.nextDouble();
    outputs[t] = scanner.nextDouble();
  }
  for (int i = 0; i < nConst; i++) {
    vConst[i].first = scanner.nextDouble();
    vConst[i].second = scanner.nextDouble();
  }
}

// void changeIO(double*** inputs, double** outputs, int training, int nVars,
//               int tests, int nConst) {
void changeIO(Vec<Vec<double>>& inputs, Vec<double>& outputs, int training,
              int nVars, int tests, int nConst) {
  //
  // double** auxInputs = (double**)malloc((tests) * sizeof(double*));
  // double* auxOutputs = (double*)malloc((tests) * sizeof(double));
  Vec<Vec<double>> auxInputs(tests);
  Vec<double> auxOutputs(tests);

  for (int t = 0; t < (tests); t++) {
    // auxInputs[t] = (double*)malloc((nVars) * sizeof(double));
    auxInputs[t] = Vec<double>(nVars);
    // memcpy(auxInputs[t], (*inputs)[t], sizeof(double) * nVars);
    auxInputs[t] = inputs[t];
  }
  // memcpy(auxOutputs, (*outputs), sizeof(double) * tests);
  auxOutputs = outputs;
  //
  for (int i = 0; i < tests; i++) {
    if (i < training) {
      // memcpy((*inputs)[i], auxInputs[i + (tests - training)],
      //        sizeof(double) * nVars);
      inputs[i] = auxInputs[i + (tests - training)];
      // (*outputs)[i] = auxOutputs[i + (tests - training)];
      outputs[i] = auxOutputs[i + (tests - training)];
    } else {
      // memcpy((*inputs)[i], auxInputs[i - training], sizeof(double) * nVars);
      inputs[i] = auxInputs[i - training];
      // (*outputs)[i] = auxOutputs[i - training];
      outputs[i] = auxOutputs[i - training];
    }
  }
  // for (int t = 0; t < (tests); t++) {
  //   free(auxInputs[t]);
  // }
  // free(auxInputs);
  // free(auxOutputs);
}
