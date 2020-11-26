#include <stdio.h>
#include <stdlib.h>
int n;

main(int argc, char *argv[])
{
    register int i, j, n, nn;
    int pn;

    if (argc != 2 || sscanf(argv[1], "%d", &pn) != 1)
    {
        fprintf(stderr, "Usage: %s n\n", argv[0]);
        exit(-1);
    }
    n = pn;
    nn = n + n;
    int mod = n % 4 ;
    if (mod % 3 != 0)
    {
        fprintf(stderr, "Sorry, Langford Pairing doesn't exists for n != {0,3} mod 4, and here n = %d mod 4\n",mod);
        exit(-2);
    }
    int numberOfOptions = (3*n*(n-1))>>1; // (3/2)*n*(n-1)
    // print (number of primary items), (number of secondary items), (number of options)
    printf("%d %d %d\n",n+nn,0,numberOfOptions);
    // print each items name
    //_ print each number
    for(i = 1; i <= n; ++i){
        printf("N%d\n",i);
    }
    //_ print each place that the numbers can take
    for(i = 0; i < nn; ++i){
        printf("S%d\n",i);
    }

    // print options
    for(i = 1; i <= n; ++i){
        for(j = 0;j < nn-i-1;j++){
            printf("%d %d %d\n",i-1,n+j,n+j+i+1);
        }
    }
}