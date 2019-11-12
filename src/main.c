#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <Graph.h>
#include <Parsing.h>
#include "Z3Tools.h"
#include "Solving.h"

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

    while( (arg = getopt(argc, argv, "hvFsdatfo:")) != -1) {
        printf("arg : %c ==== optarg : %s\n", arg, optarg);
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
                printf("output name : %s\n", outputName);
                break;
            }
        }
    }

    for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);

    // check if we correctly get the good arg
    printf("option :\n \
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
            outputName
    );

    Graph g = getGraphFromFile("test");

    return 0;
}