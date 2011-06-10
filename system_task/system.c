/*********************************************
 * File name: system.c
 * Author: Cassidy
 * Time-stamp: <2011-06-10 14:56:05>
 *********************************************
 */

#include <unistd.h>

void system_task(void)
{
  struct Msg msg;
  long src = MSG_ANY;
  while(1)
    {
      big_msg_receive(&src, &msg);
      src = MSG_ANY;
    }
}
