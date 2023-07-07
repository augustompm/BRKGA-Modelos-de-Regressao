#pragma once

#include <cstdint>
#include <utility>
#include <vector>

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
  std::vector<char> operationsBi;
  std::vector<char> operationsU;

  // int operationsBiLen;
  // int operationsULen;

  // int training;
  int stackLen;
  int maxConst;
  int individualLen;
};
