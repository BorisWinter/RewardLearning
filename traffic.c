#include <stdio.h>
#include <string.h>
#include <math.h> 
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

//State space: 4^(4*2) = 65536 ((#differentWaitingTimes^(#lanes*#maxCars))

// Structure that stores the states of the lights and cars on the intersection
struct Intersection{
    int **lanes;
    int waitingTime;
};

// Writes outcomes to a csv file, this file is used in python to create plots
void writeToCsvFile(int* rewards, int numEpochs, int algorithm){	
	FILE *fp1;
	if(algorithm==1){ //randomActionSelection
		fp1 = fopen("randomActionSelection.csv", "w");											
		for (int i = 0; i< numEpochs; i++){
			fprintf(fp1, "\n%d,%d",i,rewards[i]);
		}
    fclose(fp1);
	}
	if(algorithm==2){ //qLearning
		fp1 = fopen("qLearning.csv", "w");											
		for (int i = 0; i< numEpochs; i++){
			fprintf(fp1, "\n%d,%d",i,rewards[i]);
		}
    fclose(fp1);
	}
}

// Function that returns a random integer between two values
int getRandomNumber(int lower, int upper){
    return (rand() % (upper - lower + 1)) + lower; 
}

// Function that takes an amount of lanes, a maximum number of cars, and returns an intersection
struct Intersection initializeIntersection(int numLanes, int numCars, int maxTime){
    struct Intersection intersection;
    intersection.waitingTime = 0;

    // Dynamically allocate memory for the lanes and cars
    intersection.lanes = malloc(numLanes * sizeof(int *));
    for(int i=0; i<numLanes; i++){
        intersection.lanes[i] = malloc(numCars * sizeof(int));
    }

    // Initialize the cars, cars cannot be initialized with waiting times higher than cars in front of them
    for(int i=0; i<numLanes; i++){
        for(int j=0; j<numCars; j++){
            if(j==0){
                intersection.lanes[i][j] = getRandomNumber(0,maxTime); // Front-most car
            }else{
                intersection.lanes[i][1] = getRandomNumber(0,intersection.lanes[i][j-1]);
            }
            intersection.waitingTime += intersection.lanes[i][j];
        }
    }

    return intersection;
}

// Function that sets a light to green and clears all cars from that lane
void setGreenLight(struct Intersection *intersection, int lane, int numCars){
    for(int i=0; i<numCars; i++){
        intersection->lanes[lane][i] = 0;
    }
}

// Function that adds a random car to one of the available lanes
void addRandomCar(struct Intersection *intersection, int numLanes, int numCars, int maxTime){

    // Loop through all lanes to see which are available
    int availableCount = 0;
    for(int i=0; i<numLanes; i++){
        if(intersection->lanes[i][numCars-1] == 0){
            availableCount++;
        }
    }

    // If an available lane exists, choose a random lane until an available is chosen
    if(availableCount != 0){
        int lane = getRandomNumber(0,numLanes-1);
        while (intersection->lanes[lane][numCars-1] != 0){
            lane = getRandomNumber(0,numLanes-1);
        }

        // For the chosen lane, put a car in the furthermost position
        if(intersection->lanes[lane][0] == 0){
            intersection->lanes[lane][0] = 1;
            printf("New car at position 0 of lane %d.", lane);
        }else{
            intersection->lanes[lane][1] = 1;
            printf("New car at position 1 of lane %d.", lane);
        }
    }else{
        printf("No available spots.");
    }
}

// Update the waiting time of each car
void updateWaitingTimes(struct Intersection *intersection, int numLanes, int numCars, int maxTime){
    for(int i=0; i<numLanes; i++){
        for(int j=0; j<numCars; j++){
            if(intersection->lanes[i][j] != 0 && intersection->lanes[i][j] < maxTime){
                intersection->lanes[i][j] +=1;
            }
        }
    }
}

// Function that updates the total waiting time
int updateTotalWaitingTime(struct Intersection *intersection, int numLanes, int numCars){
    intersection->waitingTime = 0;
    for(int i=0; i<numLanes; i++){
        for(int j=0; j<numCars; j++){
        intersection->waitingTime += intersection->lanes[i][j];
        }
    }
    return intersection->waitingTime;
}

