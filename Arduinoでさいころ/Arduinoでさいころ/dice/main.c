#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "user.h"
#define CTOP    (0.1*F_CPU/1024-1)
#define SW_MASK 0x30
#define SW_SFT  4

volatile unsigned char sw = 0;	// スイッチの確定値
volatile unsigned char sw_flag = 0;	// スイッチ状態の変化ビット
static volatile unsigned char rnd = 1;	//  擬似乱数のカウンタ 
static unsigned char period = 0;	// ブザー音の長さ       

unsigned char led[8];

// 2mSec ごとに起動
ISR(TIMER0_COMPA_vect)
{
	static unsigned char sc = 0xfe;
	static unsigned char scan = 0;
	static unsigned char sw_cnt = 0;	// 値を確定するまでの繰り返し數
	static unsigned char sw_current = 0;
	unsigned char sw_new;
	unsigned char pb, pd;

	// マトリクス LED を消灯（残像対策）
	PORTB = 0;
	PORTD &= ~0xc0;

	// SW 読み取りの準備
	if(!sw_flag && !(~PORTC & SW_MASK)) {	// SW 接続ポート出力がすべて H レベルのとき
		DDRC &= ~SW_MASK;	// SW 接続ポートをプルアップされた入力ポートに変更
	}

	// マトリクス LED のつぎの出力値を準備
	sc = (sc << 1) | (sc >> 7);
	scan = (scan + 1) & 7;
	pb = led[scan];
	pd = (pb & 0xc0) | ((sc & 0xc0) >> 2) | (PORTD & 0x0f);

	// SW の読み取り
	if(!(DDRC & SW_MASK)) {	// SW 接続ポートが入力ポートなら
		sw_new = (~PINC & SW_MASK) >> SW_SFT;	// SW の現在値を読み取り
		if(sw_current != sw_new) {	// 変化があれば
			sw_current = sw_new;
			sw_cnt = 10;	// カウンタを初期化
		}
		else if(sw_cnt) {
			if(--sw_cnt == 0) {	// 読み取り値が安定するまで待つ
				sw_flag = sw ^ sw_current;	// ON/OFF 変化を検出
				sw = sw_current;	// 読み取り値を更新
				rnd ^= rnd >> 1;
			}
		}
		DDRC |= SW_MASK;	// SW 接続ポートを出力に戻す
	}

// マトリクス LED を表示
	PORTB = pb;
	PORTC = sc;
	PORTD = pd;
}

int main(void)
{
	DDRB = 0x3f;
	DDRC = 0x3f;
	DDRD = 0xf8;
	PORTB = 0xc0;
	PORTC = 0xc0;
	PORTD = 0x07;

	// Timer0
	TCCR0A = 0x02;	//
	TCCR0B = 0x04;	// CTC, 1/256
	OCR0A = 124;
	TCNT0 = 0;
	TIFR0 = 0x07;	// タイマフラグを全クリア
	TIMSK0 |= _BV(OCIE0A);	// 比較一致割り込み許可

	// Timer1
	TCCR1A = 0x00;
	TCCR1B = 0x0d;	// CTC, 1/1024
	OCR1A = CTOP;
	TCNT1 = 0;
	TIFR1 = 0x07;	// タイマフラグを全クリア

	// Timer2
	TCCR2A = 0x12;	// Compare Match Output Toggle COM2B
	TCCR2B = 0x05;	// CTC, 1/128
	OCR2A = 1;
	TCNT2 = 0;

	user_init();

	sei();
	for(;;) {
		wdt_reset();

		if(TIFR1 & _BV(OCF1A)) {
			TIFR1 = _BV(OCF1A);
			if(period) {
				if(--period == 0) {
					TCCR2A = 0;	// ブザー停止
				}
			}
			user_main();
		}
	}
	return 0;
}

/* ブザー */
unsigned char _sound(unsigned char tone, unsigned char length)
{
	if(period) {
		return 0;
	}
	OCR2A = tone;
	period = length;
	TCCR2A = tone ? 0x12 : 0;
	return 1;
}

/* 擬似乱数生成 */
unsigned char _rand(void)
{
	unsigned char y, z;

	for(y = rnd & 0x95, z = 0; y; z++) {
		y &= y - 1;
	}
	return (rnd = (rnd << 1) | (z & 1));
}
