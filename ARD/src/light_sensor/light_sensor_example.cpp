/******************************************************************************
simple light detector and obstacle avoidance robot
------------------------------------------------------

This code demonstrats a robot that inhabits in a darker environment and avoid obstacles.
Light sensor is use to stop the robot when too much light is detected. 
The robot will fallback to obstacle avoidance when the light level is low enought for 
its inhabitant environment.

You can adjust to inhabit the robot in a brighter environment by changing the logic.

******************************************************************************/

#include <SparkFun_TB6612.h>
#include <HCSR04.h>

// Pins for ultrasonic sensor
#define TRIG_PIN D11
#define ECHO_PIN D10

// Pins for motor control
#define AIN1 D4
#define BIN1 D7
#define AIN2 D5
#define BIN2 D8
#define PWMA D6
#define PWMB D9
#define STBY D2

// Pin for light sensor
#define LIGHT_SENSOR D3

// Motor offsets
const int offsetA = 1;
const int offsetB = 1;

// Motor initialization
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

// Ultrasonic sensor initialization
HCSR04 hc(TRIG_PIN, ECHO_PIN);

// Distance threshold for obstacle detection
const int OBSTACLE_THRESHOLD = 20;

// Light detection threshold (adjust based on your sensor's behavior)
const int BRIGHT_LIGHT_THRESHOLD = 400;    // bright light threshold level
const int LOW_LIGHT_THRESHOLD = 350; // low light threshold level

bool lightDetected = true; // Track the current state of light detection

// Function declarations
void obstacleAvoidance();

// Initialization
void setup() {
    // Initialize serial communication
    Serial.begin(115200);

    // Set up light sensor pin
    pinMode(LIGHT_SENSOR, INPUT);

    // Set up ultrasonic sensor pins
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Start obstacle avoidance
    Serial.println("Robot initialized and ready.");
}

// Main control loop
void loop() {
    int lightValue = analogRead(LIGHT_SENSOR); // Read the light sensor value

    // Print the light value for debugging
    // Serial.print("Light value: ");
    // Serial.println(lightValue);

    
    if (!lightDetected && lightValue >= BRIGHT_LIGHT_THRESHOLD) {
        // Transition to "bright light detected" state
        lightDetected = true;
        Serial.println("Bright light detected. Stopping the robot.");
        brake(motor1, motor2);
    } else if (lightDetected && lightValue < LOW_LIGHT_THRESHOLD) {
        // Transition to "dim light detected" state
        lightDetected = false;
        Serial.println("Dim light detected. Resuming obstacle avoidance.");
    }

    // Perform obstacle avoidance only if no bright light is detected
    if (!lightDetected) {
        obstacleAvoidance();
    }

    delay(50); // Small delay for stability    
}


// Obstacle avoidance logic
void obstacleAvoidance() {
    float distance = hc.dist(); // Get distance from the ultrasonic sensor

    if (distance > 0 && distance <= OBSTACLE_THRESHOLD) {
        // Obstacle detected
        brake(motor1, motor2);
        delay(100);

        // Reverse the robot briefly
        back(motor1, motor2, 150);
        delay(500);

        // Generate a random speed between 150 and 300
        int randomSpeed = random(150, 301); // Upper limit is exclusive, so use 301

        // Randomly choose to turn left or right with the generated speed
        if (random(0, 2) == 0) {
            Serial.println("Turning left...");
            left(motor1, motor2, randomSpeed);
        } else {
            Serial.println("Turning right...");
            right(motor1, motor2, randomSpeed);
        }

        // Keep turning for a fixed duration
        delay(500);

        // Stop briefly after turning
        brake(motor1, motor2);
        delay(100);
    } else {
        // No obstacle detected, move forward
        Serial.println("Path clear, moving forward.");
        forward(motor1, motor2, 150);
    }
}


