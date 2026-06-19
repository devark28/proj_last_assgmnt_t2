#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <string.h>
#include <strings.h>

#define MAX_BUILDINGS 10
#define MAX_NAME_LEN  32
#define DIJKSTRA_INF  999999

typedef struct {
    int nodeCount;
    char names[MAX_BUILDINGS][MAX_NAME_LEN];
    int weight[MAX_BUILDINGS][MAX_BUILDINGS];
} CampusGraph;

typedef struct {
    int dist[MAX_BUILDINGS];
    int prev[MAX_BUILDINGS];
} DijkstraResult;

static void graphInit(CampusGraph *g) {
    g->nodeCount = 0;
    memset(g->weight, 0, sizeof(g->weight));
}

static int graphAddNode(CampusGraph *g, const char *name) {
    int idx = g->nodeCount++;
    strncpy(g->names[idx], name, MAX_NAME_LEN - 1);
    g->names[idx][MAX_NAME_LEN - 1] = '\0';
    return idx;
}

static int graphFindNode(const CampusGraph *g, const char *name) {
    for (int i = 0; i < g->nodeCount; i++) {
        if (strcasecmp(g->names[i], name) == 0)
            return i;
    }
    return -1;
}

static void graphAddEdge(CampusGraph *g, int u, int v, int w) {
    g->weight[u][v] = w;
    g->weight[v][u] = w;
}

static void dijkstra(const CampusGraph *g, int src, DijkstraResult *out) {
    int visited[MAX_BUILDINGS] = {0};

    for (int i = 0; i < g->nodeCount; i++) {
        out->dist[i] = DIJKSTRA_INF;
        out->prev[i] = -1;
    }
    out->dist[src] = 0;

    for (int iter = 0; iter < g->nodeCount; iter++) {
        int u = -1;
        for (int i = 0; i < g->nodeCount; i++) {
            if (!visited[i] && (u == -1 || out->dist[i] < out->dist[u]))
                u = i;
        }
        if (u == -1 || out->dist[u] == DIJKSTRA_INF)
            break;
        visited[u] = 1;

        for (int v = 0; v < g->nodeCount; v++) {
            if (g->weight[u][v] > 0 && !visited[v]) {
                int newDist = out->dist[u] + g->weight[u][v];
                if (newDist < out->dist[v]) {
                    out->dist[v] = newDist;
                    out->prev[v] = u;
                }
            }
        }
    }
}

static void printPath(const CampusGraph *g, const DijkstraResult *r, int dest) {
    int stack[MAX_BUILDINGS];
    int top = 0;
    int cur = dest;

    while (cur != -1) {
        stack[top++] = cur;
        cur = r->prev[cur];
    }

    for (int i = top - 1; i >= 0; i--) {
        printf("%s", g->names[stack[i]]);
        if (i > 0) printf(" -> ");
    }
    printf("\n");
}

#endif