// Function that returns current total waiting time
int getTotalWaitingTime(struct Intersection *intersection){
    return intersection->waitingTime;
}

// Function that prints the intersection table
void printWaitingTime(struct Intersection *intersection, int numLanes, int numCars){
    // Print the total waiting time
    printf("\nIntersection: (Waiting time = %d)\n", intersection->waitingTime);
}

// Function that prints the intersection table
void printIntersection(struct Intersection *intersection, int numLanes, int numCars){
    // Print the new intersection state
    printf("\nIntersection: (Waiting time = %d)\n", intersection->waitingTime);
    for(int i=0; i<numLanes; i++){
        printf("Lane %d:", i);
        for(int j=0; j<numCars; j++){
            printf(" %d ", intersection->lanes[i][j]);
        }
        printf("\n");
    }
}

// Function that prints a coloured visual representation of the intersection
void printIntersectionVisualInColor(struct Intersection *intersection, int lane){
    // Top(lane 0)
    // Print in green if lane 0 has a green light, else in red
    if(lane==0){
        printf("\033[0;32m"); 
    }
    else{
        printf("\033[0;31m"); 
    }
    printf("          (0)\n");
    printf("\033[0;0m");
    for(int i=1; i>=0; i--){
        printf("    ");
        for(int j=0; j<6; j++){
            printf("/");
        }
        printf(" ");
        // Print in green if lane 0 has a green light, else in red
        if(lane==0){
            printf("\033[0;32m"); 
        }
        else{
            printf("\033[0;31m"); 
        }
        printf("%d", intersection->lanes[0][i]);
        printf("\033[0;0m");
        
        for(int j=0; j<4; j++){
            printf(" ");
        }
        for(int j=0; j<6; j++){
            printf("/");
        }
        printf("\n");   
    }

    // Middle top(lane 1)
    // Print in green if lane 1 has a green light, else in red
    if(lane==1){
        printf("\033[0;32m"); 
    }
    else{
        printf("\033[0;31m"); 
    }
    printf("                %d %d    (1)\n", intersection->lanes[1][0], intersection->lanes[1][1]);
    printf("\033[0;0m");
    
    // Middle bottom(lane 3)
    // Print in green if lane 3 has a green light, else in red
    if(lane==3){
        printf("\033[0;32m"); 
    }
    else{
        printf("\033[0;31m"); 
    }
    printf("(3)    %d %d          \n", intersection->lanes[3][1], intersection->lanes[3][0]);
    printf("\033[0;0m");
    
    // Bottom
    for(int i=0; i<2; i++){
        printf("    ");
        for(int j=0; j<6; j++){
            printf("/");
        }
        for(int j=0; j<4; j++){
            printf(" ");
        }
        // Print in green if lane 2 has a green light, else in red
        if(lane==2){
            printf("\033[0;32m"); 
        }
        else{
            printf("\033[0;31m"); 
        }
        printf("%d", intersection->lanes[2][i]);
        printf("\033[0;0m"); 
        
        printf(" ");
       
        for(int j=0; j<6; j++){
            printf("/");
        }
        printf("\n");
    }
    // Print in green if lane 2 has a green light, else in red
    if(lane==2){
        printf("\033[0;32m"); 
    }
    else{
        printf("\033[0;31m"); 
    }
    printf("             (2)\n");
    printf("\033[0;0m");
    printf("\n");
}

// Function that prints a visual representation of the intersection
void printIntersectionVisual(struct Intersection *intersection, int lane){
    // Top
    printf("          (0)\n");
    for(int i=1; i>=0; i--){
        printf("    ");
        for(int j=0; j<6; j++){
            printf("/");
        }
        printf(" ");
        
        printf("%d", intersection->lanes[0][i]);
        
        for(int j=0; j<4; j++){
            printf(" ");
        }
        for(int j=0; j<6; j++){
            printf("/");
        }
        printf("\n");   
    }

    // Middle top
    printf("                %d %d    (1)\n", intersection->lanes[1][0], intersection->lanes[1][1]);

    // Middle bottom
    printf("(3)    %d %d          \n", intersection->lanes[3][1], intersection->lanes[3][0]);

    // Bottom
    for(int i=0; i<2; i++){
        printf("    ");
        for(int j=0; j<6; j++){
            printf("/");
        }
        for(int j=0; j<4; j++){
            printf(" ");
        }
        
        printf("%d", intersection->lanes[2][i]);
        printf(" ");
       
        for(int j=0; j<6; j++){
            printf("/");
        }
        printf("\n");
    }
    printf("             (2)\n");
    printf("\n");
}

