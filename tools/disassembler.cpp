#include "disassembler.h"

        void Disassembler::print_byte(const frumul::byte b, const ByteType t) {
            /* Print b on the screen following the type t
             */
            std::cout << count++ << " : ";
            std::cout << static_cast<int>(b) << " : ";
            try {
                switch(t) {
                    case INSTRUCTION:
                        std::cout << instructions_names.at(b);
                        break;
                    case TYPE:
                        std::cout << frumul::ExprType::typeToString(
                                static_cast<frumul::ExprType::Type>(b));
                        break;
                    case INT:
                        std::cout << static_cast<int>(b);
                        break;
                    case BOOL:
                        std::cout << bools.at(b);
                        break;
                };
            } catch (...) {
                std::cerr << "UNRECOGNIZED BYTE : "<< b;
            }
            std::cout << std::endl;
        }

        void Disassembler::manage_real_type(std::vector<frumul::byte>::iterator& it) {
            /* Prints the real type name on the screen
             * until it founds a primitive
             */
            for (;*it > frumul::ET::MIN_CONTAINER;++it) {
                print_byte(*it,TYPE);
            }
            //print primitive
            print_byte(*it,TYPE);
        }


        void Disassembler::run (frumul::ByteCode& bt) {
            /* Run the disassembler and prints it to the screen
             */
            using namespace frumul;
            count = 0;
            // printing general informations
            printl("Bytecode\n========");
            std::cout << "Number of variables: " << bt.getVariableNumber() << std::endl;
            printl("Constants:");
            for (size_t i=0; i< bt.getConstants().size(); ++i) {
                std::cout << i << " : " << bt.getConstants().at(i) << std::endl;
            }
            printl("Statics:");
            for (auto const& [key,val] : bt.getStatics()) 
                std::cout << key << " : " << val << std::endl;
            // printing bytecode
            printl("Bytecode:");

            for (auto it = bt.getBegin(); it != bt.getEnd();) {
                BT::Instruction in { static_cast<BT::Instruction>(*it)};
                print_byte(*it,INSTRUCTION);

                switch (in) {
                    case BT::BOOL_EQUAL:
                    case BT::LENGTH:
                    case BT::TEXT_GET_CHAR:
                        print_byte(*++it,TYPE);
                        break;
                    case BT::JUMP_TRUE:
                    case BT::JUMP_FALSE:
                    case BT::JUMP:
                        {
                            int_least16_t address{*++it};
                            print_byte(address,INT);
                            address = address << 8;
                            address += *++it;
                            print_byte(*it,INT);
                            std::cout << "Address: " << address << std::endl;
                        }
                        break;

                    case BT::CALL:
                        {
                            int arg_byte_nb = *++it;
                            std::cout << "Number of args: " << std::endl;
                            print_byte(arg_byte_nb,INT);

                            for (;arg_byte_nb > 0; --arg_byte_nb) {
                                std::cout << "Arg number: " << arg_byte_nb << std::endl;
                                // real type
                                manage_real_type(it);
                                // name ?
                                std::cout << "Has arg a name?" << std::endl;
                                print_byte(*++it,BOOL);
                            }

                        }
                        break;
                    case BT::CAST:
                        {
                        printl("Source");
                        print_byte(*++it,TYPE);
                        printl("Target");
                        auto target = *++it;
                        print_byte(target,TYPE);
                        if (target == ET::SYMBOL)
                            manage_real_type(it);
                        }
                        break;
                    case BT::LIST_SET_ELT:
                        printl("Number of indices");
                        print_byte(*++it,INT);
                        printl("Is a char?");
                        print_byte(*++it,BOOL);
                        break;
                    case BT::PUSH:
                        print_byte(*++it,TYPE);
                        print_byte(*++it,INT);
                        break;

                    case BT::ASSIGN:
                        print_byte(*++it,INT);
                        break;
                    case BT::CHECK_TYPE:
                        manage_real_type(++it);
                        break;
                    default:
                        // majority of instructions are here
                        break;
                };
                ++it;
            }
        }

// static members
const std::vector<std::string> Disassembler::instructions_names {"NULL_INSTRUCTION",
"INT_ADD",
"INT_DIV",
"INT_SUB",
"INT_NEG",
"INT_MOD",
"INT_POS",
"INT_MUL",
"BOOL_AND",
"BOOL_OR",
"BOOL_NOT",
"TEXT_ADD",
"TEXT_MUL",
"TEXT_GET_CHAR",
"TEXT_SET_CHAR",
"BOOL_EQUAL",
"BOOL_INFERIOR",
"BOOL_SUPERIOR",
"BOOL_INF_EQUAL",
"BOOL_SUP_EQUAL",
"FIND_SYMBOL",
"CHECK_TYPE",
"LIST_ADD",
"LIST_APPEND",
"LIST_GET_ELT",
"LIST_SET_ELT",
"LENGTH",
"JUMP_TRUE",
"JUMP_FALSE",
"JUMP",
"CALL",
"CAST",
"ASSIGN",
"PUSH",
"RETURN",
"MAX_INSTRUCTIONS"};
const std::vector<std::string> Disassembler::bools  {"false","true"};
