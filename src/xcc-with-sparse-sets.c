#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define bool short
#define true 1
#define false 0

typedef struct{
    int nb;   // current number of values in the sparse set
    int* val; // val[0..nb-1] = current values in the sparse set
    int* pos; // pos[v] = position of v in val, i.e., val[i] = v <=> pos[v] = i
} SparseSet;

typedef struct{
    int nbOptions;
    int nbItems;
    int nbSec; // number of secondary items
    // secondary items are numbered from 0 to nbSec-1, and primary items from nbSec to nbItems-1
    char** name; // for each item a, name[a] = name of a
    int* optionSize; // for each option u, optionSize[u] = number of items in u
    int** items; // for each option u, items[u][i] = ith item in u, with 0<=i<optionSize[u]
    int** colour; // for each option u and each secondary item a, colour[u][a] = colour of a in u
} ExactCover;

bool isPrimary(int a, ExactCover* xc){
    // return true if a is a primary item
    return (a >= xc->nbSec);
}

ExactCover* getInstance(){
    // read an instance on the standard input and create the corresponding ExactCover structure
    // the first line gives the number n of items, the number k of secondary items, and the number m of options
    // the next n lines give the names of the n items
    // the next m lines give the items of the k options
    ExactCover* xc = (ExactCover*)malloc(sizeof(ExactCover));
    scanf("%d %d %d\n",&(xc->nbItems),&(xc->nbSec),&(xc->nbOptions));
    xc->optionSize = (int*) malloc(xc->nbOptions*sizeof(int));
    xc->items = (int**) malloc(xc->nbOptions*sizeof(int*));
    xc->colour = (int**) malloc(xc->nbOptions*sizeof(int*));
    xc->name = (char**) malloc(xc->nbItems*sizeof(char*));
    for (int i=0; i<xc->nbItems; i++){
        char n[20];
        scanf("%s",n);
        xc->name[i] = (char*)malloc(strlen(n));
        strcpy(xc->name[i],n);
    }
    int items[xc->nbItems];
    int colour[xc->nbItems];
    for (int o=0; o<xc->nbOptions; o++){
        char c;
        int nb = 0;
        do{
            scanf("%d",&(items[nb]));
            if (!isPrimary(items[nb],xc)){
                scanf(":%d",&(colour[nb]));
            }
            scanf("%c",&c);
            nb++;
        } while (c != '\n');
        xc->items[o] = (int*) malloc(nb*sizeof(int));
        xc->colour[o] = (int*) malloc((xc->nbSec)*sizeof(int));
        xc->optionSize[o] = nb;
        for (int i=0; i<nb; i++){
            xc->items[o][i] = items[i];
            if (items[i]<xc->nbSec)
                xc->colour[o][items[i]] = colour[i];
        }
    }
    return xc;
}

SparseSet** initializeCovers(ExactCover* xc){
    // for each item a, create the sparse set cover[a] which contains all options that may cover a
    SparseSet** cover = (SparseSet**) malloc(xc->nbItems*sizeof(SparseSet*));
    for (int a = 0; a < xc->nbItems; a++){
        cover[a] = (SparseSet*) malloc(sizeof(SparseSet));
        cover[a]->nb = 0;
        cover[a]->val = (int*) malloc(xc->nbOptions*sizeof(int));
        cover[a]->pos = (int*) malloc(xc->nbOptions*sizeof(int));
    }
    for (int u = 0; u < xc->nbOptions; u++){
        for (int ia=0; ia<xc->optionSize[u]; ia++){
            int a = xc->items[u][ia];
            cover[a]->val[cover[a]->nb] = u;
            cover[a]->pos[u] = cover[a]->nb;
            cover[a]->nb++;
        }
    }
    return cover;
}

void removeValue(int v, SparseSet* s){
    // remove v from s, i.e., exchange v with the last value in s
    s->nb--;
    s->val[s->pos[v]] = s->val[s->nb];
    s->pos[s->val[s->nb]] = s->pos[v];
    s->val[s->nb] = v;
    s->pos[v] = s->nb;
}

void removeAllValuesButOne(int v, SparseSet* s){
    // remove all values but v from s, i.e., exchange v with the first value in s
    s->val[s->pos[v]] = s->val[0];
    s->pos[s->val[0]] = s->pos[v];
    s->val[0] = v;
    s->pos[v] = 0;
    s->nb = 1;
}

bool isIn(int v, SparseSet* s){
    // return true is v is a value in s
    return (s->pos[v] < s->nb);
}

