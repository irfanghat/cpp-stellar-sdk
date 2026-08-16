#include "stellar/keypair.h"
#include "stellar/strkey.h"

#include <sodium.h>
#include <stdexcept>

namespace stellar
{

KeyPair
KeyPair::fromSecretSeed(std::string const& seed)
{
    KeyPair kp;
    kp.m_secretSeed = StrKey::decodeEd25519SecretSeed(seed);

    kp.m_publicKey.resize(crypto_sign_PUBLICKEYBYTES);
    kp.m_signingKey = std::vector<uint8_t>(crypto_sign_SECRETKEYBYTES);

    crypto_sign_seed_keypair(kp.m_publicKey.data(), kp.m_signingKey->data(),
                             kp.m_secretSeed->data());

    return kp;
}

KeyPair
KeyPair::fromPublicKey(std::string const& publicKey)
{
    KeyPair kp;
    kp.m_publicKey = StrKey::decodeEd25519PublicKey(publicKey);
    return kp;
}

KeyPair
KeyPair::random()
{
    KeyPair kp;
    kp.m_secretSeed = std::vector<uint8_t>(crypto_sign_SEEDBYTES);
    randombytes_buf(kp.m_secretSeed->data(), crypto_sign_SEEDBYTES);

    kp.m_publicKey.resize(crypto_sign_PUBLICKEYBYTES);
    kp.m_signingKey = std::vector<uint8_t>(crypto_sign_SECRETKEYBYTES);

    crypto_sign_seed_keypair(kp.m_publicKey.data(), kp.m_signingKey->data(),
                             kp.m_secretSeed->data());

    return kp;
}

std::string
KeyPair::getPublicKey() const
{
    return StrKey::encodeEd25519PublicKey(m_publicKey);
}

std::string
KeyPair::getSecretSeed() const
{
    if (!m_secretSeed)
    {
        throw std::runtime_error("KeyPair does not contain secret seed");
    }
    return StrKey::encodeEd25519SecretSeed(*m_secretSeed);
}

std::vector<uint8_t> const&
KeyPair::getRawPublicKey() const
{
    return m_publicKey;
}

std::vector<uint8_t> const&
KeyPair::getRawSecretSeed() const
{
    if (!m_secretSeed)
    {
        throw std::runtime_error("KeyPair does not contain secret seed");
    }
    return *m_secretSeed;
}

bool
KeyPair::canSign() const
{
    return m_signingKey.has_value();
}

std::vector<uint8_t>
KeyPair::sign(std::vector<uint8_t> const& message) const
{
    if (!canSign())
    {
        throw std::runtime_error("Cannot sign: no secret key available");
    }
    std::vector<uint8_t> signature(crypto_sign_BYTES);
    crypto_sign_detached(signature.data(), nullptr, message.data(),
                         message.size(), m_signingKey->data());
    return signature;
}

bool
KeyPair::verify(std::vector<uint8_t> const& message,
                std::vector<uint8_t> const& signature) const
{
    if (signature.size() != crypto_sign_BYTES)
    {
        return false;
    }
    return crypto_sign_verify_detached(signature.data(), message.data(),
                                       message.size(), m_publicKey.data()) == 0;
}

}