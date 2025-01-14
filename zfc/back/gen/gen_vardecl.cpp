#include <back/gen/gen_main.hpp>
#include <common/type.hpp>

void VarDeclNode::write(std::ofstream& file) {

    gen::write(file, this->var->type->to_output_str());
    gen::write(file, " ");
    gen::write(file, this->var->name);
    
    if (this->expr != nullptr) {
        gen::write(file, " = ");
        this->expr->write(file);
    }

}
