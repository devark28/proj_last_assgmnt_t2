#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "graph.h"

static void readCode(const char *prompt, char *buf) {
    printf("%s", prompt);
    if (!fgets(buf, MAX_CODE_LEN + 2, stdin)) buf[0] = '\0';
    buf[strcspn(buf, "\n")] = '\0';
    for (int i = 0; buf[i]; i++) buf[i] = (char)toupper((unsigned char)buf[i]);
}

int main(void) {
    RouteGraph g;
    graphInit(&g);

    graphAddAirport(&g, "KGL");
    graphAddAirport(&g, "NBO");
    graphAddAirport(&g, "EBB");
    graphAddAirport(&g, "JNB");
    graphAddAirport(&g, "ADD");
    graphAddAirport(&g, "CAI");
    graphAddAirport(&g, "CPT");

    graphAddRoute(&g, "KGL", "NBO");
    graphAddRoute(&g, "KGL", "EBB");
    graphAddRoute(&g, "NBO", "JNB");
    graphAddRoute(&g, "EBB", "ADD");
    graphAddRoute(&g, "ADD", "CAI");
    graphAddRoute(&g, "JNB", "CPT");

    printf("Airline Route Analyzer (7 airports loaded)\n\n");

    int running = 1;
    while (running) {
        printf("1. Query airport\n");
        printf("2. Add airport\n");
        printf("3. Remove airport\n");
        printf("4. Add route\n");
        printf("5. Remove route\n");
        printf("6. Print adjacency matrix\n");
        printf("7. Quit\n");
        printf("Choice: ");

        char line[8];
        if (!fgets(line, sizeof(line), stdin)) break;

        char from[MAX_CODE_LEN + 2], to[MAX_CODE_LEN + 2];

        switch (line[0]) {
            case '1':
                readCode("Airport code: ", from);
                graphQueryOutgoing(&g, from);
                graphQueryIncoming(&g, from);
                break;
            case '2':
                readCode("New airport code: ", from);
                if (graphAddAirport(&g, from) == -1)
                    printf("Already exists or capacity reached.\n");
                else
                    printf("Added %s.\n", from);
                break;
            case '3':
                readCode("Airport code to remove: ", from);
                graphRemoveAirport(&g, from);
                printf("Removed %s (if it existed).\n", from);
                break;
            case '4':
                readCode("From: ", from);
                readCode("To:   ", to);
                if (!graphAddRoute(&g, from, to))
                    printf("One or both airports not found.\n");
                else
                    printf("Route %s -> %s added.\n", from, to);
                break;
            case '5':
                readCode("From: ", from);
                readCode("To:   ", to);
                graphRemoveRoute(&g, from, to);
                printf("Route %s -> %s removed (if it existed).\n", from, to);
                break;
            case '6':
                graphPrintMatrix(&g);
                break;
            case '7':
                running = 0;
                break;
            default:
                printf("Invalid choice.\n");
        }
        printf("\n");
    }

    return 0;
}
