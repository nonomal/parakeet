#pragma once

#include "um-crypto/abstract/AXorStreamCipher.h"
#include "um-crypto/interface/IStreamCipher.h"

#include "um-crypto/types.h"

namespace umc::tencent {

class RC4Cipher : public AXorStreamCipher {
 public:
  RC4Cipher(const Vec<u8>& key);
  RC4Cipher(const u8* key, const usize key_len)
      : RC4Cipher(Vec<u8>(key, key + key_len)){};

  void HardSeek(usize offset) override;

 protected:
  void YieldNextXorBuf(Vec<u8>& buf) override;

 private:
  Vec<u8> S_;
  u32 N_;  // S.size()

  Vec<u8> key_;
  double key_hash_;
  usize segment_id_;

  void EncodeFirstSegment(u8* out);
  void EncodeOtherSegment(u8* out);
  void InitSeedbox();
  double CalculateKeyHash() const;
  u64 GetSegmentKey(usize segment_id, u64 seed) const;
};

}  // namespace umc::tencent
