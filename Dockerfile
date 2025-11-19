FROM ubuntu:22.04

# Устанавливаем часовой пояс сразу
ENV TZ=Europe/Moscow
RUN apt-get update && DEBIAN_FRONTEND=noninteractive \
    apt-get install -y tzdata \
    && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime \
    && echo "$TZ" > /etc/timezone \
    && dpkg-reconfigure --frontend noninteractive tzdata \
    && rm -rf /var/lib/apt/lists/*

# Базовые зависимости для сборки и рантайма
RUN apt-get update && apt-get install -y --no-install-recommends \
       build-essential cmake \
       libboost-all-dev \
       libssl-dev \
       libcurl4-openssl-dev \
       ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Копируем исходники и собираем
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j

EXPOSE 8080

CMD ["./build/BITHUMB"]
