// SPDX-License-Identifier: MIT
// Copyright (C) 2023

#pragma once

// C
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// C++
#include <optional>
#include <stack>
#include <string>
#include <vector>
//
#include <Scanner/Scanner.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <kahan-float/kahan.hpp>

template <class T>
using opt = std::optional<T>;

enum class OpType { BIN, UN, PUSH, SPECIAL };

bool isOperation(int rk, OpType op) {
  // BIN:  [0, 2500)
  if ((op == OpType::BIN) && (rk < 2500) && (rk >= 0)) return true;
  // UN:  [2500, 5000)
  if ((op == OpType::UN) && (rk < 5000) && (rk >= 2500)) return true;
  // PUSH:  [5000, 7500)
  if ((op == OpType::PUSH) && (rk < 7500) && (rk >= 5000)) return true;
  // SPECIAL:  [7500, 10000)
  if ((op == OpType::SPECIAL) && (rk < 10000) && (rk >= 7500)) return true;
  return false;
}

int makePUSH(int rk) {
  if (isOperation(rk, OpType::BIN)) {
    // Se BIN (<2500) vira PUSH
    return rk + 5000;
  } else if (isOperation(rk, OpType::SPECIAL)) {
    // Se SPECIAL (>=7500) vira PUSH
    return rk - 2500;
  } else {
    // Se UN vira PUSH
    return rk + 2500;
  }
}

opt<double> execBinaryOp(int idop, double v1, double v2,
                         const std::vector<char>& operationsBi) {
  if (operationsBi[idop] == '+') return std::make_optional<double>(v1 + v2);
  if (operationsBi[idop] == '-') return std::make_optional<double>(v1 - v2);
  if (operationsBi[idop] == '*') return std::make_optional<double>(v1 * v2);
  if (operationsBi[idop] == '/') {
    if (v2 < 0.000001)
      return std::nullopt;
    else
      return std::make_optional<double>(v1 / v2);
  }
  return std::nullopt;
}

opt<double> execUnaryOp(int idop, double v1,
                        const std::vector<char>& operationsU) {
  if (operationsU[idop] == 's') return std::make_optional<double>(::sin(v1));
  if (operationsU[idop] == 'c') return std::make_optional<double>(cos(v1));
  if (operationsU[idop] == 'i') return std::make_optional<double>(v1);
  if (operationsU[idop] == 'a') return std::make_optional<double>(v1 * v1);
  if (operationsU[idop] == 'v') return std::make_optional<double>(v1 * v1 * v1);
  if (operationsU[idop] == 'r') {
    if (v1 < 0)
      return std::nullopt;
    else
      // return std::make_optional<double>(::pow(v1, 1.0 / 2.0));
      return std::make_optional<double>(::sqrt(v1));
  }
  if (operationsU[idop] == 'e')
    return std::make_optional<double>(::exp(v1));  // e^x
  if (operationsU[idop] == 'n')
    return std::make_optional<double>(::log(v1));  // ln(x)
  if (operationsU[idop] == 'p')
    return std::make_optional<double>(::pow(2.0, v1));  // 2^x
  if (operationsU[idop] == 'l')
    return std::make_optional<double>(::log2(v1));  // log_2(x)

  return std::nullopt;
}

// error between values v1 and v2 RMSE
// 'problem' informs mode:
//   * isSquared activated (force square root on both sides to correct it)
double computeError(const RProblem& problem, double v1, double v2) {
  if (problem.isSquared()) {
    // must apply square root on both sides, to correct "squared" nature!
    // v1 = ::sqrt(v1);
    // v2 = ::sqrt(v2);
  }
  // RMSE
  return ::sqrt((v1 - v2) * (v1 - v2));
}

// ============

