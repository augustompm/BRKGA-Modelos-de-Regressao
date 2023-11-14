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

int main(int argc, char* argv[]) {  // BRKGA

  // Início da contagem de tempo
  auto start = std::chrono::high_resolution_clock::now();

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

  int seed = 728462315;

  Scenario other;
  other.operationsBi = {'+', '-', '*', '/'};
  // other.operationsU = {'i', 'r', 'a'};
  other.operationsU = {'i', 'a'};

  // other.stackLen = 15;
  // other.stackLen = 9;
  other.stackLen = 41;
  // other.maxConst = 3;
  other.maxConst = 6;

  std::cout << "|ARGS| = " << argc << std::endl;

  if (argc > 1) other.stackLen = atoi(argv[1]);
  if (argc > 2) other.maxConst = atoi(argv[2]);

  other.individualLen = 3 * other.stackLen + other.maxConst + 1;

  BRKGAParams params;
  // params.populationLen = 30;
  params.populationLen = 100;
  params.eliteSize = 25;
  // params.eliteSize = 25;
  // params.mutantSize = 5;
  params.mutantSize = 10;
  // params.eliteBias = 70;
  params.eliteBias = 85;
  // params.noImprovementMax = 20000;
  params.noImprovementMax = 10;
  // params.restartMax = 1000;
  // params.restartMax = 20;
  params.restartMax = 5;

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

  // printf("Aqui foi 1");
  ValuedChromosome bestFoundSolution;
  bestFoundSolution.cost = -1;
  bestFoundSolution.randomKeys = Vec<chromosome>(other.individualLen, 0);
  bestFoundSolution.cost = INFINITY;

  // char instance[] = "instances/lit3_BSR_func1_1.in";
  // char instance[] = "instances/xcubic_xsquare_px_12.in";
  // char instance[] = "instances/xcubic_xsquare_px_5.in";
  //
  //
  // std::string instance = "instances/Eq. Feynman  (10) (4).in";
  //
  std::string instance = "instances/Test.in";
  //  std::string instance = "instances/xcubic_xsquare_px_5.in";

  // char instance[] = "C:/Users/Filip/OneDrive/Área de
  // char instance[] =
  // "instances_short_range/generate_india_function_short_range_1.in"; char
  // instance[] = "instances_short_range/lit3_BSR_func1_short_range1.in";

  // printf("2");

  Scanner scanner(new File(instance));
  // printf("3");

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

  // =====================================================================
  // FOR EUCLIDEAN DISTANCE!
  // STACKLEN = 9
  // 5001   5001      0      3000  5001 5001  0 3000  3000        | 4000  2000
  // ?   ?  8000 6000 ?  ? ?      | ?   ?   4000  8000  ? ? 4000 8000  4000
  Vec<chromosome> initialSol = {5001, 5001, 0,    3000, 5001, 5001, 0,
                                3000, 3000, 4000, 2000, 9999, 9999, 8000,
                                6000, 9999, 9999, 9999, 9999, 9999, 4000,
                                8000, 9999, 9999, 4000, 8000, 4000};

  std::optional<Vec<chromosome>> opInitialSol = std::nullopt;

  // IF EUCLEAN DISTANCE & STACKLEN = 9
  opInitialSol = initialSol;
  // =====================================================================

  run_brkga(problem, params, seed, bestFoundSolution, other, training,
            opInitialSol);
  // individualGenerator(bestFoundSolution.randomKeys,seed);
  // stackAdjustment(bestFoundSolution.randomKeys,N,nVars,nConst,MAXCONST,seed);
  // bestFoundSolution.cost =
  // solutionEvaluator(bestFoundSolution.randomKeys,operationsBi,operationsU,N,nVars,tests,inputs,outputs,vConstMin,vConstMax,nConst);
  printCodChromosome(bestFoundSolution.randomKeys);
  printDecodChromosome(bestFoundSolution.randomKeys, problem, other);
  std::cout << "printSolution: NOT USED" << std::endl;
  // printSolution(problem, bestFoundSolution.randomKeys, other);
  std::cout << "trueStackSize=" << bestFoundSolution.trueStackSize << std::endl;
  std::string eq = printSolution2(problem, bestFoundSolution.randomKeys, other);
  std::cout << "printSolution2: " << eq << std::endl;
  bestFoundSolution.cost =
      solutionEvaluator(problem, bestFoundSolution.randomKeys, other, training);
  printf("best: %lf \n", bestFoundSolution.cost);
  double auxBestFoundSolutionCost = bestFoundSolution.cost;

  changeIO(problem.inputs, problem.outputs, training, problem.nVars,
           problem.tests, problem.nConst);
  printFile(problem);
  run_brkga(problem, params, seed, bestFoundSolution, other, training,
            opInitialSol);
  // individualGenerator(bestFoundSolution.randomKeys,seed);
  // stackAdjustment(bestFoundSolution.randomKeys,N,nVars,nConst,MAXCONST,seed);
  // bestFoundSolution.cost =
  // solutionEvaluator(bestFoundSolution.randomKeys,operationsBi,operationsU,N,nVars,tests,inputs,outputs,vConstMin,vConstMax,nConst);
  printCodChromosome(bestFoundSolution.randomKeys);
  printDecodChromosome(bestFoundSolution.randomKeys, problem, other);
  std::cout << "printSolution (ENTENDER E REMOVER): " << std::endl;
  printSolution(problem, bestFoundSolution.randomKeys, other);
  std::string eq2 =
      printSolution2(problem, bestFoundSolution.randomKeys, other);
  std::cout << "printSolution2: " << eq2 << std::endl;
  printf("best before: %lf\n", bestFoundSolution.cost);
  bestFoundSolution.cost =
      solutionEvaluator(problem, bestFoundSolution.randomKeys, other, training);
  printf("best after: %lf \n", bestFoundSolution.cost);

  printf("Validation Mean: %lf  \n",
         (auxBestFoundSolutionCost + bestFoundSolution.cost) / 2);

  // código para medir o tempo, abaixo;

  // Fim da contagem de tempo
  auto end = std::chrono::high_resolution_clock::now();

  // Cálculo do tempo decorrido em segundos
  std::chrono::duration<double> duration = end - start;
  double executionTime = duration.count();

  // Exibição do tempo de execução
  std::cout << "Tempo de execucao: " << executionTime << " segundos"
            << std::endl;

  // testing GINAC
  {
    unsigned result = 0;
    {
      ex e;
      symbol x("x");
      lst syms = {x};
      e =
          ex("((x * x) + ((x - (x)) * {((x) + {(({-7^2} + ((((-1 - "
             "((-1))))))))^2})^2}))",
             syms);
      cout << "equacao: " << flush;
      std::cout << latex << e << std::endl;  // x^2
      //
      my_visitor v;
      e.accept(v);
      //
      // Substitute values
      ex substituted_expr = e.subs(x == 3);

      // Evaluate the substituted expression
      numeric result = ex_to<numeric>(substituted_expr);
      std::cout << e.subs(x == 3) << std::endl;
      std::cout << e.subs(x == 5) << std::endl;
      //
      auto ex2 = ex("x+1/x", syms);
      std::cout << ex2.subs(x == 3) << std::endl;
      std::cout << ex2.subs(x == 1) << std::endl;
    }
    std::cout << "OK" << std::endl;
  }

  std::cout << "FIM" << std::endl;

  return 0;
}
