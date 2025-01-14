/**
 * Compile - provide a function to compile one file to another.
 */

#pragma once

#include <fstream>

#include <common/ast.hpp>
#include <front/module.hpp>
#include <util/error.hpp>
#include <util/fs.hpp>

// Only visible for compile individual mode
void compile_unit(std::string path, bool leave_at_c);

void compile_default();
