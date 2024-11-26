//
#pragma once

#include <lcm/lcm_coretypes.h>

#include <string>

namespace openrobot::ocm {

/**
 * @class DesiredGroupData
 * @brief DesiredGroupData 类
 *
 * @details
 * The DesiredGroupData class handles encoding and decoding of desired group information.
 * DesiredGroupData 类负责编码和解码期望的组信息。
 */
class DesiredGroupData {
 public:
  std::string desired_group;

 public:
  /**
   * @brief Encode a message into binary form.
   * @brief 将消息编码为二进制形式。
   *
   * @param buf The output buffer.
   * @param buf 输出缓冲区。
   * @param offset Encoding starts at the byte offset into @p buf.
   * @param offset 编码从 @p buf 的字节偏移处开始。
   * @param maxlen Maximum number of bytes to write. This should generally be
   * equal to getEncodedSize().
   * @param maxlen 要写入的最大字节数。通常应等于 getEncodedSize()。
   * @return The number of bytes encoded, or <0 on error.
   * @return 编码的字节数，或在错误时返回 <0。
   */
  inline int encode(void *buf, int offset, int maxlen) const;

  /**
   * @brief Check how many bytes are required to encode this message.
   * @brief 检查编码此消息所需的字节数。
   */
  inline int getEncodedSize() const;

  /**
   * @brief Decode a message from binary form into this instance.
   * @brief 从二进制形式解码消息到此实例。
   *
   * @param buf The buffer containing the encoded message.
   * @param buf 包含编码消息的缓冲区。
   * @param offset The byte offset into @p buf where the encoded message starts.
   * @param offset 编码消息开始的 @p buf 的字节偏移量。
   * @param maxlen The maximum number of bytes to read while decoding.
   * @param maxlen 解码时要读取的最大字节数。
   * @return The number of bytes decoded, or <0 if an error occurred.
   * @return 解码的字节数，或在发生错误时返回 <0。
   */
  inline int decode(const void *buf, int offset, int maxlen);

  /**
   * @brief Retrieve the 64-bit fingerprint identifying the structure of the message.
   * @brief 获取用于标识消息结构的64位指纹。
   *
   * @details
   * Note that the fingerprint is the same for all instances of the same
   * message type, and is a fingerprint on the message type definition, not on
   * the message contents.
   *
   * 注意，该指纹对于同一消息类型的所有实例都是相同的，并且是基于消息类型定义的指纹，而不是消息内容。
   */
  inline static int64_t getHash();

  /**
   * @brief Returns "DesiredGroupData"
   * @brief 返回 "DesiredGroupData"
   */
  inline static const char *getTypeName();

  // LCM support functions. Users should not call these
  // LCM 支持函数。用户不应调用这些函数。
  inline int _encodeNoHash(void *buf, int offset, int maxlen) const;
  inline int _getEncodedSizeNoHash() const;
  inline int _decodeNoHash(const void *buf, int offset, int maxlen);
  inline static uint64_t _computeHash(const __lcm_hash_ptr *p);
};

int DesiredGroupData::encode(void *buf, int offset, int maxlen) const {
  int pos = 0, tlen;
  int64_t hash = getHash();

  tlen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &hash, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  tlen = this->_encodeNoHash(buf, offset + pos, maxlen - pos);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  return pos;
}

int DesiredGroupData::decode(const void *buf, int offset, int maxlen) {
  int pos = 0, thislen;

  int64_t msg_hash;
  thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &msg_hash, 1);
  if (thislen < 0)
    return thislen;
  else
    pos += thislen;
  if (msg_hash != getHash()) return -1;

  thislen = this->_decodeNoHash(buf, offset + pos, maxlen - pos);
  if (thislen < 0)
    return thislen;
  else
    pos += thislen;

  return pos;
}

int DesiredGroupData::getEncodedSize() const { return 8 + _getEncodedSizeNoHash(); }

int64_t DesiredGroupData::getHash() {
  static int64_t hash = static_cast<int64_t>(_computeHash(NULL));
  return hash;
}

const char *DesiredGroupData::getTypeName() { return "DesiredGroupData"; }

int DesiredGroupData::_encodeNoHash(void *buf, int offset, int maxlen) const {
  int pos = 0, tlen;

  char *desired_group_cstr = const_cast<char *>(this->desired_group.c_str());
  tlen = __string_encode_array(buf, offset + pos, maxlen - pos, &desired_group_cstr, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;

  return pos;
}

int DesiredGroupData::_decodeNoHash(const void *buf, int offset, int maxlen) {
  int pos = 0, tlen;

  int32_t __desired_group_len__;
  tlen = __int32_t_decode_array(buf, offset + pos, maxlen - pos, &__desired_group_len__, 1);
  if (tlen < 0)
    return tlen;
  else
    pos += tlen;
  if (__desired_group_len__ > maxlen - pos) return -1;
  this->desired_group.assign(static_cast<const char *>(buf) + offset + pos, __desired_group_len__ - 1);
  pos += __desired_group_len__;

  return pos;
}

int DesiredGroupData::_getEncodedSizeNoHash() const {
  int enc_size = 0;
  enc_size += this->desired_group.size() + 4 + 1;
  return enc_size;
}

uint64_t DesiredGroupData::_computeHash(const __lcm_hash_ptr *) {
  uint64_t hash = 0xe21c68d2e41b458dLL;
  return (hash << 1) + ((hash >> 63) & 1);
}

}  // namespace openrobot::ocm
//