opt<ex> execBinaryOpUnit(const RProblem& problem, int idop, ex v1Unit,
                         ex v2Unit, const std::vector<char>& operationsBi) {
  // std::cout << "DEBUG: execBinaryOpUnit = " << idop << std::endl;
  if ((operationsBi[idop] == '+') || (operationsBi[idop] == '-')) {
    // std::cout << "DEBUG: binary + ou -" << std::endl;
    if (v1Unit == v2Unit) {
      return std::make_optional<ex>(v1Unit);
    } else {
      std::cout << "WARNING: different UNITS in binary op!" << std::endl;
      return std::nullopt;
    }
  }
  if ((operationsBi[idop] == '*') || (operationsBi[idop] == '/')) {
    // std::cout << "DEBUG: execBinaryOpUnit => * ou / " << std::endl;
    // std::cout << "DEBUG: execBinaryOpUnit =>  vai computar e1" << std::endl;
    //  ex e1 = ex(v1Unit, problem.syms);
    ex e1 = v1Unit;
    // std::cout << "DEBUG: execBinaryOpUnit =>  vai computar e2" << std::endl;
    //  ex e2 = ex(v2Unit, problem.syms);
    ex e2 = v2Unit;
    // std::cout << "DEBUG: execBinaryOpUnit =>  vai fazer assert " <<
    // std::endl;
    assert(e1 != 0);  // never is ZERO
    assert(e2 != 0);  // never is ZERO
    // std::cout << "DEBUG: execBinaryOpUnit =>  vai computar e3 " << std::endl;
    ex ex_one = ex("1", problem.syms);
    ex e3 = ex_one;
    if (operationsBi[idop] == '*') e3 = e1 * e2;
    if (operationsBi[idop] == '/') e3 = e1 / e2;
    // check if it's adimensional ("numeric")
    // example: 2m / 1m = 2;  1m * (1/2m) = 1/2;
    assert(e3 != 0);  // never is ZERO
    // std::cout << "DEBUG: execBinaryOpUnit => e3:" << e3 << std::endl;
    bool bnumeric = false;
    if (is_exactly_a<numeric>(e3)) bnumeric = true;
    // get text
    // std::stringstream ss;
    // ss << e3;
    if (bnumeric)
      return std::make_optional<ex>(ex_one);  // unit
    else
      return std::make_optional<ex>(e3);
  }
  return std::nullopt;
}

// fix expression e, after sqrt... according to list of symbols
// Example: sqrt{m^2} => m; sqrt{1/m^4} => 1/m²; 1/sqrt{m²} => 1/m
bool fix_sqrt(ex& e, const lst& syms) {
  for (auto& s : syms) {
    for (int i = 2; i < 20; i += 2)
      if (pow(e, 2) == pow(s, i)) {
        e = pow(s, i / 2);
        return true;
      }
    for (int i = 2; i < 20; i += 2)
      if (pow(e, 2) == 1 / pow(s, i)) {
        e = 1 / pow(s, i / 2);
        return true;
      }
  }
  return false;
}

opt<ex> execUnaryOpUnit(const RProblem& problem, int idop, ex v1Unit,
                        const std::vector<char>& operationsU) {
  // RAD input -> adimensional output
  // if (operationsU[idop] == 's') return std::make_optional<double>(::sin(v1));
  // if (operationsU[idop] == 'c') return std::make_optional<double>(::cos(v1));
  //
  if (operationsU[idop] == 'i') {
    return std::make_optional<ex>(v1Unit);
  }
  if ((operationsU[idop] == 'a') || (operationsU[idop] == 'v') ||
      (operationsU[idop] == 'r') || (operationsU[idop] == 'e') ||
      (operationsU[idop] == 'n') || (operationsU[idop] == 'p') ||
      (operationsU[idop] == 'l')) {
    // ex e1 = ex(v1Unit, problem.syms);
    ex e1 = v1Unit;
    ex e_out = ex("1", problem.syms);
    if (operationsU[idop] == 'a') e_out = e1 * e1;
    if (operationsU[idop] == 'v') e_out = e1 * e1 * e1;
    if (operationsU[idop] == 'r') {
      e_out = sqrt(e1);
      // try to simplify sqrt... not perfect! TODO: improve!
      fix_sqrt(e_out, problem.syms);
    }
    if (operationsU[idop] == 'e') e_out = exp(e1);               // e^x
    if (operationsU[idop] == 'n') e_out = log(e1);               // ln(x)
    if (operationsU[idop] == 'p') e_out = pow(2.0, e1);          // 2^x
    if (operationsU[idop] == 'l') e_out = log(e1) / log(ex{2});  // log_2(x)
    // get text
    // std::stringstream ss;
    // ss << e_out;
    // std::cout << "DEBUG: Unary: " << ss.str() << std::endl;
    // ex new_ex(ss.str(), problem.syms);  // ERRO!
    //
    return std::make_optional<ex>(e_out);
  }
  return std::nullopt;
}

