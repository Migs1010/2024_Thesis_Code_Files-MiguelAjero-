#include <SPI.h>
#include <ADE9000RegMap.h>
#include <ADE9000API.h>
#include <Wire.h>
#include "GeneticAlgorithm.h"

/* Basic initializations */
ADE9000Class ade9000;
#define SPI_SPEED 5000000     // SPI Speed
#define CS_PIN 8 // 8 --> Arduino Zero. 16 --> ESP8266
#define ADE9000_RESET_PIN 5 // Reset Pin on HW
#define PM_1 4              // PM1 Pin: 4 --> Arduino Zero. 15 --> ESP8266 
#define RELAY_PIN 9

/* Conversions */
#define CAL_IRMS_CC    3.28849         // (uA/code) for Current RMS
#define CAL_VRMS_CC    10.73364        // (uV/code) for Voltage RMS
#define PICKUP_CURRENT 5.0             // Pick-up current for tripping in Amps

/* Operating Time Formula Coefficients */
#define A 0.14
#define B 0.02

/* Structure declarations */
struct CurrentRMSRegs curntRMSRegs;   // Current RMS
struct VoltageRMSRegs vltgRMSRegs;    // Voltage RMS
struct CurrentTHDRegs currTHDRegs;    // Current THD

/* Function declarations */
void readRegisterData(void);
void resetADE9000(void);
void readAndTransferEepromData(void);
float calculateOperatingTime(float current);
float stabilizeCurrentReading();  // No longer tracking stabilization time

// Variable to track relay state
bool relayTripped = false;

void setup() 
{
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C
  Serial.println("Setup started");

  pinMode(PM_1, OUTPUT);    // Set PM1 pin as output 
  digitalWrite(PM_1, LOW);   // Set PM1 select pin low for PSM0 mode
  pinMode(ADE9000_RESET_PIN, OUTPUT);
  digitalWrite(ADE9000_RESET_PIN, HIGH); 
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Relay in normal condition (HIGH)

  resetADE9000(); 
  delay(1000);
  ade9000.SPI_Init(SPI_SPEED, CS_PIN); // Initialize SPI
  ade9000.SetupADE9000();              // Initialize ADE9000 registers according to values in ADE9000API.h
  
  Serial.println("Reading and transferring EEPROM data");
  readAndTransferEepromData(); // Read and transfer EEPROM data
  Serial.println("Transfer completed");

  Serial.println("Setup completed");
}

void loop() 
{
  readRegisterData();
  delay(200); 
}

void readAndTransferEepromData() {
  // Read calibration constants from EEPROM and transfer them to ADE9000 registers
  for (int i = 0; i < CALIBRATION_CONSTANTS_ARRAY_SIZE; i++) {
    uint32_t value = ade9000.readWordFromEeprom(ADE9000_Eeprom_CalibrationRegAddress[i]);
    ade9000.SPI_Write_32(ADE9000_CalibrationRegAddress[i], value);
    Serial.print("Value written to register: ");
    Serial.println(value, HEX);
  }
}

#include "GeneticAlgorithm.h" // Include GA header file

