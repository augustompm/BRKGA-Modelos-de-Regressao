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

void compactIndividual(Vec<chromosome>& individual, int stackLen, long seed,
                       int _k = -1) {
  srand(seed);
  // std::cout << "compactIndividual!" << std::endl;
  for (int i = 0; i < stackLen; i++) {
    if (isOperation(individual[i], OpType::SPECIAL)) {
      // std::cout << "found empty i=" << i << std::endl;
      for (int j = i; j < stackLen; j++) {
        individual[j] = individual[j + 1];
        individual[1 * stackLen + j] = individual[1 * stackLen + j + 1];
        individual[2 * stackLen + j] = individual[2 * stackLen + j + 1];
      }
      individual[stackLen - 1] = 9999;
      assert(isOperation(individual[stackLen - 1], OpType::SPECIAL));
    }
  }
  // count empty spaces
  int countEmpty = 0;
  for (int i = 0; i < stackLen; i++)
    if (isOperation(individual[i], OpType::SPECIAL)) countEmpty++;
  // std::cout << "countEmpty=" << countEmpty << std::endl;
  //
  if (countEmpty < 2) {
    // std::cout << "WARNING: no space for mutation! at least two..." <<
    // std::endl;
    return;
  }
  //
  std::vector<int> pushes;
  for (int i = 0; i < stackLen; i++)
    if (isOperation(individual[i], OpType::PUSH)) pushes.push_back(i);
  // std::cout << "pushes = " << pushes.size() << std::endl;
  //
  if (pushes.size() == 0) {
    // std::cout << "WARNING: no push for mutation!" << std::endl;
    return;
  }

  int k = rand() % pushes.size();
  if (_k >= 0) k = _k;  // manual k
  int i = pushes[k];
  // Mutation: InsertBinaryOp after i+1... so, must PUSH then BIN
  // Step 1: create two slots of space.
  for (int j = stackLen - 2; j > (i + 1); j--) {
    individual[j] = individual[j - 1];
    individual[stackLen + j] = individual[stackLen + j - 1];
    individual[2 * stackLen + j] = individual[2 * stackLen + j - 1];
  }
  for (int j = stackLen - 1; j > (i + 2); j--) {
    individual[j] = individual[j - 1];
    individual[stackLen + j] = individual[stackLen + j - 1];
    individual[2 * stackLen + j] = individual[2 * stackLen + j - 1];
  }
  // now i+1 and i+2 are empty
  individual[i + 1] = 9999;
  individual[i + 2] = 9999;
  // i continues to be PUSH
  assert(isOperation(individual[i], OpType::PUSH));
  // i+1 will become push (the variable or const does not matter... so, repeat!)
  individual[i + 1] = 6000;
  individual[stackLen + i + 1] = individual[stackLen + i];
  // i+2 will become binary (the operation does not matter...)
  individual[i + 2] = 1250;
  individual[2 * stackLen + i + 2] = rand() % 10000;
  //
  // std::cout << "MUTATION: " << std::endl;
  // printSolution(problem, individual, other);
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
               int mutantSize, uint16_t eliteBias, int seed, bool doMutation,
               int stackLen, const RProblem& problem, const Scenario& other) {
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
    if (doMutation) {
      compactIndividual(auxPopulation[i].randomKeys, stackLen, seed);
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
  //
  int stackLen = other.getStackLen();
  int maxConst = other.maxConst;
  //
  for (int i = 0; i < (int)population.size(); i++) {
    // std::cout << "DEBUG: will decode i=" << i << std::endl;
    if (population[i].cost == 0) {
      // store seed used on stack adjustment...
      population[i].seed = seed;
      // 'stackAdjustment' modifica as randomKeys...

      // std::cout << "DEBUG: will enter stackAdjustment" << std::endl;
      // population[i].print();
      //
      StackInfo si =
          stackAdjustment(problem, other, population[i].randomKeys, stackLen,
                          nVars, nConst, maxConst, population[i].seed);
      population[i].trueStackSize = si.trueStackLen;
      // std::cout << "DEBUG: finished stackAdjustment" << std::endl;
      //  std::cout << "i=" << i << " trueStackSize=" <<
      //  population[i].trueStackSize
      //            << "/" << stackLen << " seed = " << seed << std::endl;
      int idSol = i;
      idSol = -1;  // DISABLE DEBUG
      population[i].cost =
          solutionEvaluator(problem, population[i].randomKeys, other, 0, idSol);
      //
      if (problem.hasUnits) {
        if (problem.outUnit != si.outUnit) {
          if (false) {
            std::cout << "WARNING: different output units!" << std::endl;
            std::cout << "Expected: '" << problem.outUnit << "' and got: '"
                      << si.outUnit << "'" << std::endl;
          }
          population[i].cost += 10000.0;  // PENALIDADE DE UNIDADE FINAL ERRADA!
        }
        // exponentially penalize if not all variables k are used
        // if DISABLED, W=1.0, so 1^k = 1.0
        if (si.usedVars < problem.nVars)
          population[i].cost *= ::pow(other.getWeightPerUnusedVariable(),
                                      (problem.nVars - si.usedVars));
      }
    }
  }
}

void run_brkga(const RProblem& problem, const BRKGAParams& params, int seed,
               ValuedChromosome& bestFoundSolution, const Scenario& other,
               int training, std::optional<Vec<chromosome>> initialSolution) {
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
  int individualLen = other.getIndividualLen();
  //
  Vec<ValuedChromosome> mainPopulation(populationLen);  // populationLen
  Vec<ValuedChromosome> auxPopulation(populationLen);
  // printf("populationLen1: %d eliteSize1: %d mutanteSize1: %d
  // individualLen1: %d stackLen1:
  // %d\n",populationLen,eliteSize,mutantSize,individualLen,stackLen);

  for (int i = 0; i < populationLen; i++) {
    auxPopulation[i].randomKeys = Vec<chromosome>(individualLen, 0);
    mainPopulation[i].randomKeys = Vec<chromosome>(individualLen, 0);
  }

  bestFoundSolution.cost = INFINITY;

  eliteSize = percentToInt(eliteSize, populationLen);
  mutantSize = percentToInt(mutantSize, populationLen);
  // printf("populationLen2: %d eliteSize2: %d mutanteSize2: %d
  // individualLen2: %d stackLen2:
  // %d\n",populationLen,eliteSize,mutantSize,individualLen,stackLen);
  // printf("%d\t%d\n",eliteSize,mutantSize);
  // 'noImprovement' is used to compute 'mutantGrow'
  int noImprovement = 0;
  // int noImprovement = 5000;
  bool end = false;
  int restart = 0;
  while (restart < restartMax) {
    // more diversity after X% iterations...
    bool doMutation = (restart > restartMax * (params.moreDiversity / 100.0));
    // std::cout << "restart=" << restart << std::endl;
    seed++;
    populationGenerator(mainPopulation, seed);
    if (initialSolution) {
      std::cout << "WARNING: BRKGP RECEIVED INITIAL SOLUTION!" << std::endl;
      // if exists 'initialSolution'
      mainPopulation[0].randomKeys = *initialSolution;
      // TODO: what to do with 'seed' here? seed=-1? flag as unused?
      initialSolution = std::nullopt;  // disable optional input
    }
    decoder(mainPopulation, problem, other, seed);
    if (false) {
      mainPopulation[0].print();
      std::cout << "after decode mainPopulation[0].cost: "
                << mainPopulation[0].cost << std::endl;
    }
    std::sort(mainPopulation.begin(), mainPopulation.end(), menorQue);
    if (false) {
      std::cout << "after sort mainPopulation[0].cost: "
                << mainPopulation[0].cost << std::endl;
    }
    end = false;
    while (!end) {
      assert(auxPopulation.size() == mainPopulation.size());  // NOLINT
      auxPopulation = mainPopulation;                         // copy population
      // use 'noImprovement' to grow mutation
      int mutationGrow = percentToInt(
          5 * (5 * (noImprovement / noImprovementMax)), populationLen);
      // [0 ... eliteSize] is elite

      if (doMutation) {
        int target = eliteSize + 1;
        // std::cout << "doMutation: BEST=";
        // printSolution(problem, bestFoundSolution.randomKeys, other);
        auxPopulation[target + 0] = bestFoundSolution;
        auxPopulation[target + 0].cost = 0;
        compactIndividual(auxPopulation[target + 0].randomKeys,
                          other.getStackLen(), seed);
        // std::cout << "doMutation: VARIANT=";
        // printSolution(problem, auxPopulation[target + 0].randomKeys, other);
      }

      // [eliteSize to mutantSize+mutationGrow] is mutant
      mutantGenerator(auxPopulation, eliteSize, (mutantSize + mutationGrow),
                      seed);
      if (doMutation) {
        int target = eliteSize + 1;
        // std::cout << "doMutation: BEST=";
        // printSolution(problem, bestFoundSolution.randomKeys, other);
        auxPopulation[target + 0] = bestFoundSolution;
        auxPopulation[target + 0].cost = 0;
        compactIndividual(auxPopulation[target + 0].randomKeys,
                          other.getStackLen(), seed);
        // std::cout << "doMutation: VARIANT=";
        // printSolution(problem, auxPopulation[target + 0].randomKeys, other);
      }
      crossover(mainPopulation, auxPopulation, eliteSize,
                (mutantSize + mutationGrow), eliteBias, seed, doMutation,
                other.getStackLen(), problem, other);

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
      const ValuedChromosome& newBest = auxPopulation[0];
      const ValuedChromosome& prevBest = mainPopulation[0];

      // check best (old method 'isRestart')
      if (newBest.cost < prevBest.cost)
        noImprovement = 0;
      else
        noImprovement++;
      if (noImprovement == noImprovementMax) {
        //|| (auxPopulation[0].cost <= 0.00001))
        noImprovement = 0;
        end = true;
      } else {
        end = false;
      }

      // printSolution(auxPopulation[20].randomKeys,stackLen,nVars,nConst,operationsBi,operationsU,vConstMin,vConstMax);
      // for (int i = 0; i < populationLen; i++) {
      //  mainPopulation[i].randomKeys = auxPopulation[i].randomKeys;
      //  mainPopulation[i].cost = auxPopulation[i].cost;
      //}
      mainPopulation = auxPopulation;  // copy
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
      std::cout << "BEST SOLUTION! cost=" << bestFoundSolution.cost << " ";
      printSolution(problem, bestFoundSolution.randomKeys, other);
      // SOME "INTENSIFICATION"?
      for (int i = 0; i < 1; i++) {
        auto sol2 = bestFoundSolution;
        compactIndividual(sol2.randomKeys, other.getStackLen(), seed);
        auto si = stackAdjustment(problem, other, sol2.randomKeys,
                                  other.getStackLen(), problem.nVars,
                                  problem.nConst, other.maxConst, 0);
        auto cost2 = solutionEvaluator(problem, sol2.randomKeys, other, 0, -1);
        sol2.cost = cost2;
        sol2.seed = 0;
        sol2.trueStackSize = si.trueStackLen;
        if ((bestFoundSolution.cost - sol2.cost) > 0.000001) {
          std::cout << "COST2 IS BETTER!!!" << cost2 << std::endl;
          bestFoundSolution = sol2;
          mainPopulation[0] = bestFoundSolution;
        }
      }
      //
      // printf("Chegou\n");
      restart = 0;
      //
    }
    // printf("Chegou");
    printf("restart = %d \t best= %f\t", restart, bestFoundSolution.cost);
    printSolution(problem, bestFoundSolution.randomKeys, other);
    restart++;
    if (bestFoundSolution.cost < 0.00001) {
      std::cout << "OPTIMAL FOUND! ABORTING..." << std::endl;
      break;
    }
  }
  std::cout << "FINISHED BRKGP!" << std::endl;
}
