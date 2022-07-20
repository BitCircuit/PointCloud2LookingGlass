#include "jsonHandler.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/reader.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/istreamwrapper.h"

using namespace rapidjson;
using namespace std;

void readJSON() {
    ifstream ifs("output.json");
    IStreamWrapper isw(ifs);

    Document d;
    d.ParseStream(isw);
}

void newJSON(char* destPath) {
    const char json[] = " { \"hello\" : \"world\", \"t\" : true , \"f\" : false, \"n\": null, \"i\":123, \"pi\": 3.1416, \"a\":[1, 2, 3, 4] } ";

    Document d;
    d.Parse(json);

    ofstream ofs(destPath);
    OStreamWrapper osw(ofs);

    Writer<OStreamWrapper> writer(osw);
    d.Accept(writer);
}