#include <iostream>
#include <sstream>
#include <math.h>

using namespace std;

// https://ieeexplore.ieee.org/document/4632147

int
xsqpy(int x, int y)
{
   return x * x + y;
}

//f(x1,x2) = 6*sin(x1)*cos(x2) 

double func6(double x1, double x2)
{
   return 2.5*x1*x1*x1*x1- 1.3*x1*x1*x1 + 0.5*x2*x2 -1.7*x2;
}

int
main()
{
   srand(0);
   int NUM_CASES = 5;

   for (int c = 1; c <= NUM_CASES; c++) {
      stringstream ssfilename;
      ssfilename << "lit3_BSR_func1_" << c << ".in";
      // x square plus y

      FILE* data_sq = fopen(ssfilename.str().c_str(), "w");
      // number of variables (x and y)
      int N = 2;

      // number of tests
      int T = 10*c;

      fprintf(data_sq, "%d\t%d\n", N, T);

      for (unsigned i = 0; i < T; i++) {
         double v1 = (rand() % 100 * c) - 50 * c;
         double v2 = (rand() % 100 * c) - 50 * c;
         double r = func6(v1, v2);

         fprintf(data_sq, "%f\t%f\t%f\n", v1, v2, r);
      }

      fclose(data_sq);
   }

   return 0;
}
