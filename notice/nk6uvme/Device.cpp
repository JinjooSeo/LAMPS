/*
 * Device.cpp
 *
 *  Created on: Oct 2, 2012
 *      Author: evgeniy
 */

#include "Device.h"
#include <cstring>
#include <iostream>

#include <stdio.h>

ErrorCode Device::read(int request, uint16_t value, uint16_t index, size_t lenght, void *data) {
	int res;
	if (handle == NULL) {
		return NULL_POINTER;
	}
	res = libusb_control_transfer(handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN, request, value, index,
			(unsigned char *) data, lenght, 5000);


//        printf("res = %d, length = %d\n", res, lenght);

	if (res < 0) {
		return LIBUSB_ERROR;
	}
	return SUCCESS;
}

ErrorCode Device::write(int request, uint16_t value, uint16_t index, size_t lenght, void *data) {
	int res;
	if (handle == NULL) {
		return NULL_POINTER;
	}
	res = libusb_control_transfer(handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT, request, value, index,
			(unsigned char *) data, lenght, 5000);
	if (res < 0) {
		std::cout << "ERROR CODE  " << res << std::endl;
		return LIBUSB_ERROR;
	}
	return SUCCESS;
}

void Device::uploadFX3Firmware(size_t size, char *data) {
	size_t length = EP0SIZE;
	progress = 0;
	unsigned short wIndex = 0;
	unsigned short wValue = 0;
	size_t address = 0;
	int count = 0;
	do {
		if (length > size - address) {
			length = size - address;
		}
		memcpy(ep0buffer, data + address, length);
		error = write(I2C_EEPROM_WRITE, wValue, wIndex, EP0SIZE, ep0buffer);
		if (error != SUCCESS) {
			return;
			std::cerr << std::endl;
		}
		progress++;
		address += length;
		count++;
		wIndex = EP0SIZE * count;
		if (wIndex == 0) {
			count = 0;
			wIndex = 0;
			wValue++;
		}
		std::cerr << ".";
	} while (address != size);
	std::cerr << std::endl;
	error = SUCCESS;
}

void Device::uploadFPGAFirmware(size_t size, char *data) {
	progress = 0;
	size_t length = EP0SIZE;
	unsigned short wIndex = 0; //page address
	unsigned short wValue = 0;
	size_t address = 0;
	error = read(SPI_FLASH_READ, wValue, wIndex, EP0SIZE, ep0buffer);
	if (error != SUCCESS) {
		std::cerr << std::endl;
		return;
	}
	((int *) ep0buffer)[0] = size;
	error = write(SPI_FLASH_ERASE_POLL, 0, 0, 0, ep0buffer);
	if (error != SUCCESS) {
		return;
	}
	error = write(SPI_FLASH_WRITE, wValue, wIndex, EP0SIZE, ep0buffer);
	if (error != SUCCESS) {
		std::cerr << std::endl;
		return;
	}
	wIndex++;
	do {
		if (length > size - address) {
			length = size - address;
		}
		memcpy(ep0buffer, data + address, length);
		error = write(SPI_FLASH_WRITE, wValue, wIndex, EP0SIZE, ep0buffer);
		if (error != SUCCESS) {
			std::cerr << std::endl;
			return;
		}
		progress++;
		wIndex++;
		address += length;
		std::cerr << ".";
	} while (address != size);
	std::cerr << std::endl;
	error = SUCCESS;
}

void Device::downloadFPGAFirmware(size_t * _size, char **data) {
	progress = 0;
	size_t length = EP0SIZE;

	char * buffer = ep0buffer;
	error = read(SPI_FLASH_READ, 0, 0, 4, buffer);
	if (error != SUCCESS) {
		return;
	}
	size_t size = *((int*) buffer);
	if (size <= 0) {
		error = WRONG_VALUE;
	}
	*(_size) = size;
	*(data) = new char[size];
	unsigned short wIndex = 1; //page address
	size_t address = 0;
	do {
		if (length > size - address) {
			length = size - address;
		}
		error = read(SPI_FLASH_READ, 0, wIndex, EP0SIZE, (unsigned char *) buffer);
		if (error != SUCCESS) {
			return;
		}
			memcpy(*(data) + address, buffer, length);
		wIndex++;
		address += length;
		progress++;
	} while (address != size);
	error = SUCCESS;
}

unsigned char Device::getAddress() {
	unsigned char result;
	error = read(PVMEX_GET_ADDRESS, 0, 0, 1, (unsigned char *) ep0buffer);
	if (error != SUCCESS) {
		return 0;
	}
	result = ep0buffer[0];
	return result;
}

void Device::setAddress(unsigned char address) {
	if (address > 255) {
		error = WRONG_VALUE;
		return;
	}
	error = write(PVMEX_SET_ADDRESS, address, 0, 0, (unsigned char *) ep0buffer);
	if (error != SUCCESS) {
		return;
	}
}

char * Device::getFPGAVersion() {
	char* result = new char[FPGA_VERSION_SIZE];
	error = read(PVMEX_GET_FPGA_VERSION, 0, 0, FPGA_VERSION_SIZE, (unsigned char *) ep0buffer);
	if (error != SUCCESS) {
		return 0;
	}
	memcpy(result, ep0buffer, FPGA_VERSION_SIZE);
	return result;
}

void Device::setFPGAVersion(const char *str) {
	int size = strlen(str);
	memcpy(ep0buffer, str, size);
	ep0buffer[size] = 0;
	if (size > FPGA_VERSION_SIZE) {
		error = WRONG_VALUE;
		//std::cout << "wrong value";
		return;
	}
	memcpy(ep0buffer, str, size);
	ep0buffer[size] = 0;

	error = write(PVMEX_SET_FPGA_VERSION, 0, 0, EP0SIZE, (unsigned char *) ep0buffer);
	if (error != SUCCESS) {
		return;
	}
}

int Device::getProgress() {
	return progress;
}

Device::Device(libusb_device * _device) {
	ep0buffer = new char[EP0SIZE];
	device = _device;
}

Device::~Device() {
	delete[] ep0buffer;
}

void Device::open() {
	handle = NULL;
	int r = libusb_open(device, &handle);
	if (r < 0) {
		handle = NULL;
		error = DEVICE_CANNOT_BE_OPENED;
	} else {
		error = SUCCESS;
	}

}

void Device::close() {
	if (handle != NULL) {
		libusb_close(handle);
	}
}

libusb_device *Device::getDevice() const {
	return device;
}

ErrorCode Device::getError() {
	return error;
}

void Device::reset() {
	error = write(PVMEX_RESET, 0, 0, 0, ep0buffer);
}

/* namespace pvmex */
