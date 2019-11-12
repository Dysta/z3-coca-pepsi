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

/**
 * @brief Generates a SAT formula satisfiable if and only there is at least a vertex at each position
 * @param ctx The solver context
 * @param graph A graph
 * @param pathLength The length of the path to check
 * @return Z3_ast The formula
 */
Z3_ast existOneVertex(Z3_context ctx, int number, Graph graph, int pathLength){
    Z3_ast TabEx[pathLength][orderG(graph)];
    for(int i = 0; i<pathLength ; i++){
        for (int j=0 ; j<orderG(graph) ; j++){
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


/**
 * @brief Generates a SAT formula satisfiable if and only there is at max 1 vertex at each position
 * @param ctx The solver context
 * @param graph A graph
 * @param pathLength The length of the path to check
 * @return Z3_ast The formula
 */
Z3_ast maxOneVertex(Z3_context ctx, int number, Graph graph, int pathLength){
    Z3_ast TabEx[pathLength][orderG(graph)];
    for(int i = 0; i<pathLength ; i++){
        for (int j=0 ; j<orderG(graph) ; j++){
            TabEx[i][j] = getNodeVariable(ctx, number, i, pathLength, j);
        }
    }

    Z3_ast maxAST[pathLength][orderG(graph)][orderG(graph)-1];
    int pos;
    Z3_ast neg1, neg2;
    Z3_ast TabNot[2];
    for(int i = 0; i<pathLength ; i++){
        for (int j = 0; j<orderG(graph){
            pos = 0;
            neg1 = Z3_mk_not(ctx, TabEx[i][j]);
            TabNot[0] = neg1;
            for int(k = 0; k<orderG(graph); k++){
                if(k != j){
                    neg2 = Z3_mk_not(ctx, TabEx[i][k]);
                    TabNot[1]=neg2;
                    maxAST[i][j][pos] = Z3_mk_or(ctx, 2, TabNot);
                    pos ++;
                }
            }
        }
    }
    Z3_ast maxAST2[pathLength][orderG(graph)];
    for(int i = 0; i< pathLength; i++){
        for (int j = 0; j< orderG(graph), j++){
            maxAST2[i][j] = Z3_mk_and(ctx, orderG(graph)-1, maxAST[i][j]);
        }
    }

    Z3_ast maxAST3[pathLength];
    for int(i = 0; i<pathLength; i++){
        maxAST3[i]=Z3_mk_and(ctx, pathLength, maxAST2[i]);
    }

    Z3_ast result = Z3_mk_and(ctx, pathLength, maxAST3);
    return result;
}


Z3_ast ExistPath(Z3_context ctx, int number, Graph graph, int pathLength){
    Z3_ast TabEx[pathLength][orderG(graph)];
    for(int i = 0; i<pathLength ; i++){
        for (int j=0 ; j<orderG(graph) ; j++){
            TabEx[i][j] = getNodeVariable(ctx, number, i, pathLength, j);
        }
    }
    Z_3ast ExistPathAST[pathLength-1][]

