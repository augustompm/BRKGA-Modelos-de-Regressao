// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

// C
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// C++
#include <algorithm>
#include <iostream>
#include <stack>
#include <vector>
//
#include <brkgp/BRKGA.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <brkgp/Utils.hpp>

int percentToInt(int porcentagem, int total) {
  return ceil((porcentagem / 100.0) * total);
}

bool menorQue(const ValuedChromosome& i, const ValuedChromosome& j) {
  return (i.cost < j.cost) ||
         ((i.cost == j.cost) && (i.trueStackSize < j.trueStackSize));
}

void individualGenerator(Vec<chromosome>& individual, int seed,
                         int individualLen) {
  srand(seed);
  for (int i = 0; i < individualLen; i++) {
    individual[i] = (rand() % 10000);
    // seed++;
  }
}

void mutantGenerator(Vec<ValuedChromosome>& population, int eliteSize,
                     int mutantSize, int seed, int individualLen) {
  for (int i = eliteSize; i < (eliteSize + mutantSize); i++) {
    individualGenerator(population[i].randomKeys, seed, individualLen);
    seed += 2 * individualLen;
    population[i].cost = 0;
  }
}

void crossover(Vec<ValuedChromosome>& population,
               Vec<ValuedChromosome>& auxPopulation, int eliteSize,
               int mutantSize, int seed, uint16_t eliteBias, int populationLen,
               int individualLen) {
  int parentA;
  int parentB;

  for (int i = (eliteSize + mutantSize); i < populationLen; i++) {
    srand(seed);
    // printf("sizeof %d\n",sizeof(chromosome)* LEN);
    // printf("%d",LEN);
    parentA = rand() % eliteSize;
    // printf("parentA: %d\n",parentA);
    // memcpy(parentA, population[rand() % eliteSize].randomKeys,
    // sizeof(chromosome) * 63);
    seed++;
    srand(seed);
    parentB = eliteSize + rand() % (populationLen - eliteSize);
    // printf("parentB: %d\n",parentB);
    // memcpy(parentB, population[eliteSize + rand() % (POPULATIONLEN -
    // eliteSize)].randomKeys, sizeof(chromosome) * LEN);
    seed++;
    for (int j = 0; j < individualLen; j++) {
      srand(seed);
      if ((rand() % 100) < eliteBias)
        auxPopulation[i].randomKeys[j] = population[parentA].randomKeys[j];
      else
        auxPopulation[i].randomKeys[j] = population[parentB].randomKeys[j];
      seed++;
    }
    auxPopulation[i].cost = 0;
  }
}

void populationGenerator(Vec<ValuedChromosome>& auxPopulation, int seed,
                         int populationLen, int individualLen) {
  for (int i = 0; i < populationLen; i++) {
    for (int j = 0; j < individualLen; j++) {
      auxPopulation[i].randomKeys[j] = 0;
    }
    individualGenerator(auxPopulation[i].randomKeys, seed, individualLen);
    seed += 2 * individualLen;
    auxPopulation[i].cost = 0;
  }
}

void decoder(const RProblem& problem, Vec<ValuedChromosome>& population,
             int seed, int populationLen, const Scenario& other) {
  // problem
  int nVars = problem.nVars;
  int nConst = problem.nConst;
  int tests = problem.tests;
  const Vec<Vec<double>>& inputs = problem.inputs;
  const Vec<double>& outputs = problem.outputs;
  const Vec<Pair<double, double>>& vConst = problem.vConst;
  //
  auto& operationsBi = other.operationsBi;
  auto& operationsU = other.operationsU;

  int stackLen = other.stackLen;
  int maxConst = other.maxConst;
  int operationsBiLen = other.operationsBi.size();
  int operationsULen = other.operationsU.size();
  //
  for (int i = 0; i < populationLen; i++) {
    if (population[i].cost == 0) {
      population[i].trueStackSize = stackAdjustment(
          population[i].randomKeys, stackLen, nVars, nConst, maxConst, seed);
      population[i].cost =
          solutionEvaluator(problem, population[i].randomKeys, other, 0);
    }
  }
}

char isRestart(const Vec<ValuedChromosome>& auxPopulation,
               const Vec<ValuedChromosome>& population, int& noImprovement,
               int noImprovementMax) {
  if (auxPopulation[0].cost < population[0].cost)
    noImprovement = 0;
  else
    noImprovement++;
  if (noImprovement == noImprovementMax) {
    //|| (auxPopulation[0].cost <= 0.00001))
    noImprovement = 0;
    return 1;
  }
  return 0;
}

