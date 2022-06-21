#include "stdbool.h"
#include "stdint.h"

#define formatAscii     1
#define formatBinLE     2
#define formatBinBE     3
#define datatypeInt8    1
#define datatypeUInt8   2
#define datatypeInt16   3
#define datatypeUInt16  4
#define datatypeInt32   5
#define datatypeUInt32  6
#define datatypeFloat32 7
#define datatypeFloat64 8
#define elementVertex   1
#define elementFace     2
#define coordinateX     0
#define coordinateY     1
#define coordinateZ     2
#define coordinateNX    0
#define coordinateNY    1
#define coordinateNZ    2
#define vertexRed       0
#define vertexGreen     1
#define vertexBlue      2

typedef struct {
    char* data;
    int length;
}HeaderInfo;

typedef union {
    float f;
    struct
    {
        // Order is important.
        // Here the members of the union data structure
        // use the same memory (32 bits).
        // The ordering is taken
        // from the LSB to the MSB.
        unsigned int mantissa : 23;
        unsigned int exponent : 8;
        unsigned int sign : 1;
    } raw;
} myfloat;

struct Vertex {
    int totalAmount;

    int formatMode;

    int coordinateDataType;
    int coordinateReadLength;
    int8_t* coordinateInt8[3];
    uint8_t* coordinateUnsignedInt8[3];
    int16_t* coordinateInt16[3];
    uint16_t* coordinateUInt16[3];
    int32_t* coordinateInt32[3];
    uint32_t* coordinateUInt32[3];
    float* coordinateFloat[3];
    double* coordinateDouble[3];

    bool normalEnable;
    int normalDataType;
    int normalReadLength;
    int8_t* normalInt8[3];
    uint8_t* normalUnsignedInt8[3];
    int16_t* normalInt16[3];
    uint16_t* normalUInt16[3];
    int32_t* normalInt32[3];
    uint32_t* normalUInt32[3];
    float* normalFloat[3];
    double* normalDouble[3];

    bool colorRGBEnable;
    int colorRGBReadLength;
    uint8_t* colorRGBUnsignedInt8[3];
    bool colorAlphaEnable;
    int colorAlphaReadLength;
    uint8_t* colorAlphaUnsignedInt8;
};
struct Vertex vertex1;

void readPLY(char**);
float bin2Flo(int, unsigned char*);
uint8_t bin2UChar(unsigned char*);
unsigned int convertToInt(int*, int, int);