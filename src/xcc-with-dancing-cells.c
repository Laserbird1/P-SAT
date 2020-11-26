#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define bool short
#define true 1
#define false 0

// Global variables: Parameters
bool firstSol = false; // Stop at first solution if true; enumerate all solutions otherwise
short verbose = 0;     // Full trace if 2; Print solutions if 1; Print the number of solutions if 0
bool w = false;        // Use coverSize/weight to select the next item if true; use coverSize otherwise

// Global variables used for statistics
int nbSol = 0;   // Number of solutions
int nbCall = 0;  // Number of calls to function X
int nbFail = 0;  // Number of times an inconsistency is detected
int nbProp = 0;  // Number of times the selection of an option is propagated

typedef struct{
    int nb;   // current number of values in the sparse set
    int* val; // val[0..nb-1] = current values in the sparse set
    int* pos; // pos[v] = position of v in val, i.e., val[i] = v <=> pos[v] = i
} SparseSet;

typedef struct{
    int nbOptions;
    int nbItems;
    int nbSec; // Number of secondary items
    // secondary items are numbered from 0 to nbSec-1, and primary items from nbSec to nbItems-1
    char** name; // for each a in 0..nbItems-1, name[a] = name of item a
    int* optionSize; // for each option u, optionSize[u] = number of items in u
    int** items; // for each option u, items[u][i] = ith item in u, with 0<=i<optionSize[u]
    int** colour; // for each option u and each secondary item a, colour[u][i] = colour of items[u][i] in u
} ExactCover;

typedef struct{
    // for each item a, coverSize[a] = number of options that can cover a
    // for each item a and each i in 0..coverSize[a]-1:
    //     cover[a][i] = ith option that can cover a
    //     posInItems[a][i] = index of the cell that contains a in items[cover[a][i]]
    // for each option u and each j in 0..optionSize[u]-1:
    //     posInCover[u][j] = index of the cell that contains u in cover[items[u][j]]
    // Invariant: (cover[a][i]=u and posInItems[a][i]=j) <=> (items[u][j]=a and posInCover[u][j]=i)
    int* coverSize;
    int** cover;
    int** posInItems;
    int** posInCover;
} DancingCells;

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
        xc->colour[o] = (int*) malloc(nb*sizeof(int));
        xc->optionSize[o] = nb;
        for (int i=0; i<nb; i++){
            xc->items[o][i] = items[i];
            if (items[i]<xc->nbSec)
                xc->colour[o][i] = colour[i];
        }
    }
    return xc;
}

DancingCells* initializeDancingCells(ExactCover* xc){
    // create the DancingCells data structure corresponding to xc
    DancingCells* dc = (DancingCells*)malloc(sizeof(DancingCells));
    dc->coverSize = (int*) calloc(xc->nbItems, sizeof(int));
    dc->cover = (int**) malloc(xc->nbItems*sizeof(int*));
    dc->posInItems = (int**) malloc(xc->nbItems*sizeof(int*));
    dc->posInCover = (int**) malloc(xc->nbOptions*sizeof(int*));
    for (int u = 0; u < xc->nbOptions; u++){
        for (int ia=0; ia<xc->optionSize[u]; ia++){
            dc->coverSize[xc->items[u][ia]]++;
        }
    }
    for (int a = 0; a < xc->nbItems; a++){
        dc->cover[a] = (int*) malloc(dc->coverSize[a]*sizeof(int));
        dc->posInItems[a] = (int*) malloc(dc->coverSize[a]*sizeof(int));
        dc->coverSize[a] = 0;
    }
    for (int u = 0; u < xc->nbOptions; u++){
        dc->posInCover[u] = (int*)malloc(xc->optionSize[u]*sizeof(int));
        for (int ia=0; ia<xc->optionSize[u]; ia++){
            int a = xc->items[u][ia];
            dc->cover[a][dc->coverSize[a]] = u;
            dc->posInItems[a][dc->coverSize[a]] = ia;
            dc->posInCover[u][ia] = dc->coverSize[a];
            dc->coverSize[a]++;
        }
    }
    return dc;
}

