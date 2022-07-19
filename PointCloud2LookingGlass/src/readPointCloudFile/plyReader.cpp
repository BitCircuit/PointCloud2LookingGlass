#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#include "plyReader.h"

void init() {
    vertex1.formatMode = 0;
    vertex1.normalEnable = false;
    vertex1.colorRGBEnable = false;
    vertex1.colorAlphaEnable = false;
    vertex1.normalReadLength = 0;
    vertex1.colorRGBReadLength = 0;
    vertex1.colorAlphaReadLength = 0;
}

void readPLY(char* path[]) {
    init();
    char* info;
    info = (char*)malloc(100 * sizeof(char));

    //printf("path is %s\n", path[2]);
    FILE* src = fopen(path[2], "rb+");
    if (src == NULL) {
        printf("File not exist. \n");
        return;
    }

    bool headerEnd = false;
    HeaderInfo header1, header2, header3, header4, header5;
    header1.length = 12 * sizeof(char);
    header1.data = (char*)malloc(header1.length);
    header2.length = 22 * sizeof(char);
    header2.data = (char*)malloc(header2.length);
    header3.length = 20 * sizeof(char);
    header3.data = (char*)malloc(header3.length);
    header4.length = 20 * sizeof(char);
    header4.data = (char*)malloc(header4.length);
    header5.length = 20 * sizeof(char);
    header5.data = (char*)malloc(header5.length);

    int numLineRead = 0, element = 0;
    while (!headerEnd) {
        fgets(info, 100, src);      // Read until end of line or EOF, max reading char is set to 100
        //printf("file read: %s\n", info);
        numLineRead++;
        header1.data = strtok(info, " ");    // Break read line by spaces
        //printf("header1: %s\n", header1.data);
        if (strncmp(header1.data, "ply", 3) != 0 && numLineRead == 1) {
            printf("Not ply file. \n");
            free(header1.data);
            free(header2.data);
            free(header3.data);
            free(header4.data);
            free(header5.data);
            free(info);
            fclose(src);
            return;
        }
        if (strncmp(header1.data, "format", 6) == 0) {
            header2.data = strtok(NULL, " ");
            //printf("header2: %s\n", header2.data);
            if (strncmp(header2.data, "ascii", 5) == 0) vertex1.formatMode = formatAscii;
            else if (strncmp(header2.data, "binary_big_endian", 17) == 0) vertex1.formatMode = formatBinBE;
            else if (strncmp(header2.data, "binary_little_endian", 20) == 0) vertex1.formatMode = formatBinLE;
            else {
                printf("Not supported yet");
                free(header1.data);
                free(header2.data);
                free(header3.data);
                free(header4.data);
                free(header5.data);
                free(info);
                fclose(src);
                return;
            }
            header3.data = strtok(NULL, " ");
            //printf("header3: %s\n", header3.data);
            if (strncmp(header3.data, "1.0", 3) != 0) {
                printf("Not supported yet");
                free(header1.data);
                free(header2.data);
                free(header3.data);
                free(header4.data);
                free(header5.data);
                free(info);
                fclose(src);
                return;
            }
        }
        if (strncmp(header1.data, "end_header", 10) == 0) {
            headerEnd = true;
            //printf("End. \n");
        }
        if (strncmp(header1.data, "comment", 7) == 0) {
            // Do nothing
        }
        if (strncmp(header1.data, "element", 7) == 0) {
            header2.data = strtok(NULL, " ");
            //printf("header2: %s\n", header2.data);
            if (strncmp(header2.data, "vertex", 6) == 0) {
                element = elementVertex;
                header3.data = strtok(NULL, " ");
                //printf("header3: %s\n", header3.data);
                vertex1.totalAmount = atoi(header3.data);
            }
            else {
                element = elementFace;
                printf("Only read vertex. \n");
            }
        }
        if (strncmp(header1.data, "property", 8) == 0 && element == elementVertex) {
            header2.data = strtok(NULL, " ");
            //printf("header2: %s\n", header2.data);
            header3.data = strtok(NULL, " ");
            //printf("header3: %s\n", header3.data);
            if (strncmp(header3.data, "x", 1) == 0) {     // Assume x,y,z share same datatype
                if (strncmp(header2.data, "float", 5) == 0) {
                    vertex1.coordinateDataType = datatypeFloat32;
                    for (int i = 0; i < 3; i++) vertex1.coordinateFloat[i] = (float*)malloc(vertex1.totalAmount * sizeof(float));
                    vertex1.coordinateReadLength = 4;   // 4 from float32 bytes
                }
            }
            if (strncmp(header3.data, "nx", 1) == 0) {    // Assume nx,ny,nz share same datatype
                if (strncmp(header2.data, "float", 5) == 0) {
                    vertex1.normalEnable = true;
                    vertex1.normalDataType = datatypeFloat32;
                    for (int i = 0; i < 3; i++) vertex1.normalFloat[i] = (float*)malloc(vertex1.totalAmount * sizeof(float));
                    vertex1.normalReadLength = 4;   // 4 from float32 bytes
                }
            }
            if (strncmp(header3.data, "red", 3) == 0) {
                vertex1.colorRGBEnable = true;
                for (int i = 0; i < 3; i++) vertex1.colorRGBUnsignedInt8[i] = (uint8_t*)malloc(vertex1.totalAmount * sizeof(uint8_t));
                vertex1.colorRGBReadLength = 1;     // 1 from uint8_t byte
            }
            if (strncmp(header3.data, "alpha", 5) == 0) {
                vertex1.colorAlphaEnable = true;
                vertex1.colorAlphaUnsignedInt8 = (uint8_t*)malloc(vertex1.totalAmount * sizeof(uint8_t));
                vertex1.colorAlphaReadLength = 1;   // 1 from uint8_t byte
            }
        }
        memset(header1.data, 0, header1.length);
        memset(header2.data, 0, header2.length);
        memset(header3.data, 0, header3.length);
        memset(header4.data, 0, header4.length);
        memset(header5.data, 0, header5.length);
        memset(info, 0, 100 * sizeof(char));
    }
    // ---------------------------------------- End of header decoding----------------------------//
    for (int i = 0; i < vertex1.totalAmount; i++) {
        for (int j = 0; j < 3; j++) {
            fread(info, 1, vertex1.coordinateReadLength, src);
            vertex1.coordinateFloat[j][i] = bin2Flo(vertex1.formatMode, (unsigned char*)info);
            memset(info, 0, 100 * sizeof(char));
        }
        if (vertex1.normalEnable) {
            for (int j = 0; j < 3; j++) {
                fread(info, 1, vertex1.normalReadLength, src);
                vertex1.normalFloat[j][i] = bin2Flo(vertex1.formatMode, (unsigned char*)info);
                memset(info, 0, 100 * sizeof(char));
            }
        }
        if (vertex1.colorRGBEnable) {
            for (int j = 0; j < 3; j++) {
                fread(info, 1, vertex1.colorRGBReadLength, src);
                vertex1.colorRGBUnsignedInt8[j][i] = bin2UChar((unsigned char*)info);
                memset(info, 0, 100 * sizeof(char));
            }
        }
        if (vertex1.colorAlphaEnable) {
            fread(info, 1, vertex1.colorAlphaReadLength, src);
            vertex1.colorAlphaUnsignedInt8[i] = bin2UChar((unsigned char*)info);
            memset(info, 0, 100 * sizeof(char));
        }
    }

    // Print something to test
    printf("i\t\tx\t\ty\t\tz\tR\tG\tB\n");
    for (int j = 0; j < 50; j++) printf("%d\t%f\t%f\t%f\t%d\t%d\t%d\n", j + 1, vertex1.coordinateFloat[coordinateX][j], vertex1.coordinateFloat[coordinateY][j], vertex1.coordinateFloat[coordinateZ][j], vertex1.colorRGBUnsignedInt8[vertexRed][j], vertex1.colorRGBUnsignedInt8[vertexGreen][j], vertex1.colorRGBUnsignedInt8[vertexBlue][j]);

    /*
    if (header1.data)   free(header1.data);
    if (header2.data)   free(header2.data);
    if (header3.data)   free(header3.data);
    if (header4.data)   free(header4.data);
    if (header5.data)   free(header5.data);
    if(vertex1.coordinateDataType==datatypeFloat32 && vertex1.coordinateFloat) for (int i = 0; i<3; i++) free(vertex1.coordinateFloat[i]);
    if(vertex1.normalEnable && vertex1.normalDataType==datatypeFloat32 && vertex1.normalFloat) for (int i = 0; i<3; i++) free(vertex1.normalFloat[i]);
    if(vertex1.colorRGBEnable && vertex1.colorRGBUnsignedInt8)  for (int i = 0; i<3; i++) free(vertex1.colorRGBUnsignedInt8[i]);
    if(vertex1.colorAlphaEnable && vertex1.colorAlphaUnsignedInt8)    free(vertex1.colorAlphaUnsignedInt8);
    if (info)   free(info);
    */

    fclose(src);
}

