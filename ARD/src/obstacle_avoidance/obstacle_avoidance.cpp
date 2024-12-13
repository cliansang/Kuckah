/******************************************************************************
 * obstacle_avoidance.cpp
 * 
 * This program demonstrates obstacle avoidance using an ultrasonic sensor and
 * a pair of DC motors. The robot moves forward until an obstacle is detected
 * within a certain distance threshold. When an obstacle is detected, the robot
 * stops, reverses, turns left or right randomly, and then continues moving
 * forward.
 * 
 * The program uses the SparkFun TB6612 motor driver library to control the
 * motors and the HCSR04 library to interface with the ultrasonic sensor. 
 * See the libraries in the ARD/lib folder for more information.

******************************************************************************/

#include <SparkFun_TB6612.h>
#include <HCSR04.h>

// Pins for the ultrasonic sensor
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

void setup() {
    Serial.begin(115200); // Initialize serial communication
}

void loop() {
    // Measure distance
    float distance = hc.dist(); // Get distance in cm

    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance > 0 && distance <= OBSTACLE_THRESHOLD) {
        // Obstacle detected
        Serial.println("Obstacle detected! Stopping motors.");
        brake(motor1, motor2);
        delay(100); // Pause for 100 ms

        // Reverse
        Serial.println("Reversing...");
        back(motor1, motor2, 150);
        delay(500); // Move backward for 500 ms

        // Randomly turn left or right
        if (random(0, 2) == 0) {
            Serial.println("Turning left...");
            left(motor1, motor2, 150);
        } else {
            Serial.println("Turning right...");
            right(motor1, motor2, 150);
        }
        delay(500); // Turn for 500 ms

        // Stop briefly
        brake(motor1, motor2);
        delay(100);
    } else {
        // No obstacle, move forward
        Serial.println("Path clear, moving forward.");
        forward(motor1, motor2, 150);
    }

    delay(100); // Short delay for stability
}
