#include "mbed.h"
#include "mbed_wait_api.h"

//モーター
#define MIGI_UE 0x14
#define MIGI_SITA 0x22
#define HIDARI_UE 0x40
#define HIDARI_SITA 0x30
char forward = 0xa0;//正転出力パワー
char back    = 0x60;//逆転出力パワー
char turnr;
char turnl;
char stop    = 0x80;//モーター止める


//12V制御
DigitalOut sig(PC_12);//12v緊急停止
DigitalIn  sta(PC_10);//12V検知
DigitalOut air1(D11), air2(D12);//エア尻
DigitalOut red(D6),green(D7),blue(D5);//LED

//通信
I2C i2c(D14,D15);//i2c通信
UnbufferedSerial tuusin(D8, D2, 9600);//nucleo同士の通信

//フォトリフレクタ
AnalogIn pr0(A0); 
AnalogIn pr1(A1); 
AnalogIn pr2(A2); 
AnalogIn pr3(A3); 
AnalogIn pr4(A4);
AnalogIn pr5(A5);
float wd = 0.0;//Wood distance　木材検知のしきい値

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

        printf("%d\n",select);
        //緊急停止
        if(select){
            sig = 1;
        }

        if(start){
            sig = 0;
        }


        moved_asimawari = 0;


//エアシリンダー（タイヤ上げ用）
        if(button_sankaku){
            if(iswood(pr0, pr1)){
                air1 = 1;//前方輪上げ
                if(iswood(pr2, pr3)){
                    air1 = 0;
                    air2 = 1;
                    if (iswood(pr4, pr5)) {
                        air2 = 0;
                    }
                }
            }



        }else {
            air1 = 0;
            air2 = 0;
        }


//旋回
        //機体左に旋回
        if(L1 && !moved_asimawari){
            send_all(forward, forward, back, back);
            moved_asimawari = 1;
        }

        //機体右に旋回
        if(R1 && !moved_asimawari){
            send_all(back, back, forward, forward);
            moved_asimawari = 1;
        }


//移動
        //前移動
        if(button_ue && !moved_asimawari){
            send_all(forward, forward, forward, forward);
            moved_asimawari = 1;
        }

        //右移動
        if(button_migi && !moved_asimawari){
            send_all(back, forward, back, forward);
            moved_asimawari = 1;
        }

        //左移動
        if(button_hidari && !moved_asimawari){
            send_all(forward, back, forward, back);
            moved_asimawari = 1;
        }

        //後移動
        if(button_sita && !moved_asimawari){
            send_all(back, back, back, back);
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
int iswood(AnalogIn pr_left, AnalogIn pr_right){
    //R2L2同時押しでキャンセル
    while(!(R2 && L2) && !select){
        if((pr_right*5)<wd && (pr_left*5)<wd){//前のフォトリフレクタがどちらも検知したら
            return 1;
        }else if((pr_right*5)<wd){//右前だけ検知
            send_all(stop, back, forward, stop);//前方軸に時計回り
        }else if((pr_left*5)<wd){//左前だけ検知
            send_all(stop, forward, back, stop);//前方軸に反時計回り
        }else{
            send_all(forward, forward, forward, forward);//直進
        }
        receive_data();
    }//while
    return 0;
}