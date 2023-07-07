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

#include "brkgp/Utils.hpp"

// using namespace std;

// typedef unsigned short chromosome;

// typedef struct{
// chromosome randomKeys[LEN];
// double cost;
//} valuedChromosome;

int main(int argc, char* argv[]) {  // BRKGA
  // arguments sequence
  // stackLen,maxConst,populationLen,eliteSize,mutantSize,eliteBias,restartMax,noImprovmentMax,validation
  char operationsBi[] = {'+', '-', '*', '/', '\0'};
  char operationsU[7];
  operationsU[0] = 'i';
  operationsU[1] = '\0';
  int operationsBiLen = 4;
  int operationsULen = 1;

  int training = 70;
  int stackLen = 15;
  int maxConst = 3;

  // tirar elses

  /*if (argc>1)
      stackLen = atoi(argv[1]);
   if(argc>2)
      maxConst = atoi(argv[2]);*/

  int individualLen = 3 * stackLen + maxConst + 1;

  double auxBestFoundSolutionCost;

  BRKGAParams params;
  params.populationLen = 30;
  params.eliteSize = 25;
  params.mutantSize = 5;
  params.eliteBias = 70;
  params.noImprovementMax = 10;
  // params.restartMax = 1000;
  params.restartMax = 10;

  /*if (argc>3)
      populationLen = atoi(argv[3]);
   if(argc>4)
      eliteSize = atoi(argv[4]);
  if(argc>5)
      mutantSize = atoi(argv[5]);
   if(argc>6)
      eliteBias = atoi(argv[6]);
  if(argc>7)
      restartMax = atoi(argv[7]);
   if(argc>8)
      noImprovimentMax = atoi(argv[8]);
  if(argc>9)
      training = atoi(argv[9]);
  if(argc>10){
      if (argv[10][0] == 't')
    */
  {
    operationsU[operationsULen] = 's';
    operationsU[operationsULen + 1] = 'c';
    operationsU[operationsULen + 2] = '\0';
    operationsULen += 2;
  }
  /*else if (argv[10][0] == 'e')
  {
      operationsU[operationsULen] = 'a';
      operationsU[operationsULen+1] = 'v';
      operationsU[operationsULen+2] = 'r';
      operationsU[operationsULen+3] = '\0';
      operationsULen+= 3;
  }
}
if(argc>11){
  if (argv[11][0] == 't')
  {
      operationsU[operationsULen] = 's';
      operationsU[operationsULen+1] = 'c';
      operationsU[operationsULen+2] = '\0';
      operationsULen+= 2;
  }
  else if (argv[11][0] == 'e')
  {
      operationsU[operationsULen] = 'a';
      operationsU[operationsULen+1] = 'v';
      operationsU[operationsULen+2] = 'r';
      operationsU[operationsULen+3] = '\0';
      operationsULen+= 3;
  }
}*/

  printf("Aqui foi 1");
  ValuedChromosome bestFoundSolution;
  bestFoundSolution.cost = -1;
  bestFoundSolution.randomKeys =
      //    (chromosome*)malloc(sizeof(chromosome) * individualLen);
      Vec<chromosome>(individualLen, 0);
  bestFoundSolution.cost = INFINITY;

  // char instance[] = "instances/lit3_BSR_func1_1.in";
  // char instance[] = "instances/xcubic_xsquare_px_12.in";
  // char instance[] = "instances/xcubic_xsquare_px_5.in";
  //
  std::string instance = "instances/Test.in";
  // std::string instance = "instances/xcubic_xsquare_px_5.in";

  // char instance[] = "C:/Users/Filip/OneDrive/Área de
  // Trabalho/Temporário/Projeto/CMakeProject2/instances_short_range/generate_india_function_short_range_1.in";
  // char instance[] =
  // "instances_short_range/generate_india_function_short_range_1.in"; char
  // instance[] = "instances_short_range/lit3_BSR_func1_short_range1.in";

  printf("2");

  Scanner scanner(new File(instance));
  int seed = 0;

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

  almostBestSolution(problem, params, seed, &bestFoundSolution, operationsBi,
                     operationsU, training, individualLen, stackLen, maxConst,
                     operationsBiLen, operationsULen);
  // individualGenerator(bestFoundSolution.randomKeys,seed);
  // stackAdjustment(bestFoundSolution.randomKeys,N,nVars,nConst,MAXCONST,seed);
  // bestFoundSolution.cost =
  // solutionEvaluator(bestFoundSolution.randomKeys,operationsBi,operationsU,N,nVars,tests,inputs,outputs,vConstMin,vConstMax,nConst);
  printCodChromosome(bestFoundSolution.randomKeys, individualLen);
  printDecodChromosome(bestFoundSolution.randomKeys, stackLen, problem.nVars,
                       problem.nConst, operationsBiLen, operationsULen);
  printSolution(problem, bestFoundSolution.randomKeys, stackLen, operationsBi,
                operationsU, operationsBiLen, operationsULen);
  bestFoundSolution.cost = solutionEvaluator(
      problem, bestFoundSolution.randomKeys, operationsBi, operationsU,
      stackLen, training, operationsBiLen, operationsULen);
  printf("best: %lf \n", bestFoundSolution.cost);
  auxBestFoundSolutionCost = bestFoundSolution.cost;

  changeIO(problem.inputs, problem.outputs, training, problem.nVars,
           problem.tests, problem.nConst);
  printFile(problem);
  almostBestSolution(problem, params, seed, &bestFoundSolution, operationsBi,
                     operationsU, training, individualLen, stackLen, maxConst,
                     operationsBiLen, operationsULen);
  // individualGenerator(bestFoundSolution.randomKeys,seed);
  // stackAdjustment(bestFoundSolution.randomKeys,N,nVars,nConst,MAXCONST,seed);
  // bestFoundSolution.cost =
  // solutionEvaluator(bestFoundSolution.randomKeys,operationsBi,operationsU,N,nVars,tests,inputs,outputs,vConstMin,vConstMax,nConst);
  printCodChromosome(bestFoundSolution.randomKeys, individualLen);
  printDecodChromosome(bestFoundSolution.randomKeys, stackLen, problem.nVars,
                       problem.nConst, operationsBiLen, operationsULen);
  printSolution(problem, bestFoundSolution.randomKeys, stackLen, operationsBi,
                operationsU, operationsBiLen, operationsULen);
  printf("best after: %lf\n", bestFoundSolution.cost);
  bestFoundSolution.cost = solutionEvaluator(
      problem, bestFoundSolution.randomKeys, operationsBi, operationsU,
      stackLen, training, operationsBiLen, operationsULen);
  printf("best after: %lf \n", bestFoundSolution.cost);

  printf("Validation Mean: %lf  \n",
         (auxBestFoundSolutionCost + bestFoundSolution.cost) / 2);

  // for (int i = 0; i < problem.tests; i++) {
  //   free(inputs[i]);
  // }
  // free(inputs);
  // free(outputs);
  return 0;
}
