// GeneticAlgorithm.h

#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <math.h>

// Parameters for Genetic Algorithm
extern int populationSize;  // Population size
extern int numGenerations;  // Maximum number of generations
const float crossoverRate = 0.6;
const int chromosomeLength = 1;  // Each chromosome contains multiple operating times
const float mutationRate = 0.2;  // Fixed mutation rate

// Constants for the standard inverse time overcurrent function
const float A = 0.14;
const float B = 0.02;

// Structure to hold the results of the genetic algorithm
struct GAResult {
    float bestOperatingTime; // The best operating time found
    int bestGeneration;       // The generation at which the best operating time was found
};

// Function declarations
void initializePopulation(float population[][chromosomeLength]);
GAResult runGeneticAlgorithm(float currentMultiple); // Updated function signature
float mean_inverse_time_function(float A, float B, float currentMultiple, float operatingTimes[]);
void selection(float population[][chromosomeLength], float fitnessValues[], float selectedPopulation[][chromosomeLength]);
void crossover(float selectedPopulation[][chromosomeLength], float newPopulation[][chromosomeLength]);
void mutation(float population[][chromosomeLength]);
int findBestIndex(float fitnessValues[]);

#endif // GENETICALGORITHM_H
