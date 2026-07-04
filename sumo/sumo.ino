#include <Wire.h>
#include <BluetoothSerial.h>
#include <string.h>

const char* device_name = "hornet_sumo1";

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;

#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define motor_int1 23
#define motor_int2 22
#define motor_int3 21
#define motor_int4 19


int motors[] = {motor_int1, motor_int2, motor_int3, motor_int4};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(100);
  Wire.setClock(100000); // Normal clock hz
  delay(100); // allow i2c device to initialize
  for (int i = 0; i < (sizeof(motors) / sizeof(motors[0])); i++) {
      analogWriteFrequency(motors[i], 100);

  }

  // begin bluetooth connection
  //SerialBT.begin(device_name);
  delay(500); // bluetooth amirite haha
  Serial.println("setup complete");
}


// Moves the wheels and sets the position of the fork
void execute_move(int motor1_speed, int motor2_speed, int ang) {

  if (motor1_speed == 0) {
    analogWrite(motor_int1, 0);
    analogWrite(motor_int2, 0);
  } else if (motor1_speed > 0) {
      analogWrite(motor_int1, motor1_speed);
      analogWrite(motor_int2, 0);
  } else {
      analogWrite(motor_int1, 0);
      analogWrite(motor_int2, motor1_speed);
  }
  
  if (motor2_speed == 0) {
    analogWrite(motor_int3, 0);
    analogWrite(motor_int4, 0);
  } else if (motor2_speed > 0) {
      analogWrite(motor_int3, motor1_speed);
      analogWrite(motor_int4, 0);
  } else {
      analogWrite(motor_int3, 0);
      analogWrite(motor_int4, motor1_speed);
  }

  // TODO set servo
}

// "#" -> start byte
#define MSG_START '#'
#define MSG_DELIN '+'
#define MSG_END '$'
// "$" -> end byte
// motor 1 is the left motor where -1 means reverse, 0 means stopped, 1 means forwards, same for motor2
// example packet with each byte in brackets: <start><int motor1><delin><int motor2><delin><int ang><end>
int motor1;
int motor2;
void loop() { 
  /*
  
  while (SerialBT.available()) {
    if (SerialBT.read() == MSG_START) {
      motor1 = SerialBT.read();
      } if (SerialBT.read() == MSG_DELIN) {
        motor2 = SerialBT.read();
        if (SerialBT.read() == MSG_DELIN) {
          ang = SerialBT.read();
          if (SerialBT.read() == MSG_END) {
            execute_move(motor1, motor2, ang);
          }
        }
      }
    }
  delay(20); // 50 hz = 20ms period
  */ 
  // execute_move(1, 1, 150);
  execute_move(50, 50, 100);
  delay(100);
}

