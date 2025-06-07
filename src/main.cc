#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <tam/error.h>
#include <tam/tam.h>
#include <vector>

std::vector<uint32_t> readProgramFromFile(const char *Filename) {
    std::ifstream In(Filename, std::ios::binary);
    std::vector<uint32_t> Codes;

    // find file size
    int FileLen;
    In.seekg(0, In.end);
    FileLen = In.tellg();
    In.seekg(0, In.beg);

    if (FileLen % 4 != 0) {
        throw tam::TamException(tam::EK_IOError, 0);
    }

    for (int J = 0; J < FileLen / 4; ++J) {
        int C;
        uint32_t Code = 0;
        for (int I = 0; I < 4; ++I) {
            C = In.get();
            Code = (Code << 8) | C;
        }
        Codes.push_back(Code);
    }
    return Codes;
}

int main(int Argc, const char **Argv) {
    tam::TamEmulator emulator;

    std::vector<uint32_t> Program;
    try {
        Program = readProgramFromFile(Argv[1]);
        emulator.loadProgram(Program);
    } catch (const std::exception &e) {
        return 1;
    }

    bool Running = true;
    while (Running) {
        try {
            tam::TamInstruction Instr = emulator.fetchDecode();
            Running = emulator.execute(Instr);
        } catch (const tam::TamException &e) {
            std::cerr << e.str() << std::endl;
            break;
        }
    }
}
