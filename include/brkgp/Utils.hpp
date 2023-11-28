// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
//
#include <ginac.h>
using namespace GiNaC;
//
template <typename T>
using Vec = std::vector<T>;

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;

typedef uint16_t chromosome;

// Regression Problem Data
struct RProblem {
  int nVars{-1};
  int tests{-1};
  int nConst{-1};
  Vec<Vec<double>> inputs;
  Vec<double> outputs;
  Vec<Pair<double, double>> vConst;
  // units
  bool hasUnits;
  Vec<std::string> varUnits;
  std::string outUnit;
  Vec<std::string> constUnits;
  lst syms;  // ginac

  void setupUnits() {
    if (hasUnits) {
      for (unsigned i = 0; i < varUnits.size(); i++)
        syms.append(symbol{varUnits[i]});
      syms.append(symbol{outUnit});
      // remove duplicates! sort + unique
      syms.sort();
      syms.unique();
    }
  }
};

struct BRKGAParams {
  int populationLen = 30;
  int eliteSize = 25;
  int mutantSize = 5;
  uint16_t eliteBias = 70;
  int noImprovementMax = 10;
  // int restartMax = 1000;
  int restartMax = 10;
};

struct Scenario {
  // binárias tipo 1 (+, -): não podem com unidades distintas
  std::vector<char> operationsBiT1;
  // binárias tipo 2 (*, /): podem com unidades distintas
  std::vector<char> operationsBiT2;
  // concatenação de T1 e T2, nessa ordem
  std::vector<char> operationsBi;
  std::vector<char> operationsU;
  int stackLen;
  int maxConst;
  int individualLen;
};

class ValuedChromosome {
 public:
  Vec<chromosome> randomKeys;
  double cost;
  int trueStackSize;
  // seed parameter used during StackAdjustment!
  // Why seed?
  // - initial generation / mutant (OK)
  // - given input solution (PROBLEM)
  // - crossover solution (PROBLEM)
  // IDEA: seed could be used ONLY on StackAdjustment,
  // not for random key generation...
  int seed;

  void print() const {
    std::cout << "ValuedChromossome(cost=" << cost << "; seed=" << seed;
    std::cout << "; trueStackSize=" << trueStackSize
              << "; size=" << randomKeys.size() << "): " << std::endl;
    for (auto rk : randomKeys) std::cout << rk << " ";
    std::cout << std::endl;
  }
};
