#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include "displayax206.hpp"
#include "updatestrategy.hpp"

namespace
{

void PrintBytes(const char* name, uint8_t* bytes, size_t length)
{
	std::cout << name;
	for (size_t i = 0; i < length; ++i)
	{
		std::cout << std::hex << std::setw(2) << static_cast<int>(bytes[i]) << ",";
	}
	std::cout << std::dec << std::setw(0) << std::endl;
}

uint16_t GetWord(uint8_t* buffer)
{
	return static_cast<uint16_t>(buffer[0]) | (static_cast<uint16_t>(buffer[1]) << 8);
}

void SetWord(uint8_t* buffer, uint16_t value)
{
	buffer[0] = static_cast<uint8_t>(value & 0xff);
	buffer[1] = static_cast<uint8_t>((value >> 8) & 0xff);
}

void SetDoubleWord(uint8_t* buffer, uint32_t value)
{
	buffer[0] = static_cast<uint8_t>(value & 0xff);
	buffer[1] = static_cast<uint8_t>((value >> 8) & 0xff);
	buffer[2] = static_cast<uint8_t>((value >> 16) & 0xff);
	buffer[3] = static_cast<uint8_t>((value >> 24) & 0xff);
}

}


DisplayAx206::DisplayAx206()
{
    libusb_init(&m_ucontext);
}

DisplayAx206::~DisplayAx206()
{
	Close();
}

bool DisplayAx206::Open()
{
    if (m_udev == nullptr)
    {
        m_udev = libusb_open_device_with_vid_pid(m_ucontext, vid, pid);
        if (m_udev == nullptr)
        {
            return false;
        }

        libusb_set_auto_detach_kernel_driver(m_udev, 1);

        int err = libusb_claim_interface(m_udev, ax206interface);
        if (err != 0)
        {
            libusb_close(m_udev);
            m_udev = nullptr;
            return false;
        }
    }
    return true;
}

void DisplayAx206::Close()
{
    if (m_udev != nullptr)
    {
        libusb_release_interface(m_udev, ax206interface);
        libusb_close(m_udev);
        m_udev = nullptr;
    }
}

bool DisplayAx206::GetDimensions(size_t& width, size_t& height)
{
    uint8_t cmd[dpfCmdLen];
	CreateDpfCommand(cmd, cdCmdGetLcdProps);

	uint8_t data[5];
	memset(data, 0 , sizeof(data));
	int error = ScsiRead(cmd, data, sizeof(data));
	if (error != 0 || data[4] != 0xff)
	{
		return false;
	}
	width = GetWord(&data[0]);
	height = GetWord(&data[2]);

	return true;
}

void DisplayAx206::SetBrightness(int level)
{
	level = std::max(0, std::min(level, 7));

	uint8_t cmd[dpfCmdLen];
	CreateDpfCommand(cmd, cdCmdUser, usbCmdSetProperty);
	cmd[7] = propertyBrightness;
	SetWord(&cmd[9], level);

	ScsiWrite(cmd, nullptr, 0);
}

void DisplayAx206::Blit(size_t x0, size_t x1, size_t y0, size_t y1, uint16_t* image, size_t imageLen)
{
	uint8_t cmd[dpfCmdLen];
	CreateDpfCommand(cmd, cdCmdUser, usbCmdBlit);
	SetWord(&cmd[7], x0);
	SetWord(&cmd[9], y0);
	SetWord(&cmd[11], x1 - 1);
	SetWord(&cmd[13], y1 - 1);

	ScsiWrite(cmd, reinterpret_cast<uint8_t*>(image), imageLen);
}

void DisplayAx206::CreateDpfCommand(uint8_t* buffer, uint8_t cdCmd, uint8_t userCmd)
{
	memset(buffer, 0, dpfCmdLen);
	buffer[0] = 0xcd;
	buffer[5] = cdCmd;
	buffer[6] = userCmd;
}

