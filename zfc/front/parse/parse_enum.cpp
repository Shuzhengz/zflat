#include <front/parse/parse_header.hpp>

void EnumDeclNode::read(std::ifstream& file) {

    Token nametok;
    bool is_bitfield;

    auto bitftok = lex::lex(file);
    if (bitftok.type == BITFIELD) {
        is_bitfield = true; 
    } else {
        lex::unlex(bitftok);
        is_bitfield = false;
    }

    this->line = expect(file, ENUM).line;

    nametok = expect(file, IDENTIFIER);
    this->sym = new Enum(nametok.str);

    this->sym->bitfield = is_bitfield;

    // this->sym->lineno = this->line; TODO???

    expect(file, OBRACE);

    int val = is_bitfield ? 1 : 0; // The value of the currently-being-parsed enum constant.
    int decls = 0;
    
    // Normal enums are uchar, bitfield enums are uint
    int maxval = is_bitfield ? sizeof(unsigned long) : ( 1 << (8 * sizeof(unsigned char)) );
    
    while (1) {

        auto id = expect(file, IDENTIFIER);
        auto id_val = new sym::EnumVal(id.str, val, this->sym, is_bitfield);
        this->sym->values.push_back(id_val);

        auto closetest = lex::lex(file);
        if (closetest.type == CBRACE) {
            break;
        } else {
            lex::unlex(closetest);
        }

        expect(file, COMMA);

        // Check for max
        if (decls == maxval) {
            ZF_ERROR("line %d: %s enum can have a max of %d values",
                this->line,
                (is_bitfield ? "bitfield" : "non-bitfield"),
                maxval
            );
        }

        is_bitfield ? val <<= 1 : ++val;
        ++decls;

    }
    
    user_types.push_back(this->sym);

}
