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
double RMSE(const RProblem& problem, double v1, double v2) {
  if (problem.isSquared()) {
    // must apply square root on both sides, to correct "squared" nature!
    // v1 = ::sqrt(v1);
    // v2 = ::sqrt(v2);
  }
  // RMSE
  return ::sqrt((v1 - v2) * (v1 - v2));
}

// Any value with MAPE below 0.2% is TECHNICALLY ZERO
constexpr double MAPE_ZERO = 0.002;

double MAPE(const RProblem& problem, double actual, double forecast) {
  return ::abs((actual - forecast) / forecast);
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
    if(e1 == 0) 
      std::cout << "ERROR Evaluator: UNIT e1 == 0" << std::endl;
    if(e2 == 0) 
      std::cout << "ERROR Evaluator: UNIT e2 == 0" << std::endl;
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
  // Identidade - sempre retorna a mesma unidade
  if (operationsU[idop] == 'i') {
    return std::make_optional<ex>(v1Unit);
  }

  ex e1 = v1Unit;
  ex ex_one = ex("1", problem.syms);
  ex e_out = ex_one;

  // Tratamento específico por tipo de operação
  switch(operationsU[idop]) {
    case 'a': // quadrado
      e_out = e1 * e1;
      break;
    case 'v': // cubo
      e_out = e1 * e1 * e1;
      break;
    case 'r': // raiz quadrada
      {
        e_out = sqrt(e1);
        if (!fix_sqrt(e_out, problem.syms)) {
          // Se não conseguir simplificar a raiz, retorna unidade original
          // Isso é necessário para distância euclidiana
          return std::make_optional<ex>(e1);
        }
      }
      break;
    // Operações que exigem entrada adimensional
    case 'e': // exponencial
    case 'n': // logaritmo natural
    case 'p': // potência de 2
    case 'l': // logaritmo base 2
    case 's': // seno
    case 'c': // cosseno
      if (e1 == 1) {
        return std::make_optional<ex>(ex_one);
      } else {
        // Ao invés de falhar, vamos tentar continuar com a unidade original
        return std::make_optional<ex>(e1);
      }
      break;
    default:
      // Operação desconhecida - retorna unidade original
      return std::make_optional<ex>(e1);
  }

  return std::make_optional<ex>(e_out);
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
                         int nConst, int maxConst, int seed,
                         bool printFix = false) {
    int stackCount = 0;
    int contConst = 0;
    int trueStackLen = 0;
    std::stack<ex> stkUnit;
    int firstFix = 100 * stackLen;
    int usedVars = 0;
    std::vector<bool> vUsedVars(problem.nVars, false);

    for (int i = 0; i < stackLen; i++) {
        assert(stackCount >= 0);

        // Determina o tipo de operação
        int stackDiff = 2; // valor padrão/flag
        if (isOperation(individual[i], OpType::BIN)) stackDiff = -1;
        if (isOperation(individual[i], OpType::UN)) stackDiff = 0;
        if (isOperation(individual[i], OpType::PUSH)) stackDiff = 1;

        // Calcula índices
        int varConstInt = ::floor((individual[stackLen + i] / 10000.0) * (nVars + nConst));
        int idConstOrVar = varConstInt - nConst;
        int stackCountAfterOperation = stackCount;

        // Atualiza contagem da pilha se não for flag
        if (stackDiff != 2) {
            stackCountAfterOperation += stackDiff;
        }

        // Verifica se há elementos suficientes na pilha
        if (stackCountAfterOperation < 1) {
            if (i < firstFix) firstFix = i;
            if (printFix) {
                std::cout << "stackAdjustment fix: makePUSH stackCountAfterOperation < 1" << std::endl;
            }
            individual[i] = makePUSH(individual[i]);
            assert(isOperation(individual[i], OpType::PUSH));
            stackCountAfterOperation = stackCount + 1;
        }

        assert(stackCountAfterOperation >= 1);

        // Verifica overflow da pilha
        if (stackCountAfterOperation > (stackLen - i)) {
            if (stackCount == 1) {
                if (i < firstFix) firstFix = i;
                if (printFix) {
                    std::cout << "stackAdjustment fix: += 2500 stackCountAfterOperation > (stackLen - i)" << std::endl;
                }
                individual[i] += 2500;
                stackCountAfterOperation = 1;
            } else if (stackCount > 1) {
                if (isOperation(individual[i], OpType::UN)) {
                    if (i < firstFix) firstFix = i;
                    if (printFix) {
                        std::cout << "stackAdjustment fix: -= 2500 (stackCount > 1)" << std::endl;
                    }
                    individual[i] -= 2500;
                } else if (isOperation(individual[i], OpType::SPECIAL)) {
                    if (i < firstFix) firstFix = i;
                    if (printFix) {
                        std::cout << "stackAdjustment fix: -= 7500 (SPECIAL)" << std::endl;
                    }
                    individual[i] -= 7500;
                } else {
                    if (i < firstFix) firstFix = i;
                    if (printFix) {
                        std::cout << "stackAdjustment fix: -= 5000 (else)" << std::endl;
                    }
                    individual[i] -= 5000;
                }
                stackCountAfterOperation = stackCount - 1;
            }
        }

        // Verifica constantes
        if ((stackDiff == 1) && idConstOrVar < 0) {
            contConst++;
            if (contConst > maxConst) {
                srand(seed);
                if (i < firstFix) firstFix = i;
                if (printFix) {
                    std::cout << "stackAdjustment fix: SD1 (PUSH) + Constant" << std::endl;
                }
                individual[stackLen + i] = ((10000.0 / (nVars + nConst)) * nConst) +
                                         ((10000.0 / (nVars + nConst)) * (rand() % nVars) + 1);
                seed++;
            }
        }

        stackCount = stackCountAfterOperation;
        if (!isOperation(individual[i], OpType::SPECIAL)) {
            trueStackLen++;
        }

        // Verificação de unidades
        if (problem.hasUnits) {
            int j = i;
            
            // Tratamento de PUSH
            if (isOperation(individual[j], OpType::PUSH)) {
                int my_floor = ::floor((individual[stackLen + j] / 10000.0) * (nVars + nConst));
                int idVar = my_floor - nConst;
                std::string varUnit = "_";
                
                if (idVar >= 0) {
                    // Variável
                    if (!vUsedVars[idVar]) {
                        usedVars++;
                        vUsedVars[idVar] = true;
                    }
                    varUnit = problem.varUnits[idVar];
                    ex exVar(varUnit, problem.syms);
                    stkUnit.push(exVar);
                } else {
                    // Constante
                    idVar += nConst;
                    if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
                        varUnit = problem.constUnits[idVar];
                        if (varUnit == "*") varUnit = "1";
                        try {
                            ex exVar(varUnit, problem.syms);
                            stkUnit.push(exVar);
                        } catch (...) {
                            std::cout << "Warning: Invalid unit expression, using adimensional" << std::endl;
                            stkUnit.push(ex("1", problem.syms));
                        }
                    } else {
                        // Constante com intervalo - trata como adimensional
                        stkUnit.push(ex("1", problem.syms));
                    }
                }
            }
            
            // Tratamento de operação binária
            else if (isOperation(individual[j], OpType::BIN)) {
                int idOpBi = floor((individual[2 * stackLen + j] / 10000.0) * 
                                 (double)other.operationsBi.size());
                assert(stkUnit.size() >= 2);
                ex v1Unit = stkUnit.top();
                stkUnit.pop();
                ex v2Unit = stkUnit.top();
                stkUnit.pop();

                if (v1Unit != v2Unit) {
                    if (idOpBi < (int)other.operationsBiT1.size()) {
                        if (j < firstFix) firstFix = j;
                        individual[2 * stackLen + j] = individual[2 * stackLen + j] +
                            (chromosome)((10000.0 / (double)other.operationsBi.size()) * 
                                       (double)other.operationsBiT1.size());
                        idOpBi = floor((individual[2 * stackLen + j] / 10000.0) * 
                                     (double)other.operationsBi.size());
                        assert(idOpBi >= (int)other.operationsBiT1.size());
                    }
                }

                opt<ex> binResult = execBinaryOpUnit(problem, idOpBi, v1Unit, v2Unit,
                                                   other.operationsBi);
                if (!binResult) {
                    stkUnit.push(ex("1", problem.syms)); // Fallback para adimensional
                } else {
                    stkUnit.push(*binResult);
                }
            }
            
            // Tratamento de operação unária
            else if (isOperation(individual[j], OpType::UN)) {
                int idOpU = floor((individual[2 * stackLen + j] / 10000.0) * 
                                (double)other.operationsU.size());
                assert(stkUnit.size() >= 1);
                ex v1Unit = stkUnit.top();
                stkUnit.pop();

                if (v1Unit != 1) {
                    if (idOpU < (int)other.operationsUT1.size()) {
                        if (j < firstFix) firstFix = j;
                        individual[2 * stackLen + j] = individual[2 * stackLen + j] +
                            (chromosome)((10000.0 / (double)other.operationsU.size()) * 
                                       (double)other.operationsUT1.size());
                        idOpU = floor((individual[2 * stackLen + j] / 10000.0) * 
                                    (double)other.operationsU.size());
                        assert(idOpU >= (int)other.operationsUT1.size());
                    }
                }

                opt<ex> unResult = execUnaryOpUnit(problem, idOpU, v1Unit, other.operationsU);
                if (!unResult) {
                    stkUnit.push(ex("1", problem.syms)); // Fallback para adimensional
                } else {
                    stkUnit.push(*unResult);
                }
            }
        }
    }

    std::string outUnit = "";
    if (problem.hasUnits) {
        if (!stkUnit.empty()) {
            std::stringstream ss;
            ex last = stkUnit.top();
            stkUnit.pop();
            ss << last;
            outUnit = ss.str();
        } else {
            outUnit = "1"; // Fallback para adimensional se pilha vazia
        }
    }

    return StackInfo{trueStackLen, outUnit, usedVars, firstFix};
}

