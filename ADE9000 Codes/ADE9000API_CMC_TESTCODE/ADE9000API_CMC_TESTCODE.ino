#include <SPI.h>
#include <ADE9000RegMap.h>
#include <ADE9000API.h>
#include <Wire.h>
#include "currentCorrectionB.h" // Include the correction header file
#include "currentCorrectionC.h" // Include the correction header file


/* Basic initializations */
ADE9000Class ade9000;
#define SPI_SPEED 5000000     // SPI Speed
#define CS_PIN 8              // 8 --> Arduino Zero. 16 --> ESP8266
#define ADE9000_RESET_PIN 5    // Reset Pin on HW
#define PM_1 4                // PM1 Pin: 4 --> Arduino Zero. 15 --> ESP8266 
#define RELAY_PIN 9

/* Conversions */
#define CAL_IRMS_CC    3.28849         // (uA/code) for Current RMS
#define CAL_VRMS_CC    10.73364        // (uV/code) for Voltage RMS
#define ANGL_MUL_CC    0.017578125     // Phase Angle Multiplier constant for 50Hz system

/* Structure declarations */
struct CurrentRMSRegs curntRMSRegs;   // Current RMS
struct VoltageRMSRegs vltgRMSRegs;    // Voltage RMS
struct CurrentTHDRegs currTHDRegs;    // Current THD
struct AngleRegs angleRegs;           // Angle Registers
struct PowerFactorRegs PFRegs;           // PF Registers

/* Function declarations */
void readRegisterData(void);
void resetADE9000(void);
void readAndTransferEepromData(void);

/* EEPROM data structure */
extern uint32_t ADE9000_Eeprom_CalibrationRegAddress[CALIBRATION_CONSTANTS_ARRAY_SIZE];
extern uint32_t ADE9000_CalibrationRegAddress[CALIBRATION_CONSTANTS_ARRAY_SIZE];

