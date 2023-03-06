#include "mbed.h"
#include "mbed_wait_api.h"

#define MIGI_UE 0x14
#define MIGI_SITA 0x22
#define HIDARI_UE 0x40
#define HIDARI_SITA 0x30


I2C i2c(D14,D15);//i2c通信
DigitalOut sig(PC_10);//12v緊急停止
DigitalOut sta(PC_12);//12V検知
DigitalOut air1(PC_7), air2(PC_6);//エア尻
UnbufferedSerial tuusin(PA_9, PA_10, 9600);//nucleo同士の通信

//フォトリフレクタ
DigitalIn em0(PA_0); 
DigitalIn em1(PA_1); 
DigitalIn em2(PA_4); 
DigitalIn em3(PB_0); 
DigitalIn em4(PC_0);
DigitalIn em5(PC_1);

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

char power;//使ってない

int res = 0;
char data;



void send(char add, char data);
void send_all(char d_mu, char d_ms, char d_hs, char d_hu);
void receive_data(void);


int main(){

    bool moved_asimawari;////////消す予定////////

    while (true) {
        receive_data();//データ取得

        //緊急停止
        if(select){
            sig = 1;
        }

        if(start){
            sig = 0;
        }


        moved_asimawari = 0;




//旋回
        //機体左に旋回
        if(L1 && !moved_asimawari){
            send_all(0xa0, 0xa0, 0x60, 0x60);
            moved_asimawari = 1;
        }

        //機体右に旋回
        if(R1 && !moved_asimawari){
            send_all(0x60, 0x60, 0xa0, 0xa0);
            moved_asimawari = 1;
        }


//移動
        //前移動
        if(button_ue && !moved_asimawari){
            send_all(0xa0, 0xa0, 0xa0, 0xa0);
            moved_asimawari = 1;
        }

        //右移動
        if(button_migi && !moved_asimawari){
            send_all(0x60, 0xa0, 0x60, 0xa0);
            moved_asimawari = 1;
        }

        //左移動
        if(button_hidari && !moved_asimawari){
            send_all(0xa0, 0x60, 0xa0, 0x60);
            moved_asimawari = 1;
        }

        //後移動
        if(button_sita && !moved_asimawari){
            send_all(0x60, 0x60, 0x60, 0x60);
            moved_asimawari = 1;
        }



//エアシリンダー（タイヤ上げ用）
        if(button_sankaku){
            air1 = 1;
        }else {
            air1 = 0;
        }

        //足回り静止
        if(!moved_asimawari){
            send_all(0x80, 0x80, 0x80, 0x80); 
        }
    }//while
}//main



void send(char address, char data){
    wait_us(15000);
    i2c.start();
    i2c.write(address);
    i2c.write(data);
    i2c.stop();
}

void send_all(char d_mu, char d_ms, char d_hs, char d_hu){
    send(MIGI_UE, d_mu);
    send(MIGI_SITA, d_ms);
    send(HIDARI_SITA, d_hs);
    send(HIDARI_UE, d_hu);
}

void receive_data(void){
    res = tuusin.read(&data, 1);
    printf("%c\n", data);

    if(res == 1){
        if(data == '0') select = 1;         else select = 0;
        if(data == '1') start = 1;          else start = 0;

        if(data == '2') button_sankaku = 1; else button_sankaku = 0;
        if(data == '3') button_maru = 1;    else button_maru = 0;
        if(data == '4') button_batu = 1;    else button_batu = 0;
        if(data == '5') button_sikaku = 1;  else button_sikaku = 0;

        if(data == '6') button_ue = 1;      else button_ue = 0;
        if(data == '7') button_migi = 1;    else button_migi = 0;
        if(data == '8') button_sita = 1;    else button_sita = 0;
        if(data == '9') button_hidari = 1;  else button_hidari = 0;

        if(data == 'a') R1 = 1;             else R1 = 0;
        if(data == 'b') R2 = 1;             else R2 = 0;
        if(data == 'c') L1 = 1;             else L1 = 0;
        if(data == 'd') L2 = 1;             else L2 = 0;

    }
}