// stackAdjustment: ajusta chaves aleatórias do indivíduo, caso seja
// impossível decodificá-lo! Por exemplo, operações binárias seguidas
// sem operandos disponíveis na pilha!
// IDEIA: usar realStackValue para representar o fim do indivíduo, ao
// invés de ajustá-lo!
// EXEMPLO: fazer vetor simples para x² (só precisa de stackLen=2)!
// IDEIA 2: QUANDO PARAR DECODIFICAÇÃO?
//  - Hipótese: quando stack size = 1?
//  - TALVEZ... Marcador de chave aleatória que só é ativado se stack size = 1.
// IDEIA 3: INVOCAR 'mathomatic' para simplificar equações... Fazer só no Best?
// IDEIA 4: USAR UNIDADE SIMBOLICA COMO CRITERIO DE SIMPLICIDADE AO LONGO DA
// EXECUCAO...
//

// stack information
struct StackInfo {
  int trueStackLen;
  std::string outUnit;
  int usedVars;
  int firstFix{-1};
};

StackInfo stackAdjustment(const RProblem& problem, const Scenario& other,
                          Vec<chromosome>& individual, int stackLen, int nVars,
                          int nConst, int maxConst, int seed) {
  int stackCount = 0;
  int contConst = 0;
  // Significado: número de operações "boas" (não SPECIAL/NOP)
  int trueStackLen = 0;
  //
  // unit on stack element; example: meter, second, ...
  std::stack<ex> stkUnit;         // stack for units!!
  int firstFix = 100 * stackLen;  // BIG NUMBER!!!
  int usedVars = 0;
  std::vector<bool> vUsedVars(problem.nVars, false);
  //
  for (int i = 0; i < stackLen; i++) {
    // std::cout << "DEBUG: stackAdjustment i=" << i << std::endl;
    // std::cout << "individual[i]=" << individual[i] << std::endl;
    //
    // invariante: 'stackCount' sempre >= 0
    assert(stackCount >= 0);
    //
    int stackDiff = 2;  // DEFAULT = 2 (FLAG)
    // Binary: remove two elements and add one
    // Unary: remove one element and add one
    // Push: add one
    if (isOperation(individual[i], OpType::BIN)) stackDiff = -1;
    if (isOperation(individual[i], OpType::UN)) stackDiff = 0;
    if (isOperation(individual[i], OpType::PUSH)) stackDiff = 1;
    //
    // varConstInt: [0, ..., var+const)
    int varConstInt =
        ::floor((individual[stackLen + i] / 10000.0) * (nVars + nConst));
    // idConstOrVar: constant is negative, var is non-negative
    int idConstOrVar = varConstInt - nConst;
    // stackCountAfterOperation é cópia local do stackCount
    // aplica operação no 'stackCountAfterOperation'
    int stackCountAfterOperation = stackCount;
    //
    // if not flag, update 'stackCountAfterOperation'
    if (stackDiff != 2) stackCountAfterOperation += stackDiff;

    // < 1 => ERRO: EXIGE CORREÇÃO!
    // SIGNIFICA: NENHUM OPERANDO APÓS OPERAÇÃO!
    // OU ESTAVA VAZIO, E VEIO UM 'SPECIAL' (NOP) - ERRO! VIRA PUSH!
    //    - Não permitido começar com NOP! ("NOP à esquerda")
    // OU TINHA UM, E VEIO UMA BINARIA - ERRO! VIRA PUSH!
    // OU ESTAVA VAZIO, E VEIO UMA UNARIA - ERRO! VIRA PUSH!
    //
    // CONSERTO: Transforma em PUSH a operação!
    if (stackCountAfterOperation < 1) {
      if (i < firstFix) firstFix = i;
      individual[i] = makePUSH(individual[i]);
      assert(isOperation(individual[i], OpType::PUSH));
      stackCountAfterOperation = stackCount + 1;
    }

    // INVARIANTE Importante: Sempre alguém na pilha após "conserto"
    assert(stackCountAfterOperation >= 1);

    // assert(auxSomador == somador); // errado!

    // ===============================================================
    // TOO MANY ELEMENTS IN STACK: MUST USE BINARY OPS TO REMOVE THEM!
    // ===============================================================
    // | PUSH | PUSH | PUSH |  ?  |  ?  |
    //    0      1      i=2    3     4
    // STRANGE... Must test if it covers all cases...
    // stackCountAfterOperation=3  >  (5 - 2) = 3   (?)
    if (stackCountAfterOperation > (stackLen - i)) {
      if (stackCount == 1) {
        if (i < firstFix) firstFix = i;
        // std::cout << "WARNING! ALTERANDO individual COD3!" << std::endl;
        individual[i] += 2500;
        stackCountAfterOperation = 1;
      } else if (stackCount > 1) {
        if (isOperation(individual[i], OpType::UN)) {
          // std::cout << "WARNING! ALTERANDO individual COD4!" << std::endl;
          // TODO: transforma em UN, já que não pode ser BIN?
          if (i < firstFix) firstFix = i;
          individual[i] -= 2500;

        } else if (isOperation(individual[i], OpType::SPECIAL)) {
          // std::cout << "WARNING! ALTERANDO individual COD5!" << std::endl;
          if (i < firstFix) firstFix = i;
          individual[i] -= 7500;
        } else {  // se der testar depois, no caso i=0
          // std::cout << "WARNING! ALTERANDO individual COD6!" << std::endl;
          if (i < firstFix) firstFix = i;
          individual[i] -= 5000;
        }
        stackCountAfterOperation = stackCount - 1;
      }
    }

    // SD1 (PUSH) + Constant
    if ((stackDiff == 1) && idConstOrVar < 0) {
      contConst++;
      if (contConst > maxConst) {
        srand(seed);
        // std::cout << "WARNING! ALTERANDO individual COD7!" << std::endl;
        if ((i) < firstFix) firstFix = i;
        individual[stackLen + i] =
            ((10000.0 / (nVars + nConst)) * nConst) +
            ((10000.0 / (nVars + nConst)) * (rand() % nVars) + 1);  // NOLINT
        seed++;
      }
    }
    //
    stackCount = stackCountAfterOperation;
    // CONTA OPERAÇÕES "BOAS"/"ÚTEIS"
    if (!isOperation(individual[i], OpType::SPECIAL)) trueStackLen++;
    // ===========================================
    // OPERAÇÕES JÁ CORRIGIDAS! VERIFICAR UNIDADES
    // -------------------------------------------
    if (problem.hasUnits) {
      // std::cout << "DEBUG: hasUnits!  i=" << i << std::endl;
      // problem instance SUPPORTS units!
      int j = i;  // workaround
      // std::cout << "individual[j]=" << individual[j] << std::endl;
      //
      if (isOperation(individual[j], OpType::PUSH)) {
        // push variable or constant [5000,7500)
        int my_floor =
            ::floor((individual[stackLen + j] / 10000.0) * (nVars + nConst));
        int idVar = my_floor - nConst;
        // std::cout << "DEBUG: idVar = " << idVar << std::endl;
        std::string varUnit = "_";  // adimensional?
        // push variable
        if (idVar >= 0) {
          if (!vUsedVars[idVar]) {
            usedVars++;
            vUsedVars[idVar] = true;
          }
          varUnit = problem.varUnits[idVar];
          ex exVar(varUnit, problem.syms);
          stkUnit.push(exVar);
        } else {
          // push constant
          idVar += nConst;
          if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
            varUnit = problem.constUnits[idVar];
            if (varUnit == "*") varUnit = "1";  // "*" means adimensional
            ex exVar(varUnit, problem.syms);
            stkUnit.push(exVar);
          } else {
            std::cout << "ERROR! unsupported units on interval constants!"
                      << std::endl;
            assert(false);
          }
        }
      }  // end-check units for PUSH
      if (isOperation(individual[j], OpType::BIN)) {
        // pop binary operation
        int idOpBi = floor(
            (individual[2 * stackLen + j] / 10000.0) *
            (double)other.operationsBi.size());  // 4 is lenght of operationBi
        assert(stkUnit.size() >= 2);
        ex v1Unit = stkUnit.top();
        stkUnit.pop();
        ex v2Unit = stkUnit.top();
        stkUnit.pop();
        // std::cout << "DEBUG: v1Unit: " << v1Unit << " v2Unit: " << v2Unit <<
        // std::endl;
        // CHECA UNIDADES!!! FASE 2!!!
        if (v1Unit != v2Unit) {
          // unidades diferentes!! Só permite em binárias TIPO 2, não TIPO 1
          // verifica se é TIPO 1, se for, vira TIPO 2
          if (idOpBi < (int)other.operationsBiT1.size()) {
            // TIPO 1! Exemplo, idOpB1 = 0... 1... e |BiT1|=2
            // Muda "gene"... += 2 ops...
            if ((j) < firstFix) firstFix = j;
            individual[2 * stackLen + j] =
                individual[2 * stackLen + j] +
                (chromosome)((10000.0 / (double)other.operationsBi.size()) *
                             (double)other.operationsBiT1.size());
            // lê chromossomo novamente! PARA GARANTIR!
            idOpBi = floor((individual[2 * stackLen + j] / 10000.0) *
                           (double)other.operationsBi
                               .size());  // 4 is lenght of operationBi
            // virou operação do TIPO 2
            assert(idOpBi >= (int)other.operationsBiT1.size());
          }
        }  // ok fix binary

        // std::cout << "DEBUG: WILL EXEC v1Unit: " << v1Unit
        //            << " v2Unit: " << v2Unit << " OP = " << idOpBi <<
        //            std::endl;
        //  RETORNAR opcional VAZIO!
        opt<ex> binResult = execBinaryOpUnit(problem, idOpBi, v1Unit, v2Unit,
                                             other.operationsBi);
        // error in binary operation
        if (!binResult) {
          std::cout << "ERROR: binary operation UNIT failed!" << std::endl;
          std::cout << "DEBUG: v1Unit: " << v1Unit << " v2Unit: " << v2Unit
                    << " OP = " << idOpBi << std::endl;
          assert(false);
        } else {
          stkUnit.push(*binResult);
        }
      }  // end-check units for Binary
      if (isOperation(individual[j], OpType::UN)) {
        // pop: Unary Operation [2500, 5000)
        int idOpU = floor(
            (individual[2 * stackLen + j] / 10000.0) *
            (double)other.operationsU.size());  // 3 is lenght of operationU
        // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
        //
        assert(stkUnit.size() >= 1);
        ex v1Unit = stkUnit.top();
        stkUnit.pop();

        // no fix necessary!

        opt<ex> unResult =
            execUnaryOpUnit(problem, idOpU, v1Unit, other.operationsU);

        // error in unary operation
        if (!unResult) {
          assert(false);
        } else {
          stkUnit.push(*unResult);
        }
      }
      if (isOperation(individual[j], OpType::SPECIAL)) {
        // special (currently: NOP): [7500, 10000)
        // nothing to do...
      }
    }  // end-if problem.hasUnits
  }

  std::string outUnit = "";
  if (problem.hasUnits) {
    assert(stkUnit.size() >= 1);  // TODO: improve!
    std::stringstream ss;
    ex last = stkUnit.top();
    stkUnit.pop();
    ss << last;
    outUnit = ss.str();
  }

  return StackInfo{trueStackLen, outUnit, usedVars, firstFix};
}

