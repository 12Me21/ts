#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <err.h>

// :(
//#pragma scalar_storage_order big-endian
#define BE __attribute__((packed, scalar_storage_order("big-endian")))

typedef uint32_t U32;
typedef uint8_t U8;
typedef uint16_t U16;
typedef int16_t I16;

typedef uint64_t Date64;

typedef uint32_t Fixed32; // todo?
typedef uint32_t Version32; // todo

typedef char Tag[4];

typedef struct {
	U32 offset, length;
} Range; 

typedef struct BE {
	Tag tag;
	U32 checksum;
	U32 offset, length;
} TableRecord;

typedef struct BE {
	U32 version;
	U16 tableCount;
	struct { U16 searchRange, entrySelector, rangeShift; };
	//TableRecord tableRecords[0];
} TableDirectory;

typedef union BE {
	U16 small[0];
	U32 large[0];
} Loca;

typedef union BE {
	U16 majorVersion, minorVersion;
	Fixed32 fontRevision;
	U32 checksumAdjustment;
	U32 magicNumber;
	U16 flags;
	U16 unitsPerEm;
	Date64 created, modified;
	U16 xMin, yMin, xMax, yMax;
	U16 macStyle;
	U16 lowestRecPPEM;
	U16 fontDirectionHint;
	U16 indexToLocFormat;
	U16 glyphDataFormat;
} Head;

typedef struct BE {
	Version32 version;
	Fixed32 italicAngle;
	I16 underlinePosition, underlineThickness;
	U32 isFixedPitch;
	U32 minMemType42, maxMemType42, minMemType1, maxMemType1;
} Post;

typedef struct BE {
	U16 glyphCount;
	U16 glyphNameIndex[0];
} Post2;

typedef struct {
	char* name;
	int total;
} collect;

collect collects[100];

void main(int argc, char* argv[argc]) {
	FILE* file = fopen("raw.ttf", "r");
	
	int sizes[1000] = {0};
	TableRecord loca = {0}, head = {0}, post = {0};
	
	TableDirectory td;
	fread(&td, sizeof(td), 1, file);
	for (int i=0; i<td.tableCount; i++) {
		TableRecord tr;
		fread(&tr, sizeof(tr), 1, file);
		printf("table: %4.4s\n", tr.tag);
		if (!memcmp(tr.tag, "loca", 4))
			loca = tr;
		else if (!memcmp(tr.tag, "head", 4))
			head = tr;
		else if (!memcmp(tr.tag, "post", 4))
			post = tr;
	}
	
	if (!loca.offset||!head.offset||!post.offset)
		return;
	
	fseek(file, head.offset, SEEK_SET);
	Head head2;
	fread(&head2, sizeof(head2), 1, file); // todo: other head versions?
	
	printf("loca fmt: %d\n", head2.indexToLocFormat);
	
	fseek(file, loca.offset, SEEK_SET);
	Loca* loca2 = malloc(loca.length);
	fread(loca2, loca.length, 1, file);
	
	int prev = -1;
	for (int i=0; i<=loca.length/2; i++) {
		int curr = loca2->small[i];
		if (prev>=0) {
			sizes[i-1] += 2;
			int len = curr-prev;
			sizes[i-1] += len;
			//printf("glyph %d: size=%d\n", i-1, len);
		}
		prev = curr;
	}
	
	fseek(file, post.offset, SEEK_SET);// && err(1, "fseek...");
	Post post2;
	fread(&post2, sizeof(post2), 1, file);
	
	printf("version: %X\n", post2.version);
	
	Post2* post3 = malloc(post.length - sizeof(post2));
	fread(post3, sizeof(Post2), 1, file);
	fread(&post3->glyphNameIndex, sizeof(U16), post3->glyphCount, file);
	
	char* strings[1000];
	for (int i=0; ; i++) {
		U8 len;
		fread(&len, 1, 1, file);
		if (!len)
			break;
		strings[i] = malloc(len+1);
		fread(strings[i], 1, len, file);
		strings[i][len] = '\0';
		//printf("%s\n", strings[i]);
	}
	
	for (int i=0; i<post3->glyphCount; i++) {
		int curr = post3->glyphNameIndex[i];
		char* name;
		if (curr>=258)
			name = strings[curr-258];
		else
			name = "<name>";
		printf("%d | glyph %d: %s\n", sizes[i], i, name);
	}
	
	/*
	int names[1000];
	for (int i=0; i<post3.glyphCount; i++) {
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
		char* name = strings[names[i]];
		int size = sizes[i];
		printf("%d : glyph %d (%s)\n", size, i, name);
	}
	*/
}
