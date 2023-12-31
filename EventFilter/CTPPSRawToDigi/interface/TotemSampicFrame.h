/****************************************************************************
*
* This is a part of the TOTEM offline software.
* Authors:
*   Nicola Minafra
*
****************************************************************************/

#ifndef EventFilter_CTPPSRawToDigi_TotemSampicFrame
#define EventFilter_CTPPSRawToDigi_TotemSampicFrame

#include <vector>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <bitset>
#include <iostream>

#include "EventFilter/CTPPSRawToDigi/interface/VFATFrame.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

enum TotemSampicConstant {
  hwId_Position = 0,  // hwId_Size = 1,
  controlBits0_Position = 1,
  controlBits1_Position = 2,
  controlBits2_Position = 3,
  controlBits3_Position = 4,
  controlBits4_Position = 5,
  controlBits5_Position = 6,
  fpgaTime_Position = 7,     // fpgaTime_Size = 5,
  timestampA_Position = 12,  // timestampA_Size = 2,
  fpgaTime_Size = timestampA_Position - fpgaTime_Position,
  timestampB_Position = 14,  // timestampB_Size = 2,
  timestampA_Size = timestampB_Position - timestampA_Position,
  cellInfo_Position = 16,  // cellInfo_Size = 2,
  timestampB_Size = cellInfo_Position - timestampB_Position,
  planeChannelId_Position = 18,  // planeChannelId_Size = 1,
  cellInfo_Size = planeChannelId_Position - cellInfo_Position,
  reserved_Position = 19,  // reserved_Size = 5,

  boardId_Position = 0,       // boardId_Size = 1,
  l1ATimestamp_Position = 1,  // l1ATimestamp_Size = 5,
  bunchNumber_Position = 6,   // bunchNumber_Size = 2,
  l1ATimestamp_Size = bunchNumber_Position - l1ATimestamp_Position,
  orbitNumber_Position = 8,  // orbitNumber_Size = 4,
  bunchNumber_Size = orbitNumber_Position - bunchNumber_Position,
  eventNumber_Position = 12,  // eventNumber_Size = 4,
  orbitNumber_Size = eventNumber_Position - orbitNumber_Position,
  channelMap_Position = 16,  // channelMap_Size = 2,
  eventNumber_Size = channelMap_Position - eventNumber_Position,
  l1ALatency_Position = 18,  // l1ALatency_Size = 2,
  channelMap_Size = l1ALatency_Position - channelMap_Position,
  numberOfSamples_Position = 20,  // numberOfSamples_Size = 1,
  l1ALatency_Size = numberOfSamples_Position - l1ALatency_Position,
  offsetOfSamples_Position = 21,  // offsetOfSamples_Size = 1,
  fwVersion_Position = 22,        // fwVersion_Size = 1,
  pllInfo_Position = 23,          // pllInfo_Size = 1,

  numberOfSamples = 24,
  controlBits3 = 0x69,
  cellInfo_Mask = 0x3F,

};

template <typename T>
T grayToBinary(const T& gcode_data) {
  // assign b[0] = g[0]
  T binary = gcode_data & (0x0001 << (8 * sizeof(T) - 1));  // MSB is the same

  // assign b[i] = g[i] xor b[i-1]
  for (unsigned short int i = 1; i < 8 * sizeof(T); ++i)
    binary |= (gcode_data ^ (binary >> 1)) & (0x0001 << (8 * sizeof(T) - i - 1));

  return binary;
}

/**
 * This class is intended to handle the timing infromation of SAMPIC in the TOTEM implementation
**/
class TotemSampicFrame {
public:
  TotemSampicFrame(const uint8_t* chInfoPtr, const uint8_t* chDataPtr, const uint8_t* eventInfoPtr)
      : totemSampicInfoPtr_(chInfoPtr),
        totemSampicDataPtr_(chDataPtr),
        totemSampicEventInfoPtr_(eventInfoPtr),
        status_(0) {
    if (chInfoPtr != nullptr && chDataPtr != nullptr && eventInfoPtr != nullptr &&
        totemSampicInfoPtr_[TotemSampicConstant::controlBits3_Position] == TotemSampicConstant::controlBits3)
      status_ = 1;
  }
  ~TotemSampicFrame() {}

