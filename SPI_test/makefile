CXX = /opt/freescale/usr/local/gcc-4.6.2-glibc-2.13-linaro-multilib-2011.12/fsl-linaro-toolchain/bin/arm-none-linux-gnueabi-g++
TARGET = spi
$(TARGET) :main.cpp
	$(CXX) main.cpp -o $(TARGET)
clean:
	rm -rf *.o $(TARGET)

