#pragma once

#include "Scanner/Scanner.hpp"
#include "Scanner/File.h"
using namespace scannerpp;
typedef unsigned short chromosome;


void readIO(double*** inputs,double** outputs,Scanner& scanner,int* nVars,int* tests,int* nConst,double** vConstMin,double** vConstMax);
void changeIO(double*** inputs,double** outputs,int training,int nVars,int tests,int nConst);


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <Scanner/Scanner.hpp>
#include <Scanner/File.h>
#include <brkgp/ReadIO.hpp>
#include <brkgp/Evaluator.hpp>
#include <brkgp/PrintIO.hpp>

using namespace std;
using namespace scannerpp;
typedef unsigned short chromosome;

void readIO(double*** inputs, double** outputs, Scanner& scanner, int* nVars, int* tests, int* nConst, double** vConstMin, double** vConstMax)
{
    (*nVars) = scanner.nextInt();
    (*tests) = scanner.nextInt();
    (*nConst) = scanner.nextInt();


    // for each test...

    (*inputs) = (double**)malloc((*tests) * sizeof(double*));
    (*outputs) = (double*)malloc((*tests) * sizeof(double));
    (*vConstMin) = (double*)malloc(sizeof(double) * (*nConst));
    (*vConstMax) = (double*)malloc(sizeof(double) * (*nConst));

    for (int t = 0; t < (*tests); t++)
    {
        (*inputs)[t] = (double*)malloc((*nVars) * sizeof(double));
        for (unsigned i = 0; i < (*nVars); i++)
            (*inputs)[t][i] = scanner.nextDouble();
        (*outputs)[t] = scanner.nextDouble();
    }
    for (int i = 0; i < (*nConst); i++)
    {
        (*vConstMin)[i] = scanner.nextDouble();
        (*vConstMax)[i] = scanner.nextDouble();
    }
}
void changeIO(double*** inputs, double** outputs, int training, int nVars, int tests, int nConst)
{
    double** auxInputs;
    double* auxOutputs;

    auxInputs = (double**)malloc((tests) * sizeof(double*));
    auxOutputs = (double*)malloc((tests) * sizeof(double));


    for (int t = 0; t < (tests); t++)
    {
        auxInputs[t] = (double*)malloc((nVars) * sizeof(double));
        memcpy(auxInputs[t], (*inputs)[t], sizeof(double) * nVars);
    }
    memcpy(auxOutputs, (*outputs), sizeof(double) * tests);
    for (int i = 0; i < tests; i++)
    {
        if (i < training)
        {
            memcpy((*inputs)[i], auxInputs[i + (tests - training)], sizeof(double) * nVars);
            (*outputs)[i] = auxOutputs[i + (tests - training)];
        }
        else
        {
            memcpy((*inputs)[i], auxInputs[i - training], sizeof(double) * nVars);
            (*outputs)[i] = auxOutputs[i - training];
        }
    }
    for (int t = 0; t < (tests); t++)
    {
        free(auxInputs[t]);
    }
    free(auxInputs);
    free(auxOutputs);
}