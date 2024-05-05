#include <iostream>

#include <mtccore/elffile.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "mtcc <elf file>" << std::endl;
        return 0;
    }

    MTC::ElfFile elf;
    if (!elf.load(argv[1])) {
        std::cerr << elf.errorMessage() << std::endl;
        return -1;
    }

    for (int i = 0; i < elf.sectionHeaderCount(); ++i) {
        std::cout << elf.sectionHeader(i).name() << std::endl;
    }
    return 0;
}