double solutionEvaluator(const RProblem& problem,
                         const Vec<chromosome>& individual,
                         const Scenario& other, int training, int idSol) {
  // idSol: Identifier of Solution in Population (Good for DEBUG!)
  // problem counters
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  // scenario
  const int stackLen = other.getStackLen();
  // local variables
  kahan::kfloat64 sum_error = 0;
  if (idSol == 0) {
    std::cout << "DEBUG[idSol=0] BEGIN sum_error:" << sum_error << std::endl;
  }
  //
  const int realTests = problem.tests - training;
  for (int t = 0; t < realTests; t++) {
    std::stack<double> stk;
    int contSeed = 0;

    for (int j = 0; j < stackLen; j++) {
      //
      if (isOperation(individual[j], OpType::BIN)) {
        // case: pop Binary Operation [0, 2500)
        // pop operation
        int idOpBi = floor(
            (individual[2 * stackLen + j] / 10000.0) *
            (double)other.operationsBi.size());  // 4 is lenght of operationBi
        // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
        //
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] idOpBi=" << idOpBi << std::endl;
        }
        assert(stk.size() >= 2);
        double v1 = stk.top();
        stk.pop();
        double v2 = stk.top();
        stk.pop();
        // RETORNAR opcional VAZIO!
        opt<double> binResult =
            execBinaryOp(idOpBi, v1, v2, other.operationsBi);
        // error in binary operation
        if (!binResult) {
          // LIMPA PILHA E ABORTA TESTE!
          while (stk.size() > 0) stk.pop();
          stk.push(INFINITY);
          break;
        } else {
          if (idSol == 0 && t == 0)
            std::cout << "DEBUG[idSol=0] bin result: " << *binResult
                      << std::endl;
          stk.push(*binResult);
        }
      } else if (isOperation(individual[j], OpType::UN)) {
        // pop: Unary Operation [2500, 5000)
        int idOpU = floor(
            (individual[2 * stackLen + j] / 10000.0) *
            (double)other.operationsU.size());  // 3 is lenght of operationU
        // assert(idOp != -1); // guarantee that it's not "disabled" (-1)
        //
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] idOpU=" << idOpU << std::endl;
        }
        assert(stk.size() >= 1);
        double v1 = stk.top();
        stk.pop();

        opt<double> unResult = execUnaryOp(idOpU, v1, other.operationsU);

        // error in unary operation
        if (!unResult) {
          // LIMPA PILHA E ABORTA TESTE!
          while (stk.size() > 0) stk.pop();
          stk.push(INFINITY);
          break;
        } else {
          if (idSol == 0 && t == 0)
            std::cout << "DEBUG[idSol=0] un result: " << *unResult << std::endl;

          stk.push(*unResult);
        }
      } else if (isOperation(individual[j], OpType::PUSH)) {
        // push variable or constant [5000,7500)
        int my_floor =
            ::floor((individual[stackLen + j] / 10000.0) * (nVars + nConst));
        int idVar = my_floor - nConst;
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] idVar=" << idVar << std::endl;
        }
        // printf("%d\n",idVar);
        double varValue = 0;
        // push variable
        if (idVar >= 0) {
          varValue = problem.inputs[t][idVar];
          stk.push(varValue);
          if (idSol == 0 && t == 0) {
            std::cout << "DEBUG[idSol=0] pushvar: " << varValue << std::endl;
          }
        } else {
          // push constant
          idVar += nConst;
          if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
            varValue = problem.vConst[idVar].first;
            stk.push(varValue);
          } else {
            chromosome seedConst = individual[3 * stackLen + contSeed];
            srand(seedConst);
            // NOLINTNEXTLINE
            varValue = rand() % (int)(problem.vConst[idVar].second -
                                      problem.vConst[idVar].first + 1) +
                       problem.vConst[idVar].first;
            stk.push(varValue);
            contSeed++;
          }
        }
      } else if (isOperation(individual[j], OpType::SPECIAL)) {
        // special (currently: NOP): [7500, 10000)
        if (idSol == 0 && t == 0) {
          std::cout << "DEBUG[idSol=0] NOP" << std::endl;
        }
        //
        // cout << "i=" << i << " -> stack size = " << stk.size() << endl;
      } else {
        std::cout << "ERROR! UNKNOWN OPERATION " << individual[j] << std::endl;
        return INFINITY;
      }

      //
      // cout << "finished t= " << t << endl;
      // take value from stack
      // assert(stk.size() == 1);
      //
    }  // for j < stackLen

    assert(stk.size() == 1);
    double val = stk.top();
    stk.pop();
    //
    // compare with expected value

    if (idSol == 0 && t == 0) {
      std::cout << "DEBUG[idSol=0] val:" << val << std::endl;
      std::cout << "DEBUG[idSol=0] expected:" << problem.outputs[t]
                << std::endl;
      std::cout << "DEBUG[idSol=0] BEFORE sum_error:" << sum_error << std::endl;
    }

    sum_error += computeError(problem, val, problem.outputs[t]);

    if (idSol == 0 && t == 0) {
      std::cout << "DEBUG[idSol=0] AFTER sum_error:" << sum_error << std::endl;
    }
    // printf("sum_error = %lf\n",sum_error);
  }
  // average erro
  double solutionValue = (double)sum_error / realTests;

  if (idSol == 0) {
    std::cout << "DEBUG[idSol=0] Final sum_error:" << sum_error << std::endl;
    std::cout << "DEBUG[idSol=0] Final solutionValue:" << solutionValue
              << std::endl;
  }
  // printf("solutionValue = %lf\n",solutionValue);
  // printf("sum_error = %f solutionValue = %f\n",sum_error,solutionValue);
  // printf("%f\n",solutionValue);
  return solutionValue;
}

