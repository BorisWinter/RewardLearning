    algorithm=2;
    policy=1;
    printf("Running Q-learning with E-greedy policy.\n");
    averageRewards = qLearning(intersection, numLanes, numCars, maxTime, numEpochs, policy, verbosity, numRuns);
    avgLastHundred(numEpochs,averageRewards);
    writeToCsvFile(averageRewards, numEpochs, algorithm, policy);
    free(averageRewards);