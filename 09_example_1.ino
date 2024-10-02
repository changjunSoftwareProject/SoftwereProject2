// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.3    // EMA weight of new sample (range: 0 to 1)
                          // Setting EMA to 1 effectively disables EMA filter.

// global variables
unsigned long last_sampling_time;   // unit: msec
float dist_prev = _DIST_MAX;        // Distance last-measured
float dist_filtered = _DIST_MAX;        // Distance last-measured
float dist_ema;                     // EMA distance
float dist_median;    // median distance
const int N = 30; //중위수 샘플수
int count = 0;
float lastNArray[N];
float findMedianArray[N];
int temp;
int temp2;

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  for (int i=0; i < N; i++){
    findMedianArray[i] = 0;
    lastNArray[i] = 0;
  }

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float dist_raw;
  if (millis() < last_sampling_time + INTERVAL)
    return;
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);
  
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX)) {
      dist_raw = _DIST_MAX;
  } else if (dist_raw < _DIST_MIN) {
      dist_raw = _DIST_MIN;
  }
  //---------------------------------------------------------------------- filtered및 ema 계산
  if (abs(dist_raw - dist_prev) < 50) {
    dist_prev = dist_raw;
    dist_filtered = dist_raw;
  }
  else {
    dist_filtered = dist_prev;
  }
  dist_ema = _EMA_ALPHA * dist_filtered + (1 - _EMA_ALPHA) * dist_ema;
  //---------------------------------------------------------------------중위수 계산 (filtered 사용 안함)
    temp = lastNArray[count];
  lastNArray[count] = int(dist_raw);
  
  for (int i = 0;;i++){
    if (findMedianArray[i] == temp){
      findMedianArray[i] = lastNArray[count];
      if (temp < dist_raw){
        for (int j = i; j < N - 1;j++){
          if (findMedianArray[j] > findMedianArray[j+1]){
            temp2 = findMedianArray[j];
            findMedianArray[j] = findMedianArray[j+1];
            findMedianArray[j+1] = temp2;
          }
          else{
            break;
          }
        }
      }
      else if (temp > dist_raw){
        for (int j = i; j >= 1;j--){
          if (findMedianArray[j] < findMedianArray[j-1]){
            temp2 = findMedianArray[j];
            findMedianArray[j] = findMedianArray[j-1];
            findMedianArray[j-1] = temp2;
          }
          else{
            break;
          }
        }
      }
    break;
    }
  }
  count += 1;
  if (count >= N){
    count = 0;
  }
  dist_median = (N % 2 == 0)? (findMedianArray[N / 2] + findMedianArray[(N / 2) - 1]) / 2 : findMedianArray[N / 2];
  //--------------------------------------------------------------------------------------------------------------------------
  // output the distance to the serial port
  Serial.print("Min:");   Serial.print(_DIST_MIN); 
  Serial.print(",raw:");  Serial.print(dist_raw);
  Serial.print(",median:");  Serial.print(dist_median); 
  Serial.print(",ema:");  Serial.print(dist_ema);
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
