
#include <stdint.h>

#define LED_SZ  8

enum
{
  BEEP_HIGH = 46,
  BEEP_LOW = 168,
};

extern void user_init(void);
extern void user_main(void);
extern unsigned char  _sound(unsigned char tone, unsigned char length);
extern unsigned char  _rand(void);

extern unsigned char led[];
extern volatile unsigned char sw;       // スイッチの確定値
extern volatile unsigned char sw_flag;  // スイッチ状態の変化ビット
