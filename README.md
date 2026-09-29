# Linux Kernel Exercises

**Coursework (stepic.org, 2020)** — solutions for a Linux kernel modules course.

## What this repository covers

- Loading and unloading kernel modules
- Character device drivers
- Dynamic device node creation
- `ioctl` interface
- Interrupt service routines (ISR)
- Linked lists in kernel space

## Branches

Each chapter lives on its own branch:

| Branch | Contents |
|--------|----------|
| [`master`](https://github.com/maxleontev/linux-kernel-exercises/tree/master) | Module 01 — basic kernel modules (load/unload, memory allocation helpers) |
| [`char_device_driver`](https://github.com/maxleontev/linux-kernel-exercises/tree/char_device_driver) | Module 02 — character device driver |
| [`dynamic_nodes_chapter`](https://github.com/maxleontev/linux-kernel-exercises/tree/dynamic_nodes_chapter) | Module 03 — dynamic device nodes (dynamic major, udev rules) |
| [`list_isr_ioctl_chapter`](https://github.com/maxleontev/linux-kernel-exercises/tree/list_isr_ioctl_chapter) | Module 04 — linked lists, ISR, `ioctl` |

