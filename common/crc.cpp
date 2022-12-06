#include "common/crc.h"

namespace Common {

DEFINE_NORMAL_CRC(uint16, 0x1021);
DEFINE_REFLECTED_CRC(uint16, 0xa001);
DEFINE_REFLECTED_CRC(uint32, 0xEDB88320);

}
