#pragma once

typedef unsigned short chromosome;

int stackAdjustment(chromosome* individual,int stackLen,int nVars,int nConst,int maxConst,int seed);
double solutionEvaluator (chromosome* individual,char operationsBi[],char operationsU[],int stackLen,int nVars,int tests,double** inputs,double* outputs,double* vConstMin,double* vConstMax,int nConst,int training,int operationsBiLen,int operationsULen);
double execBinaryOp(int idop, double v1, double v2,char operationsBi[]);
double execUnaryOp(int idop, double v1,char operationsU[]);
double computeError(double v1, double v2);

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <brkgp/PrintIO.hpp>
#include <stack>
#include <brkgp/Evaluator.hpp>
#include <kahan-float/src/kahan.hpp>

using namespace std;
using namespace kahan;
typedef unsigned short chromosome;

int stackAdjustment(chromosome* individual, int stackLen, int nVars, int nConst, int maxConst, int seed)
{
    //5303   358   3064   9156   4199   1320   636   7306   2445   6166   3572   8249   4290   4389   4790   567   9692   202   3913   498 
    //1      -1      0      2     0      -1     -1     1     -1     1       0     2       0     0      0     -1     2      -1     0     -1
    //1       1       0     2     0       -1     1     1     -1     1       0     2       0     0      0     -1     2      -1     0     1
    int somador = 0;
    int auxSomador = 0;
    int decodValue;
    int contConst = 0;
    int idConst = 0;
    int trueStackLen = 0;
    for (int i = 0; i < stackLen; i++)
    {
        decodValue = floor((individual[i] / 10000.0) * 4) - 1;
        idConst = floor((individual[stackLen + i] / 10000.0) * (nVars + nConst)) - nConst;
        if (decodValue != 2)
        {
            auxSomador += decodValue;
        }
        if (auxSomador < 1)
        {
            if (individual[i] < 2500)
                individual[i] += 5000;

            else if (individual[i] >= 7500) // se der testar depois, só no caso i=0 
                individual[i] -= 2500;

            else                          //se der testar depois, só no caso i=0 
                individual[i] += 2500;
            auxSomador = somador + 1;
        }
        if (auxSomador > (stackLen - i))
        {
            if (somador == 1) {
                individual[i] += 2500;
                auxSomador = 1;
            }
            else if (somador > 1) {
                if ((individual[i] >= 2500) && (individual[i] < 5000))
                    individual[i] -= 2500;

                else if (individual[i] >= 7500) // se der testar depois, só no caso i=0 
                    individual[i] -= 7500;

                else                          //se der testar depois, só no caso i=0 
                    individual[i] -= 5000;
                auxSomador = somador - 1;
            }

        }
        if ((decodValue == 1) && idConst < 0)
        {
            contConst++;
            if (contConst > maxConst)
            {
                srand(seed);
                individual[stackLen + i] = ((10000.0 / (nVars + nConst)) * nConst) + ((10000.0 / (nVars + nConst)) * (rand() % nVars) + 1);
                seed++;
            }
        }
        somador = auxSomador;
        if (individual[i] < 7500)
            trueStackLen++;
    }
    return trueStackLen;
}

