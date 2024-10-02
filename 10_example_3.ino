/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/faq/how-to-control-speed-of-servo-motor
 */
#include <math.h>
#include <Servo.h>

#define PIN_SERVO 10

Servo myServo;
unsigned long MOVING_TIME = 2000; // moving time is 3 seconds
unsigned long moveStartTime;
int startAngle = 10; // 30°
int stopAngle  = 170; // 90°
//더 잘 보이게 하기 위해서 회전각을 키움
float strengthSigmoid = 10.0; //클수록 sigmoid가 급격해짐


void setup() {
  myServo.attach(PIN_SERVO);

  myServo.write(startAngle); // Set position
  delay(1500);
  moveStartTime = millis(); // start moving
  
  Serial.begin(57600);

}

void loop() {
  unsigned long progress = MOVING_TIME / (1 + exp((millis() - moveStartTime) * strengthSigmoid * -2 / MOVING_TIME + strengthSigmoid));
  
  if (progress <= MOVING_TIME) {
    long angle = map(progress, 0, MOVING_TIME , startAngle, stopAngle);
    myServo.write(angle); 
  }
  Serial.println(progress);
  if (progress > MOVING_TIME) {
    delay(10000000);
  }
}
