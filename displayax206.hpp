#pragma once

#include <cstdint>
#include "display.hpp"
#include "libusb.h"

class UpdateStrategy;

class DisplayAx206 : public Display
{
public:
    static constexpr uint16_t vid = 0x1908;
    static constexpr uint16_t pid = 0x0102;

    DisplayAx206();
    ~DisplayAx206();

    bool Open() override;
    void Close() override;
    bool GetDimensions(size_t& width, size_t& height) override;
    int GetMaxBrightness() const override { return 7; }
    void SetBrightness(int level) override;
    bool MustBlitFullScreen() const override { return true; }
    void Blit(size_t x0, size_t x1, size_t y0, size_t y1, uint16_t* image, size_t imageLen) override;

private:
    void CreateDpfCommand(uint8_t* buffer, uint8_t cdCmd, uint8_t userCmd = 0x00);
    void ScsiCmdPrepare(uint8_t* buffer, uint8_t* cmd, size_t dataLen, bool out);
    int ScsiWrite(uint8_t* cmd, uint8_t* dataIn, size_t dataLen);
    int ScsiRead(uint8_t* cmd, uint8_t* dataOut, size_t dataLen);
    int ScsiGetAck();

    // All values obtained from reference github.com/dreamlayers/dpf-ax
    // USB DPF sub-commands for command 0xcd
    static constexpr uint8_t cdCmdGetLcdProps       = 0x02;
    static constexpr uint8_t cdCmdUser              = 0x06;

    // USB DPF user commands
	static constexpr uint8_t usbCmdSetProperty      = 0x01;
	static constexpr uint8_t usbCmdBlit             = 0x12;

    // USB DPF Properties
    static constexpr uint8_t propertyBrightness     = 0x01;
    static constexpr uint8_t propertyOrientation    = 0x10;

    // USB end-point definitions
	static constexpr uint8_t ax206interface         = 0x00;
	static constexpr uint8_t ax206endpOut           = 0x01;
	static constexpr uint8_t ax206endpIn            = 0x81;

    static constexpr size_t scsiHeaderLen           = 15;
    static constexpr size_t dpfCmdLen               = 16;
    static constexpr size_t scsiPacketLen           = (scsiHeaderLen + dpfCmdLen);
    static constexpr unsigned int scsiTimeout       = 1000;

    libusb_device_handle* m_udev{ nullptr };
    libusb_context* m_ucontext{ nullptr };

    bool m_debug{ false };
};
