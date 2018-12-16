#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)
#define VMEM_SIZE 0xffffffff
#define PRESENT(x) (x & 0x00000001)
#define HIGH20(x) ((x & 0xfffff000) >> 12)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

paddr_t page_translate(vaddr_t addr) {
	uint32_t dir = addr >> 22;
	uint32_t page = (addr & 0x003ff000) >> 12;
	uint32_t offset = addr & 0x00000fff;
	uint32_t page_dir_addr = HIGH20(cpu.cr3);
	uint32_t page_addr = paddr_read(page_dir_addr + 4 * dir, 4);
	assert(PRESENT(page_addr) == 1);
	uint32_t page_num = paddr_read(page_addr + 4 * page, 4);
	assert(PRESENT(page_num) == 1);
  paddr_t paddr = HIGH20(page_num) + offset;
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
	if(addr + len - 1 > VMEM_SIZE) {
		assert(0);
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
