#include <iostream>
#include <sstream>

using namespace std;

// generate random x^2 function data

long int
xsq(long int x)
{
   return (2*x * x * x * x * x) - (3*x*x*x*x)+(4*x*x*x)-(5*x*x)+(6*x)-7;
}

int
main()
{
   srand(0);

   int NUM_CASES = 5; // different cases, with different sizes

   for (int c = 1; c <= NUM_CASES; c++) {
      stringstream ssfilename;
      ssfilename << "paper_BR_last_fuction_" << c << ".in";
      // 's' for 'simple case'

      FILE* data_sq = fopen(ssfilename.str().c_str(), "w");
      // number of variables
      int N = 1; 

      // number of tests
      int T = 10 * c;

      fprintf(data_sq, "%d\t%d\n", N, T);

      for (unsigned i = 0; i < T; i++) {
         // only one var
         long int v = (rand() % 100 * c) - 50 * c;
         long int r = xsq(v);

         fprintf(data_sq, "%ld\t%ld\n", v, r);
      }

      fclose(data_sq);
   }

   return 0;
}
