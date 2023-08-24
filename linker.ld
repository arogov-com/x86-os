ENTRY(start)

SECTIONS {
    . = 0x200000; /* Tells GRUB to load the kernel starting at the 1MiB mark */

    .boot :
    {
        /* Ensure that the multiboot header is at the beginning */
        *(.multiboot_header)
    }

    .text :
    {
        *(.text)
    }

}