void run_brkga(const RProblem& problem, const BRKGAParams& params, int seed,
               ValuedChromosome& bestFoundSolution, const Scenario& other,
               int training) {
  // problem
  int nVars = problem.nVars;
  int nConst = problem.nConst;
  int tests = problem.tests;
  const Vec<Vec<double>>& inputs = problem.inputs;
  const Vec<double>& outputs = problem.outputs;
  const Vec<Pair<double, double>>& vConst = problem.vConst;
  // params
  int restartMax = params.restartMax;
  int noImprovementMax = params.noImprovementMax;
  int eliteSize = params.eliteSize;
  int mutantSize = params.mutantSize;
  uint16_t eliteBias = params.eliteBias;
  int populationLen = params.populationLen;
  // other
  auto& operationsBi = other.operationsBi;
  auto& operationsU = other.operationsU;
  int individualLen = other.individualLen;
  int stackLen = other.stackLen;
  int maxConst = other.maxConst;
  int operationsBiLen = operationsBi.size();
  int operationsULen = operationsU.size();

  //
  Vec<ValuedChromosome> mainPopulation(populationLen);  // populationLen
  Vec<ValuedChromosome> auxPopulation(populationLen);
  // printf("populationLen1: %d eliteSize1: %d mutanteSize1: %d individualLen1:
  // %d stackLen1:
  // %d\n",populationLen,eliteSize,mutantSize,individualLen,stackLen);

  for (int i = 0; i < populationLen; i++) {
    auxPopulation[i].randomKeys = Vec<chromosome>(individualLen, 0);
    mainPopulation[i].randomKeys = Vec<chromosome>(individualLen, 0);
  }

  bestFoundSolution.cost = INFINITY;

  eliteSize = percentToInt(eliteSize, populationLen);
  mutantSize = percentToInt(mutantSize, populationLen);
  // printf("populationLen2: %d eliteSize2: %d mutanteSize2: %d individualLen2:
  // %d stackLen2:
  // %d\n",populationLen,eliteSize,mutantSize,individualLen,stackLen);
  // printf("%d\t%d\n",eliteSize,mutantSize);
  int mutationGrow = 0;
  int noImprovement = 0;
  uint8_t end{0};
  int restart = 0;
  while (restart < restartMax) {
    seed++;
    populationGenerator(mainPopulation, seed, populationLen, individualLen);
    decoder(problem, mainPopulation, seed, populationLen, other);
    std::sort(mainPopulation.begin(), mainPopulation.end(), menorQue);
    end = 0;
    while (!(end)) {
      for (int i = 0; i < populationLen; i++) {
        auxPopulation[i].randomKeys = mainPopulation[i].randomKeys;
        auxPopulation[i].cost = mainPopulation[i].cost;
      }
      mutationGrow = percentToInt(5 * (5 * (noImprovement / noImprovementMax)),
                                  populationLen);
      mutantGenerator(auxPopulation, eliteSize, (mutantSize + mutationGrow),
                      seed, individualLen);
      crossover(mainPopulation, auxPopulation, eliteSize,
                (mutantSize + mutationGrow), seed, eliteBias, populationLen,
                individualLen);
      seed += 2 * individualLen;
      decoder(problem, auxPopulation, seed, populationLen, other);
      // printPopulationCost(auxPopulation,populationLen);
      //
      std::sort(auxPopulation.begin(), auxPopulation.end(), menorQue);
      //
      // printPopulationCost(auxPopulation,populationLen);
      // printSolution(auxPopulation[0].randomKeys,stackLen,nVars,nConst,operationsBi,operationsU,vConstMin,vConstMax);
      // printf("cost = %f No Improviments =
      // %d\n",auxPopulation[0].cost,noImproviment);
      //
      //
      // IMPORTANT: PASS 'noImprovement' BY REFERENCE!
      end = isRestart(auxPopulation, mainPopulation, noImprovement,
                      noImprovementMax);
      // printSolution(auxPopulation[20].randomKeys,stackLen,nVars,nConst,operationsBi,operationsU,vConstMin,vConstMax);
      for (int i = 0; i < populationLen; i++) {
        mainPopulation[i].randomKeys = auxPopulation[i].randomKeys;
        mainPopulation[i].cost = auxPopulation[i].cost;
      }
      // printf("auxPopulation:%f\t
      // population:%f\n",auxPopulation[0].cost,population[0].cost);
    }
    // printf("Erro Atual: %f\t",population[0].cost);
    // printf("Erro Best: %f\n",bestFoundSolution.cost);
    if ((bestFoundSolution.cost - mainPopulation[0].cost) >
        0.000001) {  //|| (((bestFoundSolution->cost - mainPopulation->cost )<
                     // 0.0000001)  && (mainPopulation->trueStackSize <
                     // bestFoundSolution->trueStackSize))){
      // printf("Erro Melhorado: %f\n",population[0].cost);
      bestFoundSolution.cost = mainPopulation[0].cost;
      bestFoundSolution.randomKeys = mainPopulation[0].randomKeys;
      // printf("Chegou\n");
      restart = 0;
    }
    // printf("Chegou");
    printf("restart = %d \t best= %f\t", restart, bestFoundSolution.cost);
    printSolution(problem, bestFoundSolution.randomKeys, other);
    restart++;
  }
}
