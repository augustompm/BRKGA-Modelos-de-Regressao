#pragma once

typedef unsigned short chromosome;

class valuedChromosome {
public:
    chromosome* randomKeys;
    double cost;
    int trueStackSize;
};

void printFile (int nVars,int tests,int nConst,double** inputs,double* outputs,double* vConstMin,double* vConstMax);
void printCodChromosome(chromosome* individual,int len);
void printDecodChromosome(chromosome* individual,int stackLen,int nVars,int nConst,int operationsBiLen,int operationsULen);
void printSolution(chromosome* individual,int stackLen,int nVars,int nConst,char* operationsBi,char* operationsU,double* vConstMin,double* vConstMax,int operationsBiLen,int operationsULen);
void printPopulationCost(valuedChromosome* mainPopulation,int populationLen);

#include <stdio.h>
#include <math.h>
//#include "PrintIO.h"
#include <string.h>

using namespace std;


void printFile(int nVars, int tests, int nConst, double** inputs, double* outputs, double* vConstMin, double* vConstMax)
{
    printf("%d %d %d\n", nVars, tests, nConst);
    for (int i = 0; i < tests; i++) {
        for (int j = 0; j < nVars; j++)
            printf("%.2f\t", inputs[i][j]);
        printf("%.2f\n", outputs[i]);
    }
    for (int i = 0; i < nConst; i++)
    {
        printf("%.4f\t%.4f\n", vConstMin[i], vConstMax[i]);
    }

}

void printCodChromosome(chromosome* individual, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%d   ", individual[i]);
    }
    printf("\n");
}

void printDecodChromosome(chromosome* individual, int stackLen, int nVars, int nConst, int operationsBiLen, int operationsULen)
{
    int decodValue;
    for (int i = 0; i < stackLen; i++)
    {
        decodValue = floor((individual[i] / 10000.0) * 4) - 1;
        printf("%d   ", decodValue);
    }
    for (int i = stackLen; i < (2 * stackLen); i++)
    {
        decodValue = floor((individual[i] / 10000.0) * (nVars + nConst)) - nConst;
        printf("%d   ", decodValue);
    }
    for (int i = (2 * stackLen); i < (3 * stackLen); i++)
    {
        if (individual[i - 2 * stackLen] < 2500) {
            decodValue = floor((individual[i] / 10000.0) * operationsBiLen);
            printf("Bi:");
        }
        else if ((individual[i - 2 * stackLen] >= 2500) && (individual[i - 2 * stackLen] < 5000)) {
            decodValue = floor((individual[i] / 10000.0) * operationsULen);
            printf("U:");
        }
        else
            decodValue = individual[i];
        printf("%d   ", decodValue);
    }
    printf("\n");
}

void printSolution(chromosome* individual, int stackLen, int nVars, int nConst, char* operationsBi, char* operationsU, double* vConstMin, double* vConstMax, int operationsBiLen, int operationsULen)
{
    int decodValue;
    int cont = 0;
    double rangeConst = 0;
    for (int i = 0; i < stackLen; i++)
    {
        decodValue = floor((individual[i] / 10000.0) * 4) - 1;
        //printf("")
        if (decodValue == -1) {
            decodValue = floor((individual[i + 2 * stackLen] / 10000.0) * operationsBiLen);
            printf("%c ", operationsBi[decodValue]);
        }
        else if (decodValue == 0) {
            decodValue = floor((individual[i + 2 * stackLen] / 10000.0) * operationsULen);
            printf("%c ", operationsU[decodValue]);
        }
        else if (decodValue == 1) {
            decodValue = floor((individual[i + stackLen] / 10000.0) * (nVars + nConst)) - nConst;
            if (decodValue < 0) {
                decodValue += nConst;
                if (vConstMax[decodValue] == vConstMin[decodValue]) {
                    printf("%f  ", vConstMin[decodValue]);
                }
                else {
                    srand(individual[3 * stackLen + cont]);
                    rangeConst = rand() % (int)(vConstMax[decodValue] - vConstMin[decodValue] + 1) + vConstMin[decodValue];
                    cont++;
                    printf("%f  ", rangeConst);
                }
            }
            else
                printf("V%d  ", decodValue);
        }
    }
    printf("\n");
}

void printPopulationCost(valuedChromosome* mainPopulation, int populationLen)
{
    for (int i = 0; i < populationLen; i++)
    {
        printf("%lf  ", mainPopulation[i].cost);
    }
    printf("\n");
}
