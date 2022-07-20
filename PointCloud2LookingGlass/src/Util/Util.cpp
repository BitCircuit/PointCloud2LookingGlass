#include "Util.h"

using namespace std;

progressbar bar(100);

// Reference: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
vector<string> scanFileByType(char* path, const char fileType[4]) {
    vector<string> resultList;
    for (const auto& entry : filesystem::directory_iterator(path)) {
        filesystem::path temp0 = entry.path();
        string temp{ temp0.u8string() };
        const char* filePath = temp.c_str();
        int len = strlen(filePath);
        // Reference: https://stackoverflow.com/questions/5297248/how-to-compare-last-n-characters-of-a-string-to-another-string-in-c
        const char* lastFourChar = &filePath[len - 4];

        if (strcmp(lastFourChar, fileType) == 0)        resultList.push_back(filePath);
    }
    resultList.shrink_to_fit();

    return resultList;
}

bool findInPLY(char* path, findMode type) {
    bool result = false;
    FILE* src = fopen(path, "rb+");
    if (src == NULL) {
        printf("File not exist. \n");
        return false;
    }
    char* info;
    info = (char*)malloc(100 * sizeof(char));
    while (true) {
        fgets(info, 100, src);      // Read until end of line or EOF, max reading char is set to 100
        if (strncmp(info, "end_header", 10) == 0)   break;
        if (type == alpha && strncmp(info, "property uchar alpha", 20) == 0) {
            result = true;
            break;
        }
        if (type == color && strncmp(info, "property uchar red", 18) == 0) {
            result = true;
            break;
        }
    }
    fclose(src);
    return result;
}