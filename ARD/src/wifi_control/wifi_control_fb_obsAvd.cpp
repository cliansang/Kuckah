/******************************************************************************
wifi control with fallback to obstacle avoidance mode
------------------------------------------------------

This sketch demonstrates how to control a robot over Wi-Fi using a web interface. 
The robot can move forward, backward, left, and right based on the user's input. 
If no command is received for a certain period, the robot switches to obstacle avoidance mode. 
In this mode, the robot uses an ultrasonic sensor to detect obstacles and avoid them 
by turning left or right randomly.

******************************************************************************/

#include <SparkFun_TB6612.h>
#include <HCSR04.h>
#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "Your_SSID";
const char* password = "Your_PASSWORD";

// Web server
WebServer server(80);

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

// Timeout for returning to obstacle avoidance
unsigned long lastCommandTime = 0;
const unsigned long COMMAND_TIMEOUT = 3000; // 3 seconds

// Command received
String currentCommand = "";

// Function declarations
void stopMotors();
void forwardManual();
void backwardManual();
void leftManual();
void rightManual();
void handleCommand();
void obstacleAvoidance();

// ---------------------------------- 

// HTML for the web interface
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Robot Controller</title>
  <style>
    button { font-size: 20px; padding: 10px; margin: 10px; }
  </style>
</head>
<body>
  <h1>ESP32 Robot Controller</h1>
  <div>
    <button onclick="sendCommand('F')">Forward</button><br>
    <button onclick="sendCommand('L')">Left</button>
    <button onclick="sendCommand('S')">Stop</button>
    <button onclick="sendCommand('R')">Right</button><br>
    <button onclick="sendCommand('B')">Backward</button>
  </div>
  <script>
    function sendCommand(cmd) {
      fetch('/cmd?move=' + cmd);
    }
  </script>
</body>
</html>
)rawliteral";


void setup() {
    Serial.begin(115200);

    // Initialize motors
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);
    pinMode(STBY, OUTPUT);
    stopMotors();

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Connected to Wi-Fi!");
    Serial.println(WiFi.localIP());

    // Start the web server
    server.on("/", []() { server.send(200, "text/html", htmlPage); });
    server.on("/cmd", handleCommand);
    server.begin();
    Serial.println("Web server started.");
}

void loop() {
    // Handle incoming web server requests
    server.handleClient();
    // Serial.println(WiFi.localIP());

    // Check for user commands
    if (millis() - lastCommandTime < COMMAND_TIMEOUT) {
        // Execute the current command
        if (currentCommand == "F") forwardManual();
        else if (currentCommand == "B") backwardManual();
        else if (currentCommand == "L") leftManual();
        else if (currentCommand == "R") rightManual();
        else stopMotors();
    } else {
        // If no command received recently, switch to obstacle avoidance
        obstacleAvoidance();
    }
}


// ---------------------------------- 
// Handle command requests from the web interface
void handleCommand() {
    if (server.hasArg("move")) {
        currentCommand = server.arg("move");
        lastCommandTime = millis(); // Update last command time
        server.send(200, "text/plain", "Command received: " + currentCommand);
    } else {
        server.send(400, "text/plain", "Invalid command.");
    }
}

// Stop the motors
void stopMotors() {
    brake(motor1, motor2);
}

// Movement functions
void forwardManual() {
    forward(motor1, motor2, 150);
}

void backwardManual() {
    back(motor1, motor2, 150);
}

void leftManual() {
    left(motor1, motor2, 150);
}

void rightManual() {
    right(motor1, motor2, 150);
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