double solutionEvaluator(const RProblem& problem,
                         const Vec<chromosome>& individual,
                         const Scenario& other, int training, int idSol) {
  const int nVars = problem.nVars;
  const int nConst = problem.nConst;
  const int stackLen = other.getStackLen();
  kahan::kfloat64 sum_rmse = 0;
  kahan::kfloat64 sum_mape = 0;
  
  if (idSol == 0) {
    std::cout << "DEBUG[idSol=0] BEGIN sum_rmse:" << sum_rmse << std::endl;
  }

  const int realTests = problem.tests - training;
  
  try {
    for (int t = 0; t < realTests; t++) {
      std::stack<double> stk;
      int contSeed = 0;

      for (int j = 0; j < stackLen; j++) {
        if (isOperation(individual[j], OpType::BIN)) {
          int idOpBi = floor((individual[2 * stackLen + j] / 10000.0) * 
                            (double)other.operationsBi.size());

          if (stk.size() < 2) return INFINITY;

          double v1 = stk.top();
          stk.pop();
          double v2 = stk.top();
          stk.pop();

          opt<double> binResult = execBinaryOp(idOpBi, v1, v2, other.operationsBi);
          if (!binResult) {
            return INFINITY;
          }

          if (idSol == 0 && t == 0) {
            std::cout << "DEBUG[idSol=0] bin result: " << *binResult << std::endl;
          }

          // Permite valores muito próximos de zero
          stk.push(*binResult);

        } else if (isOperation(individual[j], OpType::UN)) {
          int idOpU = floor((individual[2 * stackLen + j] / 10000.0) * 
                           (double)other.operationsU.size());

          if (stk.empty()) return INFINITY;

          double v1 = stk.top();
          stk.pop();

          opt<double> unResult = execUnaryOp(idOpU, v1, other.operationsU);
          if (!unResult) {
            return INFINITY;
          }

          if (idSol == 0 && t == 0) {
            std::cout << "DEBUG[idSol=0] un result: " << *unResult << std::endl;
          }

          stk.push(*unResult);

        } else if (isOperation(individual[j], OpType::PUSH)) {
          int my_floor = ::floor((individual[stackLen + j] / 10000.0) * (nVars + nConst));
          int idVar = my_floor - nConst;
          
          double varValue = 0;
          if (idVar >= 0) {
            if (idVar >= (int)problem.inputs[t].size()) {
              return INFINITY;
            }
            varValue = problem.inputs[t][idVar];
            stk.push(varValue);
            
            if (idSol == 0 && t == 0) {
              std::cout << "DEBUG[idSol=0] pushvar: " << varValue << std::endl;
            }
          } else {
            idVar += nConst;
            if (idVar < 0 || idVar >= (int)problem.vConst.size()) {
              return INFINITY;
            }

            if (problem.vConst[idVar].first == problem.vConst[idVar].second) {
              varValue = problem.vConst[idVar].first;
              stk.push(varValue);
            } else {
              if (contSeed >= (int)individual.size()) return INFINITY;

              chromosome seedConst = individual[3 * stackLen + contSeed];
              srand(seedConst);
              
              double range = problem.vConst[idVar].second - problem.vConst[idVar].first;
              if (range < 0) return INFINITY;

              varValue = problem.vConst[idVar].first + 
                        (rand() / (RAND_MAX + 1.0)) * range;
              stk.push(varValue);
              contSeed++;
            }
          }
        }
      }

      if (stk.size() != 1) return INFINITY;

      double val = stk.top();
      stk.pop();

      if (idSol == 0 && t == 0) {
        std::cout << "DEBUG[idSol=0] val:" << val << std::endl;
        std::cout << "DEBUG[idSol=0] expected:" << problem.outputs[t] << std::endl;
        std::cout << "DEBUG[idSol=0] BEFORE sum_error:" << sum_rmse << std::endl;
      }

      double rmse = RMSE(problem, val, problem.outputs[t]);
      sum_rmse += rmse;

      double mape = MAPE(problem, problem.outputs[t], val);
      if (mape < MAPE_ZERO) mape = 0;
      sum_mape += mape;

      if (idSol == 0 && t == 0) {
        std::cout << "DEBUG[idSol=0] AFTER sum_error:" << sum_rmse << std::endl;
      }
    }

    if (realTests <= 0) return INFINITY;

    double avgRMSE = (double)sum_rmse / realTests;
    double avgMAPE = (double)sum_mape / realTests;

    // Se MAPE é muito próximo de zero, consideramos solução ótima
    if (avgMAPE < MAPE_ZERO) {
      avgMAPE = 0;
      avgRMSE = 0;
    }

    if (idSol == 0) {
      std::cout << "DEBUG[idSol=0] Final sum_rmse:" << sum_rmse << std::endl;
      std::cout << "DEBUG[idSol=0] Final solutionValue:" << avgRMSE << std::endl;
    }

    return avgRMSE;
  } catch (...) {
    return INFINITY;
  }
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
      StackInfo si = stackAdjustment(problem, other, testSol,
                                     other.getStackLen(), problem.nVars,
                                     problem.nConst, other.maxConst, 0, true);
      std::cout << "StackInfo firstFix=" << si.firstFix << std::endl;
      if (!(si.firstFix >= i)) {
        std::cout << "ERROR! fix at element i=" << i << "position " << (i - 1)
                  << " firstFix = " << si.firstFix << std::endl;
        assert(si.firstFix >= i);  // because i starts at 1... not zero.
      }
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
    for (int id = 0; id < nConst; id++) {
      std::stringstream ss;
      ss << "c" << id;
      sconsts.push_back(ss.str());
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
      } else if (inVStr(op, sconsts) >= 0) {
        v[idx] = getRK(OpType::PUSH);
        v[idx + stackLen] = getRKconst(inVStr(op, sconsts));
        idx++;
      } else if (inVStr(op, sopsBi) >= 0) {
        v[idx] = getRK(OpType::BIN);
        v[idx + 2 * stackLen] = getRKbi(op[0]);
        idx++;
      } else if (inVStr(op, sopsU) >= 0) {
        v[idx] = getRK(OpType::UN);
        v[idx + 2 * stackLen] = getRKun(op[0]);
        idx++;
      } else if (idx > stackLen) {
        std::cout << "WARNING: PROBLEM IN PARSING! idx > stackLen: " << idx << std::endl;
        assert(false);
        return Vec<chromosome>{};
      } else {
        std::cout << "Unknown operation '" << op << "'" << std::endl;
        assert(false);
        return Vec<chromosome>{};
      }
    }

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

  uint16_t getRKconst(int c) {
    int segSize = 10000 / (nVars + nConst);
    int cLower = c * segSize;
    int cUpper = (c + 1) * segSize;
    return (cLower + cUpper) / 2;
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
