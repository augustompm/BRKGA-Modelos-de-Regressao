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
  // Início da contagem de tempo
  auto start = std::chrono::high_resolution_clock::now();

  // Verifica se os parâmetros foram passados corretamente
  if (argc < 3) {
    std::cerr << "Uso: " << argv[0] << " <seed> <instance_file>" << std::endl;
    return 1;
  }

  // seed e instance a partir da entrada
  int seed = atoi(argv[1]);  
  std::string instance = argv[2];  

  Scenario other;
  other.operationsBi = {'+', '-', '*', '/'};
 
  /*
  other.operationsBiT1 = {'+', '-'};  // bad with units
  other.operationsBiT2 = {'*', '/'};  // ok with units
  other.operationsBi.insert(other.operationsBi.end(),
                            other.operationsBiT1.begin(),
                            other.operationsBiT1.end());
  other.operationsBi.insert(other.operationsBi.end(),
                            other.operationsBiT2.begin(),
                            other.operationsBiT2.end());
  // T1 and T2 must have same size!
  assert(other.operationsBiT1.size() == other.operationsBiT2.size());
  */

  //other.operationsU = {'i', 'r', 'a'};
  // other.operationsU = {'i', 'r', 'a', 'e'};
  other.operationsU = {'e', 'i', 'r', 'a'};
  //other.operationsU = {'e'};

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
  params.restartMax = 100;
  params.moreDiversity = 10;

  if (argc > 3) params.populationLen = atoi(argv[3]);
  if (argc > 4) params.eliteSize = atoi(argv[4]);
  if (argc > 5) params.mutantSize = atoi(argv[5]);
  if (argc > 6) params.eliteBias = atoi(argv[6]);
  if (argc > 7) params.restartMax = atoi(argv[7]);
  if (argc > 8) params.noImprovementMax = atoi(argv[8]);

  int training = 100;
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

  ValuedChromosome bestFoundSolution;
  bestFoundSolution.cost = -1;
  bestFoundSolution.randomKeys = Vec<chromosome>(other.getIndividualLen(), 0);
  bestFoundSolution.cost = INFINITY;

  Scanner scanner(new File(instance));

  RProblem problem;

  readIO(problem, scanner);

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

  run_brkga(problem, params, seed, bestFoundSolution, other, training, opInitialSol);

  std::cout << "best=" << bestFoundSolution.cost << std::endl;

  printSolution(problem, bestFoundSolution.randomKeys, other);
  std::cout << "trueStackSize=" << bestFoundSolution.trueStackSize << std::endl;
  std::string eq = printSolution2(problem, bestFoundSolution.randomKeys, other);
  std::cout << "printSolution2: " << eq << std::endl;

  // SEGUNDO ROUND DESATIVADO
  double auxBestFoundSolutionCost = bestFoundSolution.cost;
  if (false) {
    changeIO(problem.inputs, problem.outputs, training, problem.nVars,
             problem.tests, problem.nConst);
    printFile(problem);
    run_brkga(problem, params, seed, bestFoundSolution, other, training,
              opInitialSol);
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
    // testing GINAC 2
    std::cout << std::endl << std::endl << " ============== " << std::endl;
    {
      unsigned result = 0;
      {
        ex e =
            ex("((m * m) + ((m - (m)) * {((m) + {(({-7^2} + ((((-1 - "
               "((-1))))))))^2})^2}))",
               problem.syms);
        cout << "equacao: " << flush;
        std::cout << latex << e << std::endl;  // m^2
        //
        ex e2 = ex("m^2", problem.syms);
        std::cout << "e2: " << e2 << std::endl;
        ex e3 = normal(e * e2);
        std::cout << e3 << std::endl;  // m^2
        //
        bool bzero = (e3 == 0);
        bool bone = (e3 == 1);
        //
        bool bnumeric = false;
        if (is_exactly_a<numeric>(e3))
          bnumeric = true;
        else
          bnumeric = false;
        //
        std::cout << "ZERO? " << bzero << std::endl;
        std::cout << "ONE? " << bone << std::endl;
        std::cout << "NUMERIC? " << bnumeric << std::endl;
        //
        ex e4 = sqrt(e3);
        std::cout << "e4:" << normal(e4) << std::endl;
        //
        ex e_m = ex("m", problem.syms);
        ex e5 = sqrt(pow(e_m, 4));
        std::cout << "e5:" << evalf(e5) << std::endl;
        //
        ex e32(32);
        ex e32_2 = pow(4 * sqrt(ex(2)), 2);
        std::cout << "e32:" << e32 << std::endl;
        std::cout << "e32_2:" << e32_2 << std::endl;
        //
        bool issame_ex = (e4 == e2);
        std::cout << "same?" << issame_ex << std::endl;
        std::cout << "same?" << ((bool)(e2 == e)) << std::endl;
        //
        ex e6 = pow(e4, 2);
        std::cout << "e6:" << e6 << std::endl;
        //
        ex e7 = sqrt(e2);
        std::cout << "e7:" << normal(e7) << std::endl;
        //
        ex e8 = e7 / e7;
        std::cout << "e8:" << normal(e8) << std::endl;
        //
        ex e9 = e7 / e2;
        std::cout << "e9:" << normal(e9) << std::endl;
        //
        ex e_sol =
            ex("( 1 * m^2 - 2 * m^2 ) + ( 20 * m^2 - 10 * m^2 )", problem.syms);
        e_sol = sqrt(e_sol) / 3;
        std::cout << "e_sol:" << normal(e_sol) << std::endl;
        std::cout << fix_sqrt(e_sol, problem.syms) << std::endl;
        std::cout << "e_sol':   " << e_sol << std::endl;
        //
        ex e_sol2 = pow(e_sol, 2);
        std::cout << "e_sol2:" << e_sol2 << std::endl;
        std::cout << "e_sol2 = m ?:" << (bool)(e_sol2 == e2) << std::endl;
        //
        ex e11 = ex("1/m^4", problem.syms);
        e11 = sqrt(e11);  // sqrt(1/m^4)
        std::cout << "e11:   " << e11 << std::endl;
        std::cout << "e11_2: " << pow(e11, 2) << std::endl;
        std::cout << fix_sqrt(e11, problem.syms) << std::endl;
        std::cout << "e11':   " << e11 << std::endl;
        //
        ex e12 = 1 / sqrt(e6);
        std::cout << "e12:   " << e12 << std::endl;
        std::cout << "e12_2: " << pow(e12, 2) << std::endl;
        std::cout << fix_sqrt(e12, problem.syms) << std::endl;
        std::cout << "e12':   " << e12 << std::endl;
        //
        ex e13 = 1 / sqrt(pow(e_m, 3));
        std::cout << "e13:   " << e13 << std::endl;
        std::cout << fix_sqrt(e13, problem.syms) << std::endl;
        std::cout << "e13':   " << e13 << std::endl;
        //
        ex e14 = sqrt(e_m);
        std::cout << "e14: " << e14 << std::endl;
        std::stringstream ss;
        ss << e14;
        ex e15("sqrt(m)", problem.syms);
        std::cout << "e15: " << e15 << std::endl;
      }
      std::cout << "OK" << std::endl;
    }
  }

  std::cout << "FIM" << std::endl;

  return 0;
}