void removeValue(int v, SparseSet* s){
    // remove v from s, i.e., exchange v with the last value in s
    s->nb--;
    s->val[s->pos[v]] = s->val[s->nb];
    s->pos[s->val[s->nb]] = s->pos[v];
    s->val[s->nb] = v;
    s->pos[v] = s->nb;
}

bool isIn(int v, SparseSet* s){
    // return true is v is a value in s
    return (s->pos[v] < s->nb);
}

void initSparseSet(int size, SparseSet* s){
    // Initialize s with values 0..size-1
    s->val = (int*)malloc(size*sizeof(int));
    s->pos = (int*)malloc(size*sizeof(int));
    for (int i=0; i<size; i++){
        s->val[i] = i;
        s->pos[i] = i;
    }
    s->nb = size;
}

void exchange(int* i, int* j){
    int aux = *i;
    *i = *j;
    *j = aux;
}

void printOption(int u, ExactCover* xc){
    for (int i=0; i<xc->optionSize[u]; i++){
        int b = xc->items[u][i];
        if (isPrimary(b, xc)) printf(" %s",xc->name[b]);
        else printf(" %s:%d",xc->name[b],xc->colour[u][i]);
    }
    printf("\n");
}

void printSolution(ExactCover* xc, DancingCells* dc){
    // For each primary item a, print the option dc->cover[a][0] that covers a
    printf("Solution %d:\n",nbSol);
    for (int a=xc->nbSec; a<xc->nbItems; a++){
        printf("%s is covered by option ",xc->name[a]);
        printOption(dc->cover[a][0], xc);
    }
}

void removeValueDC(int u, int a, int ia_u, DancingCells* dc){
    // Input: an item a, an option u that can cover a, and the index ia_u of a in items[u]
    // remove u from cover[a] by exchanging u with the last option v in cover[a]
    int iv_a = dc->coverSize[a]-1;      // iv_a = index of the last option in cover[a]
    int v = dc->cover[a][iv_a];         // v = last option in cover[a]
    int iu_a = dc->posInCover[u][ia_u]; // iu_a = index of u in cover[a]
    int ia_v = dc->posInItems[a][iv_a]; // ia_v = index of a in items[v]
    exchange(&dc->cover[a][iv_a], &dc->cover[a][iu_a]);
    exchange(&dc->posInItems[a][iv_a], &dc->posInItems[a][iu_a]);
    exchange(&dc->posInCover[v][ia_v], &dc->posInCover[u][ia_u]);
    dc->coverSize[a]--;
}

void removeAllValuesButOne(int u, int a, int ia_u, DancingCells* dc){
    // Input: an option u, an item a, and the index ia_u of a in items[u]
    // remove all values but u from cover[a] by exchanging u with the first option v in cover[a]
    int v = dc->cover[a][0];            // v = first option in cover[a]
    int ia_v = dc->posInItems[a][0];    // ia_v = index of a in items[v]
    int iu_a = dc->posInCover[u][ia_u]; // iu_a = index of u in cover[a]
    exchange(&dc->cover[a][0],&dc->cover[a][iu_a]);
    exchange(&dc->posInItems[a][0],&dc->posInItems[a][iu_a]);
    exchange(&dc->posInCover[u][ia_u],&dc->posInCover[v][ia_v]);
    dc->coverSize[a] = 1;
}

