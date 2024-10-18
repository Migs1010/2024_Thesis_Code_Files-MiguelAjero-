#include "overcurrentDetection.h"

ADE9000Class ade9000;

// Variable to track relay state
bool relayTripped = false;

void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Setup started");

  pinMode(PM_1, OUTPUT);
  digitalWrite(PM_1, LOW);
  pinMode(ADE9000_RESET_PIN, OUTPUT);
  digitalWrite(ADE9000_RESET_PIN, HIGH);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay in normal condition (HIGH)

  resetADE9000(); 
  delay(1000);
  ade9000.SPI_Init(SPI_SPEED, CS_PIN);
  ade9000.SetupADE9000();

  Serial.println("Reading and transferring EEPROM data");
  readAndTransferEepromData(); 
  Serial.println("Transfer completed");

  Serial.println("Setup completed");
}

void loop() 
{
  readRegisterData();
  delay(1000);
}

void readAndTransferEepromData() {
  for (int i = 0; i < CALIBRATION_CONSTANTS_ARRAY_SIZE; i++) {
    uint32_t value = ade9000.readWordFromEeprom(ADE9000_Eeprom_CalibrationRegAddress[i]);
    ade9000.SPI_Write_32(ADE9000_CalibrationRegAddress[i], value);
    Serial.print("Value written to register: ");
    Serial.println(value, HEX);
  }
}

void readRegisterData() {
    ade9000.ReadCurrentRMSRegs(&curntRMSRegs); // Read Current RMS registers for all phases
    float phaseA = (curntRMSRegs.CurrentRMSReg_A * CAL_IRMS_CC) / 1e6; // Convert to Amps
    float phaseB = (curntRMSRegs.CurrentRMSReg_B * CAL_IRMS_CC) / 1e6; // Convert to Amps
    float phaseC = (curntRMSRegs.CurrentRMSReg_C * CAL_IRMS_CC) / 1e6; // Convert to Amps

    // Read phase angle differences between voltage and current
    float angle_VA_IA = ade9000.SPI_Read_16(ADDR_ANGL_VA_IA) * 0.017578125;
    float angle_VB_IB = ade9000.SPI_Read_16(ADDR_ANGL_VB_IB) * 0.017578125;
    float angle_VC_IC = ade9000.SPI_Read_16(ADDR_ANGL_VC_IC) * 0.017578125;

    // Normalize angles to be between -180 and 180 degrees
    angle_VA_IA = normalizeAngle(angle_VA_IA);
    angle_VB_IB = normalizeAngle(angle_VB_IB);
    angle_VC_IC = normalizeAngle(angle_VC_IC);

    // Check for faults and output directional information
    checkForFaults(phaseA, phaseB, phaseC, angle_VA_IA, angle_VB_IB, angle_VC_IC);
}

// Function to normalize phase angles between -180 to 180 degrees
float normalizeAngle(float angle) {
    while (angle > 180) angle -= 360;
    while (angle < -180) angle += 360;
    return angle;
}

void checkForFaults(float phaseA, float phaseB, float phaseC, float angle_VA_IA, float angle_VB_IB, float angle_VC_IC) {
    if (!relayTripped) {
        // Single-phase faults with directional check
        if (phaseA > PICKUP_CURRENT && phaseB < PICKUP_CURRENT && phaseC < PICKUP_CURRENT) {
            detectDirection("Phase A", angle_VA_IA);
            tripRelay();
        } else if (phaseB > PICKUP_CURRENT && phaseA < PICKUP_CURRENT && phaseC < PICKUP_CURRENT) {
            detectDirection("Phase B", angle_VB_IB);
            tripRelay();
        } else if (phaseC > PICKUP_CURRENT && phaseA < PICKUP_CURRENT && phaseB < PICKUP_CURRENT) {
            detectDirection("Phase C", angle_VC_IC);
            tripRelay();
        }

        // Line-to-Line faults with directional check
        else if (phaseA > PICKUP_CURRENT && phaseB > PICKUP_CURRENT && phaseC < PICKUP_CURRENT) {
            detectDirection("Line A-B", angle_VA_IA);  // Use phase A angle for line A-B fault
            tripRelay();
        } else if (phaseA > PICKUP_CURRENT && phaseC > PICKUP_CURRENT && phaseB < PICKUP_CURRENT) {
            detectDirection("Line A-C", angle_VA_IA);  // Use phase A angle for line A-C fault
            tripRelay();
        } else if (phaseB > PICKUP_CURRENT && phaseC > PICKUP_CURRENT && phaseA < PICKUP_CURRENT) {
            detectDirection("Line B-C", angle_VB_IB);  // Use phase B angle for line B-C fault
            tripRelay();
        }

        // Three-phase fault with directional check
        else if (phaseA > PICKUP_CURRENT && phaseB > PICKUP_CURRENT && phaseC > PICKUP_CURRENT) {
            Serial.println("Three-phase overcurrent detected!");
            // For three-phase, use one of the phase angles to check direction
            detectDirection("Three-phase", angle_VC_IC);  // Defaulting to Phase A for simplicity
            tripRelay();
        }
    }
}

