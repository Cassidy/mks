/*********************************************
 * File name: init.c
 * Author: Cassidy
 * Time-stamp: <2011-05-17 00:41:29>
 *********************************************
 */

#include <unistd.h>

void init_proc(void)
{
  long a = 5;
  long *p =&a;

  //  small_msg_receive(2, p);
  //  big_msg_send(2, p);

  for(;;)
    ;
}