  /// Prints the frame.
  /// If binary is true, binary format is used.
  void printRaw(bool binary = false) const {
    edm::LogInfo("TotemSampicFrame") << "Event Info: \n";
    printRawBuffer((uint16_t*)totemSampicEventInfoPtr_);

    edm::LogInfo("TotemSampicFrame") << "Channel Info: \n";
    printRawBuffer((uint16_t*)totemSampicInfoPtr_);

    edm::LogInfo("TotemSampicFrame") << "Channel Data: \n";
    printRawBuffer((uint16_t*)totemSampicDataPtr_);
  }

  void print() const {
    std::bitset<16> bitsChannelMap(getChannelMap());
    std::bitset<16> bitsPLLInfo(getPLLInfo());
    edm::LogInfo("TotemSampicFrame") << "\nEvent:"
                                     << "\nHardwareId (Event):\t" << std::hex << (unsigned int)getEventHardwareId()
                                     << "\nL1A Timestamp:\t" << std::dec << getL1ATimestamp() << "\nL1A Latency:\t"
                                     << std::dec << getL1ALatency() << "\nBunch Number:\t" << std::dec
                                     << getBunchNumber() << "\nOrbit Number:\t" << std::dec << getOrbitNumber()
                                     << "\nEvent Number:\t" << std::dec << getEventNumber() << "\nChannels fired:\t"
                                     << std::hex << bitsChannelMap.to_string() << "\nNumber of Samples:\t" << std::dec
                                     << getNumberOfSentSamples() << "\nOffset of Samples:\t" << std::dec
                                     << (int)getOffsetOfSamples() << "\nFW Version:\t" << std::hex
                                     << (int)getFWVersion() << "\nChannel:\nHardwareId:\t" << std::hex
                                     << (unsigned int)getHardwareId() << "\nFPGATimestamp:\t" << std::dec
                                     << getFPGATimestamp() << "\nTimestampA:\t" << std::dec << getTimestampA()
                                     << "\nTimestampB:\t" << std::dec << getTimestampB() << "\nCellInfo:\t" << std::dec
                                     << getCellInfo() << "\nPlane:\t" << std::dec << getDetPlane() << "\nChannel:\t"
                                     << std::dec << getDetChannel() << "\nPLL Info:\t" << bitsPLLInfo.to_string()
                                     << "\n\n";
  }

  // All getters
  inline uint8_t getHardwareId() const {
    uint8_t tmp = 0;
    if (status_)
      tmp = totemSampicInfoPtr_[TotemSampicConstant::hwId_Position];
    return tmp;
  }

  template <class T>
  T extractDataFromBytesLSB(const uint8_t* array, int start, int size) const {
    T result = 0;
    for (int i = start + size - 1; i >= start; i--)
      result = (result << 8) | array[i];
    return result;
  }

  inline uint64_t getFPGATimestamp() const {
    uint64_t tmp = 0;
    if (status_)
      tmp =
          extractDataFromBytesLSB<uint64_t>(totemSampicInfoPtr_, TotemSampicConstant::fpgaTime_Position, fpgaTime_Size);
    return tmp;
  }

  inline uint16_t getTimestampA() const {
    uint16_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint16_t>(
          totemSampicInfoPtr_, TotemSampicConstant::timestampA_Position, timestampA_Size);
    tmp = 0xFFF - tmp;
    return grayToBinary<uint16_t>(tmp);
  }

  inline uint16_t getTimestampB() const {
    uint16_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint16_t>(
          totemSampicInfoPtr_, TotemSampicConstant::timestampB_Position, timestampB_Size);
    return grayToBinary<uint16_t>(tmp);
  }

  inline uint16_t getCellInfo() const {
    uint16_t tmp = 0;
    if (status_)
      tmp =
          extractDataFromBytesLSB<uint16_t>(totemSampicInfoPtr_, TotemSampicConstant::cellInfo_Position, cellInfo_Size);
    return tmp & TotemSampicConstant::cellInfo_Mask;
  }

