#pragma once

#include <cstddef>
#include <cstdint>

class Display;

class UpdateStrategy
{
public:
    virtual void TransferToDisplay(uint16_t* frameBuffer, Display& display) = 0;
};
