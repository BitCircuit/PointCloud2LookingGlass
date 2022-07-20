//#include "readPointCloudFile/plyReader.h"
#include "vtkHandler/vtkHandler.h"
#include "jsonHandler/jsonHandler.h"

#include "stdio.h"
#include "String.h"

using namespace std;

void usage() {
    printf("MSc Project - Point Cloud to Light Field Display. \nCoded by Xiaoao Feng (s2325577) from the University of Edinburgh. \nJun 2022\n");
    printf("-h, --help\t\t\tDisplay this info\n");
    printf("-i, --input \"path to file\"\tRead single ply file\n");
}

int main(int argc, char* argv[]) {
    if (argc == 1)    usage();
    else {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) usage();
        else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--input") == 0) vtkHandler(argv);
        else if (strcmp(argv[1], "-j") == 0) newJSON(argv[1]);
        else    usage();
    }
    return 0;
}