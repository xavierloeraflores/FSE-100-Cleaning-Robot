

//Standby Duration before Cleaning
const int standByTime = 30000;
int standByCounter;
//Length of the cleaning Space
const int spaceLength = 20; 
//Distnace metric
enum metric {centimeters, inches};
const metric spaceMetric = inches;


enum status{ standby, cleaning, docking };
status robotStatus;
int dockingDistance;

bool isPoweredOn;


const int motorPinA1  = 5;  
const int motorPinA2  = 6;  
const int motorPinB1  = 10; 
const int motorPinB2  = 9;
const int motorPinC = 3;
const int distancePin = 7;
const int slideSwitchPin = 4;
const int ledPin = 2;


void setDockDistance(){
  if(spaceMetric == inches){
  	dockingDistance = 2 ;	
  } else if(spaceMetric == centimeters){
  	dockingDistance = 5;
  } 
}



void setup(){
  Serial.begin(9600);
  pinMode(motorPinA1, OUTPUT);
  pinMode(motorPinA2, OUTPUT);
  pinMode(motorPinB1, OUTPUT);
  pinMode(motorPinB2, OUTPUT);
  pinMode(motorPinC, OUTPUT);
  pinMode(slideSwitchPin, INPUT);
  pinMode(ledPin, OUTPUT);
  setDockDistance();
  standByCounter = 0;
  
}

void loop()
{
  powerLoop();
  
  wheelsLoop();
  distanceLoop();
  sweeperLoop();
  ledLoop();
  
  standByLoop();  
  
  
  printStatus();
}






//----------------------------------------
//----------------------------------------
//----------------------------------------
//-------------Logic Loops-----------------
void powerLoop(){
  bool powerSwitchOn = slideSwitchState();
  if(powerSwitchOn){
  isPoweredOn=true;
  }else if(!powerSwitchOn){
    isPoweredOn = false;
    robotStatus = standby;
  }
}



void standByLoop(){
  if (robotStatus == standby){
    delay(100);
    standByCounter+=100;
    Serial.print(standByCounter);
    Serial.print("/");
    Serial.println(standByTime);
    if(standByCounter>standByTime){
      robotStatus = cleaning;
      standByCounter = 0;
    }
  }
}

void wheelsLoop()
{
  if(robotStatus == cleaning){
  	wheelsFoward();
  }else if(robotStatus == standby){
    wheelsStop();
  }else if(robotStatus == docking ){
    wheelsBackward();
  }
}


void distanceLoop()
{
  long distance;
  distance = getDistance();
  Serial.print("distance:");
  Serial.println(distance);
  delay(100);
  
  if(robotStatus == cleaning){
    if (distance > spaceLength){
      robotStatus = docking;
    }
  }else if(robotStatus == standby && isPoweredOn){
    delay(1000);
    robotStatus = cleaning;
  }else if(robotStatus == docking ){
    if (distance <= dockingDistance){
      robotStatus = standby;
    }
  }
}

void ledLoop()
{
  if(robotStatus == cleaning){
	turnOnLED();
  }else if(robotStatus == standby){
	turnOffLED();
  }else if(robotStatus == docking ){
    flashLED();
  }
  
  
}

void sweeperLoop()
{
  if(robotStatus == cleaning){
  	sweeperSweep();
  }else if(robotStatus == standby || robotStatus == docking){
    sweeperStop();
  }
}


//----------------------------------------
//----------------------------------------
//----------------------------------------
//-------------Slide Switch---------------
bool slideSwitchState(){
	return digitalRead(slideSwitchPin);
}

//----------------------------------------
//----------------------------------------
//----------------------------------------
//-------------LED Light------------------
void flashLED(){
  turnOnLED();
  delay(15);
  turnOffLED();
}
void turnOnLED(){
  digitalWrite(ledPin, HIGH);
}
void turnOffLED(){
  digitalWrite(ledPin, LOW);
}


//----------------------------------------
//----------------------------------------
//----------------------------------------
//-------------Distance Sensor------------

// See: https://www.parallax.com/package/ping-ultrasonic-distance-sensor-downloads/
long getDistance(){
  long duration, distance;
  duration = getUltrasonicDuration();
  distance = durationToDistance(duration);
}

long getUltrasonicDuration(){
  long duration;

  // Write Signal
  pinMode(distancePin, OUTPUT);
  digitalWrite(distancePin, LOW);
  delayMicroseconds(2);
  digitalWrite(distancePin, HIGH);
  delayMicroseconds(5);
  digitalWrite(distancePin, LOW);

  //Read Signal
  pinMode(distancePin, INPUT);
  duration = pulseIn(distancePin, HIGH);

  return duration;
}

long durationToDistance(long duration){
  long distance, in, cm;
  if(spaceMetric == inches){
    in = microsecondsToInches(duration);
  	distance = in;
  } else if(spaceMetric == centimeters){
  	cm = microsecondsToCentimeters(duration);
    distance = cm;
  }
  return distance;
}

    
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}




//----------------------------------------
//----------------------------------------
//----------------------------------------
//-------------Raw Sweeper----------------
void sweeperStop(){
  digitalWrite(motorPinC, HIGH);
}

void sweeperSweep(){
  digitalWrite(motorPinC, LOW);
}


//----------------------------------------
//----------------------------------------
//----------------------------------------
//-------------Raw Wheels-----------------
void wheelsStop(){
  digitalWrite(motorPinA1, LOW);
  digitalWrite(motorPinA2, LOW);
  digitalWrite(motorPinB1, LOW);
  digitalWrite(motorPinB2, LOW);
}
void wheelsFoward() {
  wheelAFoward();
  wheelBFoward();
}
void wheelsBackward(){
  wheelABackward();
  wheelBBackward();

}
void wheelAFoward() {
  digitalWrite(motorPinA1, HIGH);
  digitalWrite(motorPinA2, LOW);
}

void wheelABackward(){
  digitalWrite(motorPinA1, LOW);
  digitalWrite(motorPinA2, HIGH);
}

void wheelBFoward(){
  digitalWrite(motorPinB1, HIGH);
  digitalWrite(motorPinB2, LOW);
}

void wheelBBackward(){
  digitalWrite(motorPinB1, LOW);
  digitalWrite(motorPinB2, HIGH);
}


//----------------------------------------
//----------------------------------------
//----------------------------------------
//-------------Debug----------------------
void printStatus(){
  if(robotStatus == cleaning){
    Serial.println("cleaning");
  }else if(robotStatus == standby){
    Serial.println("standby");
  }else if(robotStatus == docking ){
    Serial.println("docking");
  }
}

