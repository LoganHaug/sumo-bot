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

#define motor_int1 23
#define motor_int2 22
#define motor_int3 21
#define motor_int4 19
#define servo 18

#define PWM_FREQ 500 // hz
#define PWM_RESOLUTION 16 // bits

// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);


int motors[] = {motor_int1, motor_int2, motor_int3, motor_int4};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(100);
  Wire.setClock(100000); // Normal clock hz
  delay(100); // allow i2c device to initialize
  for (int i = 0; i < (sizeof(motors) / sizeof(motors[0])); i++) {
    ledcAttachChannel(motors[i], PWM_FREQ, PWM_RESOLUTION, i);
  }
  ledcAttachChannel(servo, 50, PWM_RESOLUTION, 4);

  // begin bluetooth connection
  SerialBT.begin(device_name);
  delay(500); // bluetooth amirite haha
  Serial.println("setup complete");
}


// Moves the wheels and sets the position of the fork
void execute_move(int motor1_speed, int motor2_speed, int ang) {
  motor1_speed = map(motor1_speed, -65535, 65535, -MAX_DUTY_CYCLE, MAX_DUTY_CYCLE);
  motor2_speed = map(motor2_speed, -65535, 65535, -MAX_DUTY_CYCLE, MAX_DUTY_CYCLE);
  ang = map(ang, 0, 30, 0, MAX_DUTY_CYCLE);
  if (motor1_speed == 0) {
    ledcWriteChannel(0, 0);
    ledcWriteChannel(1, 0);
  } else if (motor1_speed > 0) {
      ledcWriteChannel(0, motor1_speed);
      ledcWriteChannel(1, 0);
  } else {
      ledcWriteChannel(0, 0);
      ledcWriteChannel(1, -motor1_speed);
  }
  
  if (motor2_speed == 0) {
    ledcWriteChannel(2, 0);
    ledcWriteChannel(3, 0);
  } else if (motor2_speed > 0) {
      ledcWriteChannel(2, motor2_speed);
      ledcWriteChannel(3, 0);
  } else {
      ledcWriteChannel(2, 0);
      ledcWriteChannel(3, -motor2_speed);
  }
  // set servo
  ledcWriteChannel(4, ang);
}

// "#" -> start byte
#define MSG_START '#'
#define MSG_DELIN '+'
#define MSG_END '$'
// "$" -> end byte
// motor1_speed is a speed from -255 to 255, negative is reverse, 0 is stopped, ditto for motor2_speed
// example packet with each byte in brackets: <start><int motor1_speed><delin><int motor2_speed><delin><int ang><end>
int motor1_speed;
int motor2_speed;
int ang;
void loop() {   
  while (SerialBT.available()) {
    if (SerialBT.read() == MSG_START) {
      motor1_speed = SerialBT.read();
      Serial.println(sizeof(SerialBT.read()));
      } if (SerialBT.read() == MSG_DELIN) {
        motor2_speed = SerialBT.read();
        if (SerialBT.read() == MSG_DELIN) {
          ang = SerialBT.read();
          if (SerialBT.read() == MSG_END) {
            Serial.print(motor1_speed);
            Serial.print(" ");
            Serial.print(motor2_speed);
            Serial.print(" ");
            Serial.println(ang);
            execute_move(motor1_speed, motor2_speed, ang);
          }
        }
      }
    }
}

