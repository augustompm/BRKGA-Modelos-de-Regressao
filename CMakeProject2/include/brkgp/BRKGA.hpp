#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "PrintIO.hpp"
using namespace std;


int percentToInt(int porcentagem, int total);
void individualGenerator(chromosome* individual,int seed,int individualLen);
void mutantGenerator(valuedChromosome* auxPopulation,int eliteSize,int mutantSize,int seed,int individualLen);
void crossover(valuedChromosome *population, valuedChromosome *auxPopulation, int eliteSize, int mutantSize, int seed, unsigned short eliteBias,int populationLen,int individualLen);
void populationGenerator(valuedChromosome* population,int seed,int populationLen,int individualLen);
void decoder(valuedChromosome* population,int nVars,int nConst,char* operationsBi,char* operationsU,int tests, double** inputs,double* outputs,double* vConstMin,double* vConstMax,int seed,int populationLen,int stackLen,int maxConst,int operationsBiLen,int operationsULen);
char isRestart(valuedChromosome* auxPopulation,valuedChromosome* population,int* noImproviment,int noImprovimentMax);
void almostBestSolution(int restartMax,int noImprovimentMax,int eliteSize,int mutantSize,int seed,unsigned short eliteBias,int nVars,valuedChromosome* bestFoundSolution,
                                    int nConst,char* operationsBi,char* operationsU,int tests, double** inputs,double* outputs,double* vConstMin,double* vConstMax,
                                    int populationLen,int individualLen,int stackLen,int maxConst,int operationsBiLen,int operationsULen);


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <math.h>
#include <brkgp/Evaluator.hpp>
#include <brkgp/BRKGA.hpp>

using namespace std;

typedef unsigned short chromosome;

//typedef struct{
    //chromosome randomKeys[LEN];
    //double cost;
//} valuedChromosome;

int percentToInt(int porcentagem, int total)
{
    return ceil((porcentagem / 100.0) * total);
}
bool menorQue(valuedChromosome i, valuedChromosome j)
{

    return (i.cost < j.cost) || ((i.cost == j.cost) && (i.trueStackSize < j.trueStackSize));
}

void individualGenerator(chromosome* individual, int seed, int individualLen)
{
    //printStack(vStack,len);
    for (int i = 0; i < individualLen; i++)
    {
        srand(seed);
        individual[i] = (rand() % 10000);
        seed++;
    }
}

void mutantGenerator(valuedChromosome* population, int eliteSize, int mutantSize, int seed, int individualLen)
{
    for (int i = eliteSize; i < (eliteSize + mutantSize); i++)
    {
        individualGenerator(population[i].randomKeys, seed, individualLen);
        seed += 2 * individualLen;
        population[i].cost = 0;
    }
}

void crossover(valuedChromosome* population, valuedChromosome* auxPopulation, int eliteSize, int mutantSize, int seed, unsigned short eliteBias, int populationLen, int individualLen)
{
    int parentA;
    int parentB;

    for (int i = (eliteSize + mutantSize); i < populationLen; i++)
    {
        srand(seed);
        //printf("sizeof %d\n",sizeof(chromosome)* LEN);
        //printf("%d",LEN);
        parentA = rand() % eliteSize;
        //printf("parentA: %d\n",parentA);
        //memcpy(parentA, population[rand() % eliteSize].randomKeys, sizeof(chromosome) * 63);
        seed++;
        srand(seed);
        parentB = eliteSize + rand() % (populationLen - eliteSize);
        //printf("parentB: %d\n",parentB);
        //memcpy(parentB, population[eliteSize + rand() % (POPULATIONLEN - eliteSize)].randomKeys, sizeof(chromosome) * LEN);
        seed++;
        for (int j = 0; j < individualLen; j++)
        {
            srand(seed);
            if ((rand() % 100) < eliteBias)
                auxPopulation[i].randomKeys[j] = population[parentA].randomKeys[j];
            else
                auxPopulation[i].randomKeys[j] = population[parentB].randomKeys[j];
            seed++;
        }
        auxPopulation[i].cost = 0;
    }
}

