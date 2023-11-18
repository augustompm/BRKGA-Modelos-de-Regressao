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
