/*********************************************
 * File name: init.c
 * Author: Cassidy
 * Time-stamp: <2011-05-19 20:00:04>
 *********************************************
 */

#include <unistd.h>

void init_proc(void)
{
  struct Msg msg;
  long *p = get_msg_entry(&msg);
  //  *p = 5;
  big_msg_receive(2, &msg);
  small_msg_send(2, p);
  small_msg_send(2, p);

  for(;;)
    ;
}