// Algorithm that chooses randomly new traffic signs each epoch
int* randomActionSelection(struct Intersection intersection, int numLanes, int numCars, int maxTime, int numEpochs){
    int currentWaitingTime = 0, oldWaitingTime = 0, reward;
    int* rewards = malloc(numEpochs * sizeof(int));
    // Run through epochs
    for(int i=0; i<numEpochs; i++){
        // Print number of epoch
        printf("Epoch %d", i);
        printWaitingTime(&intersection, numLanes, numCars);
        
    // Set a random light to green(action) and remove the cars in that lane
        int action = getRandomNumber(0,numLanes-1);
        
        // Print visual representation of the intersection
        printIntersectionVisual(&intersection, action);
        
        // Choose one of the 4 lights to be green
        setGreenLight(&intersection, action, numCars);
        printf("Green light for lane %d and waiting times +1.\n", action);
        
        // Update the waiting time of each car
        updateWaitingTimes(&intersection, numLanes, numCars, maxTime);
        
        // Print visual representation of the intersection
        printIntersectionVisual(&intersection, action);
        
        // Add a car to a random spot
        addRandomCar(&intersection, numLanes, numCars, maxTime);
        
        // Update the waiting times and the reward(difference between waiting times)
        oldWaitingTime = getTotalWaitingTime(&intersection);
        currentWaitingTime = updateTotalWaitingTime(&intersection, numLanes, numCars);
        
        // Print visual representation of the intersection
        printWaitingTime(&intersection, numLanes, numCars);
        printIntersectionVisual(&intersection, action);
        
        // Get reward (reward = waitingTime_{t-1}-waitinTime_{t})
        if(oldWaitingTime!=0){
            reward = oldWaitingTime - currentWaitingTime;
            rewards[i]=reward;
            printf("Reward : %d.\n", rewards[i]);
        }
            
        // End of epoch
        printf("-------------------------------------------------\n");    
    }
    return rewards;
}

int getOptimalQValueAction(double **qValues, int state, int numActions){
    // For the qValues where the state = state, return the highest value
    int action = getRandomNumber(0,numActions-1);
    for(int i=0;i<numActions;i++){
        if(qValues[i][state] > qValues[action][state]){
            action = i;
        }
    }
    return action;
}

int selectQValueAction(double epsilon, int state, int numActions, double **qValues){
    // Random number between 0 and 1
    double randomExploration = ((double) rand() / (RAND_MAX));
    int action = 0;

    if(randomExploration < epsilon){
        action = getRandomNumber(0,numActions-1); // Random choice
    }else{
        action = getOptimalQValueAction(qValues, state, numActions); // Optimal choice
    }
    return action;
}

// Returns number of states for given parameter values
int getNumStates(int numLanes, int numCars, int maxTime){
    return pow(maxTime,(numCars*numLanes));
}

// Returns state as a 1-D array with 8 values for each car spot
int getState(struct Intersection intersection, int numLanes, int numCars, int maxTime){
    int state = 0;
    for(int i=0; i<numLanes; i++){
        for(int j=0; j<numCars; j++){
             state= state*10 + intersection.lanes[i][j];
        }
    }
    printf("\nstateIndex : %d\n", state);
    return state;
}

// Function that prints current state with q values for different actions
void printQValues(double **qValues, int state, int numLanes){
    for(int i=0; i<numLanes; i++){
        printf("Q(action == %d)=%f\n", i, qValues[i][state]);
    }
}

