#define RSDP_SIGNATURE_P2 0x20525450 // "PTR "
#define RSDP_SIGNATURE_P1 0x20445352 // "RSD "
#define RSDP_ALIGN        16
#define RSDP_ADDR_START   0xE0000
#define RSDP_ADDR_END     0xFFFFF

#define DT_TYPE_APIC 0x43495041  // Multiple APIC Description Table (MADT)
#define DT_TYPE_BERT 0x54524542  // Boot Error Record Table (BERT)
#define DT_TYPE_CPEP 0x50455043  // Corrected Platform Error Polling Table (CPEP)
#define DT_TYPE_DSDT 0x54445344  // Differentiated System Description Table (DSDT)
#define DT_TYPE_ECDT 0x54444345  // Embedded Controller Boot Resources Table (ECDT)
#define DT_TYPE_EINJ 0x4a4e4945  // Error Injection Table (EINJ)
#define DT_TYPE_ERST 0x54535245  // Error Record Serialization Table (ERST)
#define DT_TYPE_FACP 0x50434146  // Fixed ACPI Description Table (FADT)
#define DT_TYPE_FACS 0x53434146  // Firmware ACPI Control Structure (FACS)
#define DT_TYPE_HEST 0x54534548  // Hardware Error Source Table (HEST)
#define DT_TYPE_MSCT 0x5443534d  // Maximum System Characteristics Table (MSCT)
#define DT_TYPE_MPST 0x5453504d  // Memory Power State Table (MPST)
#define DT_TYPE_OEMx 0x784d454f  // OEM Specific Information Tables (Any table with a signature beginning with "OEM" falls into this definition)
#define DT_TYPE_PMTT 0x54544d50  // Platform Memory Topology Table (PMTT)
#define DT_TYPE_PSDT 0x54445350  // Persistent System Description Table (PSDT)
#define DT_TYPE_RASF 0x46534152  // ACPI RAS Feature Table (RASF)
#define DT_TYPE_RSDT 0x54445352  // Root System Description Table (This wiki page; included for completeness)
#define DT_TYPE_SBST 0x54534253  // Smart Battery Specification Table (SBST)
#define DT_TYPE_SLIT 0x54494c53  // System Locality System Information Table (SLIT)
#define DT_TYPE_SRAT 0x54415253  // System Resource Affinity Table (SRAT)
#define DT_TYPE_SSDT 0x54445353  // Secondary System Description Table (SSDT)
#define DT_TYPE_XSDT 0x54445358  // Extended System Description Table (XSDT; 64-bit version of the RSDT)

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
    uint32_t signature;
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
    uint32_t ptr[];
} __attribute__ ((packed)) rsdt_t;

typedef struct {
    acpi_sdt_header_t header;
    uint64_t ptr[];
} __attribute__ ((packed)) xsdt_t;


rsdp_t *acpi_find_rsdp(void *range_start, void *range_end);
int acpi_chk_summ(void *addr, int size);
int acpi_process_rsdt(rsdt_t *rsdt);