// ===========
// RKGenerator
// -----------

struct RKGenerator {
  std::vector<char> operationsBi;
  std::vector<char> operationsU;
  int nVars{0};
  int nConst{0};
  int stackLen{0};
  int maxConst{0};

  int inVStr(std::string s, std::vector<std::string> v) {
    for (int idx = 0; idx < (int)v.size(); idx++)
      if (s == v[idx]) return idx;
    return -1;
  }

  // INCREMENTAL check for stackAdjustment.
  // INVARIANT IS: should never change last element i...
  int checkRKexpr(std::string optimal, const RProblem& problem,
                  const Scenario& other) {
    std::cout << "checkRKexpr(optimal='" << optimal << "')" << std::endl;
    int countElems = 0;
    {
      Scanner scanner(optimal);
      while (scanner.hasNext()) {
        scanner.next();
        countElems++;
      }
    }

    std::cout << "checkRKexpr countElems = " << countElems
              << " stackLen = " << other.getStackLen() << std::endl;
    assert(other.getStackLen() >= countElems);

    for (int i = 1; i <= countElems; i++) {
      std::stringstream partialSol;
      int count = 0;
      Scanner scanner(optimal);
      while (count < i) {
        partialSol << scanner.next() << " ";
        count++;
      }
      std::string target = Scanner::trim(partialSol.str());
      std::cout << "i=" << i << " target = '" << target << "'" << std::endl;
      //
      auto testSol = getRKexpr(target);
      StackInfo si =
          stackAdjustment(problem, other, testSol, other.getStackLen(),
                          problem.nVars, problem.nConst, other.maxConst, 0);
      std::cout << "StackInfo firstFix=" << si.firstFix << std::endl;
      assert(si.firstFix > i);
    }

    return countElems;
  }

