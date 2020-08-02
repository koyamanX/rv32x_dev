#ifndef ELFLOADER_H
#define ELFLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct memlist{
		const char *name;
		unsigned long origin;
		unsigned long length;
		unsigned char *mem;
		struct memlist *next;
		unsigned long attrs; 
	}memlist_t;

	unsigned char *create_new_mem(const size_t size);
	bfd *open_exe(const char *fname, const char *arch);
	memlist_t *init_mem_list(void);
	memlist_t *insert_new_mem(memlist_t *m, const char *name, unsigned long origin, unsigned long length, unsigned long attrs);
	memlist_t *find_mem_entry(memlist_t *m, unsigned int adrs);
	void print_mem_list(memlist_t *m); 
	unsigned int read_word(memlist_t *m, unsigned long adrs);
	unsigned short read_halfword(memlist_t *m, unsigned long adrs);
	unsigned char read_byte(memlist_t *m, unsigned long adrs);
	unsigned char *write_word(memlist_t *m, unsigned long adrs, unsigned int wdata);
	unsigned char *write_halfword(memlist_t *m, unsigned long adrs, unsigned short wdata);
	unsigned char *write_byte(memlist_t *m, unsigned long adrs, unsigned char wdata);
	unsigned char *load_section(memlist_t *m, bfd *abfd, const char *section);
	void load_elf(memlist_t *m, bfd *abfd);
	extern void finish(int stat);
#ifdef __cplusplus
}
#endif
#endif
