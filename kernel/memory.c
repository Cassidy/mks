/*********************************************
 * File name: memory.c
 * Author: Cassidy
 * Time-stamp: <2011-05-26 18:36:37>
 *********************************************
 */

#include <kernel/kernel.h>

#define LOW_MEM 0x100000             /* 1M 内存开始处 */
#define PAGING_MEMORY (64*1024*1024)     /* 物理内存可用于分页的总容量 */
#define PAGING_PAGES (PAGING_MEMORY>>12) /* 物理内存可用于分页的页数量 */
#define MAP_NR(addr) (addr>>12)          /* 物理地址 addr 在物理内存页的第几页（从 0 开始计数） */
#define USED 1
#define UNUSED 0

unsigned long *pg_dir;          /* 页目录表起始地址 */

static long HIGH_MEMORY =0;     /* 全局变量，存放实际物理内存最高端地址。 */
static unsigned char mem_map[PAGING_PAGES] = {0,};

unsigned long get_free_page(void);
int create_page_table(unsigned long addr);

void invalidate(void)
{
  __asm__("movl %%eax, %%cr3"::"a"(pg_dir));
}

/* 主内存初始化 */
void mem_init(unsigned long start_mem, unsigned long end_mem)
{
  int i;
  unsigned long mem_size;        /* 主内存容量大小 */

  pg_dir = PAGE_DIR_ADDR;       /* PAGE_DIR_ADDR = 0x7000 */
  HIGH_MEMORY = end_mem;

  /* 将 64M 物理内存页初始化为 1（USED） */
  for (i=0 ; i<PAGING_PAGES ; i++)
    mem_map[i] = USED;

  i = MAP_NR(start_mem);        /* 主内存起始地址在内存页的第几页（4KB为一页） */
  mem_size = end_mem - start_mem;
  mem_size >>= 12;              /* 主内存所拥有的内存页数量 */

  /* 主内存页初始化为未使用 */
  while (mem_size-- > 0)
    mem_map[i++] = UNUSED;
}

void copy_page(unsigned long from, unsigned long to)
{
  int i;
  unsigned long *p = (unsigned long *)from;
  unsigned long *q = (unsigned long *)to;

  for(i=0; i<PAGE_SIZE/4; i++)
    *(q++) = *(p++);
}

/* 缺页处理,临时使用,并不完整,有待完善 */
void do_no_page(unsigned long addr)
{
  unsigned long * table;
  unsigned long * page;
  unsigned long phys_addr;

  table = (unsigned long *)((unsigned long)pg_dir + ((addr>>20)&0xFFC));
  while(!((*table) & 1))
    {
      create_page_table(addr);
      table = (unsigned long *)((unsigned long)pg_dir + ((addr>>20)&0xFFC));
    }
  page = (unsigned long *)(((*table)&0xFFFFF000) + ((addr>>10)&0xFFC));
  if(addr < 64*1024*1024)
    *page = (addr & 0xFFFFF000) | 7;
  else
    {
      phys_addr = get_free_page();
      if(phys_addr == 0)
    return;
      *page = phys_addr | 7;
    }

  invalidate();
}

/* 写保护处理 */
void do_wp_page(unsigned long addr)
{
  unsigned long * table;
  unsigned long * page;
  unsigned long old_page;
  unsigned long new_page;

  table = (unsigned long *)((unsigned long)pg_dir + ((addr>>20)&0xFFC));
  page = (unsigned long *)(((*table)&0xFFFFF000) + ((addr>>10)&0xFFC));
  old_page = (*page)&0xFFFFF000;
  if(mem_map[MAP_NR(old_page)] == 1)
    *page |= 2;
  else
    {
      if(!(new_page=get_free_page()))
        return;
      mem_map[MAP_NR(old_page)]--;
      copy_page(old_page, new_page);
      *page = new_page | 7;
    }

  invalidate();
}

void do_intr_page(long *eip, unsigned long error_code)
{
  unsigned long addr;

  __asm__ (                     \
       "movl %%cr2, %%eax\n\t"  \
       "sti"                    \
       :"=a"(addr));

  if((error_code & 0x1) == 0)
    do_no_page(addr);
  else /* (error_code & 0x1) != 0 */
    do_wp_page(addr);
}

/* 获取空闲页面,返回物理地址,如果没有空闲页面,就返回0. */
unsigned long get_free_page(void)
{
  long i,j;
  unsigned long * p;
  for (i=0 ; i<PAGING_PAGES ; i++)
    {
      if(mem_map[i] == 0)
    {
      if((i & 0x3FF) == 0x3FD)
        {
          mem_map[i] = 1;
          j = create_page_table((unsigned long)((i+2)<<12));
          mem_map[i] = 0;
          if(j == -1)
        return 0;
        }
      p = (unsigned long *)(i << 12);
      for(j=0; j<(PAGE_SIZE/4); j++)
        *(p++) = 0;
      mem_map[i] = 1;
      return (unsigned long)(i<<12);
    }
    }

  return 0;
}

/* 创建页表,返回0表示已存在,返回1表示创建成功,返回-1表示创建失败 */
int create_page_table(unsigned long addr)
{
  unsigned long * table;
  unsigned long page;

  table = (unsigned long *)((unsigned long)pg_dir + ((addr>>20)&0xFFC));
  if((*table) & 1)
    return 0;
  page = get_free_page();
  if(page == 0)
    return -1;
  page |= 7;
  *table = page;
  invalidate();
  return 1;
}

/* 共享页面 */
void share_page(unsigned long from, unsigned long to)
{
  unsigned long * from_table;
  unsigned long * from_page;
  unsigned long * to_table;
  unsigned long * to_page;
  unsigned long page;

  from_table = (unsigned long *)((unsigned long)pg_dir + ((from>>20)&0xFFC));
  while(!((*from_table) & 1)) {
    do_no_page(from);
    from_table = (unsigned long *)((unsigned long)pg_dir + ((from>>20)&0xFFC));
  }
  from_page = (unsigned long*)(((*from_table)&0xFFFFF000) + ((from>>10)&0xFFC));
  while(!((*from_page) & 1)) {
    do_no_page(from);
    from_page = (unsigned long *)(((*from_table)&0xFFFFF000) + ((from>>10)&0xFFC));
  }
  to_table = (unsigned long *)((unsigned long)pg_dir + ((to>>20)&0xFFC));
  while(!((*to_table) & 1)) {
    create_page_table(to);
    to_table = (unsigned long *)((unsigned long)pg_dir + ((to>>20)&0xFFC));
  }
  to_page = (unsigned long *)(((*to_table)&0xFFFFF000) + ((to>>10)&0xFFC));

  page = *from_page;
  mem_map[MAP_NR(page)]++;
  page &= ~2;
  *to_page = page;
  if(from >= 64*1024*1024)
    *from_page = page;

  invalidate();
}

/* 共享多个物理页面 */
int share_multi_pages(unsigned long from, unsigned long to, long amount)
{
  if((from&0xFFF) || (to&0xFFF))
    return -1;

  while(amount--) {
    share_page(from, to);
    from += 0x1000;
    to += 0x1000;
  }

  return 1;
}
