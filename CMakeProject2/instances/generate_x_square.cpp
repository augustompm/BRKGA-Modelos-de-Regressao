#include <iostream>
#include <sstream>

using namespace std;

// generate random x^2 function data

int
xsq(int x)
{
   return x * x;
}

int
main()
{
   srand(0);

   int NUM_CASES = 5; // different cases, with different sizes

   for (int c = 1; c <= NUM_CASES; c++) {
      stringstream ssfilename;
      ssfilename << "xsquare_s_" << c << ".in";
      // 's' for 'simple case'

      FILE* data_sq = fopen(ssfilename.str().c_str(), "w");
      // number of variables
      int N = 1; 

      // number of tests
      int T = 10 * c;

      fprintf(data_sq, "%d\t%d\n", N, T);

      for (unsigned i = 0; i < T; i++) {
         // only one var
         int v = (rand() % 100 * c) - 50 * c;
         int r = xsq(v);

         fprintf(data_sq, "%d\t%d\n", v, r);
      }

      fclose(data_sq);
   }

   return 0;
}
