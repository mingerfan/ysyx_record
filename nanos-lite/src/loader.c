#include <proc.h>
#include <elf.h>
#include <stdio.h>
#include <common.h>
#include <fs.h>


#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV64__)
#define EXPECT_TYPE EM_RISCV
#else 
#error Unsupported ISA __ISA__
#endif


size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr hd;
  Elf_Phdr pd;
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, &hd, sizeof(Elf_Ehdr));
  assert((*(uint32_t*)hd.e_ident) == 0x464c457f);
  assert(hd.e_machine == EM_RISCV);
  uint64_t phoff = hd.e_phoff;
  uint64_t phnum = hd.e_phnum;


  for (int i = 0; i < phnum; i++) {
    fs_lseek(fd, phoff + i * sizeof(Elf_Phdr), SEEK_SET);
    fs_read(fd, &pd, sizeof(Elf_Phdr));
    if (pd.p_type != PT_LOAD) {
      continue;
    }
    assert(pd.p_filesz <= pd.p_memsz);
    fs_lseek(fd, pd.p_offset, SEEK_SET);
    fs_read(fd, (void*)pd.p_vaddr, pd.p_filesz);
    memset((void*)(pd.p_vaddr + pd.p_filesz), 0, pd.p_memsz - pd.p_filesz);
  }
  fs_close(fd);

  return hd.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", (void*)entry);
  ((void(*)())entry) ();
}

