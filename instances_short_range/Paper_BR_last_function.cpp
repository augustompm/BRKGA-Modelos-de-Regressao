#include <iostream>
#include <sstream>

using namespace std;

// generate random x^2 function data

double
xsq(double x)
{
   return (2*x * x * x * x * x) - (3*x*x*x*x)+(4*x*x*x)-(5*x*x)+(6*x)-7;
}

int
main()
{
   srand(0);

   int NUM_CASES = 3; // different cases, with different sizes

   for (int c = 1; c <= NUM_CASES; c++) {
      stringstream ssfilename;
      ssfilename << "paper_BR_last_fuction_short_range_" << c << ".in";
      // 's' for 'simple case'

      FILE* data_sq = fopen(ssfilename.str().c_str(), "w");
      // number of variables
      int N = 1; 

      // number of tests
      int T = 20 * c;

      fprintf(data_sq, "%d\t%d\n", N, T);

      for (unsigned i = 0; i < T; i++) {
         // only one var
         double v1 = (rand() % 6) -3 + (rand() % 100000)/100000.0;
         double r = xsq(v1);

         fprintf(data_sq, "%f\t%f\n", v1, r);
      }

      fclose(data_sq);
   }

   return 0;
}
