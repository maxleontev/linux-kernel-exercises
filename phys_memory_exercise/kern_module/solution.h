#ifndef SolutionH
#define SolutionH

// чтение указанного количества байт физической памяти по заданному адресу в указанный буфер
// функция вернет количество прочианных байт (меньшее или 0 означает ошибку - выход за пределы памяти)
typedef unsigned int (*PREAD_FUNC) (void *buf, const unsigned int size, const unsigned int physical_addr);

// функция трансляции виртуального адреса в физический:
// virt_addr - виртуальный адрес подлежащий трансляции
// level - количество уровней трансляции 2 или 3 (соответствуют legacy и PAE трансляциям в x86)
// root_addr - физический адрес корня дерева трансляции (соответствует регистру CR3)
// read_func - функция для чтения физической памяти (см. объявление выше)
// функция возвращает успешность трансляции: 0 - успешно, не 0 - ошибки
// phys_addr - выходной оттранслированный физический адрес
extern int va2pa(
	const unsigned int virt_addr,
	const unsigned int level,
	const unsigned int root_addr,
	const PREAD_FUNC read_func,
	unsigned int * phys_addr
);

#endif