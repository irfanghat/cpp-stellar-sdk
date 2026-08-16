#include "stellar/keypair.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace stellar;

// ---------------------------------------------------------
// These vectors are a real, self-consistent
// ed25519 keypair run through StrKey::encode.
// Otherwise these tests would fail partially.
// ---------------------------------------------------------

TEST(KeyPairTest, FromSecretSeed)
{
    std::string seed =
        "SBBLQM7C5RYSRDIZZ5YJ5IPFMDDIZWJLCSM7GWVLEHPEODUCCHQNU476";
    std::string public_key =
        "GDHXSFBTYCKGUAAATSUZVKQQAGOBTOWNS5E7THZFJX3IETDDCRKVRTSY";

    KeyPair kp = KeyPair::fromSecretSeed(seed);

    ASSERT_EQ(kp.getPublicKey(), public_key);
    ASSERT_EQ(kp.getSecretSeed(), seed);
    ASSERT_TRUE(kp.canSign());
}

TEST(KeyPairTest, FromPublicKey)
{
    std::string public_key =
        "GDHXSFBTYCKGUAAATSUZVKQQAGOBTOWNS5E7THZFJX3IETDDCRKVRTSY";
    KeyPair kp = KeyPair::fromPublicKey(public_key);

    ASSERT_EQ(kp.getPublicKey(), public_key);
    ASSERT_FALSE(kp.canSign());
    ASSERT_THROW(kp.getSecretSeed(), std::runtime_error);
}

TEST(KeyPairTest, RandomKeyPair)
{
    KeyPair kp1 = KeyPair::random();
    KeyPair kp2 = KeyPair::random();

    ASSERT_NE(kp1.getPublicKey(), kp2.getPublicKey());
    ASSERT_NE(kp1.getSecretSeed(), kp2.getSecretSeed());
    ASSERT_TRUE(kp1.canSign());
}

TEST(KeyPairTest, SignAndVerify)
{
    KeyPair kp = KeyPair::random();
    std::string message_str = "Stellar C++ SDK";
    std::vector<uint8_t> message(message_str.begin(), message_str.end());

    std::vector<uint8_t> signature = kp.sign(message);
    ASSERT_EQ(signature.size(), 64);
    ASSERT_TRUE(kp.verify(message, signature));

    // -------------------------------------------------
    // Verification should fail with wrong message
    // -------------------------------------------------
    std::string wrong_message_str = "C++ Stellar SDK";
    std::vector<uint8_t> wrong_message(wrong_message_str.begin(),
                                       wrong_message_str.end());
    ASSERT_FALSE(kp.verify(wrong_message, signature));

    // -------------------------------------------------
    // Verification should fail with wrong key
    // -------------------------------------------------
    KeyPair other_kp = KeyPair::random();
    ASSERT_FALSE(other_kp.verify(message, signature));
}

TEST(KeyPairTest, DecodeRejectsCorruptedChecksum)
{
    // ------------------------------------------------------------------
    // A StrKey whose checksum does not match its payload must be rejected.
    // This is mostly for documentation purposes as reminder to contributors
    // that only real, self-consistent keypairs will work.
    // ------------------------------------------------------------------
    std::string corrupted =
        "GDBA34K24R44L6M6E52I37HZH3B2D4K2YCCQ6T4V3Z6J6S6Q4Z6J6S6Q";
    ASSERT_THROW(KeyPair::fromPublicKey(corrupted), std::invalid_argument);
}