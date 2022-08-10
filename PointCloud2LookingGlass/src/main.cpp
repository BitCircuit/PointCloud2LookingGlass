//#include "readPointCloudFile/plyReader.h"
#include "Util/Util.h"
#include "vtkHandler/vtkHandler.h"
#include "jsonHandler/jsonHandler.h"
#include "cameraHandler/cameraHandler.h"

void usage() {
    printf("----------Main Usage----------\n");
    printf("-h, --help\t\t\tDisplay this info\n");
    printf("-v [PATH]\t\t\tTo play video\n");
    printf("-c\t\t\t\tShow live signal from camera (Developing...)\n");
    printf("Drop a single file to start viewing on the Looking Glass (Supported format: .PLY)\n");
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
        else if (strcmp(argv[1], "-c") == 0)        cameraHandler::cameraHandler(argv[2]);
        else if (strcmp(argv[1], "-v") == 0)        vtkHandler::vtkHandler(argv);
        else { 
            //Util::pathResult isABagFile = Util::pathParser(argv[1], ".bag");
            if (Util::pathParser(argv[1], ".bag").isFile)  cameraHandler::realsenseBagFileReader(argv[1]);
            else if (Util::pathParser(argv[1], ".ply").isFile) vtkHandler::vtkHandler(argv);
            else printf("File not supported. If the parameter is a path, please add flag -v to run. \n");
        }
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