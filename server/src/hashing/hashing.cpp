#include "hashing.h"

#include <openssl/sha.h>
#include <memory>

namespace elegram {
  namespace server {
    pqxx::binarystring hash_password(const std::string &password) {
        std::unique_ptr<unsigned char[]> res = std::make_unique<unsigned char[]>(SHA256_DIGEST_LENGTH);
        SHA256(reinterpret_cast<const unsigned char *>(password.data()), password.size(), res.get());
        // binarystring can't be constructed from ptr with moving pointer inside it, so it COPY data =(
        return pqxx::binarystring{reinterpret_cast<const char *>(res.get()), SHA256_DIGEST_LENGTH};
    }
  } // namespace server
} // namespace elegram
