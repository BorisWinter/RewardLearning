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
void writeToCsvFile(double* rewards, int numEpochs, int algorithm, int policy){	
	FILE *fp1;
    // Random Action Selection
	if(algorithm==1){ 
		fp1 = fopen("randomActionSelection.csv", "w");											
		for (int i = 0; i< numEpochs; i++){
			fprintf(fp1, "\n%d,%f",i,rewards[i]);
		}
    fclose(fp1);
	}

    // Q-Learning
	if(algorithm==2){ 
        if(policy==1){ //E-Greedy
            fp1 = fopen("qLearning-EGreedy.csv", "w");											
		    for (int i = 0; i< numEpochs; i++){
			fprintf(fp1, "\n%d,%f",i,rewards[i]);
		    }
        }
        if(policy==2){ //Optimal Initial Values
            fp1 = fopen("qLearning-OptimalInitialValues.csv", "w");											
		    for (int i = 0; i< numEpochs; i++){
			fprintf(fp1, "\n%d,%f",i,rewards[i]);
		    }
        }
		
    fclose(fp1);
	}

    // Sarsa
    if(algorithm==3){
		if(policy==1){ //E-Greedy
            fp1 = fopen("sarsa-EGreedy.csv", "w");											
		    for (int i = 0; i< numEpochs; i++){
			fprintf(fp1, "\n%d,%f",i,rewards[i]);
		    }
        }
        if(policy==2){ //Optimal Initial Values
            fp1 = fopen("sarsa-OptimalInitialValues.csv", "w");											
		    for (int i = 0; i< numEpochs; i++){
			fprintf(fp1, "\n%d,%f",i,rewards[i]);
		    }
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

// Function that adds random cars to some available lanes
void addRandomCar(struct Intersection *intersection, int numLanes, int numCars, int maxTime, bool verbosity){
    // Go through all lanes, if the lane has a free spot, add a car in the
    // furthermost position with probability .5
    for(int lane=0; lane<numLanes; lane++){

        if(intersection->lanes[lane][0] == 0){
            if(getRandomNumber(1,10) <= 2){
                intersection->lanes[lane][0] = 1;
                if(verbosity==true){    
                    printf("New car at position 0 of lane %d.", lane);
                }
            }
        }else{
            if(getRandomNumber(1,10) <= 2){
                intersection->lanes[lane][1] = 1;
                if(verbosity==true){
                    printf("New car at position 1 of lane %d.", lane);
                }
            }
        }
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
    return state;
}

// Function that prints current state with q values for different actions
void printQValues(double **qValues, int state, int numLanes){
    for(int i=0; i<numLanes; i++){
        printf("Q(action == %d)=%f\n", i, qValues[i][state]);
    }
}

// Returns action that is optimal by estimated q values
int getOptimalAction(double **estimatedValues, int state, int numActions){
    // For the estimated values where the state equals the current state, return the action with the highest estimated value
    int action = 0;
    for(int i=0;i<numActions;i++){
        if(estimatedValues[i][state] > estimatedValues[action][state]){
            action = i;
        }
    }
    return action;
}

// Function that selects an action based on the e-greedy policy
int selectEpsilonGreedyAction(double epsilon, int state, int numActions, double **estimatedValues){
    // Random number between 0 and 1
    double randomExploration = ((double) rand() / (RAND_MAX));
    int action = 0;

    if(randomExploration < epsilon){
        action = getRandomNumber(0,numActions-1); // Random number between given boundaries
    }else{
        action = getOptimalAction(estimatedValues, state, numActions); // Optimal action based on the estimated values
    }
    return action;
}

// Function that selects an action based on the optimal initial value policy
int selectOptimalInitialValuesAction(int state, int numActions, double **estimatedValues){
    int action = 0;
    action = getOptimalAction(estimatedValues, state, numActions); // Optimal choice
    return action;
}

// Algorithm that chooses randomly new traffic signs each epoch
double* randomActionSelection(struct Intersection intersection, int numLanes, int numCars, int maxTime, int numEpochs, bool verbosity, int numRuns){
    double* averageRewards = (double*)calloc(numEpochs, sizeof(double));
    // Iterate over runs
    for(int j=0;j<numRuns;j++){
        printf("Run %d\n", j);
        int currentWaitingTime = 0, oldWaitingTime = 0, reward;
        // Run through action selections
        for(int i=0; i<numEpochs; i++){
            // Print number of epoch
            if(verbosity==true){ 
                printf("Epoch %d", i);
            }
            // Print total time for all waiting cars
            if(verbosity==true){
                printWaitingTime(&intersection, numLanes, numCars);
            }
            // Set a random light to green(action) and remove the cars in that lane
            int action = getRandomNumber(0,numLanes-1);
            // Print visual representation of the intersection
            if(verbosity==true){
                printIntersectionVisual(&intersection, action);
            }
            // Choose one of the 4 lights to be green
            setGreenLight(&intersection, action, numCars);
            if(verbosity==true){
                printf("Green light for lane %d and waiting times +1.\n", action);
            }
            // Update the waiting time of each car
            updateWaitingTimes(&intersection, numLanes, numCars, maxTime);
            // Print visual representation of the intersection
            if(verbosity==true){
                printIntersectionVisualInColor(&intersection, action);
            }
            // Add a car to a random spot
            addRandomCar(&intersection, numLanes, numCars, maxTime, verbosity);
            // Update the waiting times and the reward(difference between waiting times)
            oldWaitingTime = getTotalWaitingTime(&intersection);
            currentWaitingTime = updateTotalWaitingTime(&intersection, numLanes, numCars);
            // Print visual representation of the intersection
            if(verbosity==true){
                printWaitingTime(&intersection, numLanes, numCars);
                printIntersectionVisual(&intersection, action);
            }
            // Get reward (reward = waitingTime_{t-1}-waitinTime_{t})
            reward = oldWaitingTime - currentWaitingTime;
            // Compute average reward on action selection (i), over multiple runs (j)
            averageRewards[i]= averageRewards[i]+((reward-averageRewards[i])/(j+1));          
            // End of epoch
            if(verbosity==true){
                printf("Reward : %d.\n", reward);  
                printf("-------------------------------------------------\n");    
            }
        }
    }
    return averageRewards;
}

// Algorithm that chooses new traffic signs by Q-learning (Off policy Temporal Difference control)
double* qLearning(struct Intersection intersection, int numLanes, int numCars, int maxTime, int numEpochs, int policy, bool verbosity, int numRuns){
    double* averageRewards = (double*)calloc(numEpochs, sizeof(double));
    // Allocate memory for Q values
    double **qValues = malloc(numLanes * sizeof(double *));
    for(int i=0; i<numLanes; i++){
        qValues[i] = malloc(33333333 * sizeof(double));
    }
    
    // Iterate over runs
    for(int j=0;j<numRuns;j++){
        printf("Run %d\n", j);
        int currentWaitingTime = 0, oldWaitingTime = 0;
        double learningRate = 0.1, discountFactor = 0.1, reward = 0, epsilon = 0.1;
        int numStates = getNumStates(numLanes, numCars, maxTime);
        int state, statePrime, action;
        
        // Initialize Q values to 0 (E-greedy)
        if(policy==1){
            for(int i=0; i<numLanes; i++){
                for(int j=0; j<33333333; j++){
                    qValues[i][j] = 0;
                }
            }
        }
        // Initialize Q values to 1 (Optimistic Initial Value)
        if(policy==2){
            for(int i=0; i<numLanes; i++){
                for(int j=0; j<33333333; j++){
                    qValues[i][j] = 3;
                }
            }
        }

        // Run through epochs
        for(int i=0; i<numEpochs; i++){
            // Print number of epoch
            if(verbosity==true){
            printf("Epoch %d\n", i); 
            }

            // Print total time for all waiting cars
            if(verbosity==true){
                printWaitingTime(&intersection, numLanes, numCars);
            }

            // Get current state
            state = getState(intersection, numLanes, numCars, maxTime);

            // Prints the Q-values for the different actions
            if(verbosity==true){
                printQValues(qValues, statePrime, numLanes);
            }
            
            // Choose action by the used policy
            if(policy == 1){// E-greedy policy
                action = selectEpsilonGreedyAction(epsilon, state, numLanes, qValues);
            } 
            if(policy == 2){// Optimal initial values policy
                action = selectOptimalInitialValuesAction(state, numLanes, qValues);
            } 

            // Print visual representation of the intersection
            if(verbosity==true){
                printIntersectionVisual(&intersection, action);
            }
            
            // Choose one of the 4 lights to be green
            setGreenLight(&intersection, action, numCars);

            // Print which traffic light is set to green
            if(verbosity==true){
            printf("Green light for lane %d and waiting times +1.\n", action); 
            }
            
            // Update the waiting time of each car
            updateWaitingTimes(&intersection, numLanes, numCars, maxTime);
            
            // Print visual representation of the intersection
            if(verbosity==true){
                printIntersectionVisualInColor(&intersection, action);
            }
            
            // Add a car to a random spot
            addRandomCar(&intersection, numLanes, numCars, maxTime, verbosity);

            // Get state after performed action
            statePrime = getState(intersection, numLanes, numCars, maxTime);

            // Update the waiting times
            oldWaitingTime = getTotalWaitingTime(&intersection);
            currentWaitingTime = updateTotalWaitingTime(&intersection, numLanes, numCars);
            
            // Print visual representation of the intersection
            if(verbosity==true){
                printWaitingTime(&intersection, numLanes, numCars);
                printIntersectionVisual(&intersection, action);
            }
            
            // Get reward (reward = waitingTime_{t-1}-waitinTime_{t})
            reward = oldWaitingTime - currentWaitingTime;

            // Compute average reward on action selection (i), over multiple runs (j)
            averageRewards[i]= averageRewards[i]+((reward-averageRewards[i])/(j+1));  

            // Get next state's max qValue
            double estOptFutValue = qValues[0][statePrime];
            for(int i=1; i<numLanes;i++){
                if(qValues[i][statePrime] > estOptFutValue){
                    estOptFutValue = qValues[i][statePrime];
                }
            }

            // Update qValues Q(s,a) = Q(s,a)+alpha[reward + γ * maxQ(s(+1),a(+1+))−Q(s,a)]
            qValues[action][state] = qValues[action][state] + learningRate *
            (reward + discountFactor*estOptFutValue - qValues[action][state]);

            if(verbosity==true){
                // Print Q-values
                printf("Updated Q-values:\n");
                printQValues(qValues, statePrime, numLanes); 
            
                // End of epoch
                printf("Reward : %d.\n", reward);
                printf("-------------------------------------------------\n"); 
            }           
        }
    }
    return averageRewards;
}

// Algorithm that chooses new traffic signs by Sarsa (On policy Temporal Difference control)
double* sarsa(struct Intersection intersection, int numLanes, int numCars, int maxTime, int numEpochs, int policy, bool verbosity, int numRuns){
    double* averageRewards = (double*)calloc(numEpochs, sizeof(double));
    // Allocate memory for Q values
    double **qValues = malloc(numLanes * sizeof(double *));
    for(int i=0; i<numLanes; i++){
        qValues[i] = malloc(33333333 * sizeof(double));
    }
    // Iterate over runs
    for(int j=0;j<numRuns;j++){
        printf("Run %d\n", j);    
        int currentWaitingTime = 0, oldWaitingTime = 0;
        double learningRate = 0.1, discountFactor = 0.1, reward = 0, epsilon = 0.1;
        int numStates = getNumStates(numLanes, numCars, maxTime);
        int state, statePrime, action;

        // Initialize Q values to 0 (E-greedy)
        if(policy==1){
            for(int i=0; i<numLanes; i++){
                for(int j=0; j<33333333; j++){
                    qValues[i][j] = 0;
                }
            }
        }
        // Initialize Q values to 1 (Optimistic Initial Value)
        if(policy==2){
            for(int i=0; i<numLanes; i++){
                for(int j=0; j<33333333; j++){
                    qValues[i][j] = 3;
                }
            }
        }

        // Run through epochs
        for(int i=0; i<numEpochs; i++){
            // Print number of epoch
            if(verbosity==true){
                printf("Epoch %d\n", i);
            }
            

            // Print total time for all waiting cars
            if(verbosity==true){
                printWaitingTime(&intersection, numLanes, numCars);
            }

            // Get current state
            state = getState(intersection, numLanes, numCars, maxTime);

            // Prints the Q-values for the different actions
            if(verbosity==true){
                printQValues(qValues, statePrime, numLanes);
            }
            
            // Choose action by the used policy
            if(policy == 1){// E-greedy policy
                action = selectEpsilonGreedyAction(epsilon, state, numLanes, qValues);
            } 
            if(policy == 2){// Optimal initial values policy
                action = selectOptimalInitialValuesAction(state, numLanes, qValues);
            } 

            // Print visual representation of the intersection
            if(verbosity==true){
                printIntersectionVisual(&intersection, action);
            }
            
            // Choose one of the 4 lights to be green
            setGreenLight(&intersection, action, numCars);

            // Print which traffic light is set to green
            if(verbosity==true){
                printf("Green light for lane %d and waiting times +1.\n", action);
            }
            
            // Update the waiting time of each car
            updateWaitingTimes(&intersection, numLanes, numCars, maxTime);
            
            // Print visual representation of the intersection
            if(verbosity==true){
                printIntersectionVisualInColor(&intersection, action);
            }

            // Add a car to a random spot
            addRandomCar(&intersection, numLanes, numCars, maxTime, verbosity);

            // Get state after performed action
            statePrime = getState(intersection, numLanes, numCars, maxTime);

            // Update the waiting times
            oldWaitingTime = getTotalWaitingTime(&intersection);
            currentWaitingTime = updateTotalWaitingTime(&intersection, numLanes, numCars);
            
            // Print visual representation of the intersection
            if(verbosity==true){
                printWaitingTime(&intersection, numLanes, numCars);
                printIntersectionVisual(&intersection, action);
            }

            // Get reward (reward = waitingTime_{t-1}-waitinTime_{t})
            reward = oldWaitingTime - currentWaitingTime;
            // Compute average reward on action selection (i), over multiple runs (j)
            averageRewards[i]= averageRewards[i]+((reward-averageRewards[i])/(j+1)); 

            // Get qValue of state after performing on policy action again (not as in Q learning: use action with optimal Q-value)
            double estOptFutValue;
            if(policy == 1){// E-greedy policy
                action = selectEpsilonGreedyAction(epsilon, state, numLanes, qValues);
                estOptFutValue = qValues[action][statePrime];
            } 
            if(policy == 2){// Optimal initial values policy
                action = selectOptimalInitialValuesAction(state, numLanes, qValues);
                estOptFutValue = qValues[action][statePrime]; 
            } 

            // Update Sarsa qValues Q(s,a) = Q(s,a)+alpha[reward + γ * Q(s(+1),a(+1+))−Q(s,a)],  in which Q(s(+1),a(+1+)) is computed by taking Q value from chosen action by policy (not optimal persé)
            qValues[action][state] = qValues[action][state] + learningRate *
            (reward + discountFactor*estOptFutValue - qValues[action][state]);

            if(verbosity==true){
                // Print Q-values
                printf("Updated Q-values:\n");
                printQValues(qValues, statePrime, numLanes);

                // End of epoch
                printf("Reward : %d.\n", reward);
                printf("-------------------------------------------------\n");    
            }
        }
    }
    return averageRewards;
}

// Starts the different algorithms
void startSimulation(struct Intersection intersection, int numEpochs, int numLanes, int numCars, int maxTime, int numRuns, bool verbosity){
    double* averageRewards = malloc(numEpochs * sizeof(double));
    int policy, algorithm;

    // Perform random action selection (1) with therefore random policy (0) and write results to a csv file
    // algorithm=1;
    // policy=0;
    // printf("Running random action selection with random policy.\n");
    // averageRewards = randomActionSelection(intersection, numLanes, numCars, maxTime, numEpochs, verbosity, numRuns);
    // writeToCsvFile(averageRewards, numEpochs, algorithm, policy);
    // free(averageRewards);
    
    // Perform Q learing (2) with the E-greedy policy (1) and write results to a csv file
    // algorithm=2;
    // policy=1;
    // printf("Running Q-learning with E-greedy policy.\n");
    // averageRewards = qLearning(intersection, numLanes, numCars, maxTime, numEpochs, policy, verbosity, numRuns);
    // writeToCsvFile(averageRewards, numEpochs, algorithm, policy);
    // free(averageRewards);

    // Perform Q learing (2) with the Optimal initial values policy (2) and write results to a csv file
    // algorithm=2;
    // policy=2;
    // printf("Running Q-learning with Optimal Initial Values policy.\n");
    // averageRewards = qLearning(intersection, numLanes, numCars, maxTime, numEpochs, policy, verbosity, numRuns);
    // writeToCsvFile(averageRewards, numEpochs, algorithm, policy);
    // free(averageRewards);

    // Perform Sarsa (3) with E-greedy policy (1) and write results to a csv file
    algorithm=3;
    policy=1;
    printf("Running Sarsa with E-greedy policy.\n");
    averageRewards = sarsa(intersection, numLanes, numCars, maxTime, numEpochs, policy, verbosity, numRuns);
    writeToCsvFile(averageRewards, numEpochs, algorithm, policy);
    free(averageRewards);

    // Perform Sarsa (3) with the Optimal initial values policy (2) and write results to a csv file
    // algorithm=3;
    // policy=2;
    // printf("Running Sarsa with Optimal Initial Values policy.\n");
    // averageRewards = sarsa(intersection, numLanes, numCars, maxTime, numEpochs, policy, verbosity, numRuns);
    // writeToCsvFile(averageRewards, numEpochs, algorithm, policy);
    // free(averageRewards);
}

// Main of the program
int main(int argc, char *argv[]) {
    // Initialization
    int actionSelections = 10000, numLanes = 4, numCars = 2, maxTime = 3, numRuns = 1000;
    srand(time(0));
    bool verbosity = false;
    struct Intersection intersection = initializeIntersection(numLanes, numCars, maxTime);
      
    // Start the traffic control simulation
    startSimulation(intersection, actionSelections, numLanes, numCars, maxTime, numRuns, verbosity);

    // Free arrays in the intersection structure
    free(intersection.lanes);
    return 0;
}