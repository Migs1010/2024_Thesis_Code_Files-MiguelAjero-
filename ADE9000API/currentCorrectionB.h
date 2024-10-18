#ifndef CURRENTCORRECTIONB_H
#define CURRENTCORRECTIONB_H

// Function to apply correction for each specific step, correcting to the next expected value
float applyCurrentCorrectionB(float measuredCurrent) {
    // Correction applied for each step from 5A to 15A
    if (measuredCurrent >= 4.0 && measuredCurrent <= 26.0) {
        
        // Apply correction based on the measured current, rounding up to the next whole number
        if (measuredCurrent >= 4.0 && measuredCurrent < 4.5) {
        return 4.0; // Correct to 5A
        } else if (measuredCurrent >= 4.5 && measuredCurrent < 5.5) {
            return 5.0; // Correct to 6A
        } else if (measuredCurrent >= 5.5 && measuredCurrent < 6.5) {
            return 6.0; // Correct to 6A    
        } else if (measuredCurrent >= 5.5 && measuredCurrent < 6.5) {
            return 6.0; // Correct to 6A
        } else if (measuredCurrent >= 6.5 && measuredCurrent < 7.5) {
            return 7.0; // Correct to 7A
        } else if (measuredCurrent >= 7.5 && measuredCurrent < 8.5) {
            return 8.0; // Correct to 8A
        } else if (measuredCurrent >= 8.5 && measuredCurrent < 9.5) {
            return 9.0; // Correct to 9A
        } else if (measuredCurrent >= 9.5 && measuredCurrent < 10.5) {
            return 10.0; // Correct to 10A
        } else if (measuredCurrent >= 10.5 && measuredCurrent < 11.5) {
            return 11.0; // Correct to 11A
        } else if (measuredCurrent >= 11.5 && measuredCurrent < 12.5) {
            return 12.0; // Correct to 12A
        } else if (measuredCurrent >= 12.5 && measuredCurrent < 13.5) {
            return 13.0; // Correct to 13A
        } else if (measuredCurrent >= 13.5 && measuredCurrent < 14.5) {
            return 14.0; // Correct to 14A
        } else if (measuredCurrent >= 14.5 && measuredCurrent < 15.5) {
            return 15.0; // Correct to 15A
        } else if (measuredCurrent >= 15.5 && measuredCurrent < 16.5) {
            return 16.0; // Correct to 16A
        } else if (measuredCurrent >= 16.5 && measuredCurrent < 17.5) {
            return 17.0; // Correct to 17A
        } else if (measuredCurrent >= 17.5 && measuredCurrent < 18.5) {
            return 18.0; // Correct to 18A
        } else if (measuredCurrent >= 18.5 && measuredCurrent < 19.5) {
            return 19.0; // Correct to 19A
        } else if (measuredCurrent >= 19.5 && measuredCurrent < 20.5) {
            return 20.0; // Correct to 20A
        } else if (measuredCurrent >= 20.5 && measuredCurrent < 21.5) {
            return 21.0; // Correct to 21A
        } else if (measuredCurrent >= 21.5 && measuredCurrent < 22.5) {
            return 22.0; // Correct to 22A
        } else if (measuredCurrent >= 22.5 && measuredCurrent < 23.5) {
            return 23.0; // Correct to 23A
        } else if (measuredCurrent >= 23.5 && measuredCurrent < 24.5) {
            return 24.0; // Correct to 24A
        } else if (measuredCurrent >= 24.5 && measuredCurrent <= 25.5) {
            return 25.0; // Correct to 25A
        } else {
            return measuredCurrent; // If out of the correction range, return the original value
        }
    }

    // If current is outside the 5A to 20A range, no correction is applied
    return measuredCurrent;
}

#endif  // CURRENTCORRECTION_H
