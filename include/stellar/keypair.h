#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace stellar
{

class KeyPair
{
  public:
    static KeyPair fromSecretSeed(std::string const& seed);
    static KeyPair fromPublicKey(std::string const& publicKey);
    static KeyPair random();

    std::string getPublicKey() const;
    std::string getSecretSeed() const;

    std::vector<uint8_t> const& getRawPublicKey() const;
    std::vector<uint8_t> const& getRawSecretSeed() const;

    bool canSign() const;

    std::vector<uint8_t> sign(std::vector<uint8_t> const& message) const;
    bool verify(std::vector<uint8_t> const& message,
                std::vector<uint8_t> const& signature) const;

  private:
    KeyPair() = default;

    std::vector<uint8_t> m_publicKey;
    std::optional<std::vector<uint8_t>> m_secretSeed;
    // ----------------------------------------------------------
    // The expanded 64-byte secret key
    // used for signing by libsodium
    // -----------------------------------------------------------
    std::optional<std::vector<uint8_t>> m_signingKey;
};

} // namespace stellar