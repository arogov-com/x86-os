void _start()
{
 asm("movl $0x1FFFF4,%esp");
 asm("movl $0x1FFFFC,%ebp");
 kernel_main();
}
