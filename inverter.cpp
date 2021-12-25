#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <iostream>
#include <string>

#include "inverter.hpp"

auto Inverter::create_payload(const int serial_number)
    -> std::array<unsigned char, 16> {
  /**
   * Borrowed from: https://github.com/arjenv/omnikstats
   *
   * Generate the payload:
   * - First 4 bytes are fixed x68 x02 x40 x30
   * - Next 8 bytes are the reversed serial number twice(hex)
   * - Next 2 bytes are fixed x01 x00
   * - Next byte is a checksum (2x each binary number form the serial number +
   *   115) Last byte is fixed x16
   */
  int checksum = 0;
  std::array<unsigned char, 16> payload{
      0x68, 0x02, 0x40, 0x30, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x16,
  };
  for (auto i = 0; i < 4; i++) {
    payload.at(4 + i) = payload.at(8 + i) =
        static_cast<unsigned char>(serial_number >> 8 * i) & 0xff;
    checksum += payload.at(4 + i);
  }

  checksum *= 2;
  checksum += 115;
  checksum &= 0xff;
  payload[14] = static_cast<unsigned char>(checksum);

  return payload;
}

auto Inverter::retrieve_data(const std::array<unsigned char, 16> payload)
    -> std::array<unsigned char, 256> {
  int sock{};
  struct sockaddr_in serv_addr {};
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "Socket creation error" << '\n';
    exit(EXIT_FAILURE);
  }

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(m_port);

  if (inet_pton(AF_INET, m_host.c_str(), &serv_addr.sin_addr) <= 0) {
    std::cerr << "Invalid IP address / IP address not supported" << '\n';
    exit(EXIT_FAILURE);
  }

  sockaddr serv_addr_copy{};
  std::memcpy(&serv_addr_copy, &serv_addr, sizeof(serv_addr));
  if (connect(sock, &serv_addr_copy, sizeof(serv_addr)) < 0) {
    std::cerr << "Connection Failed" << '\n';
    exit(EXIT_FAILURE);
  }

  send(sock, payload.data(), 16, 0);

  std::array<unsigned char, 256> response{0};
  read(sock, response.data(), sizeof(response));

  return response;
}

auto Inverter::get_serial_number(
    const std::array<unsigned char, 256> &response_data) -> std::string {
  return std::string{std::begin(response_data) + 15,
                     std::begin(response_data) + 31};
}

auto Inverter::get_power(const std::array<unsigned char, 256> &response_data)
    -> int {
  return (response_data[59] << 8) | response_data[60];
};

auto Inverter::get_energy_today(
    const std::array<unsigned char, 256> &response_data) -> double {
  return static_cast<double>((response_data[69] << 8) | response_data[70]) /
         100.0;
}

auto Inverter::get_energy_total(
    const std::array<unsigned char, 256> &response_data) -> double {
  return static_cast<double>(response_data[71] << 24 | response_data[72] << 16 |
                             response_data[73] << 8 | response_data[74]) /
         10.0;
}

auto Inverter::get_input_voltage(
    const std::array<unsigned char, 256> &response_data) -> double {
  return static_cast<double>((response_data[33] << 8) | response_data[34]) /
         10.0;
}

auto Inverter::get_input_current(
    const std::array<unsigned char, 256> &response_data) -> double {
  return static_cast<double>((response_data[39] << 8) | response_data[40]) /
         10.0;
}

auto Inverter::get_output_voltage(
    const std::array<unsigned char, 256> &response_data) -> double {
  return static_cast<double>((response_data[51] << 8) | response_data[52]) /
         10.0;
}

auto Inverter::get_output_current(
    const std::array<unsigned char, 256> &response_data) -> double {
  return static_cast<double>((response_data[45] << 8) | response_data[46]) /
         10.0;
}

auto Inverter::get_output_frequency(
    const std::array<unsigned char, 256> &response_data) -> double {
  return static_cast<double>((response_data[57] << 8) | response_data[58]) /
         100.0;
}

auto Inverter::get_temperature(
    const std::array<unsigned char, 256> &response_data) -> double {
  const double temperature{
      static_cast<double>((response_data[31] << 8) | response_data[32]) / 10.0};

  return (temperature < 250) ? temperature : 0.0;
}

Inverter::Inverter(int serial_number, std::string host, int port)
    : m_serial_number{serial_number}, m_host{std::move(host)}, m_port{port} {}

auto Inverter::get_metrics() -> Metrics {
  const std::array<unsigned char, 16> payload{create_payload(m_serial_number)};
  const std::array<unsigned char, 256> response_data{retrieve_data(payload)};

  return Metrics{
      get_serial_number(response_data),    get_power(response_data),
      get_energy_today(response_data),     get_energy_total(response_data),
      get_input_voltage(response_data),    get_input_current(response_data),
      get_output_voltage(response_data),   get_output_current(response_data),
      get_output_frequency(response_data), get_temperature(response_data),
  };
}