bool propagate(int u, ExactCover* xc, SparseSet* untreated, DancingCells* dc, float* weight){
    // propagate the selection of option u
    nbProp++;
    if (verbose > 1){
        printf("    Propagation %d -> Selection of option:", nbProp);
        printOption(u, xc);
    }
    for (int ia=0; ia<xc->optionSize[u]; ia++){
        int a = xc->items[u][ia]; // a is an item covered by option u
        if (isIn(a,untreated)){
            // a belongs to untreated if a is primary or a is secondary and not yet coloured
            for (int iv=0; iv<dc->coverSize[a]; iv++){
                int v = dc->cover[a][iv]; // v is an option that can cover a
                int ia_v = dc->posInItems[a][iv];
                if ((u != v) && (isPrimary(a, xc) || xc->colour[u][ia] != xc->colour[v][ia_v])){
                    // v is incompatible with u
                    for (int ib=0; ib<xc->optionSize[v]; ib++){
                        int b = xc->items[v][ib]; // b is an item in v
                        if (b != a && isIn(b,untreated)){
                            if ((dc->coverSize[b] == 1) && (isPrimary(b, xc))){
                                // b cannot be covered
                                if (w) weight[b] += 1;
                                return false;
                            }
                            removeValueDC(v, b, ib, dc); // remove v from cover[b]
                        }
                    }
                }
            }
            if (isPrimary(a, xc)) removeAllValuesButOne(u, a, ia, dc);
            removeValue(a, untreated);
        }
    }
    return true;
}

int heuristic(int a, int b, DancingCells* dc, float* weight){
    // return the item in {a,b} that should be covered first
    if (a<0) return b;
    if (w){
        if (dc->coverSize[b]/weight[b] < dc->coverSize[a]/weight[a]) return b;
        return a;
    }
    if (dc->coverSize[b] < dc->coverSize[a]) return b;
    return a;
}

void X(ExactCover* xc, SparseSet* untreated, DancingCells* dc, float* weight){
    // untreated contains all uncovered primary items and all uncoloured secondary items
    // for each primary covered item a, dc->cover[a][0] = option that covers a
    // for each untreated item a, dc->cover[a][0..coverSize[a]-1] = options that may be used to cover a
    nbCall++;
    // Save sizes and search for the next primary item a to cover
    int untreatedSize = untreated->nb;
    int saveCoverSize[xc->nbItems];
    int a = -1;
    for (int ib=0; ib<untreated->nb; ib++){
        int b = untreated->val[ib];
        saveCoverSize[b] = dc->coverSize[b];
        if (isPrimary(b, xc)) a = heuristic(a, b, dc, weight);
    }
    if (a < 0){ // all primary items are covered!
        nbSol++;
        if (verbose > 0) printSolution(xc, dc);
        if (firstSol){
            printf("%d calls; %d failures; %d propagations\n", nbCall, nbFail, nbProp);
            exit(0);
        }
        return;
    }
    if (verbose > 1) printf("Call %d -> Try to cover item %s\n", nbCall, xc->name[a]);
    // save the list of options that can cover a, to iterate on them
    int saveCover[saveCoverSize[a]];
    for (int iu=0; iu<saveCoverSize[a]; iu++) saveCover[iu] = dc->cover[a][iu];
    // iterate on every option u that can cover a
    for (int iu=0; iu<saveCoverSize[a]; iu++){
        int u = saveCover[iu];
        // cover item a with option u
        if (propagate(u, xc, untreated, dc, weight))
            X(xc, untreated, dc, weight);
        else{
            nbFail++;
            if (verbose > 1){
                printf("        Failure: %s cannot be covered by option:",xc->name[a]);
                printOption(u, xc);
            }
        }
        // restore sizes
        untreated->nb = untreatedSize;
        for (int ib=0; ib<untreatedSize; ib++){
            int b = untreated->val[ib];
            dc->coverSize[b] = saveCoverSize[b];
        }
    }
}

int main(int argc, char* argv[]){
    char c;
    extern char* optarg;
    while ((c = getopt (argc, argv, "v:fw")) != -1){
        switch (c){
            case 'v': verbose = atoi(optarg); break;
            case 'f': firstSol = true; break;
            case 'w': w = true; break;
        }
    }
    ExactCover* xc = getInstance();
    if (xc==NULL) return 0;
    SparseSet untreated;
    initSparseSet(xc->nbItems, &untreated);
    DancingCells* dc = initializeDancingCells(xc);
    float weight[xc->nbItems];
    for (int i=0; i<xc->nbItems; i++) weight[i] = 1;
    X(xc, &untreated, dc, weight);
    printf("%d solutions; %d calls; %d failures; %d propagations\n", nbSol, nbCall, nbFail, nbProp);
}