void populationGenerator(valuedChromosome* auxPopulation, int seed, int populationLen, int individualLen)
{
    for (int i = 0; i < populationLen; i++)
    {
        for (int j = 0; j < individualLen; j++)
        {
            auxPopulation[i].randomKeys[j] = 0;
        }
        individualGenerator(auxPopulation[i].randomKeys, seed, individualLen);
        //printCodChromosome(population[i].randomKeys,LEN);
       // printDecodChromosome(population[i].randomKeys,N,nVars,nConst);

        //printCodChromosome(population[i].randomKeys,LEN);
        //printDecodChromosome(population[i].randomKeys,N,nVars,nConst);
        //printf("%.4f \n",population[i].cost);
        seed += 2 * individualLen;
        auxPopulation[i].cost = 0;
    }

}

void decoder(valuedChromosome* population, int nVars, int nConst, char* operationsBi, char* operationsU, int tests, double** inputs, double* outputs, double* vConstMin, double* vConstMax, int seed, int populationLen, int stackLen, int maxConst, int operationsBiLen, int operationsULen)
{
    for (int i = 0; i < populationLen; i++)
    {
        if (population[i].cost == 0)
        {
            population[i].trueStackSize = stackAdjustment(population[i].randomKeys, stackLen, nVars, nConst, maxConst, seed);
            population[i].cost = solutionEvaluator(population[i].randomKeys, operationsBi, operationsU, stackLen, nVars, tests, inputs, outputs, vConstMin, vConstMax, nConst, 0, operationsBiLen, operationsULen);
        }
    }
}

char isRestart(valuedChromosome* auxPopulation, valuedChromosome* population, int* noImproviment, int noImprovimentMax)
{
    if (auxPopulation[0].cost < population[0].cost)
        (*noImproviment) = 0;
    else
        (*noImproviment)++;
    if (((*noImproviment) == noImprovimentMax)) { //|| (auxPopulation[0].cost <= 0.00001))
        (*noImproviment) = 0;
        return 1;
    }
    return 0;
}

