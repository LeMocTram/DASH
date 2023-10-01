#!/bin/bash

echo "__________________________"
echo "Build Docker Images"
echo "SERVER IMAGE + DATABASE-IMAGE"
docker build --tag=server --target=server-image .
docker build --tag=database --target=database-image .
echo "------------------------------"

# echo "------------------------"
# echo "DOCKER COMPOSE RUN"
# echo "-------------------------"
# docker-compose up -d

# echo "------------------------"
# echo "DOCKER COMPOSE IS RUNNING"
# echo "-------------------------"
# sleep 10

# docker-compose down
# echo "-------------------------"
# echo "DOCKER COMPOSE IS STOPPED"
# echo "__________________________"