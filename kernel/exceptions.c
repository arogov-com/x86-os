#include "exceptions.h"
#include "vtty.h"
#include "paging.h"
#include "interrupt.h"
#include "syscounters.h"
#include "stdio.h"

extern exceptions_t exceptions;

volatile unsigned long addr;
volatile unsigned int errorcode;

char *exceptions_messages[] = {
    "\n*** DIVIDE ERROR ***\n",
    "\n*** DEBUG ***\n",
    "\n*** NMI ***\n",
    "\n*** BREAK POINT ***\n",
    "\n*** OVERFLOW ***\n",
    "\n*** BOUND Range Exceeded ***\n",
    "\n*** INVALID OPCODE ***\n",
    "\n*** DEVICE NOT AVAILABLE ***\n",
    "\n*** DOUBLE FAULT ***\n",
    "\n*** COPROCESSOR SEGMENT OVERRUN ***\n",
    "\n*** INVALID TSS, Selector: 0x%X ***\n",
    "\n*** SEGMENT NOT PRESENT Selector: 0x%X ***\n",
    "\n*** STACK FAULT Selector: 0x%X ***\n",
    "\n*** GENERAL PROTECTION FAULT Error code: %X ***\n",
    "\n*** PAGE FAULT EXCEPTION Error code: %X  Address: %X ***\n",
    "\n*** FPU FLOATING-POINT ERROR ***\n",
    "\n*** ALIGNMENT CHECK Address: %X ***\n",
    "\n*** MACHINE-CHECK ***\n",
    "\n*** SIMD FLOATING-POINT ***\n"
};

char excception_buffer[64];

void init_exceptions(void) {
    set_interrupt(0x0, DE, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x1, DB, TYPE_TRAP, 0, 1);
    set_interrupt(0x2, NMI, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x3, BP, TYPE_TRAP, 0, 1);
    set_interrupt(0x4, OF, TYPE_TRAP, 0, 1);
    set_interrupt(0x5, BR, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x6, UD, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x7, NM, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x8, DF, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x9, CO, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0xA, TS, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0xB, NP, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0xC, SS, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0xD, GP, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0xE, PF, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x10, MF, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x11, AC, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x12, MC, TYPE_INTERRUPT, 0, 1);
    set_interrupt(0x13, XF, TYPE_INTERRUPT, 0, 1);
}

EXCEPTION_HANDLER(DE) {
    ++exceptions.DE;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[0], 22);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(DB) {
    ++exceptions.DB;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[1], 15);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(NMI) {
    ++exceptions.NMI;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[2], 13);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(BP) {
    ++exceptions.BP;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[3], 21);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(OF) {
    ++exceptions.OF;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[4], 18);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(BR) {
    ++exceptions.BR;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[5], 30);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(UD) {
    ++exceptions.UD;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[6], 24);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(NM) {
    ++exceptions.NM;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[7], 30);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER_ERRCODE(DF) {
    ++exceptions.DF;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[8], 22);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(CO) {
    ++exceptions.CO;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[9], 37);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER_ERRCODE(TS) {
    ++exceptions.TS;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    int slen = sprintf(excception_buffer,exceptions_messages[10], errorcode);
    vtty_write(0, excception_buffer, slen);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER_ERRCODE(NP) {
    ++exceptions.NP;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    int slen = sprintf(excception_buffer, exceptions_messages[11], errorcode);
    vtty_write(0, excception_buffer, slen);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER_ERRCODE(SS) {
    ++exceptions.SS;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    int slen = sprintf(excception_buffer, exceptions_messages[12], errorcode);
    vtty_write(0, excception_buffer, slen);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER_ERRCODE(GP) {
    ++exceptions.GP;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    int slen = sprintf(excception_buffer, exceptions_messages[13], errorcode);
    vtty_write(0, excception_buffer, slen);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER_ERRCODE(PF) {
    ++exceptions.PF;
    asm volatile("mov %CR2, %eax");
    asm volatile("mov %eax, addr");
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    // assign_page((void*)addr, (void*)addr, 1, 1);
    int slen = sprintf(excception_buffer, exceptions_messages[14], errorcode, addr);
    vtty_write(0, excception_buffer, slen);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(MF) {
    ++exceptions.MF;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[15], 34);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER_ERRCODE(AC) {
    ++exceptions.AC;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    int slen = sprintf(excception_buffer, exceptions_messages[16], errorcode);
    vtty_write(0, excception_buffer, slen);
    set_vtty_attributeex(a, 0);
}

EXCEPTION_HANDLER(MC) {
    ++exceptions.MC;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[16], 23);
    set_vtty_attributeex(a, 0);
 }

EXCEPTION_HANDLER(XF) {
    ++exceptions.XF;
    char a = get_vtty_attribute();
    set_vtty_attributeex(FOREGROUND_RED | LIGHT, 0);
    vtty_write(0, exceptions_messages[16], 29);
    set_vtty_attributeex(a, 0);
 }
