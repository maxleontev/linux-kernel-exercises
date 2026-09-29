#include <stdio.h>

#include <unistd.h>
#include<fcntl.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/types.h>

static unsigned char char_value = 0xAB;


typedef struct {
    uint64_t pfn : 55;
    unsigned int soft_dirty : 1;
    unsigned int file_page : 1;
    unsigned int swapped : 1;
    unsigned int present : 1;
} pagemap_entry;

int pagemap_get_entry(pagemap_entry *entry, int pagemap_fd, uintptr_t vaddr)
{
    size_t nread;
    ssize_t ret;
    uint64_t data;
    uintptr_t vpn;

    vpn = vaddr / sysconf(_SC_PAGE_SIZE);
    nread = 0;
    while (nread < sizeof(data)) {
        ret = pread(pagemap_fd, &data, sizeof(data) - nread,
                vpn * sizeof(data) + nread);
        nread += ret;
        if (ret <= 0) {
            return 1;
        }
    }
    entry->pfn = data & (((uint64_t)1 << 55) - 1);
    entry->soft_dirty = (data >> 55) & 1;
    entry->file_page = (data >> 61) & 1;
    entry->swapped = (data >> 62) & 1;
    entry->present = (data >> 63) & 1;
    return 0;
}

int virt_to_phys_user(uintptr_t *paddr, pid_t pid, uintptr_t vaddr)
{
    char pagemap_file[BUFSIZ];
    int pagemap_fd;

    snprintf(pagemap_file, sizeof(pagemap_file), "/proc/%ju/pagemap", (uintmax_t)pid);
    pagemap_fd = open(pagemap_file, O_RDONLY);
    if (pagemap_fd < 0) {
        return 1;
    }
    pagemap_entry entry;
    if (pagemap_get_entry(&entry, pagemap_fd, vaddr)) {
        return 2;
    }
    close(pagemap_fd);
    *paddr = (entry.pfn * sysconf(_SC_PAGE_SIZE)) + (vaddr % sysconf(_SC_PAGE_SIZE));
    return 0;
}

int main(void)
{
    int fd, ret, ret1;
    char * addr;
    unsigned int value_addr;
	unsigned int p_id;
    unsigned int data[2];

	uintptr_t vaddr;
	uintptr_t paddr;

    p_id = getpid();
    addr = &char_value;
    value_addr = (unsigned int) addr;

    printf("My pid:%d value_addr: 0x%0*X char_value = 0x%X\n", p_id, 8, value_addr, char_value);

	vaddr = (uintptr_t) &char_value;
	ret1 = virt_to_phys_user(&paddr, p_id, vaddr);
	if (ret1)
		printf ("virt_to_phys_user error : %d\n", ret1);
	else
		printf("paddr : 0x%X\n", paddr);


    data[0] = p_id;
    data[1] = value_addr;
    fd = open("/dev/chrdrv", O_RDWR);
    ret = write(fd, data, 2 * sizeof(unsigned int));
    if(ret == -1)
        printf("writting failed\n");
    else
        printf("writting success\n");

    sleep (1);

    close(fd);

    printf("exit\n");

    return 0;
}