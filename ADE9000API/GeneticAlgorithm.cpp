// GeneticAlgorithm.cpp

#include "GeneticAlgorithm.h"
#include <Arduino.h> // Include Arduino library for random functions

float tolerance = 0.001;  // Define the stability threshold for operating time
int stabilityWindow = 10;  // Number of generations to check for stability
int stableGenerations = 0;  // Counter for stable generations


// Define populationSize and numGenerations as global variables
int populationSize = 50;      // Default value, can be modified
int numGenerations = 50;      // Default value, can be modified


void initializePopulation(float population[][chromosomeLength]) {
    for (int i = 0; i < populationSize; i++) {
        for (int j = 0; j < chromosomeLength; j++) {
            population[i][j] = (float)(rand() % 2000) / 50.0;  // Random values between 0.1 and 9.9
        }
    }
}

GAResult runGeneticAlgorithm(float currentMultiple) {
    GAResult result; // Create an instance of GAResult
    
     // Set populationSize and numGenerations based on PSM (Plug Setting Multiplier)
    if (currentMultiple < 1.5) {
        populationSize = 200;
        numGenerations = 200;
    } else if (currentMultiple >= 1.5 && currentMultiple < 2) {
        populationSize = 100;
        numGenerations = 100;
    } else if (currentMultiple >= 2 && currentMultiple <= 3) {
        populationSize = 75;
        numGenerations = 75;
    } else {
        populationSize = 50;
        numGenerations = 50;
    }
    
    float population[populationSize][chromosomeLength];
    initializePopulation(population);

    float bestFitness = INFINITY;
    float previousBestFitness = INFINITY;
    int stableGenerations = 0;
    int bestStableGeneration = -1;  

    // Main GA loop
    for (int generation = 0; generation < numGenerations; generation++) {
        float fitnessValues[populationSize];
        for (int i = 0; i < populationSize; i++) {
            fitnessValues[i] = mean_inverse_time_function(A, B, currentMultiple, population[i]);
        }

        // Identify and update the best fitness and operating time
        float currentBestFitness = INFINITY;
        for (int i = 0; i < populationSize; i++) {
            if (fitnessValues[i] < currentBestFitness) {
                currentBestFitness = fitnessValues[i];
                result.bestOperatingTime = population[i][0];  // Assuming first operating time
                result.bestGeneration = generation;  // Record the generation
            }
        }

        // Check for stabilization
        if (abs(currentBestFitness - previousBestFitness) < tolerance) {
            stableGenerations++;
            if (stableGenerations >= stabilityWindow) {
                bestStableGeneration = generation;
                break;  // Stop the algorithm early if stabilized
            }
        } else {
            stableGenerations = 0;  // Reset if no stability detected
        }

        previousBestFitness = currentBestFitness;

        // GA operations: selection, crossover, mutation, etc.
        int bestIdx = findBestIndex(fitnessValues);
        float bestIndividual[chromosomeLength];
        for (int j = 0; j < chromosomeLength; j++) {
            bestIndividual[j] = population[bestIdx][j];
        }

        float selectedPopulation[populationSize][chromosomeLength];
        selection(population, fitnessValues, selectedPopulation);

        float newPopulation[populationSize][chromosomeLength];
        crossover(selectedPopulation, newPopulation);
        mutation(newPopulation);

        // Retain the best individual
        for (int j = 0; j < chromosomeLength; j++) {
            newPopulation[0][j] = bestIndividual[j];
        }

        for (int i = 0; i < populationSize; i++) {
            for (int j = 0; j < chromosomeLength; j++) {
                population[i][j] = newPopulation[i][j];
            }
        }
    }

    // Output the generation where the solution stabilized
    if (bestStableGeneration != -1) {
        result.bestGeneration = bestStableGeneration;  // Update with stable generation
    }

    return result;  // Return the structure containing the best operating time and generation
}

float mean_inverse_time_function(float A, float B, float currentMultiple, float operatingTimes[]) {
    // Theoretical operating time based on the inverse-time formula
    float theoreticalTime = A / (pow(currentMultiple, B) - 1);
    float fitness = 0.0;
    
    // Calculate fitness as the absolute difference between theoretical and actual times
    for (int i = 0; i < chromosomeLength; i++) {
        float difference = abs(theoreticalTime - operatingTimes[i]);
        fitness += difference;  // Penalize larger deviations
    }
    
    return fitness / chromosomeLength; // Return average fitness value
}

// Selection function (roulette wheel)
void selection(float population[][chromosomeLength], float fitnessValues[], float selectedPopulation[][chromosomeLength]) {
    float invertedFitnessValues[populationSize];
    for (int i = 0; i < populationSize; i++) {
        invertedFitnessValues[i] = 1.0 / fitnessValues[i]; // Inverted to favor lower fitness values
    }

    float totalFitness = 0.0;
    for (int i = 0; i < populationSize; i++) {
        totalFitness += invertedFitnessValues[i];
    }

    for (int i = 0; i < populationSize; i++) {
        float randomValue = (float)random(0, totalFitness);
        float cumulativeProbability = 0.0;
        for (int j = 0; j < populationSize; j++) {
            cumulativeProbability += invertedFitnessValues[j];
            if (cumulativeProbability >= randomValue) {
                for (int k = 0; k < chromosomeLength; k++) {
                    selectedPopulation[i][k] = population[j][k];
                }
                break;
            }
        }
    }
}

// Crossover function (single-point crossover)
void crossover(float selectedPopulation[][chromosomeLength], float newPopulation[][chromosomeLength]) {
    for (int i = 0; i < populationSize; i += 2) {
        if (random(0, 100) / 100.0 < crossoverRate) {
            int crossoverPoint = random(1, chromosomeLength);
            for (int j = crossoverPoint; j < chromosomeLength; j++) {
                newPopulation[i][j] = selectedPopulation[i + 1][j];
                newPopulation[i + 1][j] = selectedPopulation[i][j];
            }
        } else {
            for (int j = 0; j < chromosomeLength; j++) {
                newPopulation[i][j] = selectedPopulation[i][j];
                newPopulation[i + 1][j] = selectedPopulation[i + 1][j];
            }
        }
    }
}

// Mutation function (small mutations)
void mutation(float population[][chromosomeLength]) {
    for (int i = 0; i < populationSize; i++) {
        for (int j = 0; j < chromosomeLength; j++) {
            if (random(0, 100) / 100.0 < mutationRate) {
                population[i][j] += (random(0, 100) / 1000.0) - 0.05;  // Apply small mutation
            }
        }
    }
}

// Function to find the index of the best individual
int findBestIndex(float fitnessValues[]) {
    int bestIdx = 0;
    for (int i = 1; i < populationSize; i++) {
        if (fitnessValues[i] < fitnessValues[bestIdx]) {
            bestIdx = i;
        }
    }
    return bestIdx;
}