  Vec<chromosome> getRKexpr(std::string expr) {
    std::vector<std::string> sopsBi;
    std::vector<std::string> sopsU;
    std::vector<std::string> svars;
    std::vector<std::string> sconsts;
    for (auto& i : operationsBi) sopsBi.push_back(std::string{i});
    for (auto& i : operationsU) sopsU.push_back(std::string{i});
    for (int id = 0; id < nVars; id++) {
      std::stringstream ss;
      ss << "v" << id;
      svars.push_back(ss.str());
    }

    int individualLen = 3 * stackLen + maxConst + 1;

    Vec<chromosome> v(individualLen, 0);
    scannerpp::Scanner scanner{expr};

    int idx = 0;
    while (scanner.hasNext()) {
      std::string op = scanner.next();
      if (inVStr(op, svars) >= 0) {
        v[idx] = getRK(OpType::PUSH);
        v[idx + stackLen] = getRKvar(inVStr(op, svars));
        idx++;
      }
      if (inVStr(op, sopsBi) >= 0) {
        v[idx] = getRK(OpType::BIN);
        v[idx + 2 * stackLen] = getRKbi(op[0]);
        idx++;
      }
      if (inVStr(op, sopsU) >= 0) {
        v[idx] = getRK(OpType::UN);
        v[idx + 2 * stackLen] = getRKun(op[0]);
        idx++;
      }
      if (inVStr(op, sconsts) >= 0) {
        std::cout << "WARNING: UNSUPORTED CONSTANTS FOR NOW!!" << std::endl;
        return Vec<chromosome>{};
      }
      if (idx > stackLen) {
        std::cout << "WARNING: PROBLEM IN PARSING! idx > stackLen: " << idx
                  << std::endl;
        return Vec<chromosome>{};
      }
    }
    // fill the rest with NOP's
    while (idx < stackLen) {
      v[idx] = getRK(OpType::SPECIAL);
      idx++;
    }

    return v;
  }

