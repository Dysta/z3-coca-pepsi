#include "Solving.h"
#include "Z3Tools.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief choose the order mode to explore length
 */


Z3_ast getNodeVariable(Z3_context ctx, int number, int position, int k, int node) {
    int size = 256;
    char buff[size];
    int n = snprintf(buff, size, "x(%d,%d,%d,%d)", number, position, k, node);
    if (n >= size) {
        fprintf(stderr, "Lenght of parameter in getNodeVariable too long\n");
        exit(EXIT_FAILURE);
    }

    return mk_bool_var(ctx, buff);
}


/**
 * @brief Generates a SAT formula satisfiable if and only there is at least a vertex at each position
 * @param ctx The solver context
 * @param graph A graph
 * @param pathLength The length of the path to check
 * @return Z3_ast The formula
 * FONCTIONNE !
 */
Z3_ast existOneVertex(Z3_context ctx, int number, Graph graph, int pathLength){
    //printf("ExistOneVertex \n");
    Z3_ast TabEx[pathLength+1][orderG(graph)];
    Z3_ast existAST[pathLength+1];
    for(int i = 0; i<=pathLength ; i++){
        for (int j=0 ; j<orderG(graph) ; j++){
            TabEx[i][j] = getNodeVariable(ctx, number, i, pathLength, j);
        }
        existAST[i] = Z3_mk_or(ctx, orderG(graph), TabEx[i]);
    }
    Z3_ast result = Z3_mk_and(ctx, pathLength+1, existAST);
    //printf("\n==========ExistOne============\n%s",Z3_ast_to_string(ctx,result));
    return result;
}


/**
 * @brief Generates a SAT formula satisfiable if and only there is at max 1 vertex at each position
 * @param ctx The solver context
 * @param graph A graph
 * @param pathLength The length of the path to check
 * @return Z3_ast The formula
 * FONCTIONNE !
 */
Z3_ast maxOneVertex(Z3_context ctx, int number, Graph graph, int pathLength){
    //printf("MaxOneVertex\n");
    Z3_ast TabEx[pathLength+1][orderG(graph)];
    for(int i = 0; i<pathLength+1 ; i++){
        for (int j=0 ; j<orderG(graph) ; j++){
            TabEx[i][j] = getNodeVariable(ctx, number, i, pathLength, j);
        }
    }
    Z3_ast maxAST[pathLength+1][orderG(graph)][orderG(graph)-1];
    Z3_ast maxAST2[pathLength+1][orderG(graph)];
    Z3_ast maxAST3[pathLength+1];
    int pos;
    Z3_ast neg1, neg2;
    Z3_ast TabNot[2];
    for(int i = 0; i<=pathLength ; i++){
        for (int j = 0; j<orderG(graph); j++){
            pos = 0;
            neg1 = Z3_mk_not(ctx, TabEx[i][j]);
            TabNot[0] = neg1;
            for (int k = 0; k<orderG(graph); k++){
                if(k != j){
                    neg2 = Z3_mk_not(ctx, TabEx[i][k]);
                    TabNot[1]=neg2;
                    maxAST[i][j][pos] = Z3_mk_or(ctx, 2, TabNot);
                    pos ++;
                }
            }
            maxAST2[i][j] = Z3_mk_and(ctx, orderG(graph)-1, maxAST[i][j]);
        }
        maxAST3[i]=Z3_mk_and(ctx, orderG(graph), maxAST2[i]);
    }

    Z3_ast result = Z3_mk_and(ctx, pathLength+1, maxAST3);
    //printf("\n==========MaxOne============\n%s",Z3_ast_to_string(ctx,result));
    return result;
}



/**
 * @brief Generates a SAT formula satisfiable if and only the path is valid (IE : an edge exist from 1st node to 2nd node)
 * @param ctx The solver context
 * @param graph A graph
 * @param pathLength The length of the path to check
 * @return Z3_ast The formula
 * FONCTIONNE
 */
Z3_ast ExistPath(Z3_context ctx, int number, Graph graph, int pathLength){
    //printf("ExistPath\n");
    /*if(pathLength<=1){
        return Z3_mk_true(ctx);
    }*/
    Z3_ast TabEx[pathLength+1][orderG(graph)];
    for(int i = 0; i<pathLength+1 ; i++){
        for (int j=0 ; j<orderG(graph) ; j++){
            TabEx[i][j] = getNodeVariable(ctx, number, i, pathLength, j);
        }
    }
    Z3_ast ExistPathAST[pathLength+1][orderG(graph)][orderG(graph)];
    Z3_ast ExistPathAST2[pathLength+1][orderG(graph)];
    Z3_ast ExistPathAST3[pathLength+1];
    Z3_ast TabAnd[2];
    int pos, pos2;

    for(int i = 0; i<pathLength; i++){
        pos2 = 0;
        for (int j = 0; j<orderG(graph) ; j++){
            pos = 0;
            TabAnd[0]= Z3_mk_not(ctx,TabEx[i][j]);
            for (int k = 0; k<orderG(graph) ; k++ ){
                if(!isEdge(graph, j, k)){
                    TabAnd[1]=Z3_mk_not(ctx,TabEx[i+1][k]);
                    ExistPathAST[i][j][pos]=Z3_mk_or(ctx, 2, TabAnd);
                    pos++;
                }
            }
            if (pos>=1){
                ExistPathAST2[i][pos2]=Z3_mk_and(ctx, pos, ExistPathAST[i][j]);
                pos2++;
            }
            /*if (pos==1){
                ExistPathAST2[i][pos2]=ExistPathAST[i][j][0];
                pos2++;
            }*/
        }
        ExistPathAST3[i] = Z3_mk_and(ctx, pos2, ExistPathAST2[i]);
    }


    Z3_ast result = Z3_mk_and(ctx, pathLength, ExistPathAST3);
    //printf("\n==========ExistPath============\n%s",Z3_ast_to_string(ctx,result));
    return result;

}




