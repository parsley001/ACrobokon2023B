#include "mbed.h"
#include "mbed_wait_api.h"
#include <cstdio>

//モーター
#define MIGI_UE     0x26
#define MIGI_SITA   0x24
#define HIDARI_SITA 0x22
#define HIDARI_UE   0x20
static char _forward = 0xa0;//正転出力パワー
static char back    = 0x60;//逆転出力パワー
static char turnr;
static char turnl;
static char stop    = 0x80;//モーター止める


//12V制御
DigitalOut sig(PC_12);//12v緊急停止
DigitalIn  sta(PC_10);//12V検知
DigitalOut air1(D12), air2(D11);//エア尻(PA_6),(PA_7)
DigitalOut red(D6),green(D7),blue(D5);//LED(PB_10),(PA_8),(PB_4)

//通信
I2C i2c(D14,D15);//i2c通信(PB_9),(PB_8)
UnbufferedSerial tuusin(D8, D2, 9600);//nucleo同士の通信(PA_9),(PA_10)

//フォトリフレクタ
AnalogIn pr0(A0); //(PA_0)
AnalogIn pr1(A1); //(PA_1)
AnalogIn pr2(A2); //(PA_4)
AnalogIn pr3(A3); //(PB_0)
AnalogIn pr4(A4); //(PC_1)
AnalogIn pr5(A5); //(PC_0)
static double wd = 4.8;//Wood distance　木材検知のしきい値

//ボタン定義
int Rx;            //ジョイコン　右　x軸
int Ry;            //ジョイコン　右　y軸
int Lx;            //ジョイコン　左　x軸
int Ly;            //ジョイコン　左　y軸

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


char power;  //////使ってない//////

int  res = 0;  //シリアル通信を受信で１
char data;     //シリアル通信で送られたデータ


//プロトタイプ宣言
void send(char add, char data);//モーターを回す
void send_all(char d_mu, char d_ms, char d_hs, char d_hu);//モーターを4つ回す
void receive_data(void);//シリアル通信受け取り（PS3入力受け取り）
int iswood(AnalogIn pr_left, AnalogIn pr_right);


int main(){
    tuusin.format(8, BufferedSerial::None, 1);//シリアル通信設定(bits, parity, stopbit)
    bool moved_asimawari;////////消す予定////////

    while (true) {
        receive_data();//データ取得

        //printf("%d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", start, select, button_sankaku, button_maru, button_batu, button_sikaku, button_ue, button_migi, button_sita, button_hidari, R1, R2, L1, L2);
        printf("four %.3f   one %.3f   zero %.3f   three %.3f   two %.3f   five %.3f\n", pr4*5, pr1*5, pr0*5, pr3*5, pr2*5, pr5*5);
        //緊急停止
        if(select){
            sig = 1;
        }

        if(start){
            sig = 0;
        }


        moved_asimawari = 0;




        // if(button_sankaku){
        //     air1 = 1;
        //     air2 = 1;
        // }else {
        //     air1 = 0;
        //     air2 = 0;
        // }


//エアシリンダー（タイヤ上げ用）
        if(button_sankaku){
            if(iswood(pr5, pr4)){
                air1 = 0;//前方輪上げ
                if(iswood(pr2, pr1)){
                    air1 = 1;
                    air2 = 0;
                    if (iswood(pr3, pr0)) {
                        air2 = 1;
                    }
                }
            }



        }else {
            air1 = 1;
            air2 = 1;
        }


//旋回
        //機体左に旋回
        if(L1 && !moved_asimawari){
            send_all(256-_forward, _forward, back, 256-back);
            moved_asimawari = 1;
        }

        //機体右に旋回
        if(R1 && !moved_asimawari){
            send_all(256-back, back, _forward, 256-_forward);
            moved_asimawari = 1;
        }


//移動
        //前移動
        if(button_ue && !moved_asimawari){
            send_all(256-_forward, _forward, _forward, 256-_forward);
            moved_asimawari = 1;
        }

        //右移動
        if(button_migi && !moved_asimawari){
            send_all(256-back, _forward, back, 256-_forward);
            moved_asimawari = 1;
        }

        //左移動
        if(button_hidari && !moved_asimawari){
            send_all(256-_forward, back, _forward, 256-back);
            moved_asimawari = 1;
        }

        //後移動
        if(button_sita && !moved_asimawari){
            send_all(256-back, back, back, 256-back);
            moved_asimawari = 1;
        }


        //足回り静止
        if(!moved_asimawari){
            send_all(stop, stop, stop, stop); 
        }

    }//while
}//main



void send(char address, char data){
    wait_us(15000);//send_allに移動すべきでは？
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

//nucleoからのデータ受け取り
void receive_data(void){
    res = tuusin.read(&data, 1);
    // printf("%c\n", data);

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

///////////////////！！！！！！！！！！！テストしろ！！！！！！！！！///////////////////////////
/*
   *フォトリフレクタ２つを使って木の柵を見つける
   *両方のフォトリフレクタが反応しない→前進
   *左のフォトリフレクタのみ反応→反時計回り回転
   *右のフォトリフレクタのみ反応→時計回り回転
   *両方のフォトリフレクタが反応→１を返し終了
   *R2とL2を同時押しもしくはselectを押すと０を返し終了
*/
int iswood(AnalogIn pr_left, AnalogIn pr_right){
    //R2L2同時押しでキャンセル
    while(!(R2 && L2) && !select){
        if((pr_right*5)<wd && (pr_left*5)<wd){//前のフォトリフレクタがどちらも検知したら
            return 1;
        }else if((pr_left*5)<wd){//左前だけ検知
            send_all(stop, _forward, back, stop);//前方軸に反時計回り
        }else if((pr_right*5)<wd){//右前だけ検知
            send_all(stop, back, _forward, stop);//前方軸に時計回り
        }else{
            send_all(256-_forward, _forward, _forward, 256-_forward);//直進
        }
        receive_data();
    }//while
    return 0;
}