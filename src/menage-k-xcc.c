#include <stdio.h>
#include <stdlib.h>
int n;

//implementation of the program for generating data for xcc for solving the probleme des menages,
//following the knuth way of doing presented in his 2018 christmas lecture.

//We consider the women are already seated, in order.
// For a woman i, the seat at her left is the seat i.
// Therefore, a man i cannot seat at place i and i-1, mod n. 

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
    if (n < 3)
    {
        fprintf(stderr, "Sorry, the probleme des menages exists only for n >= 3\n");
        exit(-2);
    }

    int numberOfOptions = n*(n-2);
    // print (number of primary items), (number of secondary items), (number of options)
    printf("%d %d %d\n",nn,0,numberOfOptions);
    // print each items name
    for(i = 0; i < n; ++i){
        printf("S%d\n",i);
    }
    for(i = 0; i < n; ++i){
        printf("M%d\n",i);
    }

    // print options
    for(i = 0; i < n; ++i){
        for(j = (i+1)%n; j != (i-1+n)%n; j = (j+1)%n){
            //seat_number man_number
            printf("%d %d\n",i,n+j);
        }
    }
}