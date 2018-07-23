#include <iostream>
#include <string>
#include <vector>

using namespace std;

// TODO take bounding box
vector<string> commands = {
    "fission 1 0 0 0",

    "fission 0 1 0 1",
    "smove 14 0 0",

    "fission 0 0 1 0",
    "smove 14 0 0",
    "smove 0 15 0",

    "fission 0 1 0 1",
    "smove 0 0 14",
    "smove 0 12 0",
    "smove 0 0 14",

    "fission 1 0 0 0",
    "smove 0 0 13",
    "fission 1 0 0 0",
    "smove 0 0 14",
    "smove 0 14 0",

    "smove 0 0 14",
    "smove 0 15 0",
    "smove 0 0 14",
    "smove 13 0 0",
    "smove 14 0 0",
    "fission 0 1 0 0",
    "smove 13 0 0",

    "smove 0 0 13",
    "smove 0 13 0",
    "smove 0 0 13",
    "smove 13 0 0",
    "smove 13 0 0",
    "smove 0 -15 0",
    "smove 0 12 0",
    "smove 13 0 0",

    "gvoid 1 0 1 27 28 -27",
    "gvoid -1 0 1 -27 -28 -27",
    "gvoid 1 0 1 27 -28 -27",
    "gvoid 1 0 1 -27 -28 27",
    "gvoid 1 0 -1 -27 28 -27",
    "gvoid 1 0 1 27 28 27",
    "gvoid 1 0 1 27 -28 27",
    "gvoid 1 0 1 -27 28 27",

    "smove 0 0 -13",
    "smove 0 -13 0",
    "smove 0 0 -13",
    "smove -13 0 0",
    "smove -13 0 0",
    "smove 0 15 0",
    "smove 0 -12 0",
    "smove -13 0 0",

    "smove 0 0 -14",
    "smove 0 -15 0",
    "smove 0 0 -14",
    "smove -13 0 0",
    "smove -14 0 0",
    "fusionp 0 1 0",
    "fusions 0 -1 0",
    "smove -13 0 0",

    "fusionp 1 0 0",
    "smove 0 0 -13",
    "fusionp 1 0 0",
    "fusions -1 0 0",
    "smove 0 0 -14",
    "smove 0 -14 0",
    "fusions -1 0 0",

    "fusionp 0 1 0",
    "smove 0 0 -14",
    "smove 0 -12 0",
    "smove 0 0 -14",
    "fusions 0 -1 0",

    "fusionp 0 0 1",
    "smove -14 0 0",
    "smove 0 -15 0",
    "fusions 0 0 -1",

    "fusionp 0 1 0",
    "smove -14 0 0",
    "fusions 0 -1 0",

    "fusionp 1 0 0",
    "fusions -1 0 0",

    "halt"
};

int main() {
    for (int i = 0; i < commands.size(); i++) {
        cout << commands[i] << endl;
    }
}