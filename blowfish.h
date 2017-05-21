//
// Blowfish C++ implementation
//
// CC0 - PUBLIC DOMAIN
// This work is free of known copyright restrictions.
// http://creativecommons.org/publicdomain/zero/1.0/
//

#pragma once

#ifndef __blowfish__
#define __blowfish__

#include <stdint.h>
#include <cstddef>
#include <string>
#include <sstream>
#include <iomanip>

class Blowfish {
public:
  Blowfish(const std::string &key);
  std::string Encrypt(const std::string &src) const;
  std::string Decrypt(const std::string &src) const;

private:
  void SetKey(const char *key, size_t byte_length);
  void EncryptBlock(uint32_t *left, uint32_t *right) const;
  void DecryptBlock(uint32_t *left, uint32_t *right) const;
  uint32_t Feistel(uint32_t value) const;
  std::string data2hex(std::string data) const;
  std::string hex2data(const std::string& hex) const;
	
private:
  uint32_t pary_[18];
  uint32_t sbox_[4][256];
};

#endif /* defined(__blowfish__) */
