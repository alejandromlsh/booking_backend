#!/bin/bash

CONTAINER_NAME="cpp_boost_gtest_container"

# Check if the container exists
if ! docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "Container '${CONTAINER_NAME}' does not exist."
    echo "Start it first using your build-and-run script."
    exit 1
fi

# If the container is not running, start it
if ! docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    echo "Starting container '${CONTAINER_NAME}'..."
    docker start "${CONTAINER_NAME}" > /dev/null
fi

# Attach to the container with a shell
docker exec -it "${CONTAINER_NAME}" /bin/bash
