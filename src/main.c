/**
 * @file main.c
 * @author Thimotée BROCARD
 * @author Xavier GERARDIN
 * @brief An implementation of the Coca project
 * @version 1
 * @date 2019-11-15
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <Graph.h>
#include <Parsing.h>
#include "Z3Tools.h"
#include "Solving.h"

bool increasing = true;
bool computeResult = false;
bool testSeparately = false;

void usage(const char* progName) {
    printf("Use: %s [options] files... \n \
Each file should contain a graph in dot format. \n \
The program decides if there exists a length n such that each input graph has a valid simple path of length n. \n \
Can display the result both on the command line or in dot format. \n \
Options: \n \
-h         Displays this help \n \
-v         Activate verbose mode (displays parsed graphs) \n \
-F         Displays the formula computed (obviously not in this version, but you should really display it in your code) \n \
-s         Tests separately all formula by depth [if not present: uses the global formula] \n \
-d         Only if -s is present. Explore the length in decreasing order. [if not present: in increasing order] \n \
-a         Only if -s is present. Computes a result for every length instead of stopping at the first positive result (default behaviour) \n \
-t         Displays the paths found on the terminal [if not present, only displays the existence of the path]. \n \
-f         Writes the result with colors in a .dot file. See next option for the name. These files will be produced in the folder 'sol'. \n \
-o NAME    Writes the output in \"NAME-lLENGTH.dot\" where LENGTH is the length of the solution. Writes several files in this format if both -s and -a are present. [if not present: \"result-lLENGTH.dot\"]\n",
            progName );
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
    if (argc == 1) usage(argv[0]);
    int arg;

    bool verbose          = false; //fait
    bool displayFormula   = false; //fait
    bool displayPath      = false; //fait
    bool writeColoredDot  = false;
    bool exploreLengthMode = increasing;
    char outputName[64];
    char filename[128];

    while( (arg = getopt(argc, argv, "hvFsdatfo:")) != -1) {
        printf("arg : %c ==== optarg : %s ==== optind : %d\n", arg, optarg, optind);
        switch(arg) {
            case 'h':
            {
                usage(argv[0]);
                break;
            }
            case 'v':
            {
                verbose = true;
                break;
            }
            case 'F':
            {
                displayFormula = true;
                break;
            }
            case 's':
            {
                printf("Utilisation arg -s\n");
                testSeparately = true;

                break;
            }
            case 'd':
            {
                if (testSeparately)
                    increasing = false;
                break;
            }
            case 'a':
            {
                if (testSeparately)
                    computeResult = true;
                break;
            }
            case 't':
            {
                displayPath = true;
                break;
            }
            case 'f':
            {
                writeColoredDot = true;
                break;
            }
            case 'o':
            {
                snprintf(outputName, 64, "%s-l%d.dot", optarg, 0);
                break;
            }
        }
    }

    char* filesname[argc-optind];
    for (int i = optind; i < argc; i++) 
        filesname[i - optind] = argv[i];
    
    for(int i = 0; i < argc - optind; i++)
        printf("file %d to open %s\n", i, filesname[i]);

   /* printf("option :\n \
            verbose : %d\n \
            displayFormula : %d\n \
            testSeparately : %d\n \
            computeResult  : %d\n \
            displayPath    : %d\n \
            writeColoredDot: %d\n \
            exploreLengthMode : %d\n \
            outputName     : %s\n",
            verbose,
            displayFormula,
            testSeparately,
            computeResult,
            displayPath,
            writeColoredDot,
            exploreLengthMode,
            outputName); //Display some information to debug*/

    int nbGraph= argc-optind;
    Graph g[nbGraph] ;
    for(int i = 0; i < nbGraph; i++)
        g[i]=getGraphFromFile(filesname[i]);

    Z3_context ctx = makeContext();
    Z3_ast result = graphsToFullFormula(ctx, g, nbGraph);
    int order = orderG(g[0]);
    int size = sizeG(g[0]);
    int taille;
    
    Z3_lbool isSat = isFormulaSat(ctx,result);

    if (verbose){
        for (int i = 0; i<nbGraph ; i++){
            printf("Graph number %d : \n", i+1);
            printGraph(g[i]);
        }
    }
    if (displayFormula)
        printf("=============\nThe full formula is : \n%s\n",Z3_ast_to_string(ctx,result));

    switch (isSat){
        case Z3_L_FALSE:
            printf("NON\n");
            break;

        case Z3_L_UNDEF:
                printf("JSP dsl\n",Z3_ast_to_string(ctx,result));
            break;

        case Z3_L_TRUE:
            Z3_model model = getModelFromSatFormula(ctx,result);
            if(displayPath){
                taille = getSolutionLengthFromModel(ctx, model, g);
            printPathsFromModel(ctx, model, g, nbGraph, taille);
            }
            printf("OUI\n");
            break;
    }

    /*freeing memory*/
    Z3_del_context(ctx);
     for(int i = 0; i < nbGraph; i++)
        deleteGraph(g[i]);
    return 0;
}