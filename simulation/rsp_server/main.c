#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define BUFMAX BUFSIZ

typedef struct buf {
	unsigned char buf[BUFMAX];
	unsigned long len;
}buf_t;

struct rsp {
	int client_fd;	
	buf_t buf;
}rsp;

typedef struct memlist{
	const char *name;
	unsigned long origin;
	unsigned long length;
	unsigned char *mem;
	struct memlist *next;
	unsigned long attrs; 
}memlist_t;

unsigned char *create_new_mem(const size_t size);
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
unsigned int swap32(unsigned int val);

int init_rsp(void);
int connect_rsp_client(int port);
int disconnect_rsp_client(void);
buf_t *get_rsp_packet(void);
int get_rsp_char(void);
int put_rsp_char(int ch);
int put_rsp_packet(char *str);
int ascii_to_hex(int ch);

int main(int argc, char **argv) {
	unsigned char c;	
	int i;
	char response[BUFMAX];
	memlist_t *m;

	init_rsp();
	m = init_mem_list();
	connect_rsp_client(26000);

	while(1) {
		get_rsp_packet();

		printf("%s\n", rsp.buf.buf);
		if(strncmp("qSupported", rsp.buf.buf, strlen("qSupported")) == 0) {
			sprintf(response, "PacketSize=%d", BUFMAX);
			put_rsp_packet(response);
		} else if(strncmp("v", rsp.buf.buf, strlen("v")) == 0) {
			/* vMustReplyEmpty */
			put_rsp_packet("");
		} else if(strncmp("H", rsp.buf.buf, strlen("H")) == 0) {
			/* I don't know but it's */
			put_rsp_packet("OK");
		} else if(strncmp("qTStatus", rsp.buf.buf, strlen("qTStatus")) == 0) {
			put_rsp_packet("");
		} else if(strncmp("?", rsp.buf.buf, strlen("?")) == 0) {
			char *reason_target_halted = "S00";
			put_rsp_packet(reason_target_halted);
		} else if((strncmp("qfThreadInfo", rsp.buf.buf, strlen("qfThreadInfo")) == 0)
				|| (strncmp("qsThreadInfo", rsp.buf.buf, strlen("qsThreadInfo")) == 0)) {
			put_rsp_packet("l");
		} else if(strncmp("qC", rsp.buf.buf, strlen("qC")) == 0) {
			put_rsp_packet("QC0");
		} else if(strncmp("qAttached", rsp.buf.buf, strlen("qAttached")) == 0) {
			put_rsp_packet("1");
		} else if(strncmp("g", rsp.buf.buf, strlen("g")) == 0) {
				/* respond with value inside registers in hex (64bits per reg) */
				/* it must be supplied from simulator, but for now, we use constant zero to all regs for test purpose */
				char *regs = "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
			put_rsp_packet(regs);
		} else if(strncmp("p", rsp.buf.buf, strlen("p")) == 0) {
			unsigned int reg;
			sscanf(rsp.buf.buf+1, "%x", &reg);
			/* PC (reg==0x20) */
			/* it must be supplied from simulator, but for now, we use constant 0x80000000 to PC for test purpose */
			put_rsp_packet("0000008000000000");
		} else if(strncmp("P", rsp.buf.buf, strlen("P")) == 0) {
			unsigned int reg;
			sscanf(rsp.buf.buf+1, "%x", &reg);
			/* PC (reg==0x20) */
			/* it must be supplied to simulator */
			/* write PC */
			put_rsp_packet("OK");

		} else if(strncmp("X", rsp.buf.buf, strlen("X")) == 0) {
			unsigned long long origin;
			unsigned long long size;
			unsigned char *binary;

			binary = NULL;
			origin = size = 0;
			sscanf(rsp.buf.buf+1, "%llx,%llx", &origin, &size);
			binary = strchr(rsp.buf.buf+1, ':');
			assert(binary != NULL);
			binary++;
			if(size != 0)
				/* It will create memory area dynamically depending on starting address and size, then copy data to that area */
				memcpy(insert_new_mem(m, NULL, origin, size, 0)->mem, binary, size);
			put_rsp_packet("OK");
		} else if(strncmp("m", rsp.buf.buf, strlen("m")) == 0) {
			unsigned long long origin;
			unsigned long long size;
			unsigned int data;
			char inst[32];

			sscanf(rsp.buf.buf+1, "%llx,%llx", &origin, &size);
			data = read_word(m, origin);
			data = swap32(data);
			sprintf(inst, "%08x", data);
			
			put_rsp_packet(inst);
		}
	}
	printf("%s\n", rsp.buf.buf);

	return disconnect_rsp_client();
}
unsigned int swap32(unsigned int val) {
	unsigned int ret;
	ret = val << 24;
	ret |= (val&0x0000ff00) << 8;
	ret |= (val&0x00ff0000) >> 8;
	ret |= val >> 24;
	return ret;
}
int connect_rsp_client(int port) {
	int s;
	int optval;
	socklen_t len;
	struct sockaddr_in sock_addr;

	/* TCP/IPv4 */
	s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(s < 0) {
		fprintf(stderr, "Error in (%s): Cannot open RSP socket\n", __func__);
		return EXIT_FAILURE;
	}
	optval = 1;
	//setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
	sock_addr.sin_family = PF_INET;
	sock_addr.sin_port = htons(port);
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(s, (struct sockaddr *)&sock_addr, sizeof(sock_addr))) {
		fprintf(stderr, "Error(%s): Cannot bind RSP socket\n", __func__);
		return EXIT_FAILURE;
	}
	if(listen(s, 1)) {
		fprintf(stderr, "Error(%s): Cannot listen RSP socket\n", __func__);
		return EXIT_FAILURE;
	}
	printf("Listening for RSP on port %d\n", port);
	fflush(stdout);

	len = sizeof(socklen_t);
	rsp.client_fd = accept(s, (struct sockaddr *)&sock_addr, &len);

	if(rsp.client_fd == -1) {
		fprintf(stderr, "Warning: Failed to accept RSP rsp.client_fd\n");
		return EXIT_FAILURE;
	}

	optval = 1;
	//setsockopt(rsp.client_fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&optval, sizeof(optval));
	optval = 1;
	//setsockopt(rsp.client_fd, IPPROTO_TCP, TCP_NODELAY, (char *)&optval, sizeof(optval));
	close(s);
	signal(SIGPIPE, SIG_IGN);

	return rsp.client_fd;
}
int disconnect_rsp_client(void) {
	if(rsp.client_fd != -1) 
		close(rsp.client_fd);
	rsp.client_fd = -1;

	return EXIT_SUCCESS;
}
int init_rsp(void) {
	rsp.client_fd = -1;
	rsp.buf.len = 0;
	memset(rsp.buf.buf, 0, BUFMAX);

	return EXIT_SUCCESS;
}
buf_t *get_rsp_packet(void) {
	int ch;
	int i = 0;
	unsigned char sum = 0; /* might overflow (modulo 256), assume unsigned char is 8 bits long */
	unsigned char checksum = 0;

	memset(rsp.buf.buf, 0, BUFMAX);

	ch = get_rsp_char();		
	while(ch != '$') {
		ch = get_rsp_char();		
	}
	ch = get_rsp_char();		
	while(ch != '#') {
		rsp.buf.buf[i] = ch;
		if(i >= BUFMAX-1) {
			/* exceed buffer size */
			break;
		}
		sum += (unsigned char)ch;
		ch = get_rsp_char();		
		i++;
	}
	rsp.buf.len = i;

	ch = get_rsp_char();		
	checksum = ascii_to_hex(ch) << 4;
	ch = get_rsp_char();		
	checksum += ascii_to_hex(ch);

	if(sum == checksum) {
		put_rsp_char('+');
	} else {
		puts("incorrect checksum");
		printf("sum %x, checksum %x\n", sum, checksum);
		put_rsp_char('-');
		disconnect_rsp_client();
		exit(EXIT_SUCCESS);
	}
	return &rsp.buf;
}
int ascii_to_hex(int ch) {
	if('0' <= ch && ch <= '9') {
		return ch - '0';
	} else if('a' <= ch && ch <= 'f') {
		return ch - 'a' + 10;
	} else if('A' <= ch && ch <= 'F') {
		return ch - 'A' + 10;
	} else {
		return -1;
	}
}
int get_rsp_char(void) {
	int nr;
	unsigned char ch;

	nr = read(rsp.client_fd, &ch, sizeof(char));
	if(nr < 0) {
		fprintf(stderr, "Error %s: failed to read character from socket\n", __func__);
		exit(EXIT_FAILURE);
	} else if(nr == 0) {
		/* EOF */
		disconnect_rsp_client();
		exit(EXIT_SUCCESS);
	}
	return (unsigned int )ch;
}
int put_rsp_char(int ch) {
	int nw;

	nw = write(rsp.client_fd, &ch, sizeof(char));
	if(nw < 0) {
		fprintf(stderr, "Error %s: failed to write character to socket\n", __func__);
		exit(EXIT_FAILURE);
	} else if(nw == 0) {
		/* EOF */
		disconnect_rsp_client();
		exit(EXIT_SUCCESS);
	}
	return nw;
}
int put_rsp_packet(char *str) {
	unsigned char sum = 0;
	int ch;
	int i;

	put_rsp_char('$');
	for(i = 0; i < strlen(str); i++) {
		ch = str[i];

		if((ch == '$') || (ch == '#') || (ch == '}') || (ch == '*')) {
			ch = ch ^ 0x20;			
			sum += '}';
			put_rsp_char('}');
		}
		sum += ch;
		put_rsp_char(ch);
	}
	put_rsp_char('#');
	put_rsp_char("0123456789abcdef"[sum>>4]);
	put_rsp_char("0123456789abcdef"[sum%16]);

	ch = get_rsp_char();
	while(ch != '+') 
		;
	return 0;
}

unsigned char *create_new_mem(const size_t size) {
	unsigned char *p;

	p = malloc(size);
	if(!p) {
		fprintf(stderr, "%s: failed to allocate memory", __func__);
		exit(EXIT_FAILURE);
	}
	return p;
}
memlist_t *init_mem_list(void) {
	memlist_t *p;

	if((p = (memlist_t*) malloc(sizeof(memlist_t))) == NULL)
	{
		fprintf(stderr, "failed to allocate memoroy for new entry\n");
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
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
		exit(EXIT_FAILURE);
	}
	assert(t->mem != NULL);
	t->mem[adrs - t->origin] = wdata;
	return &t->mem[adrs - t->origin];
}

