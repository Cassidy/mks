#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define MAKE         0
#define BREAK        0x80
#define KBD_BUF_SIZE 64

#define MAP_COL      3
#define NO_SHIFT     0
#define WITH_SHIFT   1
#define BRA          3

/* 键盘扫描前缀码 */
#define PREFIX_E0 0xE0
#define PREFIX_E1 0xE1

/* 控制键的接通码（make code） */
#define SHIFT_L   0x2A
#define SHIFT_R   0x36
#define CTRL_L    0x1D          /* CTRL_R: 0xE0 0x1D */
#define ALT_T     0x38          /* ALT_R: 0xE0 0x38 */
#define SPACE     0x39
#define TAB       0x0F
#define CAPS_LOCK 0x3A
#define ENTER     0x1C
#define BACKSPACE 0x0E
#define ESC       0x01
#define SCROLL    0x46

/* 键盘缓冲区队列 */
struct kbdqueue {
  unsigned char buf[KBD_BUF_SIZE];
  unsigned char *head;
  unsigned char *tail;
};

/* 扫描码 = 数组下标 × 3 */
unsigned char keymap[] = {
  0,            0,              0, /* 第一个留空 */
  0,            0,              0, /* esc */
  '1',          '!',            0,
  '2',          '@',            0,
  '3',          '#',            0,
  '4',          '$',            0,
  '5',          '%',            0,
  '6',          '^',            0,
  '7',          '&',            0,
  '8',          '*',            0,
  '9',          '(',            0,
  '0',          ')',            0,
  '-',          '_',            0,
  '=',          '+',            0,
  '\b',         '\b',           0, /* backspace */
  '\t',         '\t',           0, /* tab */
  'q',          'Q',            0,
  'w',          'W',            0,
  'e',          'E',            0,
  'r',          'R',            0,
  't',          'T',            0,
  'y',          'Y',            0,
  'u',          'U',            0,
  'i',          'I',            0,
  'o',          'O',            0,
  'p',          'P',            0,
  '[',          '{',            0,
  ']',          '}',            0,
  '\n',         '\n',           0, /* enter */
  0,            0,              0, /* ctrl_l */
  'a',          'A',            0,
  's',          'S',            0,
  'd',          'D',            0,
  'f',          'F',            0,
  'g',          'G',            0,
  'h',          'H',            0,
  'j',          'J',            0,
  'k',          'K',            0,
  'l',          'L',            0,
  ';',          ':',            0,
  '\'',         '"',            0,
  '`',          '~',            0,
  0,            0,              0, /* shift_l */
  '\\',         '|',            0,
  'z',          'Z',            0,
  'x',          'X',            0,
  'c',          'C',            0,
  'v',          'V',            0,
  'b',          'B',            0,
  'n',          'N',            0,
  'm',          'M',            0,
  ',',          '<',            0,
  '.',          '>',            0,
  '/',          '?',            0,
  0,            0,              0, /* shift_r */
  '*',          '*',            0,
  0,            0,              0, /* alt_l */
  ' ',          ' ',            0,
  0,            0,              0, /* caps_lock */
};

#endif /* _KEYBOARD_H */
