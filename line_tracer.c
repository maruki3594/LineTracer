// wiringPi.h と wiringPiI2C.h をインクルードするのを忘れないこと
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
// 以下、定数宣言です
// PWM ユニットの I2C アドレス
// i2cdetect で確認可能、違っていたら修正して下さい
#define PWMI2CADR 0x40
// モータードライバの各入力が接続されている PWM ユニットのチャネル番号
// 右側のモーター：パワーユニットの K1 または K2 に接続（説明書は誤り）
// ENA は PWM 駆動に使う（1 でブリッジ動作、0 はブリッジオフ）
// IN1 と IN2 は右車輪の回転方向を決める（後進：0,1、前進：1,0）（0,0 と 1,1 はブレーキ）
#define ENA_PWM 8
#define IN1_PWM 9
#define IN2_PWM 10

// 左側のモーター：パワーユニットの K3 または K4 に接続（説明書は誤り）
// ENB は PWM 駆動に使う（1 でブリッジ動作、0 はブリッジオフ）
// IN3 と IN4 は左車輪の回転方向を決める（後進：0,1、前進：1,0）（0,0 と 1,1 はブレーキ）
#define ENB_PWM 13
#define IN3_PWM 11
#define IN4_PWM 12

// PWM モジュールのレジスタ番号
#define PWM_MODE1 0
#define PWM_MODE2 1
#define PWM_SUBADR1 2
#define PWM_SUBADR2 3
#define PWM_SUBADR3 4
#define PWM_ALLCALL 5

// PWM 番号×4+PWM_0_??_? でレジスタ番号は求まる
#define PWM_0_ON_L 6
#define PWM_0_ON_H 7
#define PWM_0_OFF_L 8
#define PWM_0_OFF_H 9

// PWM 出力定数
#define PWMFULLON 16
#define PWMFULLOFF 0

// プリスケーラのレジスタ番号
// PWM 周波数を決めるレジスタ番号、100Hz なら 61 をセット
#define PWM_PRESCALE 254

int set_pwm_output(int fd, int pwmch, int outval)
// motor_drive( )から呼ばれる関数、PWM ユニットへの書き込みをやっています
// 直接、他から呼び出す必要はないと思われますが、必要ならどうぞ
{
    int ef = 0;
    int regno;
    if ((pwmch < 0) || (pwmch > 15))
        ef = 1; // チャネルの指定違反チェック
    if ((outval < 0) || (outval > 16))
        ef = ef + 2; // 出力値の指定違反チェック
    if (ef == 0)
    {
        regno = PWM_0_ON_L + pwmch * 4; // 1ch あたり 4 レジスタで 16ch 分あるので
        if (outval == 16)
        {
            wiringPiI2CWriteReg8(fd, regno + 3, 0);
            wiringPiI2CWriteReg8(fd, regno + 1, 0x10);
        }
        else
        {
            wiringPiI2CWriteReg8(fd, regno + 1, 0);
            wiringPiI2CWriteReg8(fd, regno + 3, outval);
        }
    }
    return ef; // エラーがなければ 0 が返る
}

int motor_drive(int fd, int lm, int rm)
// モーターを制御するための関数
// fd は I2C 初期化時のファイルディスクプリタ（デバイス番号のようなもの）
// lm は左モーター、rm は右モーターの駆動数値で、-16～+16 の範囲で指定
// 負の場合は後方向に回転、正の場合は前方向に回転
// 絶対値が大きいほど、パワーが大きくなる
// PWM ユニット自体は 12 ビット精度だが、上位 4 ビット分を制御
// あまり細かく制御しても、ロボカーの動きとしては大差ないと考えられるため
// 必要と思うなら、自分でマニュアルを見てプログラムを書いて下さい
{
    set_pwm_output(fd, ENA_PWM, 0); // Right motor disable
    set_pwm_output(fd, ENB_PWM, 0); // Left motor disable
    // Right motor PWM control
    if (rm < 0)
    {
        ;
        set_pwm_output(fd, IN1_PWM, 0);  // OUT1->GND;
        set_pwm_output(fd, IN2_PWM, 16); // OUT2->+Vs
        rm = abs(rm);
    }
    else
    {
        set_pwm_output(fd, IN1_PWM, 16); // OUT1->+Vs
        set_pwm_output(fd, IN2_PWM, 0);  // OUT2->GND
    }
    // Left motor PWM control
    if (lm < 0)
    {
        set_pwm_output(fd, IN3_PWM, 0);  // OUT3->GND
        set_pwm_output(fd, IN4_PWM, 16); // OUT4->+Vs
        lm = abs(lm);
    }
    else
    {
        set_pwm_output(fd, IN3_PWM, 16); // OUT3->+Vs
        set_pwm_output(fd, IN4_PWM, 0);  // OUT4->GND
    }
    if (lm > 16)
        lm = 16;
    if (rm > 16)
        rm = 16;
    set_pwm_output(fd, ENA_PWM, rm); // Right motor PWM start
    set_pwm_output(fd, ENB_PWM, lm); // Left motor PWM start
    return 0;                        // 戻り値は常に 0
}

// この他に、プログラムの最初の方で以下の PWM ユニットの初期化が必要。
int main()
{
    int fd;
    wiringPiSetupGpio();              /* BCM_GPIO ピン番号で指定 */
    fd = wiringPiI2CSetup(PWMI2CADR); // この fd がファイルディスクプリタ
    if (fd < 0)
    {
        printf("I2C の初期化に失敗しました。終了します。¥n");
        exit(EXIT_FAILURE);
    }
    wiringPiI2CWriteReg8(fd, PWM_PRESCALE, 61); // PWM 周期 10ms に設定
    wiringPiI2CWriteReg8(fd, PWM_MODE1, 0x10);  // SLEEP mode
    wiringPiI2CWriteReg8(fd, PWM_MODE1, 0);     // normal mode
    delay(1);                                   // wait for stabilizing internal oscillator
    wiringPiI2CWriteReg8(fd, PWM_MODE1, 0x80);  // Restart all PWM ch

    motor_drive(fd, 10, 10);    // 左右のタイヤを前方向に動かす
    delay(1000);
    motor_drive(fd, 0, 0);      // モーターの動きを止める
}