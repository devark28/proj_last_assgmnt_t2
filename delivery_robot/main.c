#include <stdio.h>
#include <string.h>
#include "graph.h"

#define DORMITORY "Dormitory"

int main(void) {
    CampusGraph g;
    graphInit(&g);

    int library      = graphAddNode(&g, "Library");
    int cafeteria    = graphAddNode(&g, "Cafeteria");
    int engineering  = graphAddNode(&g, "Engineering");
    int scienceBlock = graphAddNode(&g, "Science Block");
    int dormitory    = graphAddNode(&g, "Dormitory");
    int admin        = graphAddNode(&g, "Administration");
    int charging     = graphAddNode(&g, "Charging Station");

    graphAddEdge(&g, library,      cafeteria,    6);
    graphAddEdge(&g, library,      engineering, 15);
    graphAddEdge(&g, cafeteria,    scienceBlock,  4);
    graphAddEdge(&g, scienceBlock, dormitory,     8);
    graphAddEdge(&g, engineering,  admin,         5);
    graphAddEdge(&g, admin,        dormitory,     3);
    graphAddEdge(&g, cafeteria,    charging,      2);
    graphAddEdge(&g, charging,     admin,         4);

    printf("Campus buildings:\n");
    for (int i = 0; i < g.nodeCount; i++)
        printf("  %d. %s\n", i + 1, g.names[i]);

    printf("\nEnter starting building: ");
    char input[MAX_NAME_LEN];
    if (!fgets(input, sizeof(input), stdin)) return 1;
    input[strcspn(input, "\n")] = '\0';

    int start = graphFindNode(&g, input);
    if (start == -1) {
        printf("Unknown building: \"%s\"\n", input);
        printf("Valid buildings:\n");
        for (int i = 0; i < g.nodeCount; i++)
            printf("  %s\n", g.names[i]);
        return 1;
    }

    int dest = graphFindNode(&g, DORMITORY);

    if (start == dest) {
        printf("Already at Dormitory.\n");
        return 0;
    }

    DijkstraResult result;
    dijkstra(&g, start, &result);

    if (result.dist[dest] == DIJKSTRA_INF) {
        printf("No path found to Dormitory.\n");
        return 1;
    }

    printf("Shortest route from %s to %s:\n", g.names[start], DORMITORY);
    printf("  Path:     ");
    printPath(&g, &result, dest);
    printf("  Distance: %d units\n", result.dist[dest]);

    return 0;
}
