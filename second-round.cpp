// SEGUNDO ROUND DESATIVADO - removido da linha 194 main.cpp Aug 04-12-24
  double auxBestFoundSolutionCost = bestFoundSolution.cost;
  if (false) {
    changeIO(problem.inputs, problem.outputs, training, problem.nVars,
             problem.tests, problem.nConst);
    printFile(problem);
    run_brkga(problem, params, seed, bestFoundSolution, other, training,
              opInitialSol);
    printCodChromosome(bestFoundSolution.randomKeys);
    printDecodChromosome(bestFoundSolution.randomKeys, problem, other);
    std::cout << "printSolution (ENTENDER E REMOVER): " << std::endl;
    printSolution(problem, bestFoundSolution.randomKeys, other);
    std::string eq2 =
        printSolution2(problem, bestFoundSolution.randomKeys, other);
    std::cout << "printSolution2: " << eq2 << std::endl;
    printf("best before: %lf\n", bestFoundSolution.cost);
    bestFoundSolution.cost = solutionEvaluator(
        problem, bestFoundSolution.randomKeys, other, training, -1);
    printf("best after: %lf \n", bestFoundSolution.cost);
    printf("Validation Mean: %lf  \n",
           (auxBestFoundSolutionCost + bestFoundSolution.cost) / 2);
  }