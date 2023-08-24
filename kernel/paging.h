#include "kernel.h"

typedef struct _PAGE_DIR
 {
  unsigned present:1;
  unsigned writable:1;
  unsigned user:1;
  unsigned pwt:1;
  unsigned pcd:1;
  unsigned accessed:1;
  unsigned reserved1:1;
  unsigned size:1;
  unsigned global:1;
  unsigned reserved2:1;
  unsigned reserved3:1;
  unsigned reserved4:1;
  unsigned address:20;
 }PAGE_DIR;

typedef struct _PAGE_TABLE
 {
  unsigned present:1;
  unsigned writable:1;
  unsigned user:1;
  unsigned pwt:1;
  unsigned pcd:1;
  unsigned accessed:1;
  unsigned written:1;
  unsigned pat:1;
  unsigned global:1;
  unsigned reserved1:1;
  unsigned reserved2:1;
  unsigned reserved3:1;
  unsigned address:20;
 }PAGE_TABLE;

void pde_init(void);                                      //Initialize Page Directory
void get_pde_entry(int number,PAGE_DIR *store);           //Store PDE[number] in "&store"
void get_pte_entry(int table,int page,PAGE_TABLE *store); //Store PTE[page] in "&store"
void load_pde(void *addr);                                //Load PDE address to CR3
void paging_enable(int enable);                           //Enable page translation

int assign_page(void *virtual,void *physical,int writable,int user); //Assign "virtual" address to "physical" address
void free_page(void *virtual);                                       //Free page

void *get_physical_address(void *virtual);                           //Get physical address from virtual
unsigned long get_virtual_addresses(void *physical,unsigned long *virtuals_array); //Get virtual addresses list assigned to physical address (return value count of virt. addresses)
unsigned long get_used_pages(void);                                  //Get used pages
unsigned long find_free_page_sq(unsigned long count);                //Search continuous sequence with "count" free pages
