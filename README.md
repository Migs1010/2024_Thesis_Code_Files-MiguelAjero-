File Descriptions:
Arduino Library Files (Ensure these files are uploaded to the Arduino Library):
1.	ADE9000API:
1.1.	ADE9000API.cpp – Library for ADE9000
1.2.	ADE9000API.h – Definitions/Structs/Functions 
1.3.	ADE9000CalibrationInputs.h – Calibration Inputs and Configuration Registers
1.4.	ADE9000RegMap.h – API macros header definition register file
1.5.	GeneticAlgorithm.h – GA Parameter Setting and Function Declarations
1.6.	GeneticAlgorithm.cpp – GA structure and Functions
1.7.	currentCorrectionC.h – Current Correction for Phase C 
1.8.	currentCorrectionB.h – Current Correction for Phase B
1.9.	keywords.txt
Test Code Files:
2.	ADE9000 Codes:
2.1.	ADE9000Calbiration.ino – Calibrating the ADE9000 Board
2.2.	ADE9000API_TESTCODE.ino – Single-Phase (F2700) Reading and Outputting Current, Voltage, THD and Phase Angles
2.3.	ADE9000API_CMC_TESTCODE.ino – Single-Phase/Three-Phase (CMC 356) Reading and Outputting Current, Voltage, THD, Power Factor and Phase Angles
2.4.	Overcurrent_Code.ino – Single-Phase Overcurrent Testing without GA on F2700
2.5.	GA_Overcurrent.ino – Single-Phase Overcurrent Testing with GA on F2700
2.6.	OC_CMC_Test.ino – Single-Phase Overcurrent Testing without GA on CMC 356
2.7.	GA_CMC_TEST.ino – Single-Phase Overcurrent Testing with GA on CMC 356
2.8.	3P_CMC_OC.ino – Three-Phase Directional Overcurrent Testing on CMC 356

