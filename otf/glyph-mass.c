#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <err.h>


// :(
//#pragma scalar_storage_order big-endian
#define BE __attribute__((packed, scalar_storage_order("big-endian")))

typedef uint32_t U32;
typedef uint8_t U8;
typedef uint16_t U16;
typedef int16_t I16;

typedef int64_t Date64;

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
	U32 large[0];
	U16 small[0];
} Loca;

typedef struct BE {
	U16 majorVersion, minorVersion;
	Fixed32 fontRevision;
	U32 checksumAdjustment;
	U32 magicNumber;
	U16 flags;
	U16 unitsPerEm;
	Date64 created, modified;
	I16 xMin, yMin, xMax, yMax;
	U16 macStyle;
	U16 lowestRecPPEM;
	I16 fontDirectionHint;
	I16 indexToLocFormat;
	I16 glyphDataFormat;
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

typedef struct BE {
	I16 contourCount;
	I16 xMin, yMin, xMax, yMax;
} Glyph1;

typedef struct BE {
	U16 flags;
	U16 glyphIndex;
} Component1;

typedef struct {
	char* name;
	int total;
} collect;

collect collects[100];

void main(int argc, char* argv[argc]) {
	FILE* file = fopen("raw.ttf", "r");
	
	int sizes[1000] = {0};
	TableRecord loca = {0}, head = {0}, post = {0}, glyf = {0};
	
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
		else if (!memcmp(tr.tag, "glyf", 4))
			glyf = tr;
	}
	
	if (!loca.offset||!head.offset||!post.offset)
		return;
	
	fseek(file, head.offset, SEEK_SET);
	Head head2;
	fread(&head2, sizeof(Head), 1, file); // todo: other head versions?
	//printf("head sizeof: %d, length: %d\n", sizeof(Head), head.length);
	
	printf("loca fmt: %d\n", head2.indexToLocFormat);
	
	fseek(file, loca.offset, SEEK_SET);
	Loca* loca2 = malloc(loca.length);
	fread(loca2, loca.length, 1, file);
	
	int prev = -1;
	for (int i=0; i<=loca.length/2; i++) {
		int curr = loca2->small[i]*2;
		//printf("glyph %d: loca=%d\n", i, curr);
		if (prev>=0) {
			//sizes[i-1] += 2;
			int len = curr-prev;
			sizes[i-1] += len;
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
		
		//int size = loca2->small[i+1]*2 - loca2->small[i]*2;
		fseek(file, glyf.offset+loca2->small[i]*2, SEEK_SET);
		Glyph1 glyph;
		fread(&glyph, sizeof(glyph), 1, file);
		if (glyph.contourCount<0) {
			while (1) {
				Component1 comp;
				fread(&comp, sizeof(comp), 1, file);
				int len = (comp.flags&0x001) ? 4 : 2;
				if (comp.flags&0x008)
					len += 2;
				else if (comp.flags&0x040)
					len += 4;
				else if (comp.flags&0x080)
					len += 8;
				
				int ref = post3->glyphNameIndex[comp.glyphIndex];
				char* name;
				if (ref>=258)
					name = strings[ref-258];
				else
					name = "<name>";
				
				printf("/ component: %d(%s). flags: %3X\n", comp.glyphIndex, name, comp.flags);
				fseek(file, len, SEEK_CUR);
				if (!(comp.flags&0x020))
					break;
			}
		} else {
			
		}
		
		int type = (glyph.contourCount>0) + (glyph.contourCount<0)*2;
		//glyph.contourCount
		printf("%d %d+10 | glyph %d: %s\n", glyph.contourCount, sizes[i]-2*5, i, name); // -10 means it reuses the previous def
		
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
