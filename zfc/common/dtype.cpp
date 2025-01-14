#include "dtype.hpp"

namespace {

Type* get_btype(BinaryExprNode* expr) {
    if (expr->left != nullptr && expr->right == nullptr) {
        return new Type(static_cast<sym::Function*>(dynamic_cast<FuncCallNode*>(expr->left)->call)->ret);
    }
    if (expr->left != nullptr && expr->right != nullptr) {

        Type *left = get_type(expr->left), *right = get_type(expr->right);
        if (*left == VOID || *right == VOID) {
            ZF_ERROR("invalid expression on line %d: operand cannot have void type", expr->line);
        }

        if (!are_types_compatible(left, right)) {
            ZF_ERROR("line %d: operands don't match types (found %s and %s)", expr->line, left->to_human_str().c_str(), right->to_human_str().c_str());
        }

        // Check ==, !=, etc., and <, >, etc.
        if (
            op::is_bool(expr->op)
        ) {
            return new Type(BOOL);
        }

        // Array index
        if (expr->op == op::Operator::INDEX) {
            // Check left and right
            if (left->indirection == 0) {
                ZF_ERROR("line %d: expr is not of array type (is type %s instead)", expr->left->line, left->to_human_str().c_str());
            }
            if (right->indirection != 0) {
                // TODO: detect other non-numerical types
                ZF_ERROR("line %d: array index is not numerical", expr->left->line);
            }
            return new Type(left->deref());
        }

        return left;

    }
    // TODO
    return new Type(INT); // the only literal type
}

}

Type* get_type(ExprNode* expr) {
    
    auto* bexpr = dynamic_cast<BinaryExprNode*>(expr->inner);
    auto* lexpr = dynamic_cast<LiteralNode*>(expr->inner);
    auto* vexpr = dynamic_cast<VariableNode*>(expr->inner);
    auto* fexpr = dynamic_cast<FuncCallNode*>(expr->inner);

    if (bexpr != nullptr) {
        return get_btype(bexpr);
    }

    if (lexpr != nullptr) {
        if (lexpr->lit[0] == '\'') {
            return new Type(CHAR); // TODO: signeed or unsigned? Assumes signed now.
        } else if (lexpr->lit[0] == '"') {
            // String literal
            Type* ret = new Type(CHAR);
            ++ret->indirection;
            return ret;
        }
        return new Type(INT); // the only literal type
    }

    if (vexpr != nullptr) {
        auto type = vexpr->sym->type;
        type->ref = vexpr->ref; // NOT vexpr->sym->ref; remember implicit conversion
        return type;
    }

    if (fexpr != nullptr) {
        return new Type(fexpr->call->ret);
    }

    return new Type(MAX_INVALID);

}

bool are_types_compatible(Type* a, Type* b) {
    if (a == b) return true;
    if (a->type_flavor == TT_ENUM) {
        if (static_cast<Enum*>(a)->underlying_type() == *b) return true;
    }
    return *a == *b;
}
