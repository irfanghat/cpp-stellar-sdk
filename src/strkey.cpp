#include "stellar/strkey.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace stellar
{

namespace
{

// CRC-16-XMODEM calculation.
uint16_t
crc16(std::vector<uint8_t> const& data)
{
    uint16_t crc = 0x0000;

    for (uint8_t byte : data)
    {
        crc ^= static_cast<uint16_t>(byte) << 8;

        for (int i = 0; i < 8; ++i)
        {
            if (crc & 0x8000)
            {
                crc = static_cast<uint16_t>((crc << 1) ^ 0x1021);
            }
            else
            {
                crc = static_cast<uint16_t>(crc << 1);
            }
        }
    }

    return crc;
}

// RFC 4648 Base32 alphabet used by Stellar StrKey.
constexpr char BASE32_ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

std::string
base32_encode(std::vector<uint8_t> const& data)
{
    std::string out;

    uint32_t buffer = 0;
    int bits = 0;

    for (uint8_t byte : data)
    {
        buffer = (buffer << 8) | byte;
        bits += 8;

        while (bits >= 5)
        {
            bits -= 5;

            out += BASE32_ALPHABET[(buffer >> bits) & 0x1F];

            if (bits == 0)
            {
                buffer = 0;
            }
            else
            {
                buffer &= (1U << bits) - 1;
            }
        }
    }

    if (bits > 0)
    {
        out += BASE32_ALPHABET[(buffer << (5 - bits)) & 0x1F];
    }

    // -----------------------------------------------------------
    // Stellar StrKeys are unpadded.
    // Reference:
    // https://github.com/stellar/stellar-core/blob/master/src/crypto/StrKey.cpp
    // -----------------------------------------------------------
    return out;
}

uint8_t
base32_value(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        c = static_cast<char>(c - 'a' + 'A');
    }

    if (c >= 'A' && c <= 'Z')
    {
        return static_cast<uint8_t>(c - 'A');
    }

    if (c >= '2' && c <= '7')
    {
        return static_cast<uint8_t>(c - '2' + 26);
    }

    throw std::invalid_argument("Invalid character in Base32 string");
}

std::vector<uint8_t>
base32_decode(std::string const& encoded)
{
    std::vector<uint8_t> out;

    uint32_t buffer = 0;
    int bits = 0;

    for (char c : encoded)
    {
        uint8_t const value = base32_value(c);

        buffer = (buffer << 5) | value;
        bits += 5;

        if (bits >= 8)
        {
            bits -= 8;

            out.push_back(static_cast<uint8_t>((buffer >> bits) & 0xFF));

            if (bits == 0)
            {
                buffer = 0;
            }
            else
            {
                buffer &= (1U << bits) - 1;
            }
        }
    }

    return out;
}

} // namespace

std::string
StrKey::encode(StrKeyVersionByte version, std::vector<uint8_t> const& data)
{
    std::vector<uint8_t> payload;

    payload.reserve(data.size() + 3);

    uint8_t const version_byte = static_cast<uint8_t>(version);
    payload.push_back(version_byte);

    payload.insert(payload.end(), data.begin(), data.end());

    // --------------------------------------
    // CRC covers version byte & payload.
    // --------------------------------------
    uint16_t const checksum = crc16(payload);

    // -----------------------------------------
    // Stellar Core stores CRC16 little-endian
    // i.e. LSB -> MSB
    // -----------------------------------------
    payload.push_back(static_cast<uint8_t>(checksum & 0xFF));

    payload.push_back(static_cast<uint8_t>((checksum >> 8) & 0xFF));

    return base32_encode(payload);
}

std::vector<uint8_t>
StrKey::decode(StrKeyVersionByte expected_version, std::string const& encoded)
{
    if (encoded.empty())
    {
        throw std::invalid_argument("Encoded string is empty");
    }

    // -------------------------------------------------------
    // Stellar StrKeys have Base32 lengths divisible by 8.
    // -------------------------------------------------------
    if ((encoded.size() & 7U) != 0)
    {
        throw std::invalid_argument("Invalid Base32 string length");
    }

    std::vector<uint8_t> const decoded = base32_decode(encoded);

    if (decoded.size() < 3)
    {
        throw std::invalid_argument("Decoded data is too short");
    }

    // ----------------------------------
    // Remove the two checksum bytes.
    // ----------------------------------
    std::vector<uint8_t> const payload(decoded.begin(), decoded.end() - 2);

    // ------------------------------------------
    // Reconstruct the little-endian checksum.
    // ------------------------------------------
    uint16_t const checksum =
        static_cast<uint16_t>(decoded[decoded.size() - 2]) |
        (static_cast<uint16_t>(decoded[decoded.size() - 1]) << 8);

    uint16_t const expected_checksum = crc16(payload);

    if (checksum != expected_checksum)
    {
        throw std::invalid_argument("Invalid checksum");
    }

    uint8_t const version = payload[0];

    if (version != static_cast<uint8_t>(expected_version))
    {
        throw std::invalid_argument("Invalid version byte");
    }

    return std::vector<uint8_t>(payload.begin() + 1, payload.end());
}

std::string
StrKey::encodeEd25519PublicKey(std::vector<uint8_t> const& data)
{
    if (data.size() != 32)
    {
        throw std::invalid_argument("Invalid public key size");
    }

    return encode(StrKeyVersionByte::ACCOUNT_ID, data);
}

std::string
StrKey::encodeEd25519SecretSeed(std::vector<uint8_t> const& data)
{
    if (data.size() != 32)
    {
        throw std::invalid_argument("Invalid secret seed size");
    }

    return encode(StrKeyVersionByte::SECRET_SEED, data);
}

std::vector<uint8_t>
StrKey::decodeEd25519PublicKey(std::string const& data)
{
    return decode(StrKeyVersionByte::ACCOUNT_ID, data);
}

std::vector<uint8_t>
StrKey::decodeEd25519SecretSeed(std::string const& data)
{
    return decode(StrKeyVersionByte::SECRET_SEED, data);
}

} // namespace stellar