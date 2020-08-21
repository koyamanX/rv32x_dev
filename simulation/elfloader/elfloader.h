#ifndef ELFLOADER_H
#define ELFLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct memlist{
		const char *name;
		uint32_t origin;
		uint32_t length;
		unsigned char *mem;
		struct memlist *next;
		unsigned long attrs; 
	}memlist_t;

	unsigned char *create_new_mem(const size_t size);
	bfd *open_exe(const char *fname, const char *arch);
	memlist_t *init_mem_list(void);
	memlist_t *insert_new_mem(memlist_t *m, const char *name, uint32_t origin, uint32_t length, unsigned long attrs);
	memlist_t *find_mem_entry(memlist_t *m, uint32_t adrs);
	void print_mem_list(memlist_t *m); 
	int read_word(memlist_t *m, uint32_t adrs, uint32_t *rdata);
	int read_halfword(memlist_t *m, uint32_t adrs, uint16_t *rdata);
	int  read_byte(memlist_t *m, uint32_t adrs, uint8_t *rdata);
	int write_word(memlist_t *m, uint32_t adrs, uint32_t wdata);
	int write_halfword(memlist_t *m, uint32_t adrs, uint16_t wdata);
	int write_byte(memlist_t *m, uint32_t adrs, uint8_t wdata);
	unsigned char *load_section(memlist_t *m, bfd *abfd, const char *section);
	void load_elf(memlist_t *m, bfd *abfd);
void print_mem_contents(memlist_t *m);
#ifdef __cplusplus
}
#endif
#endif