// Function to detect forward or reverse direction based on phase angle
void detectDirection(String faultType, float angleDifference) {
    if (angleDifference >= 30 && angleDifference <= 150) {
        Serial.print("Forward Overcurrent Detected at ");
    } else if ((angleDifference >= 0 && angleDifference < 30) || (angleDifference < -150 && angleDifference >= -180)) {
        Serial.print("Reverse Overcurrent Detected at ");
    } else {
        Serial.print("Indeterminate Direction for Overcurrent at ");
    }
    Serial.print(faultType);
    Serial.print(" (Angle: ");
    Serial.print(angleDifference, 2);
    Serial.println(" degrees)");
}


void tripRelay(void) {
    unsigned long startTime = millis(); 
    float stableCurrent = stabilizeCurrentReading();

    Serial.print("Max Fault Current: ");
    Serial.print(stableCurrent, 2);
    Serial.println(" A");
    
    float PSM = stableCurrent / PICKUP_CURRENT;

    // Use the Genetic Algorithm to find the best operating time based on PSM
    GAResult result = runGeneticAlgorithm(PSM); // Get the GA results
        
    float BestOPtime = result.bestOperatingTime;
    float BestGen = result.bestGeneration;

    unsigned long stopTime = millis(); 

    Serial.print("Best Operating Time from GA: ");
    Serial.print(BestOPtime, 2); // Print the best operating time
    Serial.println(" seconds");

    // Print the generation at which the best operating time was found
    Serial.print("Best Operating Time found at Generation: ");
    Serial.println(BestGen);

    unsigned long timeElapsed = stopTime - startTime;
    unsigned long totalDelay = (static_cast<unsigned long>(BestOPtime * 1000)) - (timeElapsed + 600);
    totalDelay = totalDelay < 0 ? 0 : totalDelay;

    delay(totalDelay);  
    
    Serial.println("*****************************************************");

    Serial.println("Tripping the relay...");
    digitalWrite(RELAY_PIN, LOW);  
    relayTripped = true;

    delay(200);
    resetRelay(); // Resetting relay automatically after trip
}

void resetRelay(void) {
    Serial.println("Resetting the relay...");
    digitalWrite(RELAY_PIN, HIGH);  
    delay(1000);
    relayTripped = false; 
    Serial.println("Relay reset. Monitoring current again.");
}


float stabilizeCurrentReading() {
    float previousCurrentA = 0.0;
    float previousCurrentB = 0.0;
    float previousCurrentC = 0.0;
    float currentA = 0.0;
    float currentB = 0.0;
    float currentC = 0.0;

    unsigned long stabilizationStartTime = millis();
    unsigned long stabilizationDuration = 1000; // Max stabilization time (ms)
    float threshold = 0.05; // Current reading difference threshold (A)

    while (true) {
        // Read RMS values for all phases
        ade9000.ReadCurrentRMSRegs(&curntRMSRegs);
        currentA = (curntRMSRegs.CurrentRMSReg_A * CAL_IRMS_CC) / 1e6;
        currentB = (curntRMSRegs.CurrentRMSReg_B * CAL_IRMS_CC) / 1e6;
        currentC = (curntRMSRegs.CurrentRMSReg_C * CAL_IRMS_CC) / 1e6;

        // Check for stabilization across all three phases
        if (abs(currentA - previousCurrentA) < threshold && 
            abs(currentB - previousCurrentB) < threshold && 
            abs(currentC - previousCurrentC) < threshold && 
            previousCurrentA != 0 && previousCurrentB != 0 && previousCurrentC != 0) {
            break; 
        }

        // Update previous current readings
        previousCurrentA = currentA; 
        previousCurrentB = currentB; 
        previousCurrentC = currentC; 


        delay(200); 
    }

    float corrected_currentA = applyCurrentCorrectionB(currentA);  // Call the correction function
    float corrected_currentB = applyCurrentCorrectionB(currentB);
    float corrected_currentC = applyCurrentCorrectionC(currentC);

    // Compare stabilized readings against the pickup current and return the greatest
    float maxStableCurrent = 0.0;
    if (corrected_currentA > PICKUP_CURRENT) {
        maxStableCurrent = max(maxStableCurrent, corrected_currentA);
    }
    if (corrected_currentB > PICKUP_CURRENT) {
        maxStableCurrent = max(maxStableCurrent, corrected_currentB);
    }
    if (corrected_currentC > PICKUP_CURRENT) {
        maxStableCurrent = max(maxStableCurrent, corrected_currentC);
    }

    // Return the maximum current reading that exceeds the pickup threshold
    return maxStableCurrent;
}


float calculateOperatingTime(float current) {
    return A / (pow((current / PICKUP_CURRENT), B) - 1);
}

void resetADE9000(void)
{
    Serial.println("Resetting ADE9000");
    digitalWrite(ADE9000_RESET_PIN, LOW);
    delay(50);
    digitalWrite(ADE9000_RESET_PIN, HIGH)