// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <algorithm>
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

int main(int argc, char* argv[]) {  // BRKGA
  // arguments sequence
  // 1: stackLen
  // 2: maxConst
  // 3: populationLen
  // 4: eliteSize
  // 5: mutantSize
  // 6: eliteBias
  // 7: restartMax
  // 8: noImprovementMax
  // 9: training (validation)
  // 10: 't' or 'e' (adds functions 's' and 'c' OR 'a', 'v' and 'r')
  // 11: 't' or 'e' (adds functions 's' and 'c' OR 'a', 'v' and 'r')

  int seed = 400002;

  Scenario other;
  other.operationsBi = {'+', '-', '*', '/'};
  other.operationsU = {'i'};
  //other.stackLen = 15;
  other.stackLen = 41;
  //other.maxConst = 3;
  other.maxConst = 6;

  std::cout << "|ARGS| = " << argc << std::endl;

  if (argc > 1) other.stackLen = atoi(argv[1]);
  if (argc > 2) other.maxConst = atoi(argv[2]);

  other.individualLen = 3 * other.stackLen + other.maxConst + 1;

  BRKGAParams params;
  //params.populationLen = 30;
  params.populationLen = 100;
  params.eliteSize = 25;
  //params.eliteSize = 25;
  //params.mutantSize = 5;
  params.mutantSize = 10;
  //params.eliteBias = 70;
  params.eliteBias = 85;
  params.noImprovementMax = 500;
  // params.restartMax = 1000;
  params.restartMax = 20;

  if (argc > 3) params.populationLen = atoi(argv[3]);
  if (argc > 4) params.eliteSize = atoi(argv[4]);
  if (argc > 5) params.mutantSize = atoi(argv[5]);
  if (argc > 6) params.eliteBias = atoi(argv[6]);
  if (argc > 7) params.restartMax = atoi(argv[7]);
  if (argc > 8) params.noImprovementMax = atoi(argv[8]);

  int training = 70;
  if (argc > 9) training = atoi(argv[9]);

  if (argc > 10) {
    if (argv[10][0] == 't') {
      other.operationsU.push_back('s');
      other.operationsU.push_back('c');
    } else if (argv[10][0] == 'e') {
      other.operationsU.push_back('a');
      other.operationsU.push_back('v');
      other.operationsU.push_back('r');
    }
  }

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

  printf("Aqui foi 1");
  ValuedChromosome bestFoundSolution;
  bestFoundSolution.cost = -1;
  bestFoundSolution.randomKeys = Vec<chromosome>(other.individualLen, 0);
  bestFoundSolution.cost = INFINITY;

  // char instance[] = "instances/lit3_BSR_func1_1.in";
  // char instance[] = "instances/xcubic_xsquare_px_12.in";
  // char instance[] = "instances/xcubic_xsquare_px_5.in";
  //
   std::string instance = "instances/Set1(1).in";
  //std::string instance = "instances/Test.in";
  // std::string instance = "instances/xcubic_xsquare_px_5.in";

  // char instance[] = "C:/Users/Filip/OneDrive/Área de
  // Trabalho/Temporário/Projeto/CMakeProject2/instances_short_range/generate_india_function_short_range_1.in";
  // char instance[] =
  // "instances_short_range/generate_india_function_short_range_1.in"; char
  // instance[] = "instances_short_range/lit3_BSR_func1_short_range1.in";

  printf("2");

  Scanner scanner(new File(instance));

  //
  RProblem problem;

  //
  // readIO(inputs, outputs, scanner, nVars, tests, nConst, vConst);
  readIO(problem, scanner);
  // printFile(nVars, tests, nConst, inputs, outputs, vConst);
  printFile(problem);

  training = percentToInt(training, problem.tests);

  /*
  readIO(&inputs,&outputs,scanner,&nVars,&tests);
  printFile(nVars,tests,inputs,outputs);
  //printStack(vStack,N);
  individualGenerator(individual,LEN,seed);
  printCodChromosome(individual,LEN);
  printDecodChromosome(individual,N,nVars,nConst);
  //printStack(vStack,N);
  //printStack(orderNum,N);
  //printStack(orderOps,N);
 //printConst(vConst,N);
  stackAdjustment(individual,N);
  result =
 solutionEvaluator(individual,operationsBi,operationsU,N,nVars,tests,inputs,outputs,vConstMin,vConstMax,nConst);
  printCodChromosome(individual,LEN);
  printDecodChromosome(individual,N,nVars,nConst);
  printf("%.4f \n",result);

  //printf("%s\n",operationsBi);

  */

  run_brkga(problem, params, seed, bestFoundSolution, other, training);
  // individualGenerator(bestFoundSolution.randomKeys,seed);
  // stackAdjustment(bestFoundSolution.randomKeys,N,nVars,nConst,MAXCONST,seed);
  // bestFoundSolution.cost =
  // solutionEvaluator(bestFoundSolution.randomKeys,operationsBi,operationsU,N,nVars,tests,inputs,outputs,vConstMin,vConstMax,nConst);
  printCodChromosome(bestFoundSolution.randomKeys);
  printDecodChromosome(bestFoundSolution.randomKeys, problem, other);
  printSolution(problem, bestFoundSolution.randomKeys, other);
  bestFoundSolution.cost =
      solutionEvaluator(problem, bestFoundSolution.randomKeys, other, training);
  printf("best: %lf \n", bestFoundSolution.cost);
  double auxBestFoundSolutionCost = bestFoundSolution.cost;

  changeIO(problem.inputs, problem.outputs, training, problem.nVars,
           problem.tests, problem.nConst);
  printFile(problem);
  run_brkga(problem, params, seed, bestFoundSolution, other, training);
  // individualGenerator(bestFoundSolution.randomKeys,seed);
  // stackAdjustment(bestFoundSolution.randomKeys,N,nVars,nConst,MAXCONST,seed);
  // bestFoundSolution.cost =
  // solutionEvaluator(bestFoundSolution.randomKeys,operationsBi,operationsU,N,nVars,tests,inputs,outputs,vConstMin,vConstMax,nConst);
  printCodChromosome(bestFoundSolution.randomKeys);
  printDecodChromosome(bestFoundSolution.randomKeys, problem, other);
  printSolution(problem, bestFoundSolution.randomKeys, other);
  printf("best before: %lf\n", bestFoundSolution.cost);
  bestFoundSolution.cost =
      solutionEvaluator(problem, bestFoundSolution.randomKeys, other, training);
  printf("best after: %lf \n", bestFoundSolution.cost);

  printf("Validation Mean: %lf  \n",
         (auxBestFoundSolutionCost + bestFoundSolution.cost) / 2);

  return 0;
}