void setup() 
{
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C
  Serial.println("Setup started");

  pinMode(PM_1, OUTPUT);    // Set PM1 pin as output 
  digitalWrite(PM_1, LOW);   // Set PM1 select pin low for PSM0 mode
  pinMode(ADE9000_RESET_PIN, OUTPUT);
  digitalWrite(ADE9000_RESET_PIN, HIGH); 
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
  delay(1000); 
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

void readRegisterData()
{
  /* Read using ADE9000 API functions for all phases */
  ade9000.ReadVoltageRMSRegs(&vltgRMSRegs);  // Read Voltage RMS registers for all phases using API
  ade9000.ReadCurrentRMSRegs(&curntRMSRegs); // Read Current RMS registers for all phases using API
  ade9000.ReadCurrentTHDRegsnValues(&currTHDRegs);  // Read Current THD registers for all phases using API
  ade9000.ReadAngleRegsnValues(&angleRegs);          // Read angle values using API
  ade9000.ReadPowerFactorRegsnValues(&PFRegs);        


  float avrms_A_API = (vltgRMSRegs.VoltageRMSReg_A * CAL_VRMS_CC) / 1e6; // Convert AVRMS to Volts for Phase A
  float bvrms_B_API = (vltgRMSRegs.VoltageRMSReg_B * CAL_VRMS_CC) / 1e6; // Convert BVRMS to Volts for Phase B
  float cvrms_C_API = (vltgRMSRegs.VoltageRMSReg_C * CAL_VRMS_CC) / 1e6; // Convert CVRMS to Volts for Phase C

  float airms_A_API = (curntRMSRegs.CurrentRMSReg_A * CAL_IRMS_CC) / 1e6; // Convert AIRMS to Amps for Phase A
  float birms_B_API = (curntRMSRegs.CurrentRMSReg_B * CAL_IRMS_CC) / 1e6; // Convert BIRMS to Amps for Phase B
  float cirms_C_API = (curntRMSRegs.CurrentRMSReg_C * CAL_IRMS_CC) / 1e6; // Convert CIRMS to Amps for Phase C


  // Apply correction to All Phase current
  float corrected_currentA = applyCurrentCorrectionB(airms_A_API);  // Call the correction function
  float corrected_currentB = applyCurrentCorrectionB(birms_B_API);
  float corrected_currentC = applyCurrentCorrectionC(cirms_C_API);


  // Read Power Factors for All Phases
  float APF = PFRegs.PowerFactorValue_A; 
  float BPF = PFRegs.PowerFactorValue_B; 
  float CPF = PFRegs.PowerFactorValue_C; 

  // Read Phase Angles
  float angle_VA_VB = ade9000.SPI_Read_16(ADDR_ANGL_VA_VB) * 0.017578125;
  float angle_VA_VC = ade9000.SPI_Read_16(ADDR_ANGL_VA_VC) * 0.017578125;
  float angle_VB_VC = ade9000.SPI_Read_16(ADDR_ANGL_VB_VC) * 0.017578125;

  float angle_IA_IB = ade9000.SPI_Read_16(ADDR_ANGL_IA_IB) * 0.017578125;
  float angle_IA_IC = ade9000.SPI_Read_16(ADDR_ANGL_IA_IC) * 0.017578125;
  float angle_IB_IC = ade9000.SPI_Read_16(ADDR_ANGL_IB_IC) * 0.017578125;

  float angle_VA_IA = ade9000.SPI_Read_16(ADDR_ANGL_VA_IA) * 0.017578125;
  float angle_VB_IB = ade9000.SPI_Read_16(ADDR_ANGL_VB_IB) * 0.017578125;
  float angle_VC_IC = ade9000.SPI_Read_16(ADDR_ANGL_VC_IC) * 0.017578125;



  /* Print results */
  Serial.println("***************************************************************** ");  // Add a line break for clarity
  Serial.println(" ");  // Add a line break for clarity

  Serial.println("Method 2 (Using ADE9000 API): ");
  Serial.print("Voltage_RMS: A_VRMS: ");
  Serial.print(avrms_A_API, 2);
  Serial.print(" V,  B_VRMS: ");
  Serial.print(bvrms_B_API, 2);
  Serial.print(" V,  C_VRMS: ");
  Serial.print(cvrms_C_API, 2);
  Serial.print(" V"); 
  Serial.println(" ");  // Add a line break for clarity

  Serial.print("Current_RMS: A_IRMS: ");
  Serial.print(corrected_currentA, 2);
  Serial.print(" A,  B_IRMS: ");
  Serial.print(corrected_currentB, 2);
  Serial.print(" A,  C_IRMS: ");
  Serial.print(corrected_currentC, 2); // Print the corrected current value for Phase C
  Serial.print(" A"); 
  Serial.println(" ");  // Add a line break for clarity

  Serial.print("Power Factor: A_PF: ");
  Serial.print(APF, 2);
  Serial.print("  B_PF:: ");
  Serial.print(BPF, 2);
  Serial.print("  C_PF: ");
  Serial.print(CPF, 2); // Print the corrected current value for Phase C
  Serial.println(" ");  // Add a line break for clarity

  /* Print Angle Values */
  Serial.println("Voltage Phase Angles: ");
  Serial.print("VA_VB: ");
  Serial.print(angle_VA_VB);
  Serial.print(" deg,  VA_VC: ");
  Serial.print(angle_VA_VC);
  Serial.print(" deg,  VB_VC: ");
  Serial.print(angle_VB_VC);
  Serial.print(" deg"); 
  Serial.println(" ");  // Add a line break for clarity


  Serial.println("Current Phase Angles: ");
  Serial.print("IA_IB: ");
  Serial.print(angle_IA_IB);
  Serial.print(" deg,  IA_IC ");
  Serial.print(angle_IA_IC);
  Serial.print(" deg,  IB_IC: ");
  Serial.print(angle_IB_IC);
  Serial.print(" deg"); 
  Serial.println(" ");  // Add a line break for clarity


  Serial.println("Phase Angles: ");
  Serial.print("VA_IA: ");
  Serial.print(angle_VA_IA);
  Serial.print(" deg,  VB_IB ");
  Serial.print(angle_VB_IB);
  Serial.print(" deg,  VC_IC: ");
  Serial.print(angle_VC_IC);
  Serial.print(" deg"); 

  Serial.println(" ");  // Add a line break for clarity
  Serial.println(" ");  // Add a line break for clarity
  Serial.println("***************************************************************** ");  // Add a line break for clarity
  Serial.println(" ");  // Add a line break for clarity

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
