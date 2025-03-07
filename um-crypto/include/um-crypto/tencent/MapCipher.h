#pragma once

#include "um-crypto/abstract/AXorStreamCipher.h"
#include "um-crypto/interface/IStreamCipher.h"
#include "um-crypto/types.h"

namespace umc::tencent {

class MapCipher : public AXorStreamCipher {
 public:
  MapCipher(const Vec<u8>& key);
  MapCipher(const u8* key, const usize key_len)
      : MapCipher(Vec<u8>(key, key + key_len)){};

 protected:
  void YieldNextXorBuf(Vec<u8>& buf) override;

 private:
  Vec<u8> table;
  std::pair<u8, u8> first_bytes;
};

}  // namespace umc::tencent