  inline int getDetPlane() const {
    int tmp = 0;
    if (status_)
      tmp = (totemSampicInfoPtr_[planeChannelId_Position] & 0xF0) >> 4;
    return tmp;
  }

  inline int getDetChannel() const {
    int tmp = 0;
    if (status_)
      tmp = totemSampicInfoPtr_[planeChannelId_Position] & 0x0F;
    return tmp;
  }

  const std::vector<uint8_t> getSamples() const {
    std::vector<uint8_t> samples;
    if (status_) {
      samples.assign(totemSampicDataPtr_, totemSampicDataPtr_ + TotemSampicConstant::numberOfSamples);
      for (auto it = samples.begin(); it != samples.end(); ++it)
        *it = grayToBinary<uint8_t>(*it);
    }
    return samples;
  }

  inline unsigned int getNumberOfSamples() const { return status_ * TotemSampicConstant::numberOfSamples; }

  // Event Info
  inline uint8_t getEventHardwareId() const {
    uint8_t tmp = 0;
    if (status_)
      tmp = totemSampicEventInfoPtr_[TotemSampicConstant::boardId_Position];
    return tmp;
  }

  inline uint64_t getL1ATimestamp() const {
    uint64_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint64_t>(
          totemSampicEventInfoPtr_, TotemSampicConstant::l1ATimestamp_Position, l1ATimestamp_Size);
    return tmp;
  }

  inline uint16_t getBunchNumber() const {
    uint16_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint16_t>(
          totemSampicEventInfoPtr_, TotemSampicConstant::bunchNumber_Position, bunchNumber_Size);
    return tmp;
  }

  inline uint32_t getOrbitNumber() const {
    uint32_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint32_t>(
          totemSampicEventInfoPtr_, TotemSampicConstant::orbitNumber_Position, orbitNumber_Size);
    return tmp;
  }

  inline uint32_t getEventNumber() const {
    uint32_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint32_t>(
          totemSampicEventInfoPtr_, TotemSampicConstant::eventNumber_Position, eventNumber_Size);
    return tmp;
  }

  inline uint16_t getChannelMap() const {
    uint16_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint16_t>(
          totemSampicEventInfoPtr_, TotemSampicConstant::channelMap_Position, channelMap_Size);
    return tmp;
  }

  inline uint16_t getL1ALatency() const {
    uint16_t tmp = 0;
    if (status_)
      tmp = extractDataFromBytesLSB<uint16_t>(
          totemSampicEventInfoPtr_, TotemSampicConstant::l1ALatency_Position, l1ALatency_Size);
    return tmp;
  }

  inline uint8_t getNumberOfSentSamples() const {
    uint8_t tmp = 0;
    if (status_)
      tmp = totemSampicEventInfoPtr_[TotemSampicConstant::numberOfSamples_Position];
    return tmp;
  }

  inline uint8_t getOffsetOfSamples() const {
    uint8_t tmp = 0;
    if (status_)
      tmp = totemSampicEventInfoPtr_[TotemSampicConstant::offsetOfSamples_Position];
    return tmp;
  }

  inline uint8_t getPLLInfo() const {
    uint8_t tmp = 0;
    if (status_)
      tmp = totemSampicEventInfoPtr_[pllInfo_Position];
    return tmp;
  }

  inline uint8_t getFWVersion() const {
    uint8_t tmp = 0;
    if (status_)
      tmp = totemSampicEventInfoPtr_[fwVersion_Position];
    return tmp;
  }

  inline bool valid() const { return status_ != 0; }

protected:
  const uint8_t* totemSampicInfoPtr_;
  const uint8_t* totemSampicDataPtr_;
  const uint8_t* totemSampicEventInfoPtr_;

  int status_;

  inline void printRawBuffer(const uint16_t* buffer, const bool binary = false, const unsigned int size = 12) const {
    for (unsigned int i = 0; i < size; i++) {
      if (binary) {
        std::bitset<16> bits(*(buffer++));
        edm::LogInfo("TotemSampicFrame") << bits.to_string() << "\n";
      } else
        edm::LogInfo("TotemSampicFrame") << std::setfill('0') << std::setw(4) << std::hex << *(buffer++) << "\n";
    }
  }
};

#endif
