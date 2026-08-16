#pragma once

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace stellar
{

enum class StrKeyVersionByte : uint8_t
{
    ACCOUNT_ID = 6 << 3,   // G
    SECRET_SEED = 18 << 3, // S
};

class StrKey
{
  public:
    static std::string encode(StrKeyVersionByte version,
                              std::vector<uint8_t> const& data);
    static std::vector<uint8_t> decode(StrKeyVersionByte expected_version,
                                       std::string const& encoded);

    static std::string encodeEd25519PublicKey(std::vector<uint8_t> const& data);
    static std::string
    encodeEd25519SecretSeed(std::vector<uint8_t> const& data);
    static std::vector<uint8_t> decodeEd25519PublicKey(std::string const& data);
    static std::vector<uint8_t>
    decodeEd25519SecretSeed(std::string const& data);
};

}