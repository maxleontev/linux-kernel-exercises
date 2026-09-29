#include "solution.h"

#include <linux/highmem.h>
#include <asm/pgtable.h>


int va2pa(
	const unsigned int virt_addr,
	const unsigned int level,
	const unsigned int root_addr,
	const PREAD_FUNC read_func,
	unsigned int * phys_addr
) {
	pgd_t * pgd_base;
	int pgdir_idx;
	pgd_t * pgdir_entry_ptr;
	p4d_t * p4d_entry_ptr;
	pud_t * pud_entry_ptr;
	pmd_t * pmd_entry_ptr;
	pte_t * pte_entry_ptr;
	struct page *pt_page;
	
	unsigned int offset_in_page;
	unsigned int page_addr;

	printk( KERN_INFO "virt_addr : 0x%0*X\n", 8, virt_addr);
	printk( KERN_INFO "pgd root phys addr : 0x%0*X\n", 8, root_addr);
	printk( KERN_INFO "levels : %d\n", level);

	pgd_base = (pgd_t *) phys_to_virt(root_addr);
	printk( KERN_INFO "pgd_base : 0x%X", (unsigned int) pgd_base );

	pgdir_idx = pgd_index(virt_addr);
	pgdir_entry_ptr = pgd_base + pgdir_idx;
	if (pgd_none(*pgdir_entry_ptr))
		return -2;

	p4d_entry_ptr = p4d_offset(pgdir_entry_ptr, virt_addr);
	if (p4d_none(*p4d_entry_ptr))
		return -3;

	pud_entry_ptr = pud_offset(p4d_entry_ptr, virt_addr);
	if (pud_none(*pud_entry_ptr))
		return -4;

	pmd_entry_ptr = pmd_offset(pud_entry_ptr, virt_addr);
	if (pmd_none(*pmd_entry_ptr))
		return -5;

	pte_entry_ptr = pte_offset_map(pmd_entry_ptr, virt_addr);
	if (!pte_entry_ptr)
		return -6;

	if (pte_none(*pte_entry_ptr))
		return -7;

	if (!pte_write(*pte_entry_ptr))
		return -8;

	pt_page = pte_page(*pte_entry_ptr);
	offset_in_page = (virt_addr & ~PAGE_MASK);

	page_addr = pte_val(*pte_entry_ptr) & PAGE_MASK;
	*phys_addr = page_addr | offset_in_page;
	
	printk( KERN_INFO "pgdir_idx : %d \n", pgdir_idx );
	printk( KERN_INFO "pgdir_entry_ptr : 0x%X \n", (unsigned int) pgdir_entry_ptr );
	printk( KERN_INFO "p4d_entry_ptr : 0x%X \n", (unsigned int) p4d_entry_ptr);
	printk( KERN_INFO "pud_entry_ptr : 0x%X \n", (unsigned int) pud_entry_ptr);
	printk( KERN_INFO "pmd_entry_ptr : 0x%X \n", (unsigned int) pmd_entry_ptr);
	printk( KERN_INFO "pte_entry_ptr : 0x%X \n", (unsigned int) pte_entry_ptr);
	printk( KERN_INFO "offset_in_page : %d \n", offset_in_page );
	printk( KERN_INFO "page_addr : 0x%X \n", page_addr);
	
	return 0;
}