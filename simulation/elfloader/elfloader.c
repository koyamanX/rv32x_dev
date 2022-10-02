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
		exit(EXIT_FAILURE);
	}

	return p;
}
bfd *open_exe(const char *fname, const char *arch) {
	bfd *abfd;

	bfd_init();
	abfd = bfd_openr(fname, arch);
	if (abfd == NULL) {
		fprintf(stderr, "%s: %s\n", __func__, bfd_errmsg(bfd_get_error()));
		exit(EXIT_FAILURE);
	}
	if (!bfd_check_format(abfd, bfd_object)) {
		fprintf(stderr, "%s: ", __func__); 
		bfd_perror("bfd_check_format");
		exit(EXIT_FAILURE);
	}

	return abfd;
}
memlist_t *init_mem_list(void) {
	memlist_t *p;

	if((p = (memlist_t*) malloc(sizeof(memlist_t))) == NULL)
	{
		fprintf(stderr, "%s: failed to allocate memory", __func__);
		exit(EXIT_FAILURE);
	}
	p->name = "null";
	p->origin = 0;
	p->length = 0;
	p->mem = NULL;
	p->next = NULL;
	p->attrs = 0;

	return p;
}
memlist_t *insert_new_mem(memlist_t *m, const char *name, uint32_t origin, 
							uint32_t length, unsigned long attrs) {
	memlist_t *p, *t;

	if((p = (memlist_t *) malloc(sizeof(memlist_t))) == NULL)
	{
		fprintf(stderr, "%s: failed to allocate memory", __func__);
		exit(EXIT_FAILURE);
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
		fprintf(stderr, "%s: section: %s, start: %x, end: %x\n", __func__, p->name, p->origin, p->origin+p->length);
	}
}
void print_mem_contents(memlist_t *m) {
	unsigned int i;

	fprintf(stderr, "contents of memory %s\n", m->name);
	assert(m->mem != NULL);
	for(i = 0; i < m->length; i+=4) {
		fprintf(stderr, "%08x: %02x %02x %02x %02x\n",
			m->origin+i, 
			m->mem[i+0],
			m->mem[i+1],
			m->mem[i+2],
			m->mem[i+3]);
	}
}
memlist_t *find_mem_entry(memlist_t *m, uint32_t adrs) {
	memlist_t *t;

	for(t = m; t != NULL; t = t->next) {
		if((t->origin <= adrs && adrs < t->origin+t->length) && (t->name != "null")) {
			return t;
		}
	}

	return NULL;
}
int read_word(memlist_t *m, uint32_t adrs, uint32_t *rdata) {
	memlist_t *t;

	t = find_mem_entry(m, adrs);
	if(t == NULL) {
		return -1;
	}
	assert(t->mem != NULL);
	assert(rdata != NULL);
	*rdata = ((t->mem[adrs+3 - t->origin] << 24) | (t->mem[adrs+2 - t->origin] << 16))
				| ((t->mem[adrs+1 - t->origin] << 8) | (t->mem[adrs+0 - t->origin]));

	return 0;
}
int read_halfword(memlist_t *m, uint32_t adrs, uint16_t *rdata) {
	memlist_t *t;

	t = find_mem_entry(m, adrs);
	if(t == NULL) {
		return -1;
	}
	assert(t->mem != NULL);
	assert(rdata != NULL);
	*rdata = ((t->mem[adrs+1 - t->origin] << 8) | (t->mem[adrs+0 - t->origin]));

	return 0;
}
int read_byte(memlist_t *m, uint32_t adrs, uint8_t *rdata) {
	memlist_t *t;

	t = find_mem_entry(m, adrs);
	if(t == NULL) {
		return -1;
	}
	assert(t->mem != NULL);
	assert(rdata != NULL);
	*rdata = t->mem[adrs - t->origin];

	return 0;
}

int write_word(memlist_t *m, uint32_t adrs, uint32_t wdata) {
	int stat0, stat1;

	stat0 = write_halfword(m, adrs+2, ((wdata&0xffff0000)>>16));
	stat1 = write_halfword(m, adrs+0, (wdata&0x0000ffff));

	return (stat0 | stat1);
}
int write_halfword(memlist_t *m, uint32_t adrs, uint16_t wdata) { 
	int stat0, stat1;

	stat0 = write_byte(m, adrs+1, ((wdata&0xff00)>>8));
	stat1 = write_byte(m, adrs+0, ((wdata&0x00ff)));

	return (stat0 | stat1);
}
int write_byte(memlist_t *m, uint32_t adrs, uint8_t wdata) {
	memlist_t *t;
	t = find_mem_entry(m, adrs);
	if(t == NULL) {
		//	printf("error:addr = 0x%08x\n",adrs);
		return -1;
	}
	assert(t->mem != NULL);
	t->mem[adrs - t->origin] = wdata;

	return 0;
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

