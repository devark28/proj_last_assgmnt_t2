#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "bst.h"

int main(void) {
    BSTNode *root = NULL;
    int count = 0;

    if (bstLoadFromFile("procedures.txt", &root, &count) != 0) {
        fprintf(stderr, "Error: could not open procedures.txt\n");
        return 1;
    }
    printf("Loaded %d procedures.\n\n", count);

    char input[MAX_PROC_LEN];
    while (1) {
        printf("Enter procedure (or 'quit'): ");
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = '\0';

        for (int i = 0; input[i]; i++)
            input[i] = (char)toupper((unsigned char)input[i]);

        if (strcmp(input, "QUIT") == 0) break;

        validateProcedure(root, input);
    }

    bstFree(root);
    return 0;
}