bool propagate(int u, ExactCover* xc, SparseSet* uncovered, SparseSet** cover){
    // propagate the selection of option u
    for (int ia=0; ia<xc->optionSize[u]; ia++){
        int a = xc->items[u][ia]; // a is an item covered by option u
        if (isPrimary(a, xc) || isIn(a,uncovered)){
            // we update cover either when a is primary, or when a is secondary and not yet coloured
            for (int iv=0; iv<cover[a]->nb; iv++){
                int v = cover[a]->val[iv]; // v is an option that covers a
                if ((u != v) && (isPrimary(a, xc) || xc->colour[u][a] != xc->colour[v][a])){
                    // v is incompatible with u
                    for (int ib=0; ib<xc->optionSize[v]; ib++){
                        int b = xc->items[v][ib]; // b is an item in v
                        if (b != a && isIn(b,uncovered)){
                            if ((cover[b]->nb == 1) && (isPrimary(b, xc))) return false;
                             removeValue(v, cover[b]);
                        }
                    }
                }
            }
            removeValue(a, uncovered);
            removeAllValuesButOne(u, cover[a]);
        }
    }
    return true;
}

void printSolution(int* nbSolutions, ExactCover* xc, SparseSet** cover){
    // For each primary item a, print the option cover[a]->val[0] that covers a
    printf("Solution %d:\n",*nbSolutions);
    for (int a=xc->nbSec; a<xc->nbItems; a++){
        printf("%s is covered by option ",xc->name[a]);
        for (int i=0; i<xc->optionSize[a]; i++){
            int b = xc->items[cover[a]->val[0]][i];
            if (isPrimary(b, xc)) printf("%s ",xc->name[b]);
            else printf("%s:%d ",xc->name[b],xc->colour[cover[a]->val[0]][b]);
        }
        printf("\n");
    }
}

void X(ExactCover* xc, SparseSet* uncovered, SparseSet** cover, int* nbSolutions, int* nbCalls, bool firstSol, bool verbose){
    // uncovered->val[0..uncovered->nb-1] contains all items of xc which are not yet covered
    // for each covered item a, cover[a]->val[0] = option that covers a
    // for each uncovered item a, cover[a]->val[0..cover[a]->nb-1] = options that may be used to cover a
    (*nbCalls)++;
    // Save sparse set sizes and search for the next primary item a to cover
    int uncoveredSize = uncovered->nb;
    int coverSize[xc->nbItems];
    int a = -1;
    for (int ib=0; ib<uncovered->nb; ib++){
        int b = uncovered->val[ib];
        coverSize[b] = cover[b]->nb;
        if ((isPrimary(b, xc)) && ((a<0) || (coverSize[b] < coverSize[a])))
            a = b;
    }
    if (a < 0){ // all primary items are covered
        (*nbSolutions)++;
        if (verbose) printSolution(nbSolutions, xc, cover);
        if (firstSol){
            printf("%d nodes\n",*nbCalls);
            exit(0);
        }
        return;
    }
    // save the list of options that cover a, to iterate on them
    int saveCover[coverSize[a]];
    memcpy(saveCover, cover[a]->val, coverSize[a]*sizeof(int));
    // iterate on every option u that covers a
    for (int iu=0; iu<coverSize[a]; iu++){
        int u = saveCover[iu];
         // try to cover item a with option u
        if (propagate(u, xc, uncovered, cover))
            X(xc, uncovered, cover, nbSolutions, nbCalls, firstSol, verbose);
        // restore all sparse sets
        uncovered->nb = uncoveredSize;
        for (int ib=0; ib<uncoveredSize; ib++){
            int b = uncovered->val[ib];
            cover[b]->nb = coverSize[b];
        }
    }
}

int main(int argc, char* argv[]){
    char c;
    bool firstSol = false;
    bool verbose = false;
    while ((c = getopt (argc, argv, "vf")) != -1){
        switch (c){
            case 'v': verbose = true; break;
            case 'f': firstSol = true; break;
            default: printf("Unknown option %c...\n",c);
        }
    }
    ExactCover* xc = getInstance();
    if (xc==NULL) return 0;
    // create the sparse set which contains all uncovered items
    SparseSet* uncovered = (SparseSet*) malloc(sizeof(SparseSet));
    uncovered->nb = xc->nbItems;
    uncovered->val = (int*) malloc(xc->nbItems*sizeof(int));
    uncovered->pos = (int*) malloc(xc->nbItems*sizeof(int));
    for (int a = 0; a < xc->nbItems; a++){
        uncovered->val[a] = a;
        uncovered->pos[a] = a;
    }
    // create sparse sets associated with items
    SparseSet** cover = initializeCovers(xc);
    // search for all solutions
    int nbSolutions = 0;
    int nbCalls = 0;
    X(xc, uncovered, cover, &nbSolutions, &nbCalls, firstSol, verbose);
    printf("%d solutions; %d nodes\n",nbSolutions,nbCalls);
    return 0;
}
