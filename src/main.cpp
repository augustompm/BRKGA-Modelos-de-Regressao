// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
//
#include <ginac.h>
#include <gmp.h>
using namespace GiNaC;

class my_visitor : public visitor,           // this is required
                   public add::visitor,      // visit add objects
                   public numeric::visitor,  // visit numeric objects
                   public power::visitor,
                   public basic::visitor  // visit basic objects
{
  void visit(const add& x) override {
    std::cout << "called with an add object" << std::endl;
    for (size_t i = 0; i < x.nops(); i++) {
      x.op(i).accept(*this);
    }
  }

  void visit(const numeric& x) override {
    std::cout << "called with a numeric object" << std::endl;
  }

  void visit(const power& x) override {
    std::cout << "called with a power object" << std::endl;
    const ex& base = x.op(0);
    const ex& exponent = x.op(1);

    if (is_a<numeric>(exponent)) {
      numeric n = ex_to<numeric>(exponent);
      std::cout << "Exponent is numeric: " << n << std::endl;
      if (n == 2) std::cout << "TWO!" << std::endl;
    }

    base.accept(*this);      // Visit the base expression
    exponent.accept(*this);  // Visit the exponent expression
  }

  void visit(const basic& x) {
    std::cout << "called with a basic object" << std::endl;

    if (is_a<symbol>(x)) {
      std::cout << "variable name: " << ex_to<symbol>(x) << std::endl;
    }
  }
};

//

#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>
//
#include <Scanner/Scanner.hpp>
#include <brkgp/BRKGA.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <brkgp/ReadIO.hpp>
#include <brkgp/Utils.hpp>

