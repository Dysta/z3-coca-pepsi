#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <Graph.h>
#include <Parsing.h>
#include "Z3Tools.h"
#include "Solving.h"

enum mode {INCREASING, DECREASING};

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

    bool verbose          = false;
    bool displayFormula   = false;
    bool testSeparately   = false;
    bool computeResult    = false;
    bool displayPath      = false;
    bool writeColoredDot  = false;
    int exploreLengthMode = INCREASING;
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
                // on test séparément les différentes formule, jsp ce que c'est mais c la 
                printf("Utilisation arg -s\n");
                testSeparately = true;
                break;
            }
            case 'd':
            {
                if (testSeparately)
                    exploreLengthMode = DECREASING;
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

    if (optind == argc - 1) // get the filename to open
        snprintf(filename, 128, "%s", argv[argc-1]);

    // check if we correctly get the good arg
    printf("option :\n \
            verbose : %d\n \
            displayFormula : %d\n \
            testSeparately : %d\n \
            computeResult  : %d\n \
            displayPath    : %d\n \
            writeColoredDot: %d\n \
            exploreLengthMode : %d\n \
            outputName     : %s\n \
            filename       : %s\n",
            verbose,
            displayFormula,
            testSeparately,
            computeResult,
            displayPath,
            writeColoredDot,
            exploreLengthMode,
            outputName,
            filename
    );

    Graph g[1] ;
    g[0]=getGraphFromFile(filename);
    Z3_context ctx = makeContext();
    Z3_ast result = graphsToFullFormula(ctx, g, 1);
    //Z3_ast result = maxOneVertex(ctx, 1, g[0], 2);
    int order = orderG(g[0]);
    int size = sizeG(g[0]);
    printf("order  : %d\n size : %d\n", order, size);
    Z3_lbool isSat = isFormulaSat(ctx,result);

            switch (isSat)
        {
        case Z3_L_FALSE:
            printf("is not satisfiable.\n");//,Z3_ast_to_string(ctx,result));
            break;

        case Z3_L_UNDEF:
                printf("We don't know if %s is satisfiable.\n",Z3_ast_to_string(ctx,result));
            break;

        case Z3_L_TRUE:
                printf("is satisfiable.\n");//,Z3_ast_to_string(ctx,result));
                Z3_model model = getModelFromSatFormula(ctx,result);
            break;
        }

    return 0;
}