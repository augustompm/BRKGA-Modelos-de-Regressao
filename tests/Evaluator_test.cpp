// SPDX-License-Identifier:  MIT
// Copyright (C) 2023

// #include <catch2/catch_all.hpp>
#include <catch2/catch_amalgamated.hpp>
//
#include <Scanner/Scanner.hpp>
#include <brkgp/ReadIO.hpp>

using namespace scannerpp;  // NOLINT

TEST_CASE("Teste1-X") {
  int x = 10;
  REQUIRE(x == 10);
}

TEST_CASE("testando leitura 1 2 1") {
  //
  RProblem problem;
  std::string str = "STANDARD\n1 2 1\n1.0 1.0\n2.0 2.0\n-10 10\n";
  Scanner scanner(str);
  readIO(problem, scanner);

  REQUIRE(problem.nVars == 1);
  REQUIRE(problem.tests == 2);
  REQUIRE(problem.nConst == 1);
}

TEST_CASE("montando solucao para distancia euclideana") {
  RKGenerator rkg;
  rkg.operationsBi = std::vector<char>({'+', '-', '*', '/'});
  rkg.operationsU = std::vector<char>({'i', 'r', 'a'});
  rkg.nVars = 4;
  rkg.nConst = 1;
  rkg.stackLen = 10;
  rkg.maxConst = 6;

  // Vec<chromosome> is_segI = {6000, 6000, 1000, 3500, 6000,
  //                            6000, 1000, 3500, 1000, 3500};
  Vec<chromosome> is_segI = {rkg.getRK(OpType::PUSH), rkg.getRK(OpType::PUSH),
                             rkg.getRK(OpType::BIN),  rkg.getRK(OpType::UN),
                             rkg.getRK(OpType::PUSH), rkg.getRK(OpType::PUSH),
                             rkg.getRK(OpType::BIN),  rkg.getRK(OpType::UN),
                             rkg.getRK(OpType::BIN),  rkg.getRK(OpType::UN)};
  Vec<chromosome> is_segII = {rkg.getRKvar(0),
                              rkg.getRKvar(1),
                              0,
                              0,
                              rkg.getRKvar(2),
                              rkg.getRKvar(3),
                              0,
                              0,
                              0,
                              0};
  Vec<chromosome> is_segIII = {0,
                               0,
                               rkg.getRKbi('-'),
                               rkg.getRKun('a'),
                               0,
                               0,
                               rkg.getRKbi('-'),
                               rkg.getRKun('a'),
                               rkg.getRKbi('+'),
                               rkg.getRKun('r')};

  Vec<chromosome> initialSol;
  initialSol.insert(initialSol.end(), is_segI.begin(), is_segI.end());
  initialSol.insert(initialSol.end(), is_segII.begin(), is_segII.end());
  initialSol.insert(initialSol.end(), is_segIII.begin(), is_segIII.end());
  Vec<chromosome> vMaxConst(rkg.maxConst, 0);
  initialSol.insert(initialSol.end(), vMaxConst.begin(), vMaxConst.end());
  initialSol.push_back(0);
  //

  //
  int individualLen = 3 * rkg.stackLen + rkg.maxConst + 1;
  REQUIRE((int)initialSol.size() == individualLen);
  //
  Vec<chromosome> initialTestSol = {
      6000, 6000, 1000, 3500, 6000, 6000, 1000, 3500, 1000, 3500, 3000, 5000, 0,
      0,    7000, 9000, 0,    0,    0,    0,    0,    0,    3750, 8332, 0,    0,
      3750, 8332, 1250, 4999, 0,    0,    0,    0,    0,    0,    0};

  REQUIRE(initialSol == initialTestSol);
  //
  auto sol = rkg.getRKexpr("v0 v1 - a v2 v3 - a + r");
  REQUIRE(sol == initialTestSol);
  //
  // TESTING 'SPECIAL/NOP' OPERATION
  //
  is_segI.push_back(9999);  // NOP
  is_segII.push_back(0);    // whatever
  is_segIII.push_back(0);   // whatever
  Vec<chromosome> initialSol2;
  initialSol2.insert(initialSol2.end(), is_segI.begin(), is_segI.end());
  initialSol2.insert(initialSol2.end(), is_segII.begin(), is_segII.end());
  initialSol2.insert(initialSol2.end(), is_segIII.begin(), is_segIII.end());
  initialSol2.insert(initialSol2.end(), vMaxConst.begin(), vMaxConst.end());
  initialSol2.push_back(0);
  // ======================================
  // increase stackLen to generate last NOP
  rkg.stackLen++;
  auto sol2 = rkg.getRKexpr("v0 v1 - a v2 v3 - a + r");
  REQUIRE(sol2[rkg.stackLen - 1] == 9999);
  REQUIRE(sol2 == initialSol2);
  // stack adjustment should not change this good solution
  RProblem problem;          // gambiarra!
  Scenario other;            // gambiarra!
  problem.hasUnits = false;  // gambiarra!
  StackInfo si = stackAdjustment(problem, other, sol2, rkg.stackLen, rkg.nVars,
                                 rkg.nConst, rkg.maxConst, 0);
  int usedOps = si.trueStackLen;
  REQUIRE(usedOps == rkg.stackLen - 1);
  REQUIRE(sol2 == initialSol2);
}