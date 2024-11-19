// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include "kernel.h"

__attribute__ ((naked))
void start() {
    asm volatile("movl $0x1FFFFC, %esp");
    asm volatile("movl $0x1FFFFC, %ebp");
    kernel_main();
}
