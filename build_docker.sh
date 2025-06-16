#!/bin/bash

IMAGE_NAME="cpp_boost_gtest_env"
CONTAINER_NAME="cpp_boost_gtest_container"

# Build the Docker image
docker build -t $IMAGE_NAME .

# Remove any existing container with the same name
docker rm -f $CONTAINER_NAME 2>/dev/null

# Run the container, mounting the current directory for live code access
docker run -it --name $CONTAINER_NAME -v "$(pwd)":/app $IMAGE_NAME
