/**
 * main - main file of the ZFC compiler.
 */

#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "Expected file" << "\n";
        exit(-1);
    }
    std::ifstream file;
    file.open(argv[1]);
}
