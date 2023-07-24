#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

typedef uint32_t U32;
typedef uint8_t U8;
typedef uint16_t U16;

U8 temp[100000]; //hopefully no png chunks are larger than this!

U8* read2(ssize_t n) {
	U8* dest = temp;
	while (n) {
		ssize_t amt = read(0, dest, n);
		n -= amt;
		dest += amt;
	}
	return temp;
}

void write2(U8* buffer, ssize_t n) {
	while (n) {
		ssize_t amt = write(1, buffer, n);
		n -= amt;
		buffer += amt;
	}
}

U32 read32be(void) {
	read2(4);
	U32 res = *(U32*)temp; // uh idk if this is allowed... alignment and all
	return ntohl(res);
}

void skip(ssize_t b) {
	read2(b);
}

/*typedef struct {
	Tag tag;
	uint32 checksum;
	uint32 offset;
	uint32 length;
} TableRecord;

typedef struct {
	uint32 version;
	uint16 tableCount;
	uint16 searchRange, entrySelector, rangeShift;
	TableRecord tableRecords[0];
	} TableDirectory;**/

void main(int argc, char* argv[argc]) {
	int sizes[1000];
	
	read2(4);
	read2(2);
	int count = temp[0]<<8 | temp[1];
	read2(2*3);
	int loca_offset = -1, loca_len;
	int head_offset = -1, head_len;
	int post_offset = -1, post_len;
	for (int i=0; i<count; i++) {
		read2(4);
		printf("table: %4.4s\n", temp);
		if (temp[0]=='l'&&temp[1]=='o'&&temp[2]=='c'&&temp[3]=='a') {
			read2(4);
			loca_offset = read32be();
			loca_len = read32be();
		} else if (temp[0]=='h'&&temp[1]=='e'&&temp[2]=='a'&&temp[3]=='d') {
			read2(4);
			head_offset = read32be();
			head_len = read32be();
		} else if (temp[0]=='p'&&temp[1]=='o'&&temp[2]=='s'&&temp[3]=='t') {
			read2(4);
			post_offset = read32be();
			post_len = read32be();
		} else {
			read2(4);
			read2(4);
			read2(4);
		}
	}
	if (loca_offset<0||head_offset<0)
		return;
	lseek(0, head_offset, SEEK_SET);
	read2(2+2+4+4+4+2+2+8+8+2*7);
	read2(2);
	int loca_fmt = temp[0]<<8 | temp[1];
	printf("loca fmt: %d\n", loca_fmt);
	lseek(0, loca_offset, SEEK_SET);
	int prev = -1;
	for (int i=0; i<=loca_len/2; i++) {
		read2(2);
		int curr = temp[0]<<8 | temp[1];
		if (prev>=0) {
			int len = curr-prev;
			sizes[i-1]=len;
			//			printf("glyph %d: size=%d\n", i-1, len);
		}
		prev = curr;
	}
	lseek(0, post_offset, SEEK_SET);
	int version = read32be();
	read2(4+2+2+4+4*4);
	post_len-=4+ 4+2+2+4+4*4;
	read2(2);
	post_len-=2;
	printf("at post\n");
	int nnames = temp[0]<<8 | temp[1];
	int names[1000];
	for (int i=0; i<nnames; i++) {
		post_len-=2;
		read2(2);
		int curr = temp[0]<<8 | temp[1];
		if (curr<258)
			curr = 0;
		else
			curr -= 258;
		names[i] = curr;
	}
	read2(post_len); // too many..
	char* strings[1000];
	int pos = 0;
	for (int i=0; ;i++) {
		strings[i] = &temp[pos+1];
		int len = temp[pos];
		temp[pos] = 0;
		
		if (!len)
			break;
		pos += len+1;
	}
	for (int i=0; i<nnames; i++) {
		printf("glyph %d (%s): size=%d\n", i, strings[names[i]], sizes[i]);
	}
}
