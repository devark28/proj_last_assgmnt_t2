#ifndef BST_H
#define BST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_PROCEDURES 50
#define MAX_PROC_LEN   64
#define AUDIT_FILE     "audit.log"

typedef struct BSTNode {
    char procedure[MAX_PROC_LEN];
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

static BSTNode *bstCreateNode(const char *proc) {
    BSTNode *node = malloc(sizeof(BSTNode));
    strncpy(node->procedure, proc, MAX_PROC_LEN - 1);
    node->procedure[MAX_PROC_LEN - 1] = '\0';
    node->left = node->right = NULL;
    return node;
}

static BSTNode *bstInsert(BSTNode *root, const char *proc) {
    if (!root) return bstCreateNode(proc);
    int cmp = strcmp(proc, root->procedure);
    if (cmp < 0) root->left  = bstInsert(root->left,  proc);
    else if (cmp > 0) root->right = bstInsert(root->right, proc);
    return root;
}

static int bstSearch(BSTNode *root, const char *proc) {
    while (root) {
        int cmp = strcmp(proc, root->procedure);
        if (cmp == 0) return 1;
        root = cmp < 0 ? root->left : root->right;
    }
    return 0;
}

static void bstInorder(BSTNode *root, char out[][MAX_PROC_LEN], int *count) {
    if (!root) return;
    bstInorder(root->left, out, count);
    strncpy(out[(*count)++], root->procedure, MAX_PROC_LEN - 1);
    bstInorder(root->right, out, count);
}

static int editDistance(const char *a, const char *b) {
    int la = (int)strlen(a);
    int lb = (int)strlen(b);
    int dp[MAX_PROC_LEN + 1][MAX_PROC_LEN + 1];

    for (int i = 0; i <= la; i++) dp[i][0] = i;
    for (int j = 0; j <= lb; j++) dp[0][j] = j;

    for (int i = 1; i <= la; i++) {
        for (int j = 1; j <= lb; j++) {
            if (a[i-1] == b[j-1])
                dp[i][j] = dp[i-1][j-1];
            else {
                int sub = dp[i-1][j-1] + 1;
                int del = dp[i-1][j]   + 1;
                int ins = dp[i][j-1]   + 1;
                dp[i][j] = sub < del ? (sub < ins ? sub : ins) : (del < ins ? del : ins);
            }
        }
    }
    return dp[la][lb];
}

static const char *bstFindSimilar(BSTNode *root, const char *query) {
    char sorted[MAX_PROCEDURES][MAX_PROC_LEN];
    int count = 0;
    bstInorder(root, sorted, &count);

    int bestDist = MAX_PROC_LEN;
    int bestIdx  = -1;
    for (int i = 0; i < count; i++) {
        int d = editDistance(query, sorted[i]);
        if (d < bestDist) {
            bestDist = d;
            bestIdx  = i;
        }
    }

    if (bestIdx == -1 || bestDist * 3 > (int)strlen(query))
        return NULL;

    static char result[MAX_PROC_LEN];
    strncpy(result, sorted[bestIdx], MAX_PROC_LEN - 1);
    return result;
}

static const char *bstFindNeighbor(BSTNode *root, const char *query) {
    BSTNode *floor   = NULL;
    BSTNode *ceiling = NULL;
    BSTNode *cur     = root;

    while (cur) {
        int cmp = strcmp(query, cur->procedure);
        if (cmp < 0) {
            ceiling = cur;
            cur = cur->left;
        } else if (cmp > 0) {
            floor = cur;
            cur = cur->right;
        } else {
            return cur->procedure;
        }
    }

    if (floor && ceiling) {
        int df = editDistance(query, floor->procedure);
        int dc = editDistance(query, ceiling->procedure);
        return df <= dc ? floor->procedure : ceiling->procedure;
    }
    if (floor)   return floor->procedure;
    if (ceiling) return ceiling->procedure;
    return NULL;
}

static int bstLoadFromFile(const char *path, BSTNode **root, int *count) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;

    char line[MAX_PROC_LEN];
    *count = 0;

    while (*count < MAX_PROCEDURES && fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0') continue;
        for (int i = 0; line[i]; i++) line[i] = (char)toupper((unsigned char)line[i]);
        *root = bstInsert(*root, line);
        (*count)++;
    }

    fclose(f);
    return 0;
}

static void logUnknown(const char *query) {
    FILE *f = fopen(AUDIT_FILE, "a");
    if (!f) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] REJECTED: %s\n",
        t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, query);
    fclose(f);
}

static void validateProcedure(BSTNode *root, const char *query) {
    if (bstSearch(root, query)) {
        printf("  APPROVED: %s\n", query);
        return;
    }

    const char *suggestion = bstFindSimilar(root, query);
    if (suggestion) {
        printf("  REJECTED — did you mean: %s?\n", suggestion);
        return;
    }

    logUnknown(query);
    printf("  REJECTED — unknown procedure, logged to audit\n");
}

static void bstFree(BSTNode *root) {
    if (!root) return;
    bstFree(root->left);
    bstFree(root->right);
    free(root);
}

#endif
