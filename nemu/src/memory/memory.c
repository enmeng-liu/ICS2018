#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)
#define VMEM_SIZE 0xffffffff
#define PRESENT(x) (x & 0x00000001)
#define HIGH20(x) ((x & 0xfffff000) >> 12)
#define PG 0x80000000
#define PGSIZE 0x1000

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

paddr_t page_translate(vaddr_t addr) {
	CR0 pt_cr0;
	pt_cr0.val = cpu.cr0;
	if(pt_cr0.paging == 0) return addr;

	CR3 pt_cr3;
	pt_cr3.val = cpu.cr3;
	//Log("CR3 = 0x%x", pt_cr3.val);
	uint32_t pt_dir_base = pt_cr3.page_directory_base << 12;
	//Log("pt_dir_base is at 0x%x",pt_dir_base);
	
	PDE pt_pde;
	uint32_t dir = addr >> 22;
	pt_pde.val = paddr_read(pt_dir_base + 4 * dir , 4);
	//Log("pt_pde is at 0x%x", pt_dir_base + 4 * dir);
	//Log("PDE.val = 0x%x", pt_pde.val);
	Assert(pt_pde.present, "invalid PDE value when reading 0x%x", addr);

	PTE pt_pte;
	uint32_t page = (addr & 0x003ff000) >> 12;
	pt_pte.val = paddr_read((pt_pde.page_frame << 12) + 4 * page, 4);
	Assert(pt_pte.present, "invalid PTE value when reading 0x%x", addr);

	uint32_t offset = addr & 0x00000fff;
	paddr_t paddr = (pt_pte.page_frame << 12) + offset;
	//Log("paddr = 0x%x", paddr);
	return paddr;
}

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
	//int mm = is_mmio(addr);
	static int mm;
	mm = is_mmio(addr);
	if(mm == -1)
  	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
	else return mmio_read(addr, len, mm);
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
	//int mm = is_mmio(addr);
	static int mm;
	mm = is_mmio(addr);
	if(mm == -1)
  	memcpy(guest_to_host(addr), &data, len);
	else  mmio_write(addr, len, data, mm);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
	if((addr & 0xfff) + len > PGSIZE) {
		//assert(0);
		//Log("vaddr=0x%x, len=%d", addr, len);
		int this_page_len = PGSIZE - (addr & 0xfff);
		paddr_t paddr = page_translate(addr);
		uint32_t this_page = paddr_read(paddr, this_page_len);
		paddr = page_translate(addr + this_page_len);
		int next_page_len = len - this_page_len;
		uint32_t next_page = paddr_read(paddr, next_page_len);
		uint32_t real_data = (next_page << (this_page_len * 8)) + this_page;
		return real_data;
	}// data cross the page boundary
	else {
		paddr_t paddr = page_translate(addr);
  	return paddr_read(paddr, len);
	}
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
	if(addr + len - 1 > VMEM_SIZE) {
		assert(0);
	}
	else {
		paddr_t paddr = page_translate(addr);			
 	  paddr_write(paddr, data, len);
	}
}
