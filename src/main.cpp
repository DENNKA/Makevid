#include "Makevid.hpp"

int main(int argc, char** argv){
    Makevid makevid;

    if (argc < 1) {
        makevid.help();
        return 0;
    }

    std::vector<std::string> vecArgs(argv + 1, argv + argc);

    makevid.parseArgs(vecArgs);
}