  uint16_t getRK(OpType op) {
    switch (op) {
      case OpType::BIN:
        return 1000;
      case OpType::UN:
        return 3500;
      case OpType::PUSH:
        return 6000;
      default:
        return 9999;
    }
  }

  uint16_t getRKvar(int var) {
    int segSize = 10000 / (nVars + nConst);
    int varLower = var * segSize + nConst * segSize;
    int varUpper = (var + 1) * segSize + nConst * segSize;
    return (varLower + varUpper) / 2;
  }

  uint16_t getRKbi(char binaryOp) {
    int segSize = 10000 / ((int)operationsBi.size());
    int opId = -1;
    for (int i = 0; i < (int)operationsBi.size(); i++)
      if (operationsBi[i] == binaryOp) opId = i;
    int opLower = opId * segSize;
    int opUpper = (opId + 1) * segSize;
    return (opLower + opUpper) / 2;
  }

  uint16_t getRKun(char unaryOp) {
    int segSize = 10000 / ((int)operationsU.size());
    int opId = -1;
    for (int i = 0; i < (int)operationsU.size(); i++)
      if (operationsU[i] == unaryOp) opId = i;
    int opLower = opId * segSize;
    int opUpper = (opId + 1) * segSize;
    return (opLower + opUpper) / 2;
  }
};
