#include "type.hpp"

std::vector<Type*> user_types;

namespace {

std::string get_type_base(Type* t) {
    if (t->primitive != MAX_INVALID) {
        return typeToZStr(t->primitive);
    } else {
        if (t->type_flavor == TT_ENUM) {
            return reinterpret_cast<Enum*>(t)->name;
        }
    }
    return "INVALID!!";
}

}

BuiltinType zStrToType(std::string in) {
    if (in == "bool") return BOOL;
    if (in == "char") return CHAR;
    if (in == "uchar") return UCHAR;
    if (in == "short") return SHORT;
    if (in == "ushort") return USHORT;
    if (in == "int") return INT;
    if (in == "uint") return UINT;
    if (in == "float") return FLOAT;
    if (in == "double") return DOUBLE;
    if (in == "long") return LONG;
    if (in == "ulong") return ULONG;
    if (in == "void") return VOID;
    return MAX_INVALID;
}

std::string typeToZStr(BuiltinType in) {
    if (in == BOOL) return "bool";
    if (in == CHAR) return "char";
    if (in == UCHAR) return "uchar";
    if (in == SHORT) return "short";
    if (in == USHORT) return "ushort";
    if (in == INT) return "int";
    if (in == UINT) return "uint";
    if (in == FLOAT) return "float";
    if (in == DOUBLE) return "double";
    if (in == LONG) return "long";
    if (in == ULONG) return "ulong";
    if (in == VOID) return "void";
    if (in == VA_TYPE) return "...";
    return "!!INVALID_TYPE!!";
}

BuiltinType cStrToType(std::string in) {
    if (in == "_Bool") return BOOL;
    if (in == "signed char") return CHAR;
    if (in == "unsigned char") return UCHAR;
    if (in == "short") return SHORT;
    if (in == "unsigned short") return USHORT;
    if (in == "long") return INT;
    if (in == "unsigned long") return UINT;
    if (in == "float") return FLOAT;
    if (in == "double") return DOUBLE;
    if (in == "long long") return LONG;
    if (in == "unsigned long long") return ULONG;
    if (in == "void") return VOID;
    return MAX_INVALID;
}

std::string typeToCStr(BuiltinType in) {
    if (in == BOOL) return "_Bool";
    if (in == CHAR) return "char";
    if (in == UCHAR) return "unsigned char";
    if (in == SHORT) return "short";
    if (in == USHORT) return "unsigned short";
    if (in == INT) return "long";
    if (in == UINT) return "unsigned long";
    if (in == FLOAT) return "float";
    if (in == DOUBLE) return "double";
    if (in == LONG) return "long long";
    if (in == ULONG) return "unsigned long long";
    if (in == VOID) return "void";
    if (in == VA_TYPE) return "...";
    return "!!INVALID_TYPE!!";
}

bool Type::operator==(Type p) {
    return 
    this->primitive == p.primitive
    && this->indirection == p.indirection
    && this->ref == p.ref
    ;
}

bool Type::operator!=(Type p) {
    return !(*this == p);
}

Type* parse_type(std::ifstream& file) {
    Type* itype = new Type();
    // Change this later, if need be.
    auto type = lex::lex(file);
    if (type.type == REF) {
        itype->ref = true;
        // Go to next token
        type = lex::lex(file);
    } else {
        itype->ref = false;
    }
    if (type.type == TYPENAME) {
        itype->primitive = zStrToType(type.str);
        if (itype->primitive == MAX_INVALID) {
            // Try searching for a user-defined type.
            for (auto utype : user_types) {
                if (utype->type_flavor == TT_ENUM) {
                    if (static_cast<Enum*>(utype)->name == type.str) {
                        delete itype;
                        return utype;
                    }
                }
            }
        }
    } else {
        if (itype->ref) {
            lex::unlex(type);
            itype->primitive = VOID; // ref == void *
        } else {
            ZF_ERROR("line %d: expected type name or 'ref', found \"%s\"", type.line, type.raw_content());
        }
    }
    if (itype->primitive == MAX_INVALID && itype->type_flavor == TT_BUILTIN) {
        ZF_ERROR("line %d: type name \"%s\" not recognized", type.line, type.raw_content());
    }
    while (1) {
        // Parse array brackets
        auto ob = lex::lex(file);
        if (ob.type != OBRACKET) {
            lex::unlex(ob);
            break;
        }
        auto cb = lex::lex(file);
        if (cb.type != CBRACKET) {
            ZF_ERROR("line %d: expected ] to match [", cb.line);
        }
        ++itype->indirection;
    }
    return itype;
}

std::string Type::to_human_str() {
    // std::string ret = this->ref ? "ref " : "";
    std::string ret = get_type_base(this);
    for (int i = 0; i < this->indirection; i++) {
        ret += " [ ]";
    }
    return ret;
}

std::string Type::to_output_str() {
    std::string ret = get_type_base(this);
    for (int i = 0; i < this->indirection; i++) {
        ret += " *";
    }
    if (this->ref) ret += " *";
    return ret;
}

Type Type::deref() {
    Type ret = *this;
    if (ret.indirection == 0) {
        ret.primitive = MAX_INVALID;
    } else {
        --ret.indirection;
    }
    ret.ref = false;
    return ret;
}
