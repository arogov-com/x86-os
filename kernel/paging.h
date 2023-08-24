#include "kernel.h"

#define WRITABLE_PAGE       0x4
#define READABLE_PAGE       0x0
#define PAGE_SIZE_4KB       0x0
#define PAGE_SIZE_4MB       0x1
#define USER_PAGE           0x2
#define SUPERVISOR_PAGE     0x0

#define CACHE_WRITE_BACK     0x0
#define CACHE_DISABLED       0x1
#define CACHE_WRITE_THROUGH  0x2
#define CACHE_GLOBAL         0x4

#define BOUND_ERROR          (-1)
#define PDE_ALREADY_EXISTS   (-2)
#define PTE_ALREADY_EXISTS   (-3)
#define TABLE_NOT_PRESENT    (-4)

typedef struct {
    unsigned present:1;   //Entry present
    unsigned writable:1;  //Pages group writable
    unsigned user:1;      //0 - Supervisor, 1 - User
    unsigned pwt:1;       //0 - Write-back,1 - Write-through
    unsigned pcd:1;       //0 - Enable cache, 1 - Disable cache
    unsigned accessed:1;  //Accessed
    unsigned reserved:1; //Reserved set to 0
    unsigned size:1;      //0 - 4kb, 1 - 4Mb page entry
    unsigned global:1;    //1 - Global page (TBL is not clean when task switch)
    unsigned available:3;
    unsigned address:20;  //Higher 20 bits of physicly address
}page_dir_t;

typedef struct {
    unsigned present:1;   //Page present
    unsigned writable:1;  //Page writable
    unsigned user:1;      //0 - Supervisor, 1 - User
    unsigned pwt:1;       //0 - Write-back,1 - Write-through
    unsigned pcd:1;       //0 - Enable cache, 1 - Disable cache
    unsigned accessed:1;  //Accessed
    unsigned dirty:1;     //Page is written
    unsigned pat:1;       //
    unsigned global:1;    //1 - Global page (TBL is not clean when task switch)
    unsigned available:3;
    unsigned address:20;  //Higher 20 bits of physicly address
}PAGE_TABLE;

void pde_init(void);                                      //Initialize Page Directory
void get_pde_entry(int number, page_dir_t *store);           //Store PDE[number] in "&store"
void get_pte_entry(int table, int page, PAGE_TABLE *store); //Store PTE[page] in "&store"
void load_pde(void *addr);                                //Load PDE address to CR3 (DONT FORGET DISABLE INTERRUPT)
void paging_enable(int enable);                           //Enable page translation (DONT FORGET DISABLE INTERRUPT)

int create_pde(unsigned int number, int mod_if_exists, int options, int cache, void *page_table);
int create_pte(unsigned int table, unsigned int page, int mod_if_exists, int options, int cache, void *virtual);

int clear_pde(unsigned int number);
int clear_pte(unsigned int table, unsigned int page);

int assign_page(void *virtual, void *physical, int writable, int user); //Assign "virtual" address to "physical" address
void free_page(void *virtual);                                       //Free page

void *get_physical_address(void *virtual);                           //Get physical address from virtual
unsigned long get_virtual_addresses(void *physical, unsigned long *virtuals_array); //Get virtual addresses list assigned to physical address (return value count of virt. addresses)
unsigned long get_used_pages(void);                                  //Get used pages
unsigned long find_free_page_sq(unsigned long count);                //Search continuous sequence with "count" free pages
