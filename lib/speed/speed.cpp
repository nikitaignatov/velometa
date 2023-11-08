#include "speed.h"

// 01-4F-00-00-00-F7-3B
// 01-5B-00-00-00-5C-5E
// 01-62-00-00-00-11-75
// 01-69-00-00-00-95-8E
// 0x01 0x02 0x01 0x00 0x00 0x49 0x8
// 0x01 0x03 0x01 0x00 0x00 0x9F 0x8D
// 0x01 0x05 0x01 0x00 0x00 0xA8 0x98
// 0x01 0x13 0x01 0x00 0x00 0x94 0x08

// 0x01 0x48 0x01 0x00 0x00 0x6F 0x10
bool IsBitSet(byte b, int pos)
{
    return ((b >> pos) & 1) != 0;
}

void Speed::interpret(uint8_t *pData, size_t length)
{
    uint32_t revs_acc = 0;
    uint16_t lwet = 0;

    uint8_t byte0 = pData[0];
    boolean cwr_uint8 = IsBitSet(byte0, 0);
    boolean lwet_uint8 = IsBitSet(byte0, 0);

    revs_acc = (pData[4] << 24) | (pData[3] << 16) | (pData[2] << 8) | pData[1];
    lwet = (pData[6] << 8) | pData[5];

    if (revs_acc > 0 && revs_acc - last_rv > 0)
    {
        // r:673 p:672 cdiff:1 w:2105 d:677 r: 3
        int p = last_rv;
        int t = last_tv;
        last_rv = revs_acc;
        last_tv = lwet;
        int d = (lwet - t);
        d = d == 0 ? 1 : d;
        int cdiff = (revs_acc - p);
        printf("r:%d p:%d cdiff:%d w:%d d:%d r: %f\n", last_rv, p, cdiff, cdiff * 2105, d, ((cdiff * 2105) / 100.f) / ((float_t)d / 1000.0f));

        uint16_t speed = (((cdiff * 2105) / 1.e6f) / ((float_t)d / 3.6e6f));
        if (speed >= 0 && speed < 100)
        {
            Sensor::lastv = speed;
        }
    }
    else
    {
        Sensor::lastv = 0;
    }
}
