#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <string.h>

#define MAX_AIRPORTS 20
#define MAX_CODE_LEN 4

typedef struct {
    char code[MAX_CODE_LEN];
    int active;
} Vertex;

typedef struct {
    Vertex vertices[MAX_AIRPORTS];
    int matrix[MAX_AIRPORTS][MAX_AIRPORTS];
    int count;
} RouteGraph;

static void graphInit(RouteGraph *g) {
    memset(g, 0, sizeof(*g));
}

static int graphFindVertex(const RouteGraph *g, const char *code) {
    for (int i = 0; i < g->count; i++) {
        if (g->vertices[i].active && strcmp(g->vertices[i].code, code) == 0)
            return i;
    }
    return -1;
}

static int graphAddAirport(RouteGraph *g, const char *code) {
    if (graphFindVertex(g, code) != -1) return -1;

    for (int i = 0; i < g->count; i++) {
        if (!g->vertices[i].active) {
            strncpy(g->vertices[i].code, code, MAX_CODE_LEN - 1);
            g->vertices[i].code[MAX_CODE_LEN - 1] = '\0';
            g->vertices[i].active = 1;
            return i;
        }
    }

    if (g->count >= MAX_AIRPORTS) return -1;

    int idx = g->count++;
    strncpy(g->vertices[idx].code, code, MAX_CODE_LEN - 1);
    g->vertices[idx].code[MAX_CODE_LEN - 1] = '\0';
    g->vertices[idx].active = 1;
    return idx;
}

static void graphRemoveAirport(RouteGraph *g, const char *code) {
    int idx = graphFindVertex(g, code);
    if (idx == -1) return;

    g->vertices[idx].active = 0;
    for (int j = 0; j < g->count; j++) g->matrix[idx][j] = 0;
    for (int i = 0; i < g->count; i++) g->matrix[i][idx] = 0;
}

static int graphAddRoute(RouteGraph *g, const char *from, const char *to) {
    int f = graphFindVertex(g, from);
    int t = graphFindVertex(g, to);
    if (f == -1 || t == -1) return 0;
    g->matrix[f][t] = 1;
    return 1;
}

static void graphRemoveRoute(RouteGraph *g, const char *from, const char *to) {
    int f = graphFindVertex(g, from);
    int t = graphFindVertex(g, to);
    if (f == -1 || t == -1) return;
    g->matrix[f][t] = 0;
}

static void graphQueryOutgoing(const RouteGraph *g, const char *code) {
    int idx = graphFindVertex(g, code);
    if (idx == -1) { printf("Airport not found.\n"); return; }

    printf("Outgoing from %s: ", code);
    int found = 0;
    for (int j = 0; j < g->count; j++) {
        if (g->vertices[j].active && g->matrix[idx][j]) {
            printf("%s ", g->vertices[j].code);
            found = 1;
        }
    }
    if (!found) printf("(none)");
    printf("\n");
}

static void graphQueryIncoming(const RouteGraph *g, const char *code) {
    int idx = graphFindVertex(g, code);
    if (idx == -1) { printf("Airport not found.\n"); return; }

    printf("Incoming to   %s: ", code);
    int found = 0;
    for (int i = 0; i < g->count; i++) {
        if (g->vertices[i].active && g->matrix[i][idx]) {
            printf("%s ", g->vertices[i].code);
            found = 1;
        }
    }
    if (!found) printf("(none)");
    printf("\n");
}

static void graphPrintMatrix(const RouteGraph *g) {
    printf("\nAdjacency Matrix:\n     ");
    for (int j = 0; j < g->count; j++) {
        if (g->vertices[j].active)
            printf("%-4s", g->vertices[j].code);
    }
    printf("\n");

    for (int i = 0; i < g->count; i++) {
        if (!g->vertices[i].active) continue;
        printf("%-4s ", g->vertices[i].code);
        for (int j = 0; j < g->count; j++) {
            if (!g->vertices[j].active) continue;
            printf("%-4d", g->matrix[i][j]);
        }
        printf("\n");
    }
}

#endif
