#define BLINKER_WIFI
#define BLINKER_ESP_SMARTCONFIG
#define BLINKER_MIOT_LIGHT
#define WIFIRESET  13
#include <Blinker.h>
#include "OneButton.h"

int ledPin = 0;
char auth[] = "3d2a5f051235";
int counter = 0;
bool result = Blinker.connected();

OneButton button(WIFIRESET, true);

BlinkerButton Button1("btn-abc");
BlinkerNumber Number1("num-abc");
BlinkerButton Button2("btn-smartconfig");



void button2_callback(const String & state) {
    deviceReset();
}

void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);



    if (state == "on") {
      digitalWrite(ledPin,HIGH);
      Button1.text("light on");
      Button1.print("on");
    }else {
      digitalWrite(ledPin,LOW);
      Button1.text("light off");
      Button1.print("off");
    }
}

// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);
    Blinker.vibrate();
    uint32_t BlinkerTime = millis();
    Blinker.print("millis", BlinkerTime);
    counter++;
    Number1.print(counter);
}

void miotPowerState(const String & state )
{
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
    }
    else if (state == BLINKER_CMD_OFF) {
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
    }
}
//重置WIFI
void deviceReset(){
   Blinker.reset();
}

void setup()
{
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);
  Serial.print("----------------------------1------------------------------------------");
  Serial.print(result);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Blinker.begin(auth);
  delay(2000);
  Serial.print("----------------------------2------------------------------------------");
  Serial.print(result);
  button.attachLongPressStop(deviceReset);
  Button1.attach(button1_callback);//Button1这个组件被触发就执行这个回调
  Button2.attach(button2_callback);//Button1这个组件被触发就执行这个回调
  Blinker.attachData(dataRead);//如果没有被绑定的组件被触发就执行这个回调
  BlinkerMIOT.attachPowerState(miotPowerState);

}

void loop()
{
  Blinker.run();    
  button.tick();

}