double solutionEvaluator(chromosome* individual, char operationsBi[], char operationsU[], int stackLen, int nVars, int tests, double** inputs, double* outputs, double* vConstMin, double* vConstMax, int nConst, int training, int operationsBiLen, int operationsULen)
{
    double solutionValue = 0;
    int contSeed;
    chromosome seedConst = 0;
    kfloat64 sum_error = 0;
    //double sum_error = 0;
    double binaryProduct;
    for (int i = 0; (i + training) < tests; i++)
    {
        stack<double> stk;
        contSeed = 0;

        for (int j = 0; j < stackLen; j++) {
            //cout << "begin var i=" << i << " / sol_size=" << rep.vstack.size() << endl;
            // case: push
            // -1 0 1 2
            // 2 3  
            if ((individual[j] < 7500) && (individual[j] >= 5000)) {
                // push variable
                int idVar = floor((individual[stackLen + j] / 10000.0) * (nVars + nConst)) - nConst;
                //printf("%d\n",idVar);
                double varValue = 0;

                if (idVar >= 0) {
                    varValue = inputs[i][idVar];
                    stk.push(varValue);
                }
                else {
                    idVar += nConst;
                    if (vConstMin[idVar] == vConstMax[idVar]) {
                        varValue = vConstMin[idVar];
                        stk.push(varValue);
                    }
                    else {
                        seedConst = individual[3 * stackLen + contSeed];
                        srand(seedConst);
                        varValue = rand() % (int)(vConstMax[idVar] - vConstMin[idVar] + 1) + vConstMin[idVar]; // only work for constant between -2b and +2b
                        stk.push(varValue);
                        contSeed++;
                    }

                }
            }

            // case: pop
            if (individual[j] < 2500) {
                // pop operation
                int idOpBi = floor((individual[2 * stackLen + j] / 10000.0) * operationsBiLen); // 4 is lenght of operationBi
                //assert(idOp != -1); // guarantee that it's not "disabled" (-1)
                //
                double v1 = stk.top();
                stk.pop();
                double v2 = stk.top();
                stk.pop();
                binaryProduct = execBinaryOp(idOpBi, v1, v2, operationsBi);
                // if for case: division by zero
                if (abs(binaryProduct) == INFINITY) {
                    while (stk.size() > 0)
                        stk.pop();
                    stk.push(INFINITY);
                    j = stackLen;
                }
                else
                    stk.push(binaryProduct);
            }
            if ((individual[j] < 5000) && (individual[j] >= 2500)) {
                // pop operation
                int idOpU = floor((individual[2 * stackLen + j] / 10000.0) * operationsULen); // 3 is lenght of operationU
                //assert(idOp != -1); // guarantee that it's not "disabled" (-1)
                //
                double v1 = stk.top();
                stk.pop();
                stk.push(execUnaryOp(idOpU, v1, operationsU));
            }
            if (individual[j] >= 7500) {

            }
            //
            //cout << "i=" << i << " -> stack size = " << stk.size() << endl;
        }

        //
        //cout << "finished t= " << t << endl;
        // take value from stack
        //assert(stk.size() == 1);
        double val = stk.top();
        //printf("val = %lf\n",val);
        stk.pop(); // drop last

        //
        // compare with expected valu

        sum_error += computeError(val, outputs[i]);
        //printf("sum_error = %lf\n",sum_error);
    }
    // average erro
    solutionValue = (double)sum_error / (tests - training);
    //printf("solutionValue = %lf\n",solutionValue);
    //printf("sum_error = %f solutionValue = %f\n",sum_error,solutionValue);
    //printf("%f\n",solutionValue);
    return solutionValue;
}


double execBinaryOp(int idop, double v1, double v2, char operationsBi[])
{
    if (operationsBi[idop] == '+') {
        if (abs(v1) == INFINITY)
            return INFINITY;
        return v1 + v2;
    }
    if (operationsBi[idop] == '-') {
        if (abs(v1) == INFINITY)
            return INFINITY;
        return v1 - v2;
    }
    if (operationsBi[idop] == '*') {
        if ((abs(v1) == INFINITY && v2 == 0) || (v1 == 0 && abs(v2) == INFINITY))
            return INFINITY;
        return v1 * v2;
    }
    if (operationsBi[idop] == '/') {
        if (v2 < 0.000001)
            return INFINITY;
        if (abs(v2) == INFINITY)
            return 0;
        return v1 / v2;
    }
    return 0.0;
}

double execUnaryOp(int idop, double v1, char operationsU[])
{
    if (operationsU[idop] == 's') {
        if (v1 == INFINITY)
            return INFINITY;
        return sin(v1);
    }

    if (operationsU[idop] == 'c') {
        if (v1 == INFINITY)
            return INFINITY;
        return cos(v1);
    }
    if (operationsU[idop] == 'i')
        return v1;
    if (operationsU[idop] == 'a')
        return v1 * v1;
    if (operationsU[idop] == 'v')
        return v1 * v1 * v1;
    if (operationsU[idop] == 'r')
    {
        if (v1 < 0)
            return INFINITY;
        else
            return pow(v1, 1 / 2);
    }
    return 0.0;
}

// error between values v1 and v2 RMSE
double computeError(double v1, double v2)
{
    // square
    return sqrt((v1 - v2) * (v1 - v2));
}