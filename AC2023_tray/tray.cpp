#include "mbed.h"
#include "PS3.h"
#include "mbed_wait_api.h"

PS3 ps3 (PA_0, PA_1);//ps3
PwmOut servo1(D10);
PwmOut servo2(D11);
PwmOut servo3(D12);//こいつだけ逆向き

UnbufferedSerial tuusin(PA_9, PA_10, 9600);



// int Rx;            //ジョイコン　右　x軸
// int Ry;            //ジョイコン　右　y軸
// int Lx;            //ジョイコン　左　x軸
// int Ly;            //ジョイコン　左　y軸

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



void get_data(void);
void send_data(void);



int main(){
    int start=550, end=2200;//サーボPWM
    servo1.pulsewidth_us(start);//サーボ初期位置
    servo2.pulsewidth_us(start);
    servo3.pulsewidth_us(start);

    while (true){
        get_data();
        send_data();



//トレー設置
        if(button_maru){
            servo1.pulsewidth_us(end);//サーボ位置変更
        }
        if(button_batu){
            servo2.pulsewidth_us(end);
        }
        if(button_sikaku){
            servo3.pulsewidth_us(end);
        }
    }
    return 0;
}



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
    // Rx = ps3.getRightJoystickXaxis();
    // Ry = ps3.getRightJoystickYaxis();
    // Lx = ps3.getLeftJoystickXaxis();
    // Ly = ps3.getLeftJoystickYaxis();
}

//もう一方のnucleoにデータ送信
void send_data(void){
    // printf("maru:%d batu:%d\n",button_maru,button_batu);
    if(select)         tuusin.write("0", 1); //else tuusin.write("000", 1);
    if(start)          tuusin.write("1", 1); //else tuusin.write("001", 1);

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
}