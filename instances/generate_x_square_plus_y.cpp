#include <iostream>
#include <sstream>

using namespace std;

int
xsqpy(int x, int y)
{
   return x * x + y;
}

int
main()
{
   srand(0);

   int NUM_CASES = 5; // different cases, with different sizes

   for (int c = 1; c <= NUM_CASES; c++) {
      stringstream ssfilename;
      ssfilename << "xsquare_py_" << c << ".in";
      // x square plus y

      FILE* data_sq = fopen(ssfilename.str().c_str(), "w");
      // number of variables (x and y)
      int N = 2;

      // number of tests
      int T = 10 * c;

      fprintf(data_sq, "%d\t%d\n", N, T);

      for (unsigned i = 0; i < T; i++) {
         int v1 = (rand() % 100 * c) - 50 * c;
         int v2 = (rand() % 100 * c) - 50 * c;
         int r = xsqpy(v1, v2);

         fprintf(data_sq, "%d\t%d\t%d\n", v1, v2, r);
      }

      fclose(data_sq);
   }

   return 0;
}
