#ifndef CURRENTCORRECTIONC_H
#define CURRENTCORRECTIONC_H

// Function to apply correction for each specific step, correcting to the next expected value
float applyCurrentCorrectionC(float measuredCurrent) {
    // Correction applied for each step from 5A to 15A
    if (measuredCurrent >= 4.0 && measuredCurrent <= 26.0) {
        
        // Apply correction based on the measured current, rounding up to the next whole number
            if (measuredCurrent >= 4.0 && measuredCurrent < 4.5) {
            return 4.0; // Correct to 5A
        } else if (measuredCurrent >= 4.5 && measuredCurrent < 5.0) {
            return 5.0; // Correct to 6A
        } else if (measuredCurrent >= 5.5 && measuredCurrent < 6.0) {
            return 6.0; // Correct to 6A
        } else if (measuredCurrent >= 5.5 && measuredCurrent < 6.0) {
            return 6.0; // Correct to 6A
        } else if (measuredCurrent >= 6.0 && measuredCurrent < 7.0) {
            return 7.0; // Correct to 7A
        } else if (measuredCurrent >= 7.0 && measuredCurrent < 8.0) {
            return 8.0; // Correct to 8A
        } else if (measuredCurrent >= 8.0 && measuredCurrent < 9.0) {
            return 9.0; // Correct to 9A
        } else if (measuredCurrent >= 9.0 && measuredCurrent < 10.0) {
            return 10.0; // Correct to 10A
        } else if (measuredCurrent >= 10.0 && measuredCurrent < 11.0) {
            return 11.0; // Correct to 11A
        } else if (measuredCurrent >= 11.0 && measuredCurrent < 12.0) {
            return 12.0; // Correct to 12A
        } else if (measuredCurrent >= 12.0 && measuredCurrent < 13.0) {
            return 13.0; // Correct to 13A
        } else if (measuredCurrent >= 13.0 && measuredCurrent < 14.0) {
            return 14.0; // Correct to 14A
        } else if (measuredCurrent >= 14.0 && measuredCurrent < 15.0) {
            return 15.0; // Correct to 15A
        } else if (measuredCurrent >= 15.0 && measuredCurrent < 16.0) {
            return 16.0; // Correct to 16A
        } else if (measuredCurrent >= 16.0 && measuredCurrent < 17.0) {
            return 17.0; // Correct to 17A
        } else if (measuredCurrent >= 17.0 && measuredCurrent < 18.0) {
            return 18.0; // Correct to 18A
        } else if (measuredCurrent >= 18.0 && measuredCurrent < 19.0) {
            return 19.0; // Correct to 19A
        } else if (measuredCurrent >= 19.0 && measuredCurrent < 20.0) {
            return 20.0; // Correct to 20A
        } else if (measuredCurrent >= 20.0 && measuredCurrent < 21.0) {
            return 21.0; // Correct to 21A
        } else if (measuredCurrent >= 21.0 && measuredCurrent < 22.0) {
            return 22.0; // Correct to 22A
        } else if (measuredCurrent >= 22.0 && measuredCurrent < 23.0) {
            return 23.0; // Correct to 23A
        } else if (measuredCurrent >= 23.0 && measuredCurrent < 24.0) {
            return 24.0; // Correct to 24A
        } else if (measuredCurrent >= 24.0 && measuredCurrent <= 25.0) {
            return 25.0; // Correct to 25A
        } else {
            return measuredCurrent; // If out of correction range, return the original value
        }
    }

    // If current is outside the 5A to 20A range, no correction is applied
    return measuredCurrent;
}

#endif  // CURRENTCORRECTION_H
