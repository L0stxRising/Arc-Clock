#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <DHT.h>

//==================================================
// PINS AS VARIABLES - CHANGE EASILY HERE!
//==================================================
int pin_buzz=4;//buzzer gpio 4
int pin_touch=19;//ttp223 touch sensor gpio 19
int pin_dht=21;//dht11 data gpio 21
int pin_b1=32;//cherry mx button 1 (up/hour)
int pin_b2=33;//cherry mx button 2 (down/min)
int pin_b3=22;//cherry mx button 3 (mode cycle)
int pin_b4=23;//cherry mx button 4 (action/alarm dismiss)
int pin_mosi=13;//sda
int pin_sclk=14;//scl
int pin_cs=27;//cs
int pin_dc=26;//dc
int pin_bl=25;//backlight
int pin_rst=-1;//reset pin (-1 if tied to 3.3v)

//obselete old pins from prototype breadboard:
//int old_sclk=9;
//int old_mosi=10;
//int old_rst=8;
//int old_dc=6;
//int old_cs=7;
//int old_bl=21;
//int old_sw1=5;
//int old_sw2=4;
//int old_sw3=3;
//int old_sw4=2;
//int old_buzz=20;

//wifi settings
const char* ssid="YOUR_WIFI_NAME";
const char* pass="YOUR_WIFI_PASSWORD";
//const char* old_ssid="TonyStark_Lab";
//const char* old_pass="iamironman3000";

long gmt_offset_sec=19800;//UTC+5:30 (India) or change to your timezone
int daylight_offset_sec=0;

//subclass to expose offsets for the weird 1.72 / 2.25 inch 76x284 display
class MyST7789:public Adafruit_ST7789{
public:
MyST7789(int8_t cs,int8_t dc,int8_t mosi,int8_t sclk,int8_t rst)
:Adafruit_ST7789(cs,dc,mosi,sclk,rst){}
void setOffsets(uint8_t col,uint8_t row){
_colstart=_colstart2=col;
_rowstart=_rowstart2=row;
}
};

MyST7789 tft(pin_cs,pin_dc,pin_mosi,pin_sclk,pin_rst);
WebServer server(80);
#define DHTTYPE DHT11
DHT dht(pin_dht,DHTTYPE);


//variables
int hours=12;
int mins=0;
int secs=0;
int alarm_h=7;
int alarm_m=0;
bool alarm_on=false;
bool alarm_ringing=false;
bool alarm_triggered_this_minute=false;

float temp_c=25.0;
float humi=50.0;
unsigned long last_dht=0;
unsigned long last_sec=0;
unsigned long last_btn_time=0;

int ui_mode=0;//0=clock hud, 1=sensors dash, 2=ironman game, 3=alarm
bool wifi_ok=false;
int bl_on=LOW;//active low backlight on this panel
int bl_off=HIGH;
bool bl_state=true;

//mini game state ("ARC DODGER")
int player_y=35;
int obs_x[2]={200,290};
int obs_y[2]={25,50};
int obs_spd=4;
int game_score=0;
int high_score=0;
bool game_over=false;
unsigned long last_game_frame=0;

//last button states
bool last_b1=HIGH;
bool last_b2=HIGH;
bool last_b3=HIGH;
bool last_b4=HIGH;
bool last_touch=LOW;


