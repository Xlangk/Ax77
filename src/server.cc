#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <bitset>
#include "regList.hh"

// Clock speed hz
int clk = 1;

// Memory width in bytes
int mbt = 2;

// Register rows
int reg8 = 2;
int reg16 = 2;

RegisterBox reg8s(0, 0);
RegisterBox reg16s(0, 0);

std::string command;
std::string *args {};
std::string i; // Input

void dumpReg(RegisterBox reg8s) {
    if (reg8s.sections == 0) std::cout << "Empty section list, nothing to display\n";
    else {
        if (args[2] == "all")
            for (int ij = 0; ij < reg8s.sections; ij++) {
                std::string sectorAsStr = std::to_string(ij);
                std::string bit8Address = "0x00000000";

                bit8Address.erase(bit8Address.end() - sectorAsStr.length(), bit8Address.end());
                std::cout << bit8Address << sectorAsStr << ": ";

                for (int j = 0; j < reg8s.bits; j++) {
                    std::cout << (int) reg8s.data[ij][j];
                    if (j % 4 == 3) std::cout << " ";
                }
                std::cout << std::endl;
            }
        else {
            try {
                int sectorAsInt = std::stoi(args[2]);
                if (sectorAsInt > reg8s.sections - 1) std::cout << "Section " << sectorAsInt << " does not exist\n";
                else {
                    std::string sectorAsStr = std::to_string(sectorAsInt);
                    std::string bit8Address = "0x00000000";

                    bit8Address.erase(bit8Address.end() - sectorAsStr.length(), bit8Address.end());
                    std::cout << bit8Address << sectorAsStr << ": ";

                    for (int j = 0; j < reg8s.bits; j++) {
                        std::cout << (int) reg8s.data[sectorAsInt][j];
                        if (j % 4 == 3) std::cout << " ";
                    }
                    std::cout << std::endl;
                }
            } catch (...) {
                std::cout << "Invalid section number\n";
            }
        }
    }
}

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

void work() {
    if (i == "kill") exit(0);
    command = i.substr(0, i.find(' '));
    args = new std::string[i.length()];
    split(i, ' ', args);

    if (command == "dump-r")
        if (args->length() < 3)
            std::cout << "Expected 2 arguments, {bits, (section) | all}, got " << args->length() << std::endl;
        else {
            if (args[1] == "all") {
                std::cout << "Un supported\n";
            } else {
                if (args[1] == "8") {
                    dumpReg(reg8s);
                } else {
                    std::cout << "Invalid register bit width, got " << args[1] << "\n";
                }
            }
        }

    else if (command == "set-r")
        if (args->length() < 4)
            std::cout << "Expected 3 arguments, {bits, section, value}, got " << args->length() << std::endl;
        else {
            if (args[1] == "all") {
                std::cout << "Un supported\n";
            } else {
                if (args[1] == "8") {
                    try {
                        int sectorAsInt = std::stoi(args[2]);
                        if (sectorAsInt > reg8s.sections - 1) std::cout << "Section " << sectorAsInt << " does not exist\n";
                        else {
                            try {
                                int valueAsInt = std::stoi(args[3]);
                                if (valueAsInt > 255) std::cout << "Value " << valueAsInt << " is too large\n";
                                else {
                                    std::string valueAsBin = std::bitset<8>(valueAsInt).to_string();
                                    for (int j = 0; j < reg8s.bits; j++)
                                        reg8s.data[sectorAsInt][j] = valueAsBin[j] - '0';
                                }
                            } catch (...) {
                                std::cout << "Invalid value\n";
                            }
                        }
                    } catch (...) {
                        std::cout << "Invalid section number\n";
                    }
                } else {
                    std::cout << "Invalid register bit width, got " << args[1] << "\n";
                }
            }
        }

    else if (command == "exit" || command == "halt" || command == "quit")
        std::cout << "Did you mean 'kill'?\n";
    else
        std::cout << "Unknown command: " << command << std::endl;
}

int main(int argc, char *argv[]) {
    reg8s = RegisterBox(8, reg8);
    reg16s = RegisterBox(16, reg16);

    printf("Ax77 Advanced Virtual Machine\n");
    printf("-- Clock speed detect, %d hz\n", clk);
    printf("-- Memory width detect, %d bytes\n", mbt);
    printf("-- 8 bit registers detect, %d sections\n", reg8);

    std::function<void()> selfCall = [&selfCall] () {
        std::cout << "Ax77 VM > ";
        std::getline(std::cin, i);
        work();

        selfCall();
    };

    selfCall();
    return 0;
}