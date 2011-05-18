/*********************************************
 * File name: system.c
 * Author: Cassidy
 * Time-stamp: <2011-05-17 02:49:44>
 *********************************************
 */

#include <unistd.h>

void system_task(void)
{
  long a = 20;
  long *p = &a;
  //  small_msg_send(1, p);
  //  small_msg_receive(1, p);
  for(;;)
    ;
}
