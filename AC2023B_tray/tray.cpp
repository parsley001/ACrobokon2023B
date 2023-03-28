#include "mbed.h"
#include "PS3.h"
#include "mbed_wait_api.h"
#include <cstdio>


//通信
PS3 ps3(A0, A1);//SBDBT(PA_0),(PA_1)
UnbufferedSerial tuusin(D8, D2, 9600);//nucleoとの通信(PA_9),(PA_10)

//サーボモーター
PwmOut servo1(D10); //PB_6
PwmOut servo2(D11); //PA_7
PwmOut servo3(D12); //PA_6


//ボタン定義
int Rx;            //ジョイコン　右　x軸
int Ry;            //ジョイコン　右　y軸
int Lx;            //ジョイコン　左　x軸
int Ly;            //ジョイコン　左　y軸

bool Lst_ue;         //Lスティック上
bool Lst_migi;       //Lスティック右
bool Lst_sita;       //Lスティック下
bool Lst_hidari;     //Lスティック左

bool Rst_ue;         //Rスティック上
bool Rst_migi;       //Rスティック右
bool Rst_sita;       //Rスティック下
bool Rst_hidari;     //Rスティック左

float Rangle;        // ジョイコン　右　角度
float Langle;        // ジョイコン　左　角度

bool R1;            //R1
bool R2;            //R2
bool L1;            //L1
bool L2;            //L2

bool button_ue;     // ↑
bool button_migi;   // →
bool button_sita;   // ↓
bool button_hidari; // ←

bool button_sankaku; // △
bool button_maru;    // 〇
bool button_batu;    // ✕
bool button_sikaku;  // ☐

bool select; //select
bool start;  //start
//


void get_data(void);
void send_data(void);



int main(){
    tuusin.format(8, BufferedSerial::None, 1);//シリアル通信設定(bits, parity, stopbit)

    int servo_start=560, servo_end=2200;//サーボの位置
    servo1.pulsewidth_us(servo_end);//サーボ初期位置
    servo2.pulsewidth_us(servo_end);
    servo3.pulsewidth_us(servo_end);

    while (true){
        get_data();


//スティック処理

        //Lスティック
        if(Lx>60 || Lx<-60 || Ly>60 || Ly<-60){//スティックが最大限倒れている
            if(Langle<45 && Langle>=-45)   Lst_migi = 1;   else Lst_migi = 0;
            if(Langle<135 && Langle>=45)   Lst_ue = 1;     else Lst_ue = 0;
            if(Langle<-135 || Langle>=135) Lst_hidari = 1; else Lst_hidari = 0;
            if(Langle<-45 && Langle>=-135) Lst_sita = 1;   else Lst_sita = 0;
        }else{
            Lst_migi = 0;
            Lst_ue = 0;
            Lst_hidari = 0;
            Lst_sita = 0;
        }

        //Rスティック
        if(Rx>60 || Rx<-60 || Ry>60 || Ry<-60){//スティックが最大限倒れている
            if(Rangle<45 && Rangle>=-45){   Rst_migi = 1;  printf("migi\n"); }else Rst_migi = 0;
            if(Rangle<135 && Rangle>=45){   Rst_ue = 1;    printf("ue\n"); }else Rst_ue = 0;
            if(Rangle<-135 || Rangle>=135){ Rst_hidari = 1;printf("hidari\n"); }else Rst_hidari = 0;
            if(Rangle<-45 && Rangle>=-135){ Rst_sita = 1;  printf("sita\n"); }else Rst_sita = 0;
        }else{
            Rst_migi = 0;
            Rst_ue = 0;
            Rst_hidari = 0;
            Rst_sita = 0;
        }

        printf("%d %d  %d %d %d %d  %d %d %d %d  %7.2f %3d %3d  %7.2f %3d %3d  %d %d %d %d  %d %d %d %d\n", start, select,   button_sankaku, button_maru, button_batu, button_sikaku,   button_ue, button_migi, button_sita, button_hidari,   Langle, Lx, Ly,   Rangle, Rx, Ry,   Lst_ue, Lst_migi, Lst_sita, Lst_hidari,   R1, R2, L1, L2);

        send_data();


//フックを初期位置に戻す
        if(L2 && R2){
            if(button_maru){
                servo1.pulsewidth_us(servo_start);
            }
            if(button_batu){
                servo2.pulsewidth_us(servo_start);
            }
            if(button_sikaku){
                servo3.pulsewidth_us(servo_start);
            }
        }//if
//トレー設置
        else{
            if(button_maru){
                servo1.pulsewidth_us(servo_end);//サーボ位置変更
            }
            if(button_batu){
                servo2.pulsewidth_us(servo_end);
            }
            if(button_sikaku){
                servo3.pulsewidth_us(servo_end);
            }
        }//else
    }//while
    return 0;
}//main



