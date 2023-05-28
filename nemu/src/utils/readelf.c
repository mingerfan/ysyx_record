#include <stdio.h>
#include <readelf.h>
#include <stdlib.h>
#include <string.h>

static FILE *fp = NULL;
static Elf64_Ehdr *elf_header = NULL;
static char *strtab_list = NULL;
static Elf64_Shdr *elf_shsymbol = NULL;
Elf64_Sym *elf_func_syms = NULL;
int elf_func_cnt = 0;

static void FindContext();
static void FindFuncs();

int ReaderSetElf(char *file)
{
    fp = fopen(file, "rb");
    if (fp == NULL) { printf("Warning! fp is NULL\n"); exit(-1); }
    return fp!=NULL;
}

void ReaderSetCursor(long int offset, long int base)
{
    fseek(fp, offset+base, SEEK_SET);
}

Elf64_Ehdr* ReaderGetHeader()
{
    Elf64_Ehdr *p = malloc(sizeof(Elf64_Ehdr));
    ReaderSetCursor(0, 0);
    if (fread(p, sizeof(Elf64_Ehdr), 1, fp) != 1) {
        printf("Warning! Can not get header\n");
    }
    return p;
}

void ReaderInit(char *file)
{
    ReaderSetElf(file);
    elf_header = ReaderGetHeader();
    FindContext();
    FindFuncs();
}

int ReaderGetSH_Num()
{
    return (int)(unsigned int)elf_header->e_shnum;
}

Elf64_Shdr* ReaderGetSH(int idx)
{
    Elf64_Shdr *p = malloc(sizeof(Elf64_Shdr));
    ReaderSetCursor(idx*elf_header->e_shentsize, elf_header->e_shoff);
    if (fread(p, sizeof(Elf64_Shdr), 1, fp) != 1) {
        printf("Warning! Can not read section header!\n");
    }
    return p;
}

static void FindContext()
{
    Elf64_Shdr *p = malloc(sizeof(Elf64_Shdr));
    int cnt = ReaderGetSH_Num();
    uint64_t str_offset = 0, str_len = 0;
    ReaderSetCursor(elf_header->e_shoff, 0);
    for (int i = 0; i < cnt; ++i) {
        if (strtab_list && elf_shsymbol) break;
        if (fread(p, sizeof(Elf64_Shdr), 1, fp) != 1) {
            printf("Warning! Can not read section header!\n");
        }
        if (p->sh_type == SHT_SYMTAB) {
            elf_shsymbol = malloc(sizeof(Elf64_Shdr));
            memcpy(elf_shsymbol, p, sizeof(Elf64_Shdr));
        } else if (p->sh_type == SHT_STRTAB) {
            strtab_list = malloc(sizeof(char)*p->sh_size);
            str_offset = p->sh_offset;
            str_len = p->sh_size;
        }
    }
    ReaderSetCursor(str_offset, 0);
    if (fread(strtab_list, sizeof(char), str_len, fp) != str_len) {
        printf("Warning! The number of char readed is not compatiable with str_len\n");
    }
    free(p);
}

static void FindFuncs()
{
    int cnt = 0;
    elf_func_cnt = 0;
    FOREACH_ELFSYM(sym) {
        if (ELF64_ST_TYPE(sym->elf_sym.st_info) == STT_FUNC) {
            elf_func_cnt++;
        }
    }
    elf_func_syms = elf_func_cnt==0 ? NULL : malloc(sizeof(Elf64_Sym)*elf_func_cnt);
    FOREACH_ELFSYM(sym) {
        if (ELF64_ST_TYPE(sym->elf_sym.st_info) == STT_FUNC) {
            memcpy(&elf_func_syms[cnt], &sym->elf_sym, sizeof(Elf64_Sym));
            cnt++;
        }
    }
}

char* ReaderSymbolGetName(Elf64_Sym *sym)
{
    return &strtab_list[sym->st_name];
}

symStr* ReaderSymbolStart()
{
    symStr* sym = malloc(sizeof(symStr));
    ReaderSetCursor(elf_shsymbol->sh_offset, 0);
    sym->cnt = 0;
    if (fread(&sym->elf_sym, sizeof(Elf64_Sym), 1, fp) != 1) {
        printf("Warning! can not read valid symbol!\n");
    }
    return sym;
}

symStr* ReaderSymbolNext(symStr *sym)
{
    sym->cnt++;
    ReaderSetCursor(sizeof(Elf64_Sym)*(sym->cnt), elf_shsymbol->sh_offset);
    if (fread(&sym->elf_sym, sizeof(Elf64_Sym), 1, fp) != 1) {
        printf("Warning! can not read valid symbol!\n");
    }
    return sym;
}

int ReaderSymbolIsEnd(symStr *sym)
{
    if (sym->cnt == (elf_shsymbol->sh_size/sizeof(Elf64_Sym))) {
        free(sym);
        return 1;
    }
    return 0;
}

void ReaderDeInit()
{
    fclose(fp);
    fp = NULL;
    #define READER_FREE(x) do { if (x) { free(x); x = NULL; }} while(0)
    READER_FREE(elf_header);
    READER_FREE(strtab_list);
    READER_FREE(elf_shsymbol);
    READER_FREE(elf_func_syms);
    #undef READER_FREE
}