/**
 * @brief Generates a SAT formula satisfiable if and only the path is simple (IE : a node can only appear once)
 * @param ctx The solver context
 * @param graph A graph
 * @param pathLength The length of the path to check
 * @return Z3_ast The formula
 * FONCTIONNE !
 */
Z3_ast SimplePath(Z3_context ctx, int number, Graph graph, int pathLength){

 
    Z3_ast TabEx[orderG(graph)][pathLength+1];
    for(int i = 0; i<orderG(graph) ; i++){
        for (int j=0 ; j<=pathLength ; j++){
            TabEx[i][j] = getNodeVariable(ctx, number, j, pathLength, i);
        }
    }

    Z3_ast SimplePathAST[orderG(graph)][pathLength+1][pathLength+1];
    Z3_ast SimplePathAST2[orderG(graph)][pathLength+1];
    Z3_ast SimplePathAST3[orderG(graph)];
    Z3_ast neg1, neg2;
    Z3_ast negTab[2];
    int pos = 0;


    for(int i = 0; i<orderG(graph); i++){
        for (int j = 0 ; j<=pathLength ; j++){
            neg1 = Z3_mk_not(ctx,TabEx[i][j]);
            negTab[0] = neg1;
            pos = 0;
            for (int k = 0 ; k<=pathLength ; k++){
                if (j!=k){
                    neg2 =  Z3_mk_not(ctx,TabEx[i][k]);
                    negTab[1] = neg2;
                    SimplePathAST[i][j][pos]=Z3_mk_or(ctx, 2, negTab);
                    pos++;
                }
            }
            if(pos !=0)
                SimplePathAST2[i][j]=Z3_mk_and(ctx, pos, SimplePathAST[i][j]);
        }
        SimplePathAST3[i]=Z3_mk_and(ctx, pathLength, SimplePathAST2[i]);
    }
 

    
    Z3_ast result = Z3_mk_and(ctx, orderG(graph), SimplePathAST3);
    //printf("\n==========SimplePath============\n%s",Z3_ast_to_string(ctx,result));
    return result;

}

/**
 * @brief Generates a SAT formula satisfiable if and only the path starts with s and end with t
 * @param ctx The solver context
 * @param graph A graph
 * @param pathLength The length of the path to check
 * @return Z3_ast The formula
 * FONCTIONNE !
 */
Z3_ast sFirsttLast(Z3_context ctx, int number, Graph graph, int pathLength){
    //printf("FirstLast\n");
    int s, t;
    for (int i = 0; i<orderG(graph) ; i++){
        if (isSource(graph, i))
            s=i;
        if (isTarget(graph, i))
            t=i;
    }
    Z3_ast andTab[2];
    andTab[0] = getNodeVariable(ctx, number, 0, pathLength, s);
    andTab[1] = getNodeVariable(ctx, number, pathLength, pathLength, t);

    Z3_ast result = Z3_mk_and(ctx, 2, andTab);
    //printf("\n==========FirstLast============\n%s",Z3_ast_to_string(ctx,result));
    return result;
}


/******
 * 
 ******/
Z3_ast graphsToPathFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs, int pathLength){
    Z3_ast allGraphAllFormulaTab[numGraphs][5];
    //printf("on entre dans la boucle\n");
    for (int i = 0; i<numGraphs ; i++){
        //printf("Graph nb : %d \n", i);
        allGraphAllFormulaTab[i][0] = existOneVertex(ctx, i, graphs[i], pathLength);
        allGraphAllFormulaTab[i][1] = maxOneVertex(ctx, i, graphs[i], pathLength);
        allGraphAllFormulaTab[i][2] = ExistPath(ctx, i, graphs[i], pathLength); 
        allGraphAllFormulaTab[i][3] = SimplePath(ctx, i, graphs[i], pathLength); 
        allGraphAllFormulaTab[i][4] = sFirsttLast(ctx, i, graphs[i], pathLength);
    }

    Z3_ast allGraphTab[numGraphs];
    for (int i = 0; i<numGraphs ; i++){
        allGraphTab[i] = Z3_mk_and(ctx, 5, allGraphAllFormulaTab[i]);

    }

    Z3_ast result = Z3_mk_and(ctx, numGraphs, allGraphTab);
    return result;
}


/******
 * 
 ******/

Z3_ast graphsToFullFormula( Z3_context ctx, Graph *graphs,unsigned int numGraphs){
    int maxLength = orderG(graphs[0]);
    for (int i = 0; i<numGraphs ; i++){
        if (maxLength>orderG(graphs[i]));
            maxLength=orderG(graphs[i]);
    }

    Z3_ast FormulaOfI[maxLength];
    for (int i=1 ; i<maxLength ; i++){
        FormulaOfI[i-1]=graphsToPathFormula(ctx, graphs, numGraphs, i);
    }
    
    Z3_ast FullFormula = Z3_mk_or(ctx, maxLength-1, FormulaOfI);
    return FullFormula;
}

void printPathsFromModel(Z3_context ctx, Z3_model model, Graph *graphs, int numGraph, int pathLength){
    for (int i = 0 ; i<numGraph ; i++){
        printf("Graph number %d path :  ", i+1);
        for (int j=0; j<=pathLength; j++){
            for (int k = 0; k<orderG(graphs[i]); k++){
                if (valueOfVarInModel(ctx, model, getNodeVariable(ctx, i, j, pathLength, k))){
                    if(isTarget(graphs[i], k))
                        printf("%s\n", getNodeName(graphs[i], k));
                    else
                    printf("%s -> ", getNodeName(graphs[i], k));
                }
            }
        }
    }
}