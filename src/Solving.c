#include "Solving.h"
#include "Z3Tools.h"
#include <stdlib.h>
#include <stdio.h>


Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node) {
    int size = 256;
    char buff[size];
    int n = snprintf(buff, size, "x(%d, %d, %d, %d)", number, position, k, node);
    if (n >= size) {
        fprintf(stderr, "Lenght of parameter in getNodeVariable too long\n");
        exit(EXIT_FAILURE);
    }

    return mk_bool_var(ctx, buff);
}

// Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength)
Z3_ast existVertex(Z3_context ctx, int number, Graph graph, int pathLength){
    Z3_ast TabEx[pathLength][orderG(graph)];
    for(int i = 0; i<orderG(graph) ; i++){
        for (int j=0 ; j<pathLength ; j++){
            TabEx[i][j] = getNodeVariable(ctx, number, i, pathLength, j);
        }
    }
    Z3_ast existAST[pathLength];
    for(int i = 0; i<pathLength ; i++){
        existAST[i] = Z3_mk_or(ctx, orderG(graph), TabEx[i]);
    }
    Z3_ast result = Z3_mk_and(ctx, pathLength, existAST);
    return result;
}

