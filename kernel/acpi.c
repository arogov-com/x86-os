#include <stddef.h>
#include <stdint.h>
#include "acpi.h"

rsdp_t *find_rsdp(void *range_start, void *range_end) {
    for(void *rsdp = range_start; rsdp <= range_end; rsdp += 4) {
        if(*((uint32_t *)rsdp) == RSDP_SIGNATURE_P1 && *((uint32_t *)rsdp + 1) == RSDP_SIGNATURE_P2) {
            return (rsdp_t *)rsdp;
        }
    }
    return NULL;
}
