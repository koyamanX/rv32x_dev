#define PACKAGE "bfd"
#include <bfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "elfloader.h"

unsigned char *create_new_mem(const size_t size) {
	unsigned char *p;

	p = malloc(size);
	if(!p) {
		fprintf(stderr, "%s: failed to allocate memory", __func__);
		finish(EXIT_FAILURE);
	}
	return p;
}
bfd *open_exe(const char *fname, const char *arch) {
	bfd *abfd;

	bfd_init();
	abfd = bfd_openr(fname, arch);
	if (abfd == NULL) {
		fprintf(stderr, "%s\n", bfd_errmsg(bfd_get_error()));
		finish(EXIT_FAILURE);
	}
	if (!bfd_check_format(abfd, bfd_object)) {
		bfd_perror("bfd_check_format");
		finish(EXIT_FAILURE);
	}
	return abfd;
}
memlist_t *init_mem_list(void) {
	memlist_t *p;

	if((p = (memlist_t*) malloc(sizeof(memlist_t))) == NULL)
	{
		fprintf(stderr, "failed to allocate memoroy for new entry\n");
		finish(EXIT_FAILURE);
	}
	p->name = NULL;
	p->origin = 0;
	p->length = 0;
	p->mem = NULL;
	p->next = NULL;
	p->attrs = 0;
	return p;
}
memlist_t *insert_new_mem(memlist_t *m, const char *name, unsigned long origin, 
							unsigned long length, unsigned long attrs) {
	memlist_t *p, *t;

	if((p = (memlist_t *) malloc(sizeof(memlist_t))) == NULL)
	{
		fprintf(stderr, "failed to allocate memoroy for new entry\n");
		finish(EXIT_FAILURE);
	}

	p->name = name;
	p->origin = origin;
	p->length = length;
	p->mem = create_new_mem(p->length);
	p->next = NULL;
	p->attrs = attrs;
	for(t = m; t->next != NULL; t = t->next)
		;
	t->next = p;
	return p;
}
void print_mem_list(memlist_t *m) {
	memlist_t *p;
	unsigned int i;

	for(p = m; p != NULL; p = p->next) {
		fprintf(stderr, "%s: section: %s, start: %lx, end: %lx\n", __func__, p->name, p->origin, p->origin+p->length);
	}
}
memlist_t *find_mem_entry(memlist_t *m, unsigned int adrs) {
	memlist_t *t;

	for(t = m; t != NULL; t = t->next) {
		if(t->origin <= adrs && adrs < t->origin+t->length) {
			return t;
		}
	}
	return NULL;
}
unsigned int read_word(memlist_t *m, unsigned long adrs) {
	return ((read_halfword(m, adrs+2)<<16)|(read_halfword(m, adrs+0)));
}
unsigned short read_halfword(memlist_t *m, unsigned long adrs) {
	return ((read_byte(m, adrs+1)<<8) | (read_byte(m, adrs+0)));
}
unsigned char read_byte(memlist_t *m, unsigned long adrs) {
	memlist_t *t;
	t = find_mem_entry(m, adrs);
	if(t == NULL) {
		fprintf(stderr, "memory violation occurs at %lx\n", adrs);
		finish(EXIT_FAILURE);
	}
	assert(t->mem != NULL);
	return t->mem[adrs - t->origin];
}
unsigned char *write_word(memlist_t *m, unsigned long adrs, unsigned int wdata) {
	write_halfword(m, adrs+2, ((wdata&0xffff0000)>>16));
	return write_halfword(m, adrs+0, (wdata&0x0000ffff));
}
unsigned char *write_halfword(memlist_t *m, unsigned long adrs, unsigned short wdata) { 
	write_byte(m, adrs+1, ((wdata&0xff00)>>8));
	return write_byte(m, adrs+0, ((wdata&0x00ff)));
}
unsigned char *write_byte(memlist_t *m, unsigned long adrs, unsigned char wdata) {
	memlist_t *t;
	t = find_mem_entry(m, adrs);
	if(t == NULL) {
		fprintf(stderr, "memory violation occurs at %lx\n", adrs);
		finish(EXIT_FAILURE);
	}
	assert(t->mem != NULL);
	t->mem[adrs - t->origin] = wdata;
	return &t->mem[adrs - t->origin];
} 
unsigned char *load_section(memlist_t *m, bfd *abfd, const char *section) { 
	memlist_t *e; 
	asection *s; 

	s = bfd_get_section_by_name(abfd, section);
	assert(s != NULL);
	e = find_mem_entry(m, s->vma);
	assert(e != NULL);
	assert(e->mem != NULL);
	bfd_get_section_contents(abfd, s, (e->mem+(s->vma - e->origin)), 0, s->size);
	return e->mem;
}
void load_elf(memlist_t *m, bfd *abfd) {
	asection *s = abfd->sections;
	memlist_t *found = NULL;
	
	do {
		found = find_mem_entry(m, s->vma);
		if(s->flags & SEC_ALLOC) {
			if(!found) {
				insert_new_mem(m, s->name, s->vma, s->size+1024, 0);
			} else {
				if(found->length < s->size) {
					fprintf(stderr, "%s: cannot load ELF, Not enough space in memory(%s)", __func__, found->name);
					exit(1);
				}
			}
		}
		if(s->flags & SEC_LOAD) {
			load_section(m, abfd, s->name);
		}
	} while((s = s->next) != NULL);
}

