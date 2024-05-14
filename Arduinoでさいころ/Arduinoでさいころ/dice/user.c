#include "user.h"
//さいころのパターン
  static const unsigned char dice[6][3] = {
      0, 0x18, 0,            // 1
      3, 0, 0xc0,            // 2
      3, 0x18, 0xc0,         // 3
      0xc3, 0, 0xc3,         // 4
      0xc3, 0x18, 0xc3,      // 5
      0xc3, 0xc3, 0xc3       // 6
  };
void user_init(void){

}

void user_main(void)
{
  unsigned char n;

  if (sw) {
		// 賽を振る
    do
  	  n = _rand() & 7;
    while (n > 5);

    led[0]    = dice[n][0];
    led[1]    = led[0];
    led[3]    = dice[n][1];
    led[4]    = led[3];
    led[6]    = dice[n][2];
    led[7]    = led[6];
  }
  sw_flag = 0;

  return 0;
}

