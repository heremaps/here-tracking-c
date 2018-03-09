

#!/bin/sh
export VERSION=$(date +%Y%m%d-%H%M%S)

# Use BUILD_TAG if available. Mainly for CI purposes as Jenkins sets this.
if [ -z "$BUILD_TAG" ]
then
    export BUILD_NAME=iot_c_build
else
    # Docker won't accept uppercase letters, convert to lowercase.
    # Current Jenkins tag has uppercase letters.. (...-IoT-Device-Libraries-...)
    export BUILD_NAME=$(echo $BUILD_TAG | tr '[:upper:]' '[:lower:]')
fi

docker build -t $BUILD_NAME:$VERSION .

export DOCKER_OPT="--rm --user ${UID} -v ${PWD}:/src ${BUILD_NAME}:${VERSION}"

if [ -n "$DOXYGEN" ]
then
    export DOCKER_CMD="mkdir -p build && cd build && cmake .. && make doxygen"
elif [ -n "$TEST" ]
then
    if [ -n "$HERE_TRACKING_TEST_DEVICE_ID" ] && [ -n "$HERE_TRACKING_TEST_DEVICE_SECRET" ]
    then
        export DOCKER_OPT="-e HERE_TRACKING_TEST_DEVICE_ID \
                           -e HERE_TRACKING_TEST_DEVICE_SECRET \
                           ${DOCKER_OPT}"
    fi
    export DOCKER_CMD="mkdir -p build && cd build && \
        cmake .. -DBuildSampleApp=ON -DBuildTests=ON -DCodeCoverage=ON && \
        make && make test && make coverage"
else
    export DOCKER_CMD="mkdir -p build && cd build && cmake .. && make"
fi

docker run ${DOCKER_OPT} "${DOCKER_CMD}" || error=true
docker rmi $BUILD_NAME:$VERSION

if [ $error ]
then
    exit -1
fi
