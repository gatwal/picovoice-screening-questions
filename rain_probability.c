/* Filename: rain_probability.c
 * Author: Gary Atwal
 * Project: Picovoice Screening Questions
 *
 * Description: 
 * The probability of rain on a given calendar day in Vancouver is p[i], 
 * where i is the day's index. For example, p[0] is the probability of rain 
 * on January 1, and p[10] is the probability of rain on January 11.
 * 
 * Assume the year has 365 days
 * What is the chance it rains more than n days in Vancouver?
 * Write a function that accepts p and n as input arguments and returns the
 * probability of raining at least n days.
 * --> interpretation: more than n days (i.e. > n), n = 0,1,...,365
 * 
 * Solution:
 * Can model the problem using a Poisson Binomial Distribution:
 * Each day has success or failure to rain
 * Each day is independent of each other
 * Each day can have a different probability for rain
 * 
 * Let Xi be a Bernoulli random variable with
 * P(Xi = 1) = p[i]  (it rains on day i)
 * P(Xi = 0) = 1 - p[i] (it does not rain on day i)
 * 
 * Let S be the total number of rainy days in the year
 * S = sum(Xi) where i = 0, 1, ..., 364 follows a Poisson Binomial Distribution
 * 
 * We want to solve for the probability S > n (rains more than n days)
 * 1) Calculate probability mass function pmf: probability of having k days of
 *    rain out of the year
 *
 * 2) P(S > n) = sum(pmf(k)) where k = n + 1, ..., 365
 * 
 * Reference:
 * https://en.wikipedia.org/wiki/Poisson_binomial_distribution
 */

#include <stdio.h>
#include <stdlib.h>

#define DAYSPERYEAR 365

void prob_mass_func(float *p, float* pmf){
    // Direct convolution algorithm for computing PMF of Poisson binomial
    pmf[0] = 1.0;
    for (int i = 1; i <= DAYSPERYEAR; i++){
        pmf[i] = p[i] * pmf[i-1];
        for (int j = i - 1; j > 0; j--){
            pmf[j] = p[i] * pmf[j-1] + (1 - p[i]) * pmf[j];
        }
        pmf[0] = (1 - p[i]) * pmf[0];
    }
}

float prob_rain_more_than_n(float *p, int n){
    if (n < 0 || n >= DAYSPERYEAR){
        return 0.0;
    }

    // Initialize PMF array (probability of 0 days of rain to 365 days of rain)
    float pmf[DAYSPERYEAR + 1]; 
    for (int i = 0; i <= DAYSPERYEAR; i++){
        pmf[i] = 0.0;
    }

    // Calculate PMF
    prob_mass_func(p, pmf); 

    // Calculate probability it rains more than n days P(S > n)
    float probability = 0.0;
    for (int k = n + 1; k <= DAYSPERYEAR; k++){
        probability += pmf[k];
    }

    return probability;
}

/*******************************************************************************
 * CHECK SOLUTION WITH MONTE CARLO SIMULATION
 *******************************************************************************/
int simulate_year(float *p){
    // calculate how many days it rained in a year via simulation/random numbers
    int rainy_days = 0;
    
    for (int i = 0; i < DAYSPERYEAR; i++){
        if ((float)rand() / RAND_MAX <= p[i]){
            rainy_days++;
        }
    }
    
    return rainy_days;
}

float monte_carlo_prob_rain_more_than_n(float *p, int n, int num_simulations){
    // simulate each year and check if it rained more than n days
    int num_years_rain_more_than_n = 0;

    for (int i = 0; i < num_simulations; i++){
        int rainy_days = simulate_year(p);
        if (rainy_days > n){
            num_years_rain_more_than_n++;
        }
    }

    float probability = (float)num_years_rain_more_than_n/num_simulations;
    return probability;
}

/*******************************************************************************
 * MAIN FUNCTION
 *******************************************************************************/
int main(int argc, char *argv[]){
    float p[DAYSPERYEAR];
    int n = 188;
    int num_simulations = 5000;

    // assign probability of rain for each day
    for (int i = 0; i < DAYSPERYEAR; i++){
        p[i] = (float)rand() / RAND_MAX;
        //p[i] = 0.42;
    }

    float probability1 = prob_rain_more_than_n(p, n);
    float probability2 = monte_carlo_prob_rain_more_than_n(p, n, num_simulations);

    printf("Probability of raining on more than %d days: %f\n", n, probability1);
    printf("Monte Carlo Simulation results with %d iterations: %f\n", num_simulations, probability2);

    return 0;
}