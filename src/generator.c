#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

Sprout generator_generate(NodeExit *root)
{
    Sprout assembly = { 0 };

    sprout_push_cstr(&assembly, "global\t_start\n");
    sprout_push_cstr(&assembly, "_start:\n");

    size_t error_code = slice_usize(root->expr.token.slice, NULL);
    expect(error_code < 256, "Error code should be less than 256");

    sprout_insert_cstr(&assembly, 0, "%define\tSYS_EXIT\t0x3C\n");
    sprout_push_fmt(&assembly, "\tmov\trdi, %zu\n", error_code);
    sprout_push_cstr(&assembly, "\tmov\trax, SYS_EXIT\n");
    sprout_push_cstr(&assembly, "\tsyscall\n");

    return assembly;
}
