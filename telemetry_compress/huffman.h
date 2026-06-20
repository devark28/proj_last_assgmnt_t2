#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHA_SIZE   256
#define MAX_NODES    511
#define MAX_CODE_LEN 256

typedef struct {
    int freq;
    int symbol;
    int left;
    int right;
} HuffNode;

typedef struct {
    HuffNode pool[MAX_NODES];
    int poolSize;
    int root;
} HuffTree;

typedef struct {
    unsigned char bits[MAX_CODE_LEN];
    int length;
} HuffCode;

typedef struct {
    HuffCode table[ALPHA_SIZE];
} HuffCodeTable;

static int huffNewNode(HuffTree *t, int freq, int symbol, int left, int right) {
    int idx = t->poolSize++;
    t->pool[idx].freq   = freq;
    t->pool[idx].symbol = symbol;
    t->pool[idx].left   = left;
    t->pool[idx].right  = right;
    return idx;
}

static void buildFreqTable(const unsigned char *data, int len, int freq[ALPHA_SIZE]) {
    memset(freq, 0, ALPHA_SIZE * sizeof(int));
    for (int i = 0; i < len; i++)
        freq[data[i]]++;
}

static void huffBuild(HuffTree *t, const int freq[ALPHA_SIZE]) {
    t->poolSize = 0;
    t->root     = -1;

    int active[MAX_NODES];
    int activeCount = 0;

    for (int i = 0; i < ALPHA_SIZE; i++) {
        if (freq[i] > 0)
            active[activeCount++] = huffNewNode(t, freq[i], i, -1, -1);
    }

    if (activeCount == 0) return;

    if (activeCount == 1) {
        t->root = huffNewNode(t, t->pool[active[0]].freq, -1, active[0], -1);
        return;
    }

    while (activeCount > 1) {
        int min1 = 0, min2 = 1;
        if (t->pool[active[min1]].freq > t->pool[active[min2]].freq) {
            int tmp = min1; min1 = min2; min2 = tmp;
        }
        for (int i = 2; i < activeCount; i++) {
            if (t->pool[active[i]].freq < t->pool[active[min1]].freq) {
                min2 = min1;
                min1 = i;
            } else if (t->pool[active[i]].freq < t->pool[active[min2]].freq) {
                min2 = i;
            }
        }

        int merged = huffNewNode(t,
            t->pool[active[min1]].freq + t->pool[active[min2]].freq,
            -1, active[min1], active[min2]);

        int keep = (min1 < min2) ? min1 : min2;
        int drop = (min1 < min2) ? min2 : min1;

        active[keep] = merged;
        active[drop] = active[--activeCount];
    }

    t->root = active[0];
}

static void huffGenerateCodes(const HuffTree *t, int node,
                              unsigned char *path, int depth,
                              HuffCodeTable *out) {
    if (node == -1) return;
    const HuffNode *n = &t->pool[node];

    if (n->symbol >= 0) {
        if (depth == 0) {
            out->table[n->symbol].bits[0] = 0;
            out->table[n->symbol].length  = 1;
        } else {
            memcpy(out->table[n->symbol].bits, path, depth);
            out->table[n->symbol].length = depth;
        }
        return;
    }

    path[depth] = 0;
    huffGenerateCodes(t, n->left,  path, depth + 1, out);
    path[depth] = 1;
    huffGenerateCodes(t, n->right, path, depth + 1, out);
}

#endif
