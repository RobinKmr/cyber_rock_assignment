FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y \
    g++ \
    libboost-all-dev

WORKDIR /app
COPY client.cpp .

RUN g++ client.cpp -o client -lboost_system

CMD ["./client"]
