#include <iostream>
#include <sstream>

using namespace std;

// generate random x^3+x^2+x function data

long int
xsq(long int x)
{
   return (x * x * x) + (x * x ) + x ;
}

int
main()
{
   srand(0);

   int NUM_CASES = 1; // different cases, with different sizes

   for (int c = 1; c <= NUM_CASES; c++) {
      stringstream ssfilename;
      ssfilename << "xcubic_xsquare_px_special" << c << ".in";
      // 's' for 'simple case'

      FILE* data_sq = fopen(ssfilename.str().c_str(), "w");
      // number of variables
      int N = 1; 

      // number of tests
      int T = 10 * c;

      fprintf(data_sq, "%d\t%d\n", N, T);

      for (unsigned i = 0; i < T; i++) {
         // only one var
         long int v = (rand() % 100 * 1000) - 50 * 1000;
         long int r = xsq(v);
         printf("%ld\t%ld\n", v, r);
         fprintf(data_sq, "%ld\t%ld\n", v, r);
      }

      fclose(data_sq);
   }

   return 0;
}
