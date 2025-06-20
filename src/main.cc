#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <tam/error.h>
#include <tam/tam.h>
#include <vector>

std::vector<uint32_t> readProgramFromFile(const char *Filename) {
    std::ifstream In(Filename, std::ios::binary);

    // find file size
    int FileLen;
    In.seekg(0, In.end);
    FileLen = In.tellg();
    In.seekg(0, In.beg);

    if (FileLen % 4 != 0) {
        throw tam::TamException(tam::EK_IOError, 0);
    }

    // read instructions
    std::vector<uint32_t> Codes;
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
    tam::TamEmulator Emulator;

    std::vector<uint32_t> Program;
    try {
        Program = readProgramFromFile(Argv[1]);
        Emulator.loadProgram(Program);
    } catch (const std::exception &E) {
        return 1;
    }

    bool Running = true;
    while (Running) {
        try {
            tam::TamInstruction Instr = Emulator.fetchDecode();
            Running = Emulator.execute(Instr);
        } catch (const tam::TamException &E) {
            std::cerr << E.str() << std::endl;
            break;
        }
    }
}
