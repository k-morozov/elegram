#include "hashing.h"

#include <openssl/sha.h>
#include <memory>

namespace elegram {
  namespace server {
    std::string hash_password(const std::string &password) {
        std::unique_ptr<unsigned char[]> res = std::make_unique<unsigned char[]>(SHA256_DIGEST_LENGTH);
        SHA256(reinterpret_cast<const unsigned char *>(password.data()), password.size(), res.get());
        return std::string{reinterpret_cast<const char *>(res.release()), SHA256_DIGEST_LENGTH};
    }
  } // namespace server
} // namespace elegram
