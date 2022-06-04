.PHONY:			env lab1 lab2

ness:
	@sudo apt-get update
	@sudo apt-get upgrade
	@sudo apt-get install -y build-essential \
							autoconf \
							libtool \
							pkg-config \
							gcc g++ cmake \
							git libssl-dev \
							zlib1g-dev \
							librdkafka-dev \
							mysql-server \
							mysql-client \
							libmysqlclient-dev \
							libboost-all-dev \
							openjdk-8-jdk \
							openjdk-8-jre \
							default-jdk \
							docker \
							docker-compose

config-docker:
	@sudo groupadd docker
	@sudo gpasswd -a $USER docker
	@newgrp docker

google-test:
	@sudo apt-get install libgtest-dev
	@cd /usr/src/gtest/
	@sudo cmake -DBUILD_SHARED_LIBS=ON
	@sudo make
	@sudo cp lib/*.so /usr/lib
	@cd

poco:
	@git clone -b master https://github.com/pocoproject/poco.git
	@cd poco
	@mkdir cmake-build
	@cd cmake-build
	@cmake ..
	@cmake --build . --config Release
	@sudo cmake --build . --target install
	@cd

apache-ingite:
	@git clone https://github.com/apache/ignite.git
	@cd iginte/modules/platforms/cpp
	@mkdir cmake-build
	@cd cmake-build
	@cmake -DCMAKE_BUILD_TYPE=Release -DWITH_THIN_CLIENT=ON ..
	@make
	@sudo make install
	@cd

rabbit-mq:
	@git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git
	@cd AMQP-CPP
	@make
	@sudo make install
	@cd

kafka-lib:
	@git clone https://github.com/edenhill/librdkafka.git
	@cd librdkafka
	@./configure
	@make
	@sudo make install
	@cd 
	@git clone https://github.com/mfontanini/cppkafka
	@cd cppkafka
	@mkdir build 
	@cd build 
	@cmake .. 
	@make 
	@sudo make install
	@cd 

redis:
	@git clone https://github.com/tdv/redis-cpp.git
	@mkdir build 
	@cd build 
	@cmake .. 
	@make 
	@sudo make install
	@cd 

grpc:
	@git clone --recurse-submodules -b v1.43.0 https://github.com/grpc/grpc
	@cd grpc
	@mkdir -p cmake/build
	@pushd cmake/build
	@cmake -DgRPC_INSTALL=ON \
		-DgRPC_BUILD_TESTS=OFF \
		-DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
		../..
	@make -j
	@sudo make install popd -y


env: ness config-docker google-test poco apache-ingite rabbit-mq kafka-lib redis grpc

init:
	@cd ./docker && sudo docker-compose build
	@cd ./docker && sudo docker-compose up &
	@cmake -S . -B build

test:
	@./build/server.exe & 
	./build/tests.exe

clean:
	kill $(pidof server.exe)
	@cd ./docker && sudo docker-compose down