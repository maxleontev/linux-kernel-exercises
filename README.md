# Linux Kernel Exercises

**Coursework (stepic.org, 2020)** — solutions for a Linux kernel modules course.

## What this repository covers

- Loading and unloading kernel modules
- Character device drivers
- Dynamic device node creation
- `ioctl` interface
- Interrupt service routines (ISR)
- Linked lists in kernel space
- x86 virtual-to-physical address translation (page table walk)

## Branches

Each chapter lives on its own branch:

| Branch | Contents |
|--------|----------|
| [`master`](https://github.com/maxleontev/linux-kernel-exercises/tree/master) | Module 01 — basic kernel modules (load/unload, memory allocation helpers) |
| [`char_device_driver`](https://github.com/maxleontev/linux-kernel-exercises/tree/char_device_driver) | Module 02 — character device driver |
| [`dynamic_nodes_chapter`](https://github.com/maxleontev/linux-kernel-exercises/tree/dynamic_nodes_chapter) | Module 03 — dynamic device nodes (dynamic major, udev rules) |
| [`list_isr_ioctl_chapter`](https://github.com/maxleontev/linux-kernel-exercises/tree/list_isr_ioctl_chapter) | Module 04 — linked lists, ISR, `ioctl` |

## Physical memory exercise

The [`phys_memory_exercise/`](phys_memory_exercise/) directory contains a standalone training task: implement `va2pa()` — virtual-to-physical address translation for x86 (2-level / legacy and 3-level / PAE page tables), using a callback to read physical memory.

Full task statement: [`phys_memory_exercise/readme.txt`](phys_memory_exercise/readme.txt).
