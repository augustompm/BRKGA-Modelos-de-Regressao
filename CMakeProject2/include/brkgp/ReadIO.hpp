// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//
#include <iostream>
#include <utility>
#include <vector>
//
#include <Scanner/Scanner.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <brkgp/ReadIO.hpp>
#include <brkgp/Utils.hpp>

using namespace scannerpp;  // NOLINT

void readIO(RProblem& problem, Scanner& scanner) {
  Vec<Vec<double>>& inputs = problem.inputs;
  Vec<double>& outputs = problem.outputs;
  int& nVars = problem.nVars;
  int& tests = problem.tests;
  int& nConst = problem.nConst;

  std::cout << "1) READING PROBLEM" << std::endl;

  nVars = scanner.nextInt();
  tests = scanner.nextInt();
  nConst = scanner.nextInt();

  std::cout << "2) READING PROBLEM TESTS" << std::endl;

  // for each test...
  inputs = Vec<Vec<double>>(tests);
  outputs = Vec<double>(tests);
  problem.vConst.resize(nConst);

  for (int t = 0; t < tests; t++) {
    inputs[t] = Vec<double>(nVars);
    for (unsigned i = 0; i < nVars; i++) inputs[t][i] = scanner.nextDouble();
    outputs[t] = scanner.nextDouble();
  }

  std::cout << "3) READING PROBLEM CONSTANTS" << std::endl;

  for (int i = 0; i < nConst; i++) {
    problem.vConst[i].first = scanner.nextDouble();
    problem.vConst[i].second = scanner.nextDouble();
  }

  std::cout << "4) FINISHED READING PROBLEM" << std::endl;
}

void changeIO(Vec<Vec<double>>& inputs, Vec<double>& outputs, int training,
              int nVars, int tests, int nConst) {
  //
  Vec<Vec<double>> auxInputs(tests);
  Vec<double> auxOutputs(tests);

  for (int t = 0; t < (tests); t++) {
    auxInputs[t] = Vec<double>(nVars);
    auxInputs[t] = inputs[t];
  }
  auxOutputs = outputs;
  //
  for (int i = 0; i < tests; i++) {
    if (i < training) {
      inputs[i] = auxInputs[i + (tests - training)];
      outputs[i] = auxOutputs[i + (tests - training)];
    } else {
      inputs[i] = auxInputs[i - training];
      outputs[i] = auxOutputs[i - training];
    }
  }
}
