#include <Bluepad32.h>
int constr = 255;
class RhinoMotor {
  public:
    int _DIR_PIN;
    int _PWM_PIN;

    RhinoMotor(int DIR_PIN, int PWM_PIN) : _DIR_PIN(DIR_PIN), _PWM_PIN(PWM_PIN) {
        pinMode(_DIR_PIN, OUTPUT);
        pinMode(_PWM_PIN, OUTPUT);
    }

    void setSpeed(int speed) {
        if (speed >= 0) {
            // Forward
            digitalWrite(_DIR_PIN, HIGH);
            analogWrite(_PWM_PIN, speed);
        } else {
            // Backward
            digitalWrite(_DIR_PIN, LOW);
            analogWrite(_PWM_PIN, -speed);
        }
    }

    void stop() {
        analogWrite(_PWM_PIN, 0);
    }
};

#define PWM_1 4
#define DIR_1 16
#define PWM_2 17
#define DIR_2 5

RhinoMotor Motor_1(DIR_1, PWM_1);
RhinoMotor Motor_2(DIR_2, PWM_2);

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but no empty slot found");
    }
}

// Callback for when a controller is disconnected
void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }
    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

// Function to process gamepad inputs and control motors
void processGamepad(ControllerPtr ctl) {
    if (ctl->a()) {
        ctl->playDualRumble(0, 250, 0x80, 0x40);
        Motor_1.stop();
        Motor_2.stop();
        return;
    }

    if (ctl->b()) {
      constr = 200;
    }

    if (ctl->x()) {
      constr = 255;
    }

    int left_axis = -ctl->axisY();
    int right_axis = ctl->axisRX();

    int left_axis_mapped = map(left_axis, -512, 512, -255, 255);
    int right_axis_mapped = map(right_axis, -512, 512, -255, 255);

    int motor_left_speed = left_axis_mapped - right_axis_mapped;
    int motor_right_speed = left_axis_mapped + right_axis_mapped;

    motor_left_speed = constrain(motor_left_speed, -constr, constr);
    motor_right_speed = constrain(motor_right_speed, -constr, constr);

    Motor_1.setSpeed(motor_left_speed);
    Motor_2.setSpeed(motor_right_speed);
    Serial.printf("%d %d\n", motor_left_speed, motor_right_speed);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller type");
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.forgetBluetoothKeys();

    BP32.enableVirtualDevice(false);
}

void loop() {
    bool dataUpdated = BP32.update();
    if (dataUpdated) {
        processControllers();
    }
    delay(150);
}
