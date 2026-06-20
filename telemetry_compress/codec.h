#ifndef CODEC_H
#define CODEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "huffman.h"

static unsigned char *huffCompress(const unsigned char *input, int inputLen,
                                   const HuffCodeTable *codes, int *outBytes, int *outBits) {
    long totalBits = 0;
    for (int i = 0; i < inputLen; i++)
        totalBits += codes->table[input[i]].length;

    *outBits  = (int)totalBits;
    *outBytes = (int)((totalBits + 7) / 8);

    unsigned char *buf = calloc(*outBytes, 1);
    int bitPos = 0;

    for (int i = 0; i < inputLen; i++) {
        const HuffCode *code = &codes->table[input[i]];
        for (int b = 0; b < code->length; b++) {
            if (code->bits[b])
                buf[bitPos / 8] |= (unsigned char)(1 << (7 - (bitPos % 8)));
            bitPos++;
        }
    }

    return buf;
}

static unsigned char *huffDecompress(const HuffTree *t, const unsigned char *bitstream,
                                     int totalBits, int *outputLen) {
    unsigned char *out = malloc(totalBits + 1);
    int outIdx = 0;
    int cur    = t->root;

    for (int i = 0; i < totalBits; i++) {
        int bit = (bitstream[i / 8] >> (7 - (i % 8))) & 1;
        const HuffNode *n = &t->pool[cur];

        cur = bit ? n->right : n->left;
        if (cur == -1) { cur = t->root; continue; }

        if (t->pool[cur].symbol >= 0) {
            out[outIdx++] = (unsigned char)t->pool[cur].symbol;
            cur = t->root;
        }
    }

    *outputLen = outIdx;
    return out;
}

static int huffSave(const char *path, const int freq[ALPHA_SIZE],
                    const unsigned char *bitstream, int totalBits) {
    FILE *f = fopen(path, "wb");
    if (!f) return -1;

    int distinct = 0;
    for (int i = 0; i < ALPHA_SIZE; i++)
        if (freq[i] > 0) distinct++;
    fwrite(&distinct, sizeof(int), 1, f);
    for (int i = 0; i < ALPHA_SIZE; i++) {
        if (freq[i] > 0) {
            unsigned char sym = (unsigned char)i;
            fwrite(&sym,     1,           1, f);
            fwrite(&freq[i], sizeof(int), 1, f);
        }
    }

    fwrite(&totalBits, sizeof(int), 1, f);
    fwrite(bitstream, 1, (totalBits + 7) / 8, f);
    fclose(f);
    return 0;
}

static int huffLoad(const char *path, int freq[ALPHA_SIZE],
                    unsigned char **bitstream, int *totalBits) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    memset(freq, 0, ALPHA_SIZE * sizeof(int));
    int distinct = 0;
    fread(&distinct, sizeof(int), 1, f);
    for (int i = 0; i < distinct; i++) {
        unsigned char sym;
        int count;
        fread(&sym,   1,           1, f);
        fread(&count, sizeof(int), 1, f);
        freq[sym] = count;
    }

    fread(totalBits, sizeof(int), 1, f);
    int byteCount = (*totalBits + 7) / 8;
    *bitstream = malloc(byteCount);
    fread(*bitstream, 1, byteCount, f);
    fclose(f);
    return 0;
}

static void reportStats(const char *origPath, const char *huffPath) {
    struct stat so, sh;
    if (stat(origPath, &so) != 0 || stat(huffPath, &sh) != 0) return;
    double ratio = 100.0 * (1.0 - (double)sh.st_size / so.st_size);
    printf("Original:   %ld bytes\n", (long)so.st_size);
    printf("Compressed: %ld bytes\n", (long)sh.st_size);
    printf("Reduction:  %.1f%%\n", ratio);
}

static void verifyRestore(const char *origPath, const char *restoredPath) {
    FILE *fo = fopen(origPath,     "rb");
    FILE *fr = fopen(restoredPath, "rb");
    if (!fo || !fr) { printf("Verify: could not open files.\n"); return; }

    int mismatch = -1, pos = 0, co, cr;
    while ((co = fgetc(fo)) != EOF && (cr = fgetc(fr)) != EOF) {
        if (co != cr) { mismatch = pos; break; }
        pos++;
    }
    if (mismatch == -1 && fgetc(fo) == EOF && fgetc(fr) == EOF)
        printf("Verify: MATCH — restored file is identical to original.\n");
    else if (mismatch >= 0)
        printf("Verify: MISMATCH at byte %d.\n", mismatch);
    else
        printf("Verify: MISMATCH — files differ in length.\n");

    fclose(fo);
    fclose(fr);
}

#endif
