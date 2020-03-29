#include "gtest/gtest.h"

#include "utils/Utils.hpp"

using namespace sg;

TEST(UtilsTest, RsBusFloatConversion)
{
    for (unsigned int i = 1; i < 1024 * 256; i += 256)
    {
        float val = 1.0 / i;
        float enc = Utils::encodeRsBus(val);
        float res = Utils::decodeRsBus(enc);
        EXPECT_EQ(val, res);
        
        val = -1.0 / i;
        enc = Utils::encodeRsBus(val);
        res = Utils::decodeRsBus(enc);
        EXPECT_EQ(val, res);

        val = i / 256.0;
        enc = Utils::encodeRsBus(val);
        res = Utils::decodeRsBus(enc);
        EXPECT_EQ(val, res);

        val = i / -256.0;
        enc = Utils::encodeRsBus(val);
        res = Utils::decodeRsBus(enc);
        EXPECT_EQ(val, res);
    }
}