float bin2Flo(int binFormat, unsigned char* bin) {
    //printf("\nbin2Flo: input: ");
    //for (int x = 0; x<vertex1.coordinateReadLength; x++) printf("%X ", bin[x]);
    unsigned char temp[4];
    if (binFormat == formatBinLE) for (int i = 0; i < 4; i++) temp[i] = bin[3 - i];
    else if (binFormat == formatBinBE) for (int i = 0; i < 4; i++) temp[i] = bin[i];
    else printf("bin2Flo: Not supported.\n");

    unsigned int binary[32];
    // Convert char to integer array
    for (int i = 0; i < 32; i++) {
        if (i < 8)  binary[i] = (temp[0] & (128 >> i)) == (128 >> i) ? 1 : 0;   // 128 = 1000 0000
        if (i >= 8 && i < 16) binary[i] = (temp[1] & (128 >> (i - 8))) == (128 >> (i - 8)) ? 1 : 0;
        if (i >= 16 && i < 24) binary[i] = (temp[2] & (128 >> (i - 16))) == (128 >> (i - 16)) ? 1 : 0;
        if (i >= 24 && i < 32) binary[i] = (temp[3] & (128 >> (i - 24))) == (128 >> (i - 24)) ? 1 : 0;
    }

    // Reference: https://www.geeksforgeeks.org/program-for-conversion-of-32-bits-single-precision-ieee-754-floating-point-representation/
    myfloat var;
    var.raw.mantissa = convertToInt(binary, 9, 31);
    var.raw.exponent = convertToInt(binary, 1, 8);
    var.raw.sign = binary[0];
    //printf("\nbin2Flo output: %f", var.f);
    return var.f;
}

uint8_t bin2UChar(unsigned char* bin) {
    uint8_t result = 0;
    for (int i = 0; i < 8; i++)     result += (bin[0] & (1 << i)) == (1 << i) ? pow(2, i) : 0;
    return result;
}


// Function to convert a binary array to the corresponding integer
// Reference: https://www.geeksforgeeks.org/program-for-conversion-of-32-bits-single-precision-ieee-754-floating-point-representation/
unsigned int convertToInt(unsigned int* arr, int low, int high) {
    unsigned f = 0, i;
    for (i = high; i >= low; i--) {
        f = f + arr[i] * pow(2, high - i);
    }
    return f;
}