//データ取得のための関数
void get_data(void){
    //ボタン取得
    select = ps3.getSELECTState();
    start = ps3.getSTARTState();

    button_ue = ps3.getButtonState(PS3::ue);
    button_sita = ps3.getButtonState(PS3::sita);
    button_migi = ps3.getButtonState(PS3::migi);
    button_hidari = ps3.getButtonState(PS3::hidari);

    R1 = ps3.getButtonState(PS3::R1);
    R2 = ps3.getButtonState(PS3::R2);
    L1 = ps3.getButtonState(PS3::L1);
    L2 = ps3.getButtonState(PS3::L2);

    button_sankaku = ps3.getButtonState(PS3::sankaku);
    button_maru = ps3.getButtonState(PS3::maru);
    button_batu = ps3.getButtonState(PS3::batu);
    button_sikaku = ps3.getButtonState(PS3::sikaku);

    //スティックの座標取得
    Rx = ps3.getRightJoystickXaxis();
    Ry = ps3.getRightJoystickYaxis();
    Lx = ps3.getLeftJoystickXaxis();
    Ly = ps3.getLeftJoystickYaxis();

    //スティックの角度取得
    Rangle = ps3.getRightJoystickAngle();
    Langle = ps3.getLeftJoystickAngle();
}

//もう一方のnucleoにデータ送信
void send_data(void){
    // printf("maru:%d batu:%d\n",button_maru,button_batu);
    if(select || start || button_sankaku || button_maru || button_batu || button_sikaku || button_ue || button_migi || button_sita || button_hidari || Lst_ue || Lst_migi || Lst_sita || Lst_hidari || Rst_ue || Rst_migi || Rst_sita || Rst_hidari || R1 || R2 || L1 || L2){
        printf("hoge\n");
//順番変えると操作優先順位が変わる
        if(select)         tuusin.write("0", 1); //else tuusin.write("000", 1);
        if(start)          tuusin.write("1", 1); //else tuusin.write("001", 1);

        if(L2 && R2 && button_ue)   tuusin.write("B", 1);
        if(L2 && R2 && button_sita) tuusin.write("C", 1);
        if(L1 && Lst_hidari)        tuusin.write("D", 1);
        if(R1 && Lst_migi)          tuusin.write("E", 1);
        if(L2 && R2)                tuusin.write("A", 1);

        if(button_sankaku) tuusin.write("2", 1); //else tuusin.write("002", 1);
        if(button_maru)    tuusin.write("3", 1); //else tuusin.write("003", 1);
        if(button_batu)    tuusin.write("4", 1); //else tuusin.write("004", 1);
        if(button_sikaku)  tuusin.write("5", 1); //else tuusin.write("f", 1);

        if(button_ue)      tuusin.write("6", 1); //else tuusin.write("g", 1);
        if(button_migi)    tuusin.write("7", 1); //else tuusin.write("h", 1);
        if(button_sita)    tuusin.write("8", 1); //else tuusin.write("i", 1);
        if(button_hidari)  tuusin.write("9", 1); //else tuusin.write("j", 1);

        if(R1)             tuusin.write("a", 1); //else tuusin.write("k", 1);
        if(R2)             tuusin.write("b", 1); //else tuusin.write("l", 1);
        if(L1)             tuusin.write("c", 1); //else tuusin.write("m", 1);
        if(L2)             tuusin.write("d", 1); //else tuusin.write("n", 1); 

        if(Rst_ue)         tuusin.write("i", 1);
        if(Rst_migi)       tuusin.write("j", 1);
        if(Rst_sita)       tuusin.write("k", 1);
        if(Rst_hidari)     tuusin.write("l", 1);

        if(Lst_ue)         tuusin.write("e", 1);
        if(Lst_migi)       tuusin.write("f", 1);
        if(Lst_sita)       tuusin.write("g", 1);
        if(Lst_hidari)     tuusin.write("h", 1);


    }else {
        tuusin.write("z", 1);
    }
}