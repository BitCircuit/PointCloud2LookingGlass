#include "main.h"
#include "readPointCloudFile/plyReader.h"

void usage() {
    printf("MSc Project - Point Cloud to Light Field Display. Coded by Xiaoao Feng (s2325577) from the University of Edinburgh. Jun 2022\n");
    printf("-h, --help\t\t\tDisplay this info\n");
    printf("-i, --input \"path to file\"\tRead ply file\n");
}

int main(int argc, char* argv[]) {
    if (argc == 1)    usage();
    else {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) usage();
        else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--input") == 0) readPLY(argv);
        else    usage();
    }
    return 0;
}