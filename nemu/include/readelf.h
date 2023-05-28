#ifndef READELF_H__
#define READELF_H__

#include <elf.h>

typedef struct
{
    /* data */
    int cnt;
    Elf64_Sym elf_sym;
} symStr;

extern Elf64_Sym *elf_func_syms;
extern int elf_func_cnt;

int ReaderSetElf(char *file);
void ReaderSetCursor(long int offset, long int base);
Elf64_Ehdr* ReaderGetHeader();
void ReaderInit(char *file);
int ReaderGetSH_Num();
Elf64_Shdr* ReaderGetSH(int idx);
char* ReaderSymbolGetName(Elf64_Sym *sym);
symStr* ReaderSymbolStart();
symStr* ReaderSymbolNext(symStr *sym);
int ReaderSymbolIsEnd(symStr *sym);
void ReaderDeInit();

#define FOREACH_ELFSYM(p) for (symStr* p = ReaderSymbolStart(); \
!ReaderSymbolIsEnd(p); p = ReaderSymbolNext(p))

#endif