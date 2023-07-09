#ifndef KLIB_TOOL_H__
#define KLIB_TOOL_H__

typedef enum {
    PRINT_FAILED,
    PRINT_INT,
    PRINT_STR,
    PRINT_CHR,
} prtSpec;

// contain type and descriptors
typedef struct prtDes
{
    prtSpec Spec;
} prtDes;


char* Int2Char(char *buf, int num);

// Decode the Descriptor and return the new pointer after decoding
const char* PrintDesDec(prtDes *p, const char *des);

// similar to strcpy but return the end of dest
char* StrCpyTool(char *dst, const char *src);

int StrCnt(char *s);

#endif