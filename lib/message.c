/*********************************************
 * File name: message.c
 * Author: Cassidy
 * Time-stamp: <2011-05-19 18:58:04>
 *********************************************
 */

#include <unistd.h>

long * get_msg_entry(struct Msg * msgpt)
{
  long entry;
  entry = (long)msgpt;
  entry = (entry & 0xFFFFF000) + 0x1000;
  return (long *)entry;
}

long small_msg_send(long destination, long * msgpt)
{
  long res = 1;

  __asm__ volatile 
    (							\
     "int $0x80"					\
     :"=a"(res)						\
     :"0"(res), "b"(destination), "c"(*msgpt));
  if(res == 1)
    return 1;
  return -1;
}

long small_msg_receive(long source, long * msgpt)
{
  long res = 2;
  long msg;
  
  __asm__ volatile 
    (							\
     "int $0x80"					\
     :"=a"(res), "=c"(msg)				\
     :"0"(res), "b"(source));

  if(res == 1)
    {
      *msgpt = msg;
      return 1;
    }
  return -1;
}

long big_msg_send(long destination, struct Msg * msgpt)
{
  long res = 3;

  __asm__ volatile 
    (							\
     "int $0x80"					\
     :"=a"(res)						\
     :"0"(res), "b"(destination), "c"((long)msgpt));

  if(res == 1)
    return 1;
  return -1;
}

long big_msg_receive(long source, struct Msg * msgpt)
{
  long res = 4;

  __asm__ volatile 
    (							\
     "int $0x80"					\
     :"=a"(res)						\
     :"0"(res), "b"(source), "c"((long)msgpt));

  if(res == 1)
    return 1;
  return -1;
}


