/*********************************************
 * File name: time.h
 * Author: Cassidy
 * Time-stamp: <2011-04-19 20:11:28>
 *********************************************
 */

#ifndef _TIME_H
#define _TIME_H

struct tm {
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
};

#endif