// Algorithm that chooses new traffic signs by Q-learning
int* qLearning(struct Intersection intersection, int numLanes, int numCars, int maxTime, int numEpochs){
    int currentWaitingTime = 0, oldWaitingTime = 0;
    double learningRate = 0.1, discountFactor = 0.1, reward = 0, epsilon = 0.1;
    int numStates = getNumStates(numLanes, numCars, maxTime);
    int* rewards = malloc(numEpochs * sizeof(int));
    int state, statePrime, action;

    // Allocate memory for Q values
    double **qValues = malloc(numLanes * sizeof(double *));
    for(int i=0; i<numLanes; i++){
        qValues[i] = malloc(33333333 * sizeof(double));
    }

    // Initialize Q values to 1 (optimistic initial value)
    for(int i=0; i<numLanes; i++){
        for(int j=0; j<33333333; j++){
            qValues[i][j] = 1;
        }
    }

    // Run through epochs
    for(int i=0; i<numEpochs; i++){
        // Print number of epoch
        printf("Epoch %d\n", i);
        printWaitingTime(&intersection, numLanes, numCars);

        // Get current state
        state = getState(intersection, numLanes, numCars, maxTime);

        // Choose an action
        printQValues(qValues, statePrime, numLanes);
        action = selectQValueAction(epsilon, state, numLanes, qValues);
        printf("Selected action:%d\n\n",action);

        // Print visual representation of the intersection
        printIntersectionVisual(&intersection, action);
        
        // Choose one of the 4 lights to be green
        setGreenLight(&intersection, action, numCars);
        printf("Green light for lane %d and waiting times +1.\n", action);
        
        // Update the waiting time of each car
        updateWaitingTimes(&intersection, numLanes, numCars, maxTime);
        
        // Print visual representation of the intersection
        printIntersectionVisual(&intersection, action);
        
        // Add a car to a random spot
        addRandomCar(&intersection, numLanes, numCars, maxTime);

        // Get state after performed action
        statePrime = getState(intersection, numLanes, numCars, maxTime);

        // Update the waiting times and the reward(difference between waiting times)
        oldWaitingTime = getTotalWaitingTime(&intersection);
        currentWaitingTime = updateTotalWaitingTime(&intersection, numLanes, numCars);
        
        // Print visual representation of the intersection
        printWaitingTime(&intersection, numLanes, numCars);
        printIntersectionVisual(&intersection, action);
        
        // Get reward (reward = waitingTime_{t-1}-waitinTime_{t})
        if(oldWaitingTime!=0){
            reward = oldWaitingTime - currentWaitingTime;
            rewards[i]=reward;
            printf("Reward : %d.\n", rewards[i]);
        }

        // Get next state's max qValue
        double estOptFutValue = qValues[0][statePrime];
        for(int i=1; i<numLanes;i++){
            if(qValues[i][statePrime] > estOptFutValue){
                estOptFutValue = qValues[i][statePrime];
            }
        }

        // Update qValues
        qValues[action][state] = qValues[action][state] + (1 - learningRate) *
        (reward + discountFactor * (estOptFutValue - qValues[action][state]));

        // Print Q-values
        printf("Updated Q-values:\n");
        printQValues(qValues, statePrime, numLanes);

        // End of epoch
        printf("-------------------------------------------------\n");    
    }
    return rewards;
}

//Starts the different algorithms
void startSimulation(struct Intersection intersection, int numEpochs, int numLanes, int numCars, int maxTime){
    int* rewards = malloc(numEpochs * sizeof(int));
    // Perform random action selection and write results to a csv file
    //rewards = randomActionSelection(intersection, numLanes, numCars, maxTime, numEpochs);
    //writeToCsvFile(rewards, numEpochs, 1);
    
    // Perform Q learing and write results to a csv file
    rewards = qLearning(intersection, numLanes, numCars, maxTime, numEpochs);
    //writeToCsvFile(rewards, numEpochs, 2);

}

int main(int argc, char *argv[]) {
    // Initialization
    int numEpochs = 10000, numLanes = 4, numCars = 2, maxTime = 3;
    srand(time(0));
    struct Intersection intersection = initializeIntersection(numLanes, numCars, maxTime);
      
    // Start the traffic control simulation
    startSimulation(intersection, numEpochs, numLanes, numCars, maxTime);

    // Free arrays in the intersection structure
    free(intersection.lanes);
    return 0;
}