//html web dashboard for phone/browser
const char HTML_PAGE[] PROGMEM=R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>STARK // JARVIS HUD</title>
<style>
body{background:#0a0b12;color:#e0f7fa;font-family:'Courier New',monospace;margin:0;padding:15px;text-align:center;}
h1{color:#00e5ff;text-shadow:0 0 10px #00e5ff;letter-spacing:3px;font-size:22px;margin-bottom:5px;}
.sub{color:#ffd700;font-size:12px;margin-bottom:20px;}
.card{background:#111522;border:1px solid #00e5ff;box-shadow:0 0 10px rgba(0,229,255,0.2);border-radius:8px;padding:15px;margin:10px auto;max-width:380px;}
.val{font-size:32px;font-weight:bold;color:#ffd700;margin:5px 0;}
.row{display:flex;justify-content:space-around;margin:10px 0;}
.stat{flex:1;}
.stat-val{font-size:24px;color:#00e5ff;font-weight:bold;}
button{background:transparent;border:2px solid #ff1744;color:#ff1744;padding:10px 16px;margin:6px;border-radius:6px;font-size:14px;font-weight:bold;cursor:pointer;transition:all 0.3s;}
button:hover{background:#ff1744;color:#fff;box-shadow:0 0 15px #ff1744;}
.btn-c{border-color:#00e5ff;color:#00e5ff;}
.btn-c:hover{background:#00e5ff;color:#000;box-shadow:0 0 15px #00e5ff;}
.btn-g{border-color:#ffd700;color:#ffd700;}
.btn-g:hover{background:#ffd700;color:#000;box-shadow:0 0 15px #ffd700;}
</style>
</head>
<body>
<h1>STARK INDUSTRIES</h1>
<div class="sub">MARK LXXXV // JARVIS HUD v4.2</div>
<div class="card">
<div>CURRENT CLOCK</div>
<div class="val" id="time">--:--:--</div>
<div class="row">
<div class="stat"><div>TEMP</div><div class="stat-val" id="temp">-- C</div></div>
<div class="stat"><div>HUMIDITY</div><div class="stat-val" id="hum">-- %</div></div>
</div>
</div>
<div class="card">
<div>QUICK CONTROLS</div>
<button onclick="fetch('/api/buzz')">REPULSOR FIRE</button>
<button class="btn-c" onclick="fetch('/api/mode')">SWITCH SCREEN</button>
<button class="btn-g" onclick="fetch('/api/bl')">TOGGLE BL</button>
</div>
<script>
function upd(){
fetch('/api/data').then(r=>r.json()).then(d=>{
document.getElementById('time').innerText=(d.hours<10?'0':'')+d.hours+':'+(d.mins<10?'0':'')+d.mins+':'+(d.secs<10?'0':'')+d.secs;
document.getElementById('temp').innerText=d.temp+' C';
document.getElementById('hum').innerText=d.hum+' %';
});
}
setInterval(upd,1500);upd();
</script>
</body>
</html>
)rawliteral";


void beep(int ms=30){
digitalWrite(pin_buzz,HIGH);
delay(ms);
digitalWrite(pin_buzz,LOW);
}

void repulsorTone(){
//iron man repulsor charge & blast sound fx
for(int f=200;f<1200;f+=100){
digitalWrite(pin_buzz,HIGH);
delayMicroseconds(500000/f);
digitalWrite(pin_buzz,LOW);
delayMicroseconds(500000/f);
}
beep(40);
}

void setBacklight(bool on){
bl_state=on;
digitalWrite(pin_bl,on?bl_on:bl_off);
}

//arc reactor drawing
void drawArcReactor(int cx,int cy,int step){
tft.drawCircle(cx,cy,20,0x07FF);//cyan
tft.drawCircle(cx,cy,19,0x041F);
tft.drawCircle(cx,cy,14,0xFFFF);//white
tft.drawCircle(cx,cy,13,0x07FF);
tft.fillCircle(cx,cy,6,0x07FF);//core
tft.fillCircle(cx,cy,3,0xFFFF);
if(step%2==0){
tft.drawFastHLine(cx-18,cy,36,0x07FF);
tft.drawFastVLine(cx,cy-18,36,0x07FF);
}else{
tft.drawLine(cx-12,cy-12,cx+12,cy+12,0x07FF);
tft.drawLine(cx-12,cy+12,cx+12,cy-12,0x07FF);
}
//tft.drawCircle(cx,cy,24,0xF800); //old outer border looked bad
}


//MODE 0: IRON MAN ARC CLOCK HUD
void drawClockHUD(){
static int arc_step=0;
arc_step++;
drawArcReactor(24,38,arc_step);

//top header
tft.drawFastHLine(0,10,284,0xF800);
tft.setTextSize(1);
tft.setTextColor(0x07FF,0x0000);
tft.setCursor(4,1);
tft.print("MARK LXXXV HUD");
tft.setTextColor(0xFFE0,0x0000);
tft.setCursor(95,1);
tft.print("STARK INDUSTRIES");
tft.setTextColor(wifi_ok?0x07E0:0xF800,0x0000);
tft.setCursor(222,1);
tft.print(wifi_ok?"NET:ONLINE":"NET:OFFLINE");

//big time
tft.setTextSize(4);
tft.setTextColor(0xFFE0,0x0000);//gold
tft.setCursor(55,18);
if(hours<10)tft.print("0");
tft.print(hours);
tft.print(":");
if(mins<10)tft.print("0");
tft.print(mins);

//seconds
tft.setTextSize(2);
tft.setTextColor(0xF800,0x0000);//red
tft.setCursor(182,20);
tft.print(":");
if(secs<10)tft.print("0");
tft.print(secs);

//dht11 telemetry
tft.setTextSize(1);
tft.setTextColor(0xFFE0,0x0000);
tft.setCursor(226,16);
tft.printf("%.0fC",temp_c);
tft.setTextColor(0x07FF,0x0000);
tft.setCursor(226,28);
tft.printf("%.0f%%",humi);
tft.setTextColor(alarm_on?0x07E0:0x4208,0x0000);
tft.setCursor(226,40);
tft.print(alarm_on?"ALM:ON":"ALM:OFF");

//bottom bar
tft.drawFastHLine(0,62,284,0x041F);
tft.setTextSize(1);
tft.setTextColor(0xFFFF,0x0000);
tft.setCursor(4,65);
tft.print("JARVIS OS 4.2");
tft.setTextColor(0x07E0,0x0000);
tft.setCursor(130,65);
tft.print("ARC POWER: 3000% STABLE");
}


//MODE 1: TELEMETRY DASHBOARD
void drawDashboard(){
tft.drawFastHLine(0,10,284,0x07FF);
tft.setTextSize(1);
tft.setTextColor(0xFFE0,0x0000);
tft.setCursor(4,1);
tft.print("== STARK TELEMETRY DASHBOARD ==");

tft.setTextColor(0xFFFF,0x0000);
tft.setCursor(4,16);
tft.printf("TEMPERATURE: %.1f C",temp_c);
tft.drawRect(4,26,110,5,0x4208);
int tw=map(constrain((int)temp_c,0,50),0,50,0,108);
tft.fillRect(5,27,tw,3,0xF800);

tft.setCursor(4,35);
tft.printf("HUMIDITY   : %.1f %%",humi);
tft.drawRect(4,45,110,5,0x4208);
int hw=map(constrain((int)humi,0,100),0,100,0,108);
tft.fillRect(5,46,hw,3,0x07FF);

tft.setCursor(130,16);
tft.setTextColor(0x07FF,0x0000);
if(wifi_ok){
tft.printf("IP:%s",WiFi.localIP().toString().c_str());
}else{
tft.print("IP: NO CONNECTION");
}

tft.setCursor(130,28);
tft.setTextColor(0xFFFF,0x0000);
tft.printf("TOUCH SENSOR: %s",digitalRead(pin_touch)?"DETECTED":"IDLE");

tft.setCursor(130,40);
tft.setTextColor(0xFFE0,0x0000);
tft.printf("UPTIME : %lum %lus",millis()/60000,(millis()/1000)%60);

tft.setCursor(130,52);
tft.setTextColor(0x07E0,0x0000);
tft.printf("FREE RAM: %d KB",ESP.getFreeHeap()/1024);

tft.drawFastHLine(0,63,284,0x001F);
tft.setCursor(4,65);
tft.setTextColor(0x07FF,0x0000);
tft.print("B3:NEXT SCREEN  TOUCH:REPULSOR TONE");
}


//MODE 2: IRON MAN GAME ("ARC DODGER")
void updateGame(){
if(game_over){
tft.setTextColor(0xF800,0x0000);
tft.setTextSize(2);
tft.setCursor(25,22);
tft.print("SUIT COMPROMISED!");
tft.setTextSize(1);
tft.setTextColor(0xFFE0,0x0000);
tft.setCursor(55,46);
tft.printf("SCORE: %d   BEST: %d",game_score,high_score);
tft.setCursor(45,58);
tft.setTextColor(0x07FF,0x0000);
tft.print("PRESS B4 TO REBOOT SUIT");
return;
}

//erase old player
tft.fillTriangle(20,player_y,10,player_y-5,10,player_y+5,0x0000);

if(!digitalRead(pin_b1)){player_y-=3;}//thrust up
if(!digitalRead(pin_b2)){player_y+=3;}//dive down
if(player_y<16)player_y=16;
if(player_y>60)player_y=60;

//repulsor blast on touch sensor!
if(digitalRead(pin_touch)){
tft.drawFastHLine(22,player_y,260,0x07FF);
beep(15);
for(int i=0;i<2;i++){
if(abs(obs_y[i]-player_y)<8){
tft.fillRect(obs_x[i]-3,obs_y[i]-3,8,8,0x0000);
obs_x[i]=280+random(20,60);
obs_y[i]=random(15,58);
game_score+=5;
}
}
delay(15);
tft.drawFastHLine(22,player_y,260,0x0000);
}

//draw iron man icon
tft.fillTriangle(20,player_y,10,player_y-5,10,player_y+5,0xF800);
tft.drawPixel(21,player_y,0x07FF);
tft.drawPixel(10,player_y,0xFFE0);

//move & draw obstacles (enemy missiles)
for(int i=0;i<2;i++){
tft.fillRect(obs_x[i]-3,obs_y[i]-3,7,7,0x0000);
obs_x[i]-=obs_spd;
if(obs_x[i]<5){
obs_x[i]=280+random(10,50);
obs_y[i]=random(15,58);
game_score++;
if(game_score>high_score)high_score=game_score;
}
tft.fillRect(obs_x[i]-2,obs_y[i]-2,5,5,0xF800);
tft.drawPixel(obs_x[i],obs_y[i],0xFFFF);

//collision
if(abs(obs_x[i]-15)<7&&abs(obs_y[i]-player_y)<6){
game_over=true;
beep(250);
return;
}
}

tft.drawFastHLine(0,10,284,0x07FF);
tft.setTextSize(1);
tft.setTextColor(0xFFE0,0x0000);
tft.setCursor(4,1);
tft.print("ARC FLIGHT // B1:UP B2:DN TCH:BLAST");
tft.setCursor(220,1);
tft.printf("SC:%03d",game_score);
}


//MODE 3: ALARM & SOUND TEST
void drawAlarmScreen(){
tft.drawFastHLine(0,10,284,0xF800);
tft.setTextSize(1);
tft.setTextColor(0xFFE0,0x0000);
tft.setCursor(4,1);
tft.print("== JARVIS ALARM PROTOCOL ==");

tft.setTextSize(4);
tft.setTextColor(0xFFE0,0x0000);
tft.setCursor(50,18);
if(alarm_h<10)tft.print("0");
tft.print(alarm_h);
tft.print(":");
if(alarm_m<10)tft.print("0");
tft.print(alarm_m);

tft.setTextSize(2);
tft.setTextColor(alarm_on?0x07E0:0xF800,0x0000);
tft.setCursor(190,24);
tft.print(alarm_on?"[ON]":"[OFF]");

tft.drawFastHLine(0,55,284,0x4208);
tft.setTextSize(1);
tft.setTextColor(0x07FF,0x0000);
tft.setCursor(4,60);
tft.print("B1:+HR  B2:+MIN  B4:TOGGLE  TOUCH:REPULSOR TEST");
}


void initGame(){
player_y=35;
obs_x[0]=220;
obs_y[0]=random(15,58);
obs_x[1]=310;
obs_y[1]=random(15,58);
game_score=0;
game_over=false;
tft.fillScreen(ST77XX_BLACK);
}


void updateClockTime(){
unsigned long now=millis();
if(now-last_sec>=1000){
last_sec+=1000;
secs++;
if(secs>=60){secs=0;mins++;}
if(mins>=60){mins=0;hours++;}
if(hours>=24){hours=0;}
alarm_triggered_this_minute=false;

//sync from ntp occasionally if wifi is on
if(wifi_ok&&secs==0&&(mins%15==0)){
struct tm t;
if(getLocalTime(&t)){
hours=t.tm_hour;
mins=t.tm_min;
secs=t.tm_sec;
}
}
}
}


void readSensors(){
if(millis()-last_dht>2000){
last_dht=millis();
float t=dht.readTemperature();
float h=dht.readHumidity();
if(!isnan(t))temp_c=t;
if(!isnan(h))humi=h;
//Serial.printf("DHT t=%.1f h=%.1f\n",temp_c,humi);
}
}


void checkAlarm(){
if(alarm_on&&!alarm_ringing&&hours==alarm_h&&mins==alarm_m&&!alarm_triggered_this_minute){
alarm_ringing=true;
alarm_triggered_this_minute=true;
digitalWrite(pin_buzz,HIGH);
}
if(alarm_ringing){
//pulsing buzzer for alarm
static unsigned long last_buzz_pulse=0;
if(millis()-last_buzz_pulse>150){
last_buzz_pulse=millis();
digitalWrite(pin_buzz,!digitalRead(pin_buzz));
}
}
}


void handleButtons(){
bool b1=digitalRead(pin_b1);
bool b2=digitalRead(pin_b2);
bool b3=digitalRead(pin_b3);
bool b4=digitalRead(pin_b4);
bool touch=digitalRead(pin_touch);

unsigned long now=millis();
if(now-last_btn_time<80)return;

//dismiss ringing alarm on any button or touch
if(alarm_ringing){
if(b1==LOW||b2==LOW||b3==LOW||b4==LOW||touch==HIGH){
alarm_ringing=false;
digitalWrite(pin_buzz,LOW);
last_btn_time=now;
tft.fillScreen(ST77XX_BLACK);
return;
}
}

//Button 3: Cycle UI mode (Clock -> Dash -> Game -> Alarm)
if(last_b3==HIGH&&b3==LOW){
last_btn_time=now;
ui_mode=(ui_mode+1)%4;
tft.fillScreen(ST77XX_BLACK);
beep(15);
if(ui_mode==2)initGame();
}

//Button 4: Action / restart game / toggle alarm
if(last_b4==HIGH&&b4==LOW){
last_btn_time=now;
if(ui_mode==2&&game_over){
initGame();
}else if(ui_mode==3){
alarm_on=!alarm_on;
beep(25);
tft.fillScreen(ST77XX_BLACK);
}else{
beep(15);
}
}

//Button 1 & 2 in alarm mode
if(ui_mode==3){
if(last_b1==HIGH&&b1==LOW){
last_btn_time=now;
alarm_h=(alarm_h+1)%24;
beep(15);
}
if(last_b2==HIGH&&b2==LOW){
last_btn_time=now;
alarm_m=(alarm_m+5)%60;
beep(15);
}
}

//Touch sensor: repulsor tone in clock/dash/alarm modes
if(last_touch==LOW&&touch==HIGH){
last_btn_time=now;
if(ui_mode!=2){
repulsorTone();
}
}

last_b1=b1;
last_b2=b2;
last_b3=b3;
last_b4=b4;
last_touch=touch;
}


//web server endpoints
void setupWebServer(){
server.on("/",[](){
server.send_P(200,"text/html",HTML_PAGE);
});

server.on("/api/data",[](){
String json="{\"hours\":"+String(hours)+",\"mins\":"+String(mins)+",\"secs\":"+String(secs)+",\"temp\":"+String(temp_c,1)+",\"hum\":"+String(humi,1)+",\"mode\":"+String(ui_mode)+",\"alarm_h\":"+String(alarm_h)+",\"alarm_m\":"+String(alarm_m)+",\"alarm_on\":"+String(alarm_on?"true":"false")+"}";
server.send(200,"application/json",json);
});

server.on("/api/buzz",[](){
repulsorTone();
server.send(200,"text/plain","REPULSOR FIRED");
});

server.on("/api/mode",[](){
ui_mode=(ui_mode+1)%4;
tft.fillScreen(ST77XX_BLACK);
if(ui_mode==2)initGame();
server.send(200,"text/plain","MODE CHANGED");
});

server.on("/api/bl",[](){
setBacklight(!bl_state);
server.send(200,"text/plain",bl_state?"BL ON":"BL OFF");
});

server.begin();
//Serial.println("Web server started");
}



void setup(){
Serial.begin(115200);
//Serial.println("STARK INDUSTRIES CLOCK BOOTING...");

pinMode(pin_b1,INPUT_PULLUP);
pinMode(pin_b2,INPUT_PULLUP);
pinMode(pin_b3,INPUT_PULLUP);
pinMode(pin_b4,INPUT_PULLUP);
pinMode(pin_touch,INPUT);

pinMode(pin_buzz,OUTPUT);
digitalWrite(pin_buzz,LOW);

pinMode(pin_bl,OUTPUT);
setBacklight(true);

//old screen init test:
//tft.init(240,320);
//tft.setOffsets(50,20);

//init the 76x284 long strip ST7789 panel
tft.init(76,284);
tft.setOffsets(82,18);
tft.invertDisplay(false);
tft.setRotation(1);//landscape 284x76

tft.fillScreen(ST77XX_BLACK);

//boot logo
tft.setTextColor(0x07FF);
tft.setTextSize(2);
tft.setCursor(20,25);
tft.print("STARK INDUSTRIES");
tft.setTextSize(1);
tft.setTextColor(0xFFE0);
tft.setCursor(45,48);
tft.print("JARVIS OS v4.2 LOADING...");
repulsorTone();
delay(1000);

dht.begin();


//connect wifi
WiFi.mode(WIFI_STA);
WiFi.begin(ssid,pass);
unsigned long start_wf=millis();
while(WiFi.status()!=WL_CONNECTED&&millis()-start_wf<6000){
delay(250);
}

if(WiFi.status()==WL_CONNECTED){
wifi_ok=true;
configTime(gmt_offset_sec,daylight_offset_sec,"pool.ntp.org","time.google.com");
struct tm t;
if(getLocalTime(&t,5000)){
hours=t.tm_hour;
mins=t.tm_min;
secs=t.tm_sec;
}
setupWebServer();
}else{
wifi_ok=false;
//WiFi.disconnect();
}

tft.fillScreen(ST77XX_BLACK);
}



void loop(){
updateClockTime();
readSensors();
handleButtons();
checkAlarm();

if(wifi_ok){
server.handleClient();
}


//render active UI screen
if(ui_mode==2){
//game loop at ~30fps
if(millis()-last_game_frame>=35){
last_game_frame=millis();
updateGame();
}
}else{
//clock / dashboard / alarm refresh
static unsigned long last_render=0;
if(millis()-last_render>=400){
last_render=millis();
if(ui_mode==0){
drawClockHUD();
}else if(ui_mode==1){
drawDashboard();
}else if(ui_mode==3){
drawAlarmScreen();
}
}
}

}
