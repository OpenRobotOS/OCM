// Start of Selection
#pragma once
#include <arpa/inet.h>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

/**
 * @brief Converts an IPv4 address string to a unique hexadecimal hash.
 * @brief 将IPv4地址字符串转换为唯一的十六进制哈希值。
 *
 * This function takes an IPv4 address in string format (e.g., "192.168.1.1"),
 * converts it to its 32-bit integer representation, and then formats it as an
 * 8-character hexadecimal string. This unique hash can be useful for various
 * applications such as network device identification, hashing algorithms, or
 * creating compact representations of IP addresses.
 *
 * 此函数接受IPv4地址的字符串格式（例如 "192.168.1.1"），
 * 将其转换为32位整数表示形式，然后将其格式化为8个字符的十六进制字符串。
 * 这个唯一的哈希值可以用于各种应用，如网络设备识别、哈希算法，
 * 或创建IP地址的紧凑表示。
 *
 * @param ip_str The IPv4 address as a string.
 * @param ip_str IPv4地址的字符串形式。
 * @return A string containing the unique hexadecimal hash of the IPv4 address.
 * @return 一个包含IPv4地址唯一十六进制哈希值的字符串。
 *
 * @throws std::invalid_argument If the provided IP address is invalid.
 * @throws std::invalid_argument 如果提供的IP地址无效。
 *
 * @example
 * @code
 * @example
 * @code
 * try {
 *     std::string hash = ipv4_to_unique_hash_hex("192.168.1.1");
 *     std::cout << "Hex Hash: " << hash << std::endl; // Output: Hex Hash: c0a80101
 * } catch (const std::invalid_argument& e) {
 *     std::cerr << e.what() << std::endl;
 * }
 * @endcode
 */

inline std::string ipv4_to_unique_hash_hex(const std::string& ip_str) {
  struct in_addr ipv4_addr;

  // Convert the IPv4 address from text to binary form.
  // 将IPv4地址从文本形式转换为二进制形式。
  if (inet_pton(AF_INET, ip_str.c_str(), &ipv4_addr) != 1) {
    throw std::invalid_argument("Invalid IPv4 address: " + ip_str);
  }

  // Convert the binary address to host byte order (from network byte order).
  // 将二进制地址从网络字节序转换为主机字节序。
  uint32_t ip_int = ntohl(ipv4_addr.s_addr);

  // Create a string stream to format the integer as an 8-character hexadecimal string.
  // 创建一个字符串流，将整数格式化为8字符的十六进制字符串。
  std::ostringstream oss;
  oss << std::hex           // Use hexadecimal format
      << std::setw(8)       // 设置宽度为8个字符
      << std::setfill('0')  // 如果必要，用零填充
      << ip_int;            // 插入整数值

  return oss.str();
}
// End of Selection