void almostBestSolution(int restartMax, int noImprovimentMax, int eliteSize, int mutantSize, int seed, unsigned short eliteBias, int nVars, valuedChromosome* bestFoundSolution,
    int nConst, char* operationsBi, char* operationsU, int tests, double** inputs, double* outputs, double* vConstMin, double* vConstMax,
    int populationLen, int individualLen, int stackLen, int maxConst, int operationsBiLen, int operationsULen)
{
    int noImproviment = 0;
    valuedChromosome* mainPopulation; //populationLen
    valuedChromosome* auxPopulation;
    char end;
    //printf("populationLen1: %d eliteSize1: %d mutanteSize1: %d individualLen1: %d stackLen1: %d\n",populationLen,eliteSize,mutantSize,individualLen,stackLen);
    (*bestFoundSolution).cost = INFINITY;

    auxPopulation = (valuedChromosome*)calloc(populationLen, sizeof(valuedChromosome));
    mainPopulation = (valuedChromosome*)calloc(populationLen, sizeof(valuedChromosome));
    for (int i = 0; i < populationLen; i++)
    {
        auxPopulation[i].randomKeys = (chromosome*)calloc(individualLen, sizeof(chromosome));
        mainPopulation[i].randomKeys = (chromosome*)calloc(individualLen, sizeof(chromosome));
    }
    eliteSize = percentToInt(eliteSize, populationLen);
    mutantSize = percentToInt(mutantSize, populationLen);
    //printf("populationLen2: %d eliteSize2: %d mutanteSize2: %d individualLen2: %d stackLen2: %d\n",populationLen,eliteSize,mutantSize,individualLen,stackLen);
    //printf("%d\t%d\n",eliteSize,mutantSize);
    int restart = 0;
    int mutationGrow = 0;
    while (restart < restartMax)
    {
        seed++;
        populationGenerator(mainPopulation, seed, populationLen, individualLen);
        decoder(mainPopulation, nVars, nConst, operationsBi, operationsU, tests, inputs, outputs, vConstMin, vConstMax, seed, populationLen, stackLen, maxConst, operationsBiLen, operationsULen);
        std::sort(mainPopulation, mainPopulation + (populationLen), menorQue);
        end = 0;
        while (!(end))
        {
            //memcpy(auxPopulation,mainPopulation,sizeof(valuedChromosome)*populationLen);
            for (int i = 0; i < populationLen; i++)
            {
                memcpy(auxPopulation[i].randomKeys, mainPopulation[i].randomKeys, sizeof(chromosome) * individualLen);
                auxPopulation[i].cost = mainPopulation[i].cost;
            }
            mutationGrow = percentToInt(5 * (5 * (noImproviment / noImprovimentMax)), populationLen);
            mutantGenerator(auxPopulation, eliteSize, (mutantSize + mutationGrow), seed, individualLen);
            crossover(mainPopulation, auxPopulation, eliteSize, (mutantSize + mutationGrow), seed, eliteBias, populationLen, individualLen);
            seed += 2 * individualLen;
            decoder(auxPopulation, nVars, nConst, operationsBi, operationsU, tests, inputs, outputs, vConstMin, vConstMax, seed, populationLen, stackLen, maxConst, operationsBiLen, operationsULen);
            //printPopulationCost(auxPopulation,populationLen);
            std::sort(auxPopulation, auxPopulation + (populationLen), menorQue);
            //printPopulationCost(auxPopulation,populationLen);
            //printSolution(auxPopulation[0].randomKeys,stackLen,nVars,nConst,operationsBi,operationsU,vConstMin,vConstMax);
            //printf("cost = %f No Improviments = %d\n",auxPopulation[0].cost,noImproviment);    
            end = isRestart(auxPopulation, mainPopulation, &noImproviment, noImprovimentMax);
            //printSolution(auxPopulation[20].randomKeys,stackLen,nVars,nConst,operationsBi,operationsU,vConstMin,vConstMax);
            for (int i = 0; i < populationLen; i++)
            {
                memcpy(mainPopulation[i].randomKeys, auxPopulation[i].randomKeys, sizeof(chromosome) * individualLen);
                mainPopulation[i].cost = auxPopulation[i].cost;
            }
            //memcpy(mainPopulation,auxPopulation,sizeof(valuedChromosome)*populationLen);
            //printf("auxPopulation:%f\t population:%f\n",auxPopulation[0].cost,population[0].cost); 
        }
        //printf("Erro Atual: %f\t",population[0].cost);
        //printf("Erro Best: %f\n",bestFoundSolution.cost);   
        if ((bestFoundSolution->cost - mainPopulation->cost) > 0.0000001) { //|| (((bestFoundSolution->cost - mainPopulation->cost )< 0.0000001)  && (mainPopulation->trueStackSize < bestFoundSolution->trueStackSize))){
            //printf("Erro Melhorado: %f\n",population[0].cost);
            (*bestFoundSolution).cost = mainPopulation[0].cost; //alterar para memcpy
            memcpy((*bestFoundSolution).randomKeys, mainPopulation[0].randomKeys, sizeof(chromosome) * individualLen);
            //printf("Chegou\n");
            restart = 0;
        }
        //printf("Chegou");
        printf("restart = %d \t best= %f\t", restart, (*bestFoundSolution).cost);
        printSolution(bestFoundSolution->randomKeys, stackLen, nVars, nConst, operationsBi, operationsU, vConstMin, vConstMax, operationsBiLen, operationsULen);
        restart++;
    }
    for (int i = 0; i < populationLen; i++)
    {   //printf("%p\t %p \n",mainPopulation,auxPopulation);
        free(mainPopulation[i].randomKeys);
        free(auxPopulation[i].randomKeys);
    }
    free(mainPopulation);
    free(auxPopulation);
}