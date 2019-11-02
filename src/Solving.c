#include "Solving.h"
#include "Z3Tools.h"
#include <stdlib.h>
#include <stdio.h>

Z3_ast getNodeVariable(Z3_context ctx, 
                        int number, 
                        int position, 
                        int k, 
                        int node) 
{
    char buff[256];

    int n = snprintf(buff, 256, "%d%d%d%d", number, position, k, node);
    if (n >= 256) 
    {
        fprintf(stderr, "Lenght of parameter in getNodeVariable too long\n");
        exit(EXIT_FAILURE);
    }

    return mk_bool_var(ctx, buff);
}
