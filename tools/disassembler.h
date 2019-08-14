#ifndef DISASSEMBLER_BYTECODE_FRUMUL
#define DISASSEMBLER_BYTECODE_FRUMUL
#if DEBUG
#include <iostream>
#include <string>
#include <vector>
#include "bytecode.h"
#include "vmtypes.h"

class Disassembler {
    private:
        int count { 0 };
    public:
        enum ByteType {
            INSTRUCTION,
            TYPE,
            INT,
            BOOL,
        };
        static const std::vector<std::string> instructions_names ;

        static const std::vector<std::string> bools;

        void print_byte(const frumul::byte b, const ByteType t);
        void manage_real_type(std::vector<frumul::byte>::iterator& it);
        void run (frumul::ByteCode& bt);
};
#endif // DEBUG
#endif
