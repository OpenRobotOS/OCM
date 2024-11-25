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
 *
 * This function takes an IPv4 address in string format (e.g., "192.168.1.1"),
 * converts it to its 32-bit integer representation, and then formats it as an
 * 8-character hexadecimal string. This unique hash can be useful for various
 * applications such as network device identification, hashing algorithms, or
 * creating compact representations of IP addresses.
 *
 * @param ip_str The IPv4 address as a string.
 * @return A string containing the unique hexadecimal hash of the IPv4 address.
 *
 * @throws std::invalid_argument If the provided IP address is invalid.
 *
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
  // inet_pton returns 1 on success, 0 if the address is invalid, and -1 on error.
  if (inet_pton(AF_INET, ip_str.c_str(), &ipv4_addr) != 1) {
    throw std::invalid_argument("Invalid IPv4 address: " + ip_str);
  }

  // Convert the binary address to host byte order (from network byte order).
  uint32_t ip_int = ntohl(ipv4_addr.s_addr);

  // Create a string stream to format the integer as an 8-character hexadecimal string.
  std::ostringstream oss;
  oss << std::hex           // Use hexadecimal format
      << std::setw(8)       // Set width to 8 characters
      << std::setfill('0')  // Fill with leading zeros if necessary
      << ip_int;            // Insert the integer value

  return oss.str();
}
