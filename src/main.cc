/*
 * This file is part of tam-cpp, copyright (c) Ian Knight 2025.
 *
 * tam-cpp is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * tam-cpp is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with tam-cpp. If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdint>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <tam/cli.h>
#include <tam/error.h>
#include <tam/tam.h>
#include <vector>

std::vector<uint32_t> readProgramFromFile(std::string &Filename) {
    std::ifstream In(Filename, std::ios::binary);

    // find file size
    int FileLen;
    In.seekg(0, In.end);
    FileLen = In.tellg();
    In.seekg(0, In.beg);

    if (FileLen % 4 != 0) {
        throw tam::runtimeError(tam::EK_IOError, 0);
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
    tam::CliArgs *Args = tam::parseCli(Argc, Argv);
    if (!Args) {
        std::cout << "Usage: tam [-t | --trace] [-s | --step] FILENAME"
                  << std::endl;
        return 1;
    }

    tam::TamEmulator Emulator;
    try {
        std::vector<uint32_t> Program;
        Program = readProgramFromFile(Args->Filename);
        Emulator.loadProgram(Program);
    } catch (const std::exception &E) {
        std::cerr << E.what() << std::endl;
        delete Args;
        return 2;
    }

    bool Running = true;
    do {
        try {
            tam::TamInstruction Instr = Emulator.fetchDecode();

            Running = Emulator.execute(Instr);
            if (Args->Trace) {
                std::cout << Emulator.getSnapshot() << std::endl;
            }
            if (Args->Trace && Args->Step) {
                std::string s;
                std::getline(std::cin, s);
            }
        } catch (const std::exception &E) {
            std::cerr << E.what() << std::endl;
            delete Args;
            return 3;
        }
    } while (Running);

    delete Args;
}
