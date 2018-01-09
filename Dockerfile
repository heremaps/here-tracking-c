FROM ubuntu:16.04
RUN apt-get update && apt-get install -y gcc cmake check cppcheck doxygen pkgconf gcovr libmbedtls-dev libssl-dev
WORKDIR src/
ENTRYPOINT ["/bin/bash","-c"]
