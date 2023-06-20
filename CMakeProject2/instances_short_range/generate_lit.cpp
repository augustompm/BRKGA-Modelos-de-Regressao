#include <iostream>
#include <sstream>

using namespace std;

// https://ieeexplore.ieee.org/document/4632147

int
xsqpy(int x, int y)
{
   return x * x + y;
}

// f(x1,x2,x3)= 30 frac{(x1-1)(x3-1)}{x2*x2(x1-10)}
//(30v0*v2-30v0-30v2+30 )/(v1^2*v0-10v0^2)

double func5(double x1, double x2, double x3)
{
   double up =(x1-1)*(x3-1);
   double down =(x2*x2)*(x1-10);
   return 30*up/down;
}

int
main()
{
   srand(0);

   int NUM_CASES = 5;

   for (int c = 1; c <= NUM_CASES; c++) {
      stringstream ssfilename;
      ssfilename << "lit2_func5_" << c << ".in";
      // x square plus y

      FILE* data_sq = fopen(ssfilename.str().c_str(), "w");
      // number of variables (x and y)
      int N = 3;

      // number of tests
      int T = 10*c;

      fprintf(data_sq, "%d\t%d\n", N, T);

      for (unsigned i = 0; i < T; i++) {
         double v1 = (rand() % 100 * c) - 50 * c;
         double v2 = (rand() % 100 * c) - 50 * c;
         double v3 = (rand() % 100 * c) - 50 * c;
         double r = func5(v1, v2, v3);

         fprintf(data_sq, "%f\t%f\t%f\t%f\n", v1, v2, v3, r);
      }

      fclose(data_sq);
   }

   return 0;
}
