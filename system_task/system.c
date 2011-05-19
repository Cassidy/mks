/*********************************************
 * File name: system.c
 * Author: Cassidy
 * Time-stamp: <2011-05-19 18:54:53>
 *********************************************
 */

#include <unistd.h>

void system_task(void)
{
  struct Msg msg;
  long *p = get_msg_entry(&msg);
  *p = 20;
  big_msg_send(1, &msg);
  big_msg_receive(1, &msg);

  for(;;)
    ;
}