void DisplayAx206::ScsiCmdPrepare(uint8_t* buffer, uint8_t* cmd, size_t dataLen, bool out)
{
	static const uint8_t dCBWSignature[4] = 
	{
		'U', 'S', 'B', 'C'
	};
	static const uint8_t dCBWTag[4] =
	{
		0xde, 0xad, 0xbe, 0xef,
	};

	// bmCBWFlags: 0x80: data in (dev to host), 0x00: Data out
	uint8_t bCBWFlags = out ? 0x00 : 0x80;

	memcpy(&buffer[0], dCBWSignature, sizeof(dCBWSignature));
	memcpy(&buffer[4], dCBWTag, sizeof(dCBWTag));
	SetDoubleWord(&buffer[8], dataLen);	// dCBWLength (4 byte)
	buffer[12] = bCBWFlags;			
	buffer[13] = 0;						// bCBWLUN
	buffer[14] = dpfCmdLen;				// bCBWCBLength
	memcpy(&buffer[15], cmd, dpfCmdLen);

	if (m_debug)
	{
		PrintBytes("SCSI command:", buffer, scsiPacketLen);
	}
}

int DisplayAx206::ScsiWrite(uint8_t* cmd, uint8_t* dataIn, size_t dataLen)
{
	int error = 0;
	int actualLength = 0;

	// Write command to device
	if (m_debug)
	{
		std::cout << "[WRITE] Write command to device" << std::endl;
	}
	uint8_t buffer[scsiPacketLen];
	ScsiCmdPrepare(buffer, cmd, dataLen, true);
	error = libusb_bulk_transfer(m_udev, ax206endpOut, buffer, sizeof(buffer), &actualLength, scsiTimeout);
	if (error != 0)
	{
		return error;
	}

	// Write data to device
	if (dataIn)
	{
		if (m_debug)
		{
			std::cout << "[WRITE] Write data to device" << std::endl;
		}
		error = libusb_bulk_transfer(m_udev, ax206endpOut, dataIn, dataLen, &actualLength, scsiTimeout);
		if (error != 0)
		{
			return error;
		}
	}

	return ScsiGetAck();
}

int DisplayAx206::ScsiRead(uint8_t* cmd, uint8_t* dataOut, size_t dataLen)
{
	int error = 0;
	int actualLength = 0;

	// Write command to device
	if (m_debug)
	{
		std::cout << "[READ] Write command to device" << std::endl;
	}
	uint8_t buffer[scsiPacketLen];
	ScsiCmdPrepare(buffer, cmd, dataLen, false);
	error = libusb_bulk_transfer(m_udev, ax206endpOut, buffer, sizeof(buffer), &actualLength, scsiTimeout);
	if (error != 0)
	{
		return error;
	}

	// Read data from device
	if (m_debug)
	{
		std::cout << "[READ] Read data from device" << std::endl;
	}
	error = libusb_bulk_transfer(m_udev, ax206endpIn, dataOut, dataLen, &actualLength, scsiTimeout);
	if (error != 0)
	{
		return error;
	}

	if (m_debug)
	{
		PrintBytes("[READ] data ", dataOut, dataLen);
	}

	return ScsiGetAck();
}

int DisplayAx206::ScsiGetAck()
{
	static const uint8_t dCBWSignatureAck[4] = 
	{
		'U', 'S', 'B', 'S'
	};
	static const uint8_t dCBWTag[4] =
	{
		0xde, 0xad, 0xbe, 0xef,
	};
	int error = 0;
	int actualLength = 0;

	// Get ACK
	if (m_debug)
	{
		std::cout << "[ACK] Read ACK from device" << std::endl;
	}

	uint8_t data[13];
	memset(data, 0 , sizeof(data));
	error = libusb_bulk_transfer(m_udev, ax206endpIn, data, sizeof(data), &actualLength, scsiTimeout);
	if (error != 0)
	{
		return error;
	}

	if (memcmp(&data[0], dCBWSignatureAck, sizeof(dCBWSignatureAck)) != 0 ||
		memcmp(&data[4], dCBWTag, sizeof(dCBWTag)) != 0)
	{
		if (m_debug)
		{
			PrintBytes("[ACK] data invalid ", data, sizeof(data));
		}

		return 1;
	}

	error = data[12];

	if (m_debug)
	{
		std::cout << "[ACK] result " << std::hex << error << std::dec << std::endl;
	}

	return error;
}
