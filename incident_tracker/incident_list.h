#ifndef INCIDENT_LIST_H
#define INCIDENT_LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_INCIDENTS  25
#define MAX_DESC_LEN  128
#define MAX_SOURCE_LEN 32

typedef struct Incident {
    int id;
    char source[MAX_SOURCE_LEN];
    char description[MAX_DESC_LEN];
    struct Incident *prev;
    struct Incident *next;
} Incident;

typedef struct {
    Incident *head;
    Incident *tail;
    Incident *cursor;
    int count;
    int nextId;
} IncidentList;

static void listInit(IncidentList *l) {
    memset(l, 0, sizeof(*l));
}

static Incident *allocIncident(int id, const char *source, const char *desc) {
    Incident *node = malloc(sizeof(Incident));
    node->id = id;
    strncpy(node->source, source, MAX_SOURCE_LEN - 1);
    node->source[MAX_SOURCE_LEN - 1] = '\0';
    strncpy(node->description, desc, MAX_DESC_LEN - 1);
    node->description[MAX_DESC_LEN - 1] = '\0';
    node->prev = node->next = NULL;
    return node;
}

static void listEvictOldest(IncidentList *l) {
    if (!l->head) return;
    Incident *old = l->head;
    if (l->cursor == old)
        l->cursor = old->next;
    l->head = old->next;
    if (l->head) l->head->prev = NULL;
    else         l->tail = NULL;
    free(old);
    l->count--;
}

static void listAdd(IncidentList *l, const char *source, const char *desc) {
    if (l->count == MAX_INCIDENTS)
        listEvictOldest(l);

    Incident *node = allocIncident(l->nextId++, source, desc);

    if (!l->tail) {
        l->head = l->tail = l->cursor = node;
    } else {
        node->prev = l->tail;
        l->tail->next = node;
        l->tail = node;
    }
    l->count++;
}

static int listMoveForward(IncidentList *l) {
    if (!l->cursor || !l->cursor->next) return 0;
    l->cursor = l->cursor->next;
    return 1;
}

static int listMoveBackward(IncidentList *l) {
    if (!l->cursor || !l->cursor->prev) return 0;
    l->cursor = l->cursor->prev;
    return 1;
}

static void listDeleteAll(IncidentList *l) {
    Incident *cur = l->head;
    while (cur) {
        Incident *next = cur->next;
        free(cur);
        cur = next;
    }
    memset(l, 0, sizeof(*l));
}

static void listSaveToFile(const IncidentList *l, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return;
    Incident *cur = l->head;
    while (cur) {
        fprintf(f, "%d|%s|%s\n", cur->id, cur->source, cur->description);
        cur = cur->next;
    }
    fclose(f);
}

static void printCurrentIncident(const IncidentList *l) {
    if (!l->cursor) {
        printf("  [No incidents]\n");
        return;
    }
    int pos = 1;
    Incident *cur = l->head;
    while (cur && cur != l->cursor) { pos++; cur = cur->next; }
    printf("  [%d/%d] #%d %-10s %s\n",
        pos, l->count, l->cursor->id, l->cursor->source, l->cursor->description);
}

static void generateSyntheticIncident(IncidentList *l) {
    static const char *sources[] = { "AMBULANCE", "POLICE", "FIRE" };
    const char *src = sources[l->nextId % 3];
    char desc[MAX_DESC_LEN];
    snprintf(desc, sizeof(desc), "AUTO-REPORT #%d", l->nextId);
    listAdd(l, src, desc);
}

#endif
