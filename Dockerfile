# Dockerfile for testing 'nav' behavior.
# Opens nav inside the container environment.
# Useful for demonstration purposes only.

# Build and run commands:
#   sudo docker build -t nav .
#   sudo docker run -it nav

FROM ubuntu:latest

RUN apt-get update && \
    apt-get install -y build-essential libncurses-dev && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /usr/src/nav

COPY . .

RUN make

RUN cp nav /usr/local/bin/nav

ENTRYPOINT ["/usr/local/bin/nav"]
