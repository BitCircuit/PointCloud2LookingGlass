#include "Util.h"

// Reference: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
vector<string> Util::scanFileByType(const char* path, const char* fileType) {
    vector<string> resultList;
    for (const auto& entry : filesystem::directory_iterator(path)) {
        filesystem::path temp0 = entry.path();
        string temp{ temp0.u8string() };
        const char* filePath = temp.c_str();
        //int len = strlen(filePath);
        // Reference: https://stackoverflow.com/questions/5297248/how-to-compare-last-n-characters-of-a-string-to-another-string-in-c
        int formatLen = strlen(fileType);
        const char* lastNChar = &filePath[strlen(filePath) - formatLen];

        if (strcmp(lastNChar, fileType) == 0)        resultList.push_back(filePath);
    }
    resultList.shrink_to_fit();

    return resultList;
}

bool Util::findInPLY(const char* path, Util::findMode type) {
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
        if (type == Util::alpha && strncmp(info, "property uchar alpha", 20) == 0) {
            result = true;
            break;
        }
        if (type == Util::color && strncmp(info, "property uchar red", 18) == 0) {
            result = true;
            break;
        }
    }
    fclose(src);
    return result;
}

Util::pathResult Util::pathParser(const char* path, const char* format) {
    Util::pathResult result;
    char* info = (char*)malloc(strlen(path) * sizeof(char));

    // check if path represents as a file first. 
    int formatLen = strlen(format);
    const char* lastNChar = &path[strlen(path) - formatLen];
    if (strcmp(lastNChar, format) == 0) {
        result.isFile = true;

        int i;
        for (i = strlen(path); i > 0; i--)
            if (path[i - 1] == '\\' || path[i - 1] == '/')      break;
        strncpy(info, path, i);
        info[i] = '\0';
        result.directory = info;

        int x;
        for (x = i; path[x] != '.'; x++) {
            info[x - i] = path[x];
        }
        info[x - i] = '\0';
        result.fileName = info;
    }
    else {
        result.isFile = false;
        result.directory = path;
#ifdef osWindows
        result.directory += "\\";
#elif osMacOS
        result.directory += "/";
#elif osLinux
        result.directory += "/";
#endif
    }
    free(info);
    return result;
}

// Return file size in Bytes
long Util::getFileSize(const char* filename) {
    struct stat stat_buf;
    int rc = stat(filename, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}