//#include "readPointCloudFile/plyReader.h"
#include "Util/Util.h"
#include "vtkHandler/vtkHandler.h"
#include "jsonHandler/jsonHandler.h"

using namespace std;

void usage() {
    printf("MSc Project - Point Cloud to Light Field Display. \nCoded by Xiaoao Feng (s2325577) from the University of Edinburgh. \nAug 2022\n");
    printf("-h, --help\t\t\tDisplay this info\n");
    printf("-c\t\t\tShow live signal from camera (Developing...)\n");
    printf("Drop PLY file or directory to start viewing on the Looking Glass\n");
}

int main(int argc, char* argv[]) {
    if (argc == 1)    usage();
    else {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) usage();
        else if (strcmp(argv[1], "-c") == 0) ;      // call cameraHandler(); or pclHandler(); 
        else    vtkHandler(argv[1]);
    }
    return 0;
}