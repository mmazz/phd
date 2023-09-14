#include <iostream>
#include <seal/ciphertext.h>
#include <system_error>
#include <vector>
#include "../src/examples.h"
#include "../src/utils_mati.h"
#include <seal/randomgen.h>
#include <seal/keygenerator.h>
#include <memory>
#include <gtest/gtest.h>

#include <bitset>
#include <chrono>

using namespace seal;
using namespace std;

TEST(NTT, a)
{
    EXPECT_EQ(0,0);
}
int main(int argc, char * argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
