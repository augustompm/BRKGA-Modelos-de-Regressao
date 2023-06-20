#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int main (){

    double x = 80695490488613289;
    double y = pow(2,64);

    //long int z = (long int)(x-y);

    printf("%.2f\n",x);
    printf("%.2f\n",y);
    //printf("%ld\n",sizeof(z));

    return 0;
}