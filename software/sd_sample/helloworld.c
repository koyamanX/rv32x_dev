#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SD_BASE 0x10001000
#define SD_STATUS ((volatile unsigned int *) (SD_BASE + 0x200))
#define SD_ADRS ((volatile unsigned int *) (SD_BASE + 0x214))
#define SD_OP ((volatile unsigned int *) (SD_BASE + 0x210))
#define SD_DATA_BASE ((volatile unsigned int *) (SD_BASE + 0x0))

typedef unsigned long long int uint64;
typedef unsigned int uint;
typedef unsigned char uchar;

struct superblock {
#define FSMAGIC 0x10203040
	uint magic;
	uint size;
	uint nblocks;
	uint ninodes;
	uint nlog;
	uint logstart;
	uint inodestart;
	uint bmapstart;
};

struct buf {
	int valid;
	int disk;
	uint dev;
	uint blockno;
	//struct sleeplock lock;
	uint refcnt;
	struct buf *prev;
	struct buf *next;
	struct buf *qnext;
#define BSIZE 1024
	uchar data[BSIZE];
};

struct superblock sb;

void disk_init(void) {
	if((*SD_STATUS) == 0x0) {
		printf("waiting for disk to finish init\n");
	}
	while((*SD_STATUS) != 0x3) {
		asm volatile("nop");
	}
	if((*SD_STATUS) == 0x3) {
		printf("disk is inited & in idle\n");
	}
}
void panic(char *s) {
	fprintf(stderr, "panic: %s\n", s);
	exit(1);
}
void disk_read(struct buf *b) {
	uint64 sector = b->blockno * (BSIZE / 512);
	printf("b->blockno:%x, BSIZE:%x, sector:%llx\n", b->blockno, BSIZE, sector);

	*SD_ADRS = sector;
	*SD_OP = 0x00000001;

	__sync_synchronize();

	while((*SD_OP != 0x0) || (*SD_STATUS != 0x3)) {
		asm volatile("nop");
	}

	for(int i = 0; i < BSIZE/2; i+=4) {
		union {
			unsigned int v;
			unsigned char x[4];
		} data;

		data.v = SD_DATA_BASE[127-(i>>2)];

		b->data[i+0] = data.x[2];
		b->data[i+1] = data.x[3];
		b->data[i+2] = data.x[0];
		b->data[i+3] = data.x[1];
	}

	*SD_ADRS = sector+1;
	*SD_OP = 0x00000001;

	__sync_synchronize();

	while((*SD_OP != 0x0) || (*SD_STATUS != 0x3)) {
		asm volatile("nop");
	}

	for(int i = 0; i < BSIZE/2; i+=4) {
		union {
			unsigned int v;
			unsigned char x[4];
		} data;

		data.v = SD_DATA_BASE[127-(i>>2)];

		b->data[i+128+0] = data.x[2];
		b->data[i+128+1] = data.x[3];
		b->data[i+128+2] = data.x[0];
		b->data[i+128+3] = data.x[1];
	}

	/*
	for(int i = 0; i < BSIZE; i++) {
		printf("%02x\n", b->data[i]);
	}
	*/
}
struct buf *bget(uint dev, uint blockno) {
	struct buf *b = malloc(sizeof(struct buf));

	b->dev = dev;
	b->blockno = blockno;
}
struct buf *bread(uint dev, uint blockno) {
	struct buf *b;

	b = bget(dev, blockno);
	if(!b->valid) {
		//virtio_disk_rw(b, 0);
		disk_read(b);
		b->valid = 1;
	}
	return b;
}
void readsb(int dev, struct superblock *sb) {
	struct buf *bp;

	bp = bread(dev, 1);
	memmove(sb, bp->data, sizeof(*sb));
	//brelse(bp);
}
void fsinit(int dev) {
	readsb(dev, &sb);
	printf("sb.magic:%x, FSMAGIC:%x\n", sb.magic, FSMAGIC);
	if(sb.magic != FSMAGIC)
		panic("invalid file system");
	else
		printf("file system recognized!\n");
	//initlog(dev, &sb);
}


int main(void) {
	disk_init();
	fsinit(0);
}

