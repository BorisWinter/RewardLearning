algorithm=1;
    policy=0;
    printf("Running random action selection with random policy.\n");
    averageRewards = randomActionSelection(intersection, numLanes, numCars, maxTime, numEpochs, verbosity, numRuns);
    writeToCsvFile(averageRewards, numEpochs, algorithm, policy);
    free(averageRewards);