int main(int argc, char* argv[]) {
  // BRKGA
  auto start = std::chrono::high_resolution_clock::now();

  if (argc < 3) {
    std::cerr << "Uso: " << argv[0] << " <seed> <instance_file>" << std::endl;
    return 1;
  }

  int seed = atoi(argv[1]);  
  std::string instance = argv[2];  

  Scenario other;
  other.operationsBi = {'+', '-', '*', '/'};

  other.operationsU = {'e', 'i', 'r', 'a'};

  other.separateT1T2();
  
  std::cout << "Bi T1: " << other.operationsBiT1.size() << ":";
  for(auto& op: other.operationsBiT1) std::cout << op;
  std::cout << std::endl;
  std::cout << "Bi T2: " << other.operationsBiT2.size() << ":";
  for(auto& op: other.operationsBiT2) std::cout << op;
  std::cout << std::endl;
  std::cout << "U T1: " << other.operationsUT1.size() << ":";
  for(auto& op: other.operationsUT1) std::cout << op;
  std::cout << std::endl;
  std::cout << "U T2: " << other.operationsUT2.size() << ":";
  for(auto& op: other.operationsUT2) std::cout << op;
  std::cout << std::endl;

  if (argc > 2) other.maxConst = atoi(argv[2]);
  other.maxConst = 6;
  other.setStackLen(30, 41, 1.5);

  std::cout << "|ARGS| = " << argc << std::endl;
  std::cout << "individualLen:" << other.getIndividualLen() << std::endl;

  BRKGAParams params;
  params.populationLen = 100;
  params.eliteSize = 25;
  params.mutantSize = 10;
  params.eliteBias = 85;
  params.noImprovementMax = 50;
  params.iterationMax = 50;
  params.moreDiversity = 20;
  params.reset = 5;

  if (argc > 3) params.populationLen = atoi(argv[3]);
  if (argc > 4) params.eliteSize = atoi(argv[4]);
  if (argc > 5) params.mutantSize = atoi(argv[5]);
  if (argc > 6) params.eliteBias = atoi(argv[6]);
  if (argc > 7) params.iterationMax = atoi(argv[7]);
  if (argc > 8) params.noImprovementMax = atoi(argv[8]);
  if (argc > 9) params.reset = atoi(argv[9]);

  int training = 100;
  if (argc > 10) training = atoi(argv[10]);

  if (argc > 11) {
    if (argv[11][0] == 't') {
      other.operationsU.push_back('s');
      other.operationsU.push_back('c');
    } else if (argv[11][0] == 'e') {
      other.operationsU.push_back('a');
      other.operationsU.push_back('v');
      other.operationsU.push_back('r');
    }
  }

  if (argc > 12) {
    if (argv[12][0] == 't') {
      other.operationsU.push_back('s');
      other.operationsU.push_back('c');
    } else if (argv[12][0] == 'e') {
      other.operationsU.push_back('a');
      other.operationsU.push_back('v');
      other.operationsU.push_back('r');
    }
  }

  ValuedChromosome bestFoundSolution;
  bestFoundSolution.cost = -1;
  bestFoundSolution.randomKeys = Vec<chromosome>(other.getIndividualLen(), 0);
  bestFoundSolution.cost = INFINITY;

  Scanner scanner(new File(instance));
  RProblem problem;
  readIO(problem, scanner);

  std::vector<std::vector<double>> fullInputs = problem.inputs; 
  std::vector<double> fullOutputs = problem.outputs; 

  if (false) problem.make_squared();
  other.setWeightPerUnusedVariable(1.0);

  printFile(problem);

  training = percentToInt(training, problem.tests);
  std::cout << "training=" << training << std::endl;

  RKGenerator rkg;
  rkg.nVars = problem.nVars;
  rkg.nConst = problem.nConst;
  rkg.operationsBi = other.operationsBi;
  rkg.operationsU = other.operationsU;
  rkg.stackLen = other.getStackLen();
  rkg.maxConst = other.maxConst;

  std::optional<Vec<chromosome>> opInitialSol = std::nullopt;

  std::cout << "run_brkga:" << std::endl;
  std::cout << "best=" << bestFoundSolution.cost << std::endl;

  // Aqui adicionamos fullInputs e fullOutputs na chamada
  run_brkga(problem, params, seed, bestFoundSolution, other, training, opInitialSol, fullInputs, fullOutputs);

  std::cout << "best=" << bestFoundSolution.cost << std::endl;

  printSolution(problem, bestFoundSolution.randomKeys, other);
  std::cout << "trueStackSize=" << bestFoundSolution.trueStackSize << std::endl;
  std::string eq = printSolution2(problem, bestFoundSolution.randomKeys, other);
  std::cout << "printSolution2: " << eq << std::endl;

  double auxBestFoundSolutionCost = bestFoundSolution.cost;
  if (false) {
    changeIO(problem.inputs, problem.outputs, training, problem.nVars,
             problem.tests, problem.nConst);
    printFile(problem);
    // Note que aqui, se você quiser chamar run_brkga novamente,
    // precisaria também passar fullInputs e fullOutputs, mas está em false.
    printCodChromosome(bestFoundSolution.randomKeys);
    printDecodChromosome(bestFoundSolution.randomKeys, problem, other);
    std::cout << "printSolution (ENTENDER E REMOVER): " << std::endl;
    printSolution(problem, bestFoundSolution.randomKeys, other);
    std::string eq2 =
        printSolution2(problem, bestFoundSolution.randomKeys, other);
    std::cout << "printSolution2: " << eq2 << std::endl;
    printf("best before: %lf\n", bestFoundSolution.cost);
    bestFoundSolution.cost = solutionEvaluator(
        problem, bestFoundSolution.randomKeys, other, training, -1);
    printf("best after: %lf \n", bestFoundSolution.cost);
    printf("Validation Mean: %lf  \n",
           (auxBestFoundSolutionCost + bestFoundSolution.cost) / 2);
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> duration = end - start;
  double executionTime = duration.count();

  std::cout << "Tempo de execucao: " << executionTime << " segundos" << std::endl;

  if (false) {
    std::cout << "BEGIN GINAC TESTS!" << std::endl;
    // Sem alterações aqui, pois está em false.
  }

  std::cout << "FIM" << std::endl;

  return 0;
}