void readRegisterData()
{
    ade9000.ReadCurrentRMSRegs(&curntRMSRegs); // Read Current RMS registers for all phases using API
    float stableCurrent = (curntRMSRegs.CurrentRMSReg_C * CAL_IRMS_CC) / 1e6; // Convert CIRMS to Amps for Phase C

    // Check if overcurrent (greater than PICKUP_CURRENT) and the relay is not already tripped
    if (stableCurrent > PICKUP_CURRENT && !relayTripped) {
        Serial.println("----------------------------------------------");    
        Serial.println("Overcurrent detected on Phase C!");

        // **Start Time**: Record time when overcurrent is detected
        unsigned long startTime = millis();  

        // Use stabilization function to get the maximum stable reading
        float stableCurrent = stabilizeCurrentReading(); // Get stable current reading

        // Apply current correction
        if (stableCurrent >= 5.0 && stableCurrent <= 7.0) {
        // No correction for this range
       } else if (stableCurrent > 7.0 && stableCurrent <= 8.0) {
        stableCurrent += 0.00;
       } else if (stableCurrent >= 10.0 && stableCurrent <= 16.0) {
        stableCurrent -= 0.02;
       } else if (stableCurrent > 16.0 && stableCurrent <= 21) {
        stableCurrent -= 0.04;
       } else if (stableCurrent > 21.0 && stableCurrent <= 26.0) {
        stableCurrent -= 0.05;
       }

        // Print the final stabilized current
        Serial.print("Final Stabilized Current: ");
        Serial.print(stableCurrent, 2);
        Serial.println(" A");

        float PSM = stableCurrent / PICKUP_CURRENT;
        

        // Call the Genetic Algorithm to find the best operating time based on PSM
        GAResult result = runGeneticAlgorithm(PSM); // Get the GA results
        
        float BestOPtime = result.bestOperatingTime;
        float BestGen = result.bestGeneration;
        
        Serial.print("Best Operating Time from GA: ");
        Serial.print(BestOPtime, 2); // Print the best operating time
        Serial.println(" seconds");

        // Print the generation at which the best operating time was found
        Serial.print("Best Operating Time found at Generation: ");
        Serial.println(BestGen);

        // **Stop Time**: After stabilization and calculation steps are complete
        unsigned long stopTime = millis(); 

        // Calculate the time taken between overcurrent detection and finishing everything else
        unsigned long timeElapsed = stopTime - startTime;

        // Deduct the time taken from the operating time
        unsigned long totalDelay = (static_cast<unsigned long>(BestOPtime * 1000)) - (timeElapsed + 200);
        totalDelay = totalDelay < 0 ? 0 : totalDelay;  // Ensure totalDelay is not negative

        // Debug print to check the final delay after deduction
        Serial.print("Total Delay (after deduction): ");
        Serial.println(totalDelay);

        // Wait for the total delay before tripping the relay
        delay(totalDelay);  
    
        Serial.println("----------------------------------------------");

        // Trip the relay
        Serial.println("Tripping the relay...");
        digitalWrite(RELAY_PIN, LOW);  // Trip the relay (set to LOW)
        relayTripped = true; // Set the relay trip state

        // Allow a short delay to indicate the relay has tripped
        delay(1000);
        
        // Reset the relay
        Serial.println("Resetting the relay...");
        digitalWrite(RELAY_PIN, HIGH);  // Reset the relay (set to HIGH)

        // After resetting, continue monitoring
        delay(1000);
        relayTripped = false; // Reset the trip state
        Serial.println("Relay reset. Monitoring current again.");
    }
}


float stabilizeCurrentReading() {
    float previousCurrent = 0.0;
    float current = 0.0;
    unsigned long stabilizationStartTime = millis();
    unsigned long stabilizationDuration = 1000; // Maximum allowed stabilization time (in milliseconds)
    float threshold = 0.01; // Threshold for the difference between current readings (in Amps)

    // Loop until the current reading stabilizes or timeout occurs
    while (true) {
        ade9000.ReadCurrentRMSRegs(&curntRMSRegs); // Read Current RMS registers
        current = (curntRMSRegs.CurrentRMSReg_C * CAL_IRMS_CC) / 1e6; // Convert CIRMS to Amps

        // Check if the difference between readings is less than the threshold
        if (abs(current - previousCurrent) < threshold && previousCurrent != 0) {
            break; // Exit the loop if the reading is stable
        }

        previousCurrent = current; // Update previous current reading

        // Check for timeout
        if (millis() - stabilizationStartTime > stabilizationDuration) {
            Serial.println("Stabilization timeout! Using last stable reading.");
            break; // Exit the loop if it takes too long
        }

        delay(200); // Short delay before the next reading
    }

    return current; // Return the stable current value
}

float calculateOperatingTime(float PSM) {
  // Calculate the operating time using standard inverse time formula: 
  // T = A / ((I / pickup_current)^B - 1)
  return A / (pow((PSM), B) - 1);
}

void resetADE9000(void)
{
  Serial.println("Resetting ADE9000");
  digitalWrite(ADE9000_RESET_PIN, LOW);
  delay(50);
  digitalWrite(ADE9000_RESET_PIN, HIGH);
  delay(1000);
  Serial.println("Reset Done");
}

