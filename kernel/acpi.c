// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stddef.h>
#include <stdint.h>
#include "acpi.h"
#include "stdio.h"

rsdp_t *acpi_find_rsdp(void *range_start, void *range_end) {
    for(void *rsdp = range_start; rsdp <= range_end; rsdp += 4) {
        if(*((uint32_t *)rsdp) == RSDP_SIGNATURE_P1 && *((uint32_t *)rsdp + 1) == RSDP_SIGNATURE_P2) {
            return (rsdp_t *)rsdp;
        }
    }
    return NULL;
}

int acpi_chk_summ(void *addr, int size) {
    uint8_t summ = 0;
    while(size--) {
        summ += *(char*)(addr + size);
    }
    return summ;
}

int acpi_process_rsdt(rsdt_t *rsdt) {
    int dt_cnt = (rsdt->header.length - sizeof(rsdt->header)) / 4;
    for(int i = 0; i != dt_cnt; ++i) {
        switch(((acpi_sdt_header_t *)rsdt->ptr[i])->signature) {
            case DT_TYPE_APIC:
                printf("   APIC at DT %i\n", i);
                break;
            default:
                printf("   Unknown DT type %X\n", ((acpi_sdt_header_t *)rsdt->ptr[i])->signature);
                return 1;
                break;
        }

    }
    return 0;
}
