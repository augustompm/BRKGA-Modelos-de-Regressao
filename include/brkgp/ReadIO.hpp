#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <Scanner/Scanner.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>
#include <brkgp/ReadIO.hpp>
#include <brkgp/Utils.hpp>

using namespace scannerpp;

void readIO(RProblem& problem, Scanner& scanner) {
    Vec<Vec<double>>& inputs = problem.inputs;
    Vec<double>& outputs = problem.outputs;
    int& nVars = problem.nVars;
    int& tests = problem.tests;
    int& nConst = problem.nConst;
    auto& varUnits = problem.varUnits;
    auto& outUnit = problem.outUnit;
    auto& constUnits = problem.constUnits;
    auto& hasUnits = problem.hasUnits;

    try {
        std::cout << "1) READING PROBLEM" << std::endl;

        // Leitura do tipo do problema e das unidades
        std::string stype = scanner.nextLine();
        std::cout << "TYPE: '" << stype << "'" << std::endl;
        hasUnits = (stype == "HAS_UNITS");
        
        if (hasUnits) {
            std::string units = scanner.next();
            std::string all_units = Scanner::trim(scanner.nextLine());
            if (units != "UNITS") {
                throw std::runtime_error("Expected 'UNITS' keyword after HAS_UNITS");
            }
            Scanner scan(all_units);
            while (scan.hasNext()) {
                problem.allUnits.push_back(scan.next());
            }
        }

        // Leitura das dimensões do problema
        try {
            nVars = scanner.nextInt();
            tests = scanner.nextInt();
            nConst = scanner.nextInt();
            std::cout << "Debug: nVars=" << nVars << ", tests=" << tests 
                     << ", nConst=" << nConst << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Erro ao ler dimensões do problema" << std::endl;
            throw;
        }

        // Leitura das unidades das variáveis
        if (hasUnits) {
            for (int i = 0; i < nVars; i++) {
                std::string unit = scanner.nextLine();
                varUnits.push_back(unit);
                std::cout << "var unit: '" << unit << "'" << std::endl;
            }
            outUnit = scanner.nextLine();
            std::cout << "out unit: '" << outUnit << "'" << std::endl;
        } else {
            for (int i = 0; i < nVars; i++) {
                varUnits.push_back("*");
            }
        }

        // Verificação de solução conhecida
        std::string has_solution = scanner.next();
        std::string solution = scanner.nextLine();
        std::cout << "has_solution='" << has_solution << "' => '" 
                 << solution << "'" << std::endl;
        
        if (has_solution == "HAS_SOLUTION") {
            problem.hasSolution = true;
            problem.solution = solution;
            std::cout << "SOLUTION IS: '" << solution << "'";
        } else {
            problem.hasSolution = false;
            if (has_solution != "NO_SOLUTION") {
                throw std::runtime_error("Invalid solution specification");
            }
        }

        std::cout << "2) READING PROBLEM TESTS" << std::endl;

        // Alocação dos vetores de entrada/saída
        inputs.resize(tests);
        outputs.resize(tests);
        problem.vConst.resize(nConst);

        // Leitura dos casos de teste
        for (int t = 0; t < tests; t++) {
            try {
                inputs[t].resize(nVars);
                for (int i = 0; i < nVars; i++) {
                    inputs[t][i] = scanner.nextDouble();
                    std::cout << "var[" << i << "] = " << inputs[t][i] << " ";
                }
                outputs[t] = scanner.nextDouble();
                std::cout << "output = " << outputs[t] << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Erro ao ler teste " << t + 1 << std::endl;
                throw;
            }
        }

        std::cout << "3) READING PROBLEM CONSTANTS" << std::endl;

        // Leitura das constantes
        for (int i = 0; i < nConst; i++) {
            std::string line;
            do {
                line = scanner.nextLine();
            } while (line.empty());
            
            std::cout << "Lendo constante " << i + 1 << ": '" << line << "'" << std::endl;

            std::istringstream iss(line);
            std::vector<std::string> tokens;
            std::string token;
            
            while (iss >> token) {
                tokens.push_back(token);
            }

            try {
                if (tokens.empty()) {
                    // Linha vazia - usa valores default
                    problem.vConst[i].first = 1.0;
                    problem.vConst[i].second = 1.0;
                    constUnits.push_back("*");
                } else if (tokens.size() == 1) {
                    // Apenas unidade
                    problem.vConst[i].first = 1.0;
                    problem.vConst[i].second = 1.0;
                    constUnits.push_back(tokens[0]);
                } else {
                    bool firstIsNumber = std::isdigit(tokens[0][0]) || 
                                      tokens[0][0] == '-' || 
                                      tokens[0][0] == '.' ||
                                      tokens[0][0] == '+' ||
                                      tokens[0].find('e') != std::string::npos;
                    
                    if (firstIsNumber) {
                        problem.vConst[i].first = std::stod(tokens[0]);
                        
                        // Verifica se segundo token é número
                        if (tokens.size() >= 2 && 
                            (std::isdigit(tokens[1][0]) || 
                             tokens[1][0] == '-' || 
                             tokens[1][0] == '.' || 
                             tokens[1][0] == '+' ||
                             tokens[1].find('e') != std::string::npos)) {
                            problem.vConst[i].second = std::stod(tokens[1]);
                            
                            // Resto é unidade
                            std::string unit;
                            for (size_t j = 2; j < tokens.size(); j++) {
                                if (j > 2) unit += " ";
                                unit += tokens[j];
                            }
                            constUnits.push_back(unit.empty() ? "*" : unit);
                        } else {
                            // Segundo token é unidade
                            problem.vConst[i].second = problem.vConst[i].first;
                            std::string unit;
                            for (size_t j = 1; j < tokens.size(); j++) {
                                if (j > 1) unit += " ";
                                unit += tokens[j];
                            }
                            constUnits.push_back(unit);
                        }
                    } else {
                        // Tudo é unidade
                        problem.vConst[i].first = 1.0;
                        problem.vConst[i].second = 1.0;
                        constUnits.push_back(Scanner::trim(line));
                    }
                }
                
                std::cout << "Constante lida: " << problem.vConst[i].first 
                         << " " << problem.vConst[i].second 
                         << " " << constUnits.back() << std::endl;
                
            } catch (const std::exception& e) {
                std::cerr << "Erro ao processar constante " << i + 1 
                         << ": " << e.what() << std::endl;
                throw;
            }
        }

        for (const auto& cun : constUnits) {
            std::cout << "const unit: '" << cun << "'" << std::endl;
        }

        if (hasUnits) {
            std::cout << "3.5) SETUP UNITS" << std::endl;
            problem.setupUnits();
            std::cout << "UNITS: ";
            for (const auto& unit : problem.allUnits) {
                std::cout << unit << " ; ";
            }
            std::cout << std::endl;
        }

        std::cout << "4) FINISHED READING PROBLEM" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Erro durante a leitura do arquivo" << std::endl;
        std::cerr << e.what() << std::endl;
        
        std::cout << "Tentando debug do scanner..." << std::endl;
        std::cout << "Próxima linha do scanner: " << scanner.nextLine() << std::endl;
        
        throw std::runtime_error("Erro fatal durante execução");
    }
}

void changeIO(Vec<Vec<double>>& inputs, Vec<double>& outputs, int training,
             int nVars, int tests, int nConst) {
    Vec<Vec<double>> auxInputs(tests);
    Vec<double> auxOutputs(tests);

    for (int t = 0; t < tests; t++) {
        auxInputs[t] = Vec<double>(nVars);
        auxInputs[t] = inputs[t];
    }
    auxOutputs = outputs;

    for (int i = 0; i < tests; i++) {
        if (i < training) {
            inputs[i] = auxInputs[i + (tests - training)];
            outputs[i] = auxOutputs[i + (tests - training)];
        } else {
            inputs[i] = auxInputs[i - training];
            outputs[i] = auxOutputs[i - training];
        }
    }
}