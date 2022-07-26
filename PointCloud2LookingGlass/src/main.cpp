//#include "readPointCloudFile/plyReader.h"
//#include "Util/Util.h"
#include "vtkHandler/vtkHandler.h"
#include "jsonHandler/jsonHandler.h"

void usage() {
    printf("----------Main Usage----------\n");
    printf("-h, --help\t\t\tDisplay this info\n");
    printf("-c\t\t\tShow live signal from camera (Developing...)\n");
    printf("Drop PLY file or directory to start viewing on the Looking Glass\n");
}

void firstRun() {
    
}

int main(int argc, char* argv[]) {
    printf("MSc Project - Light Field 3D Image Display and Capture System for Open Day Demo. \nCoded by Xiaoao Feng (s2325577) from the University of Edinburgh. \nAug 2022\n");
    if (argc == 1) {
        usage();
        //jsonHandler::dummy();
    }
    else {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) usage();
        else if (strcmp(argv[1], "-c") == 0);      // call cameraHandler(); or pclHandler(); 
        else    vtkHandler::vtkHandler(argv);
    }
#ifdef osWindows
    system("pause");
#elif osMacOS
    system("read");
#elif osLinux
    system("read");
#endif
    return 0;
}