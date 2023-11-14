// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

// C
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// C++
#include <algorithm>
#include <iostream>
#include <optional>  // C++17
#include <stack>
#include <string>
#include <vector>
//
#include <brkgp/BRKGA.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <brkgp/Utils.hpp>

int percentToInt(int porcentagem, int total) {
  return ceil((porcentagem / 100.0) * total);
}

// ordena por menor '.cost' e, caso empate, menor '.trueStackSize'
bool menorQue(const ValuedChromosome& i, const ValuedChromosome& j) {
  return (i.cost < j.cost) ||
         ((i.cost == j.cost) && (i.trueStackSize < j.trueStackSize));
}

void individualGenerator(Vec<chromosome>& individual, int seed) {
  srand(seed);
  for (int i = 0; i < (int)individual.size(); i++) {
    // NOLINTNEXTLINE
    individual[i] = (rand() % 10000);
    // seed++;
  }
}

void mutantGenerator(Vec<ValuedChromosome>& population, int eliteSize,
                     int mutantSize, int seed) {
  for (int i = eliteSize; i < (eliteSize + mutantSize); i++) {
    // store individual seed before creation
    population[i].seed = seed;
    int individualLen = (int)(population[i].randomKeys.size());
    individualGenerator(population[i].randomKeys, seed);
    seed += 2 * individualLen;
    population[i].cost = 0;
  }
}

void crossover(const Vec<ValuedChromosome>& population,
               Vec<ValuedChromosome>& auxPopulation, int eliteSize,
               int mutantSize, uint16_t eliteBias, int seed) {
  int parentA = 0;
  int parentB = 0;

  assert(population.size() == auxPopulation.size());  // NOLINT
  int populationLen = (int)auxPopulation.size();

  for (int i = (eliteSize + mutantSize); i < populationLen; i++) {
    srand(seed);
    // printf("sizeof %d\n",sizeof(chromosome)* LEN);
    // printf("%d",LEN);
    // NOLINTNEXTLINE
    parentA = rand() % eliteSize;
    // printf("parentA: %d\n",parentA);
    // memcpy(parentA, population[rand() % eliteSize].randomKeys,
    // sizeof(chromosome) * 63);
    seed++;
    srand(seed);
    // NOLINTNEXTLINE
    parentB = eliteSize + rand() % (populationLen - eliteSize);
    // printf("parentB: %d\n",parentB);
    // memcpy(parentB, population[eliteSize + rand() % (POPULATIONLEN -
    // eliteSize)].randomKeys, sizeof(chromosome) * LEN);
    seed++;
    int individualLen = (int)auxPopulation[i].randomKeys.size();
    for (int j = 0; j < individualLen; j++) {
      srand(seed);
      // NOLINTNEXTLINE
      if ((rand() % 100) < eliteBias)
        auxPopulation[i].randomKeys[j] = population[parentA].randomKeys[j];
      else
        auxPopulation[i].randomKeys[j] = population[parentB].randomKeys[j];
      seed++;
    }
    auxPopulation[i].cost = 0;
  }
}

void populationGenerator(Vec<ValuedChromosome>& auxPopulation, int seed) {
  for (int i = 0; i < (int)auxPopulation.size(); i++) {
    // zero individual
    int individualLen = (int)(auxPopulation[i].randomKeys.size());
    for (int j = 0; j < individualLen; j++) auxPopulation[i].randomKeys[j] = 0;
    // store individual seed before creation
    auxPopulation[i].seed = seed;
    individualGenerator(auxPopulation[i].randomKeys, seed);
    seed += 2 * individualLen;
    auxPopulation[i].cost = 0;
  }
}

void decoder(Vec<ValuedChromosome>& population, const RProblem& problem,
             const Scenario& other, int seed) {
  // problem
  int nVars = problem.nVars;
  int nConst = problem.nConst;
  // int tests = problem.tests;
  // const Vec<Vec<double>>& inputs = problem.inputs;
  // const Vec<double>& outputs = problem.outputs;
  // const Vec<Pair<double, double>>& vConst = problem.vConst;
  //
  // auto& operationsBi = other.operationsBi;
  // auto& operationsU = other.operationsU;

  int stackLen = other.stackLen;
  int maxConst = other.maxConst;
  // int operationsBiLen = other.operationsBi.size();
  // int operationsULen = other.operationsU.size();
  //
  for (int i = 0; i < (int)population.size(); i++) {
    if (population[i].cost == 0) {
      // store seed used on stack adjustment...
      population[i].seed = seed;
      population[i].trueStackSize =
          stackAdjustment(population[i].randomKeys, stackLen, nVars, nConst,
                          maxConst, population[i].seed);
      // std::cout << "i=" << i << " trueStackSize=" <<
      // population[i].trueStackSize
      //           << "/" << stackLen << " seed = " << seed << std::endl;
      population[i].cost =
          solutionEvaluator(problem, population[i].randomKeys, other, 0);
    }
  }
}

bool isRestart(const Vec<ValuedChromosome>& auxPopulation,
               const Vec<ValuedChromosome>& population, int& noImprovement,
               int noImprovementMax) {
  if (auxPopulation[0].cost < population[0].cost)
    noImprovement = 0;
  else
    noImprovement++;
  if (noImprovement == noImprovementMax) {
    //|| (auxPopulation[0].cost <= 0.00001))
    noImprovement = 0;
    return true;
  }
  return false;
}

void run_brkga(const RProblem& problem, const BRKGAParams& params, int seed,
               ValuedChromosome& bestFoundSolution, const Scenario& other,
               int training, std::optional<Vec<chromosome>> initialSolution) {
  // problem
  // int nVars = problem.nVars;
  // int nConst = problem.nConst;
  // int tests = problem.tests;
  // const Vec<Vec<double>>& inputs = problem.inputs;
  // const Vec<double>& outputs = problem.outputs;
  // const Vec<Pair<double, double>>& vConst = problem.vConst;
  // ------
  // params
  // ------
  int restartMax = params.restartMax;
  int noImprovementMax = params.noImprovementMax;
  int eliteSize = params.eliteSize;
  int mutantSize = params.mutantSize;
  uint16_t eliteBias = params.eliteBias;
  int populationLen = params.populationLen;
  // -----
  // other
  // -----
  // auto& operationsBi = other.operationsBi;
  // auto& operationsU = other.operationsU;
  int individualLen = other.individualLen;
  // int stackLen = other.stackLen;
  // int maxConst = other.maxConst;
  // int operationsBiLen = operationsBi.size();
  // int operationsULen = operationsU.size();

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
  // int noImprovement = 5000;
  bool end = false;
  int restart = 0;
  while (restart < restartMax) {
    seed++;
    populationGenerator(mainPopulation, seed);
    if (initialSolution) {
      // if exists 'initialSolution'
      mainPopulation[0].randomKeys = *initialSolution;
      // TODO: what to do with 'seed' here? seed=-1? flag as unused?
      initialSolution = std::nullopt;  // disable optional input
    }
    decoder(mainPopulation, problem, other, seed);
    std::sort(mainPopulation.begin(), mainPopulation.end(), menorQue);
    end = false;
    while (!end) {
      // for (int i = 0; i < (int)auxPopulation.size(); i++) {
      //   auxPopulation[i].randomKeys = mainPopulation[i].randomKeys;
      //   auxPopulation[i].cost = mainPopulation[i].cost;
      // }
      assert(auxPopulation.size() == mainPopulation.size());
      // copy mainPopulation into auxPopulation
      auxPopulation = mainPopulation;
      // use 'noImprovement' to grow mutation
      mutationGrow = percentToInt(5 * (5 * (noImprovement / noImprovementMax)),
                                  populationLen);
      // [0 ... eliteSize] is elite
      // [eliteSize to mutantSize+mutationGrow] is mutant
      mutantGenerator(auxPopulation, eliteSize, (mutantSize + mutationGrow),
                      seed);
      crossover(mainPopulation, auxPopulation, eliteSize,
                (mutantSize + mutationGrow), eliteBias, seed);
      seed += 2 * individualLen;
      decoder(auxPopulation, problem, other, seed);
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
      //
      // bestFoundSolution.cost = mainPopulation[0].cost;
      // bestFoundSolution.randomKeys = mainPopulation[0].randomKeys;
      bestFoundSolution = mainPopulation[0];
      //
      // printf("Chegou\n");
      restart = 0;
    }
    // printf("Chegou");
    printf("restart = %d \t best= %f\t", restart, bestFoundSolution.cost);
    printSolution(problem, bestFoundSolution.randomKeys, other);
    restart++;
  }
}
