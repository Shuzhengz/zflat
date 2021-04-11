#include "symbol.hpp"

namespace {

std::vector<std::vector<sym::Symbol*>> symtab;

sym::Function* func = nullptr;

} // namespace

namespace sym {

void enter_scope() {
    symtab.push_back(std::vector<Symbol*>());
}
void exit_scope() {
    symtab.pop_back();
}

void add_symbol(Symbol* s) {
    symtab[symtab.size() - 1].push_back(s);
}

void add_global_symbol(Symbol* s) {
    symtab[0].push_back(s);
}

Variable* resolve_var(std::string name) {
    for (int i = symtab.size(); i > 0; --i) {
        for (Symbol* sym : symtab[i - 1]) {
            Variable* var = static_cast<Variable*>(sym);
            if (sym->name == name && var != nullptr) return var;
        }
    }
    return nullptr;
}

Function* resolve_fn(std::string name, std::vector<Type> args) {
    for (int i = symtab.size(); i > 0; --i) {
        for (Symbol* sym : symtab[i - 1]) {
            Function* fn = static_cast<Function*>(sym);
            if (fn != nullptr) {
                bool match = true;
                if (args.size() != fn->args.size()) continue;
                auto size = args.size();
                for (int i = 0; i < size; i++) {
                    if (args[i] != fn->args[i].type) match = false;
                }
                match &= fn->name == name;
                if (match) return fn;
            }
        }
    }
    return nullptr;
}

bool in_global_scope() {
    return symtab.size() == 1;
}

Function* current_function() {
    return func;
}

void set_function(Function* fn) {
    func = fn;
}

std::string Function::get_overloaded_name() {
    std::string out = this->name;
    for (sym::Variable str : this->args) {
        out += "$";
        out += typeToStr(str.type);
    }
    out += "$$";
    out += typeToStr(this->ret);
    return out;
}

} // namespace sym