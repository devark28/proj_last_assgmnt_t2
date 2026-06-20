#include <stdio.h>
#include <stdlib.h>
#include "codec.h"

#define INPUT_FILE    "telemetry.txt"
#define HUFF_FILE     "telemetry.huff"
#define RESTORED_FILE "telemetry_restored.txt"

int main(void) {
    FILE *fin = fopen(INPUT_FILE, "rb");
    if (!fin) {
        fprintf(stderr, "Error: could not open %s\n", INPUT_FILE);
        return 1;
    }
    fseek(fin, 0, SEEK_END);
    int inputLen = (int)ftell(fin);
    rewind(fin);
    unsigned char *inputData = malloc(inputLen);
    fread(inputData, 1, inputLen, fin);
    fclose(fin);

    printf("=== Compression ===\n");

    int freq[ALPHA_SIZE];
    buildFreqTable(inputData, inputLen, freq);

    HuffTree tree;
    huffBuild(&tree, freq);

    unsigned char pathBuf[MAX_CODE_LEN];
    HuffCodeTable codes;
    memset(&codes, 0, sizeof(codes));
    huffGenerateCodes(&tree, tree.root, pathBuf, 0, &codes);

    int outBytes, outBits;
    unsigned char *bitstream = huffCompress(inputData, inputLen, &codes, &outBytes, &outBits);

    huffSave(HUFF_FILE, freq, bitstream, outBits);
    free(bitstream);

    reportStats(INPUT_FILE, HUFF_FILE);

    printf("\n=== Decompression ===\n");

    int freq2[ALPHA_SIZE];
    unsigned char *bitstream2;
    int totalBits2;
    huffLoad(HUFF_FILE, freq2, &bitstream2, &totalBits2);

    HuffTree tree2;
    huffBuild(&tree2, freq2);

    int restoredLen;
    unsigned char *restored = huffDecompress(&tree2, bitstream2, totalBits2, &restoredLen);
    free(bitstream2);

    FILE *fout = fopen(RESTORED_FILE, "wb");
    if (fout) {
        fwrite(restored, 1, restoredLen, fout);
        fclose(fout);
    }
    free(restored);
    free(inputData);

    verifyRestore(INPUT_FILE, RESTORED_FILE);

    return 0;
}
