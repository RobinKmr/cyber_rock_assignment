FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y \
    g++ \
    make \
    build-essential \
    libboost-all-dev

WORKDIR /app
COPY server.cpp .

RUN g++ server.cpp -o server -lboost_system

CMD ["./server"]
