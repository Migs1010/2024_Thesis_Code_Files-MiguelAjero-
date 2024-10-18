#ifndef OVERCURRENT_DETECTION_H
#define OVERCURRENT_DETECTION_H

#include <SPI.h>
#include <ADE9000RegMap.h>
#include <ADE9000API.h>
#include <Wire.h>
#include "currentCorrectionC.h" // Include the correction header file
#include "currentCorrectionB.h" // Include the correction header file
#include "GeneticAlgorithm.h"


// Basic initializations
#define SPI_SPEED 5000000
#define CS_PIN 8
#define ADE9000_RESET_PIN 5
#define PM_1 4
#define RELAY_PIN 9

// Conversions
#define CAL_IRMS_CC 3.28849 // (uA/code) for Current RMS
#define PICKUP_CURRENT 5.0   // Pick-up current for tripping in Amps

// Operating Time Formula Coefficients
#define A 0.14
#define B 0.02

// Structure declarations
struct CurrentRMSRegs curntRMSRegs; // Current RMS
struct VoltageRMSRegs vltgRMSRegs;   // Voltage RMS

// Function declarations
void readRegisterData(void);
void resetADE9000(void);
void readAndTransferEepromData(void);
float calculateOperatingTime(float current);
float stabilizeCurrentReading();
void tripRelay(void);
void resetRelay(void);
void checkForFaults(float phaseA, float phaseB, float phaseC);

#endif // OVERCURRENT_DETECTION_H
