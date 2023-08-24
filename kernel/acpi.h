#define RSDP_SIGNATURE_P2 0x20525450 // "PTR "
#define RSDP_SIGNATURE_P1 0x20445352 // "RSD "
#define RSDP_ALIGN        16
#define RSDP_ADDR_START   0xE0000
#define RSDP_ADDR_END     0xFFFFF

typedef struct { // Root System Description Pointer
    char signature[8];  // 8-byte string (not null terminated!) must contain "RSD PTR ". It stands on a 16-byte boundary.
    uint8_t checksum;
    char oemid[6];
    uint8_t version;
    uint32_t rsdt_ptr;
} __attribute__ ((packed)) rsdp_t;

typedef struct {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t version;
    uint32_t rsdt_ptr;      // deprecated since version 2.0
    uint32_t length;
    uint64_t xsdt_ptr;
    uint8_t extendedchecksum;
    uint8_t reserved[3];
} __attribute__ ((packed)) xsdp_t;

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t version;
    uint8_t checksum;
    char oemid[6];
    char oemtableid[8];
    uint32_t oemversion;
    uint32_t creatorid;
    uint32_t creatorversion;
} __attribute__ ((packed)) acpi_sdt_header_t;

typedef struct {
    acpi_sdt_header_t header;
    uint32_t ptr;
} __attribute__ ((packed)) rsdt_t;

typedef struct {
    acpi_sdt_header_t header;
    uint64_t ptr;
} __attribute__ ((packed)) xsdt_t;


rsdp_t *find_rsdp(void *range_start, void *range_end);
