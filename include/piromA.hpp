/*
// Copyright (c) 2023 9elements GmbH
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#pragma once

#include "pirom.hpp"

namespace pirom {
class PIROMA : public PIROM
{
    struct PIROMHeader
    {
        uint8_t  DataFormatRevision;
        uint16_t PIROMSize;
        uint8_t  ProcessorDataAddress;
        uint8_t  ProcessorCoreAddress;
        uint8_t  ProcessorUnCoreAddress;
        uint8_t  CacheDataAddress;
        uint8_t  PackageDataAddress;
        uint8_t  VoltageDataAddress;
        uint8_t  PartNumberDataAddress;
        uint8_t  ThermalDataAddress;
        uint8_t  FeatureDataAddress;
        uint8_t  PPINDataAddress;
   } __attribute__((packed));

    struct PIROMProcessorData
    {
        char Sspec[6];
        uint8_t SampleProd;
        uint16_t NumberOfCores;
        uint8_t HyperThreadingSupport;
        uint16_t SystemBusSpeed;
        uint8_t Segment;
    } __attribute__((packed));

    struct PIROMCoreData
    {
        uint16_t Cpuid;
        uint16_t ExtendedModelID;
        uint16_t MaximumP1CoreFrequency;
        uint16_t MaximumP0CoreFrequency;
        uint8_t CpuSku;
    } __attribute__((packed));

    struct PIROMPartNumbers
    {
        char FamilyNumber[7];
        char SkuNumber[8];
    } __attribute__((packed));

    struct PIROMTHermalRefData
    {
        uint8_t TcaseMaximum;
        uint16_t TDP;
        uint16_t DTSMaximum;
        uint16_t PowerLimit;
    } __attribute__((packed));

    struct PIROMPPIN
    {
        uint64_t PPIN;
    } __attribute__((packed));

  public:
    std::optional<uint16_t> Size()
    {
        return ReadUInt16(offsetof(struct PIROMHeader, PIROMSize));
    }

    std::optional<std::string> Sspec()
    {
       if (!header.ProcessorDataAddress)
       {
           return std::nullopt;
       }
       return ReadString(header.ProcessorDataAddress +
                         offsetof(struct PIROMProcessorData, Sspec),
                         member_size(struct PIROMProcessorData, Sspec));
    }

    std::optional<bool> ProductionGrade()
    {
       if (!header.ProcessorDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt8(header.ProcessorDataAddress +
                        offsetof(struct PIROMProcessorData, SampleProd)) & 1;
    }

    std::optional<size_t> NumberOfCores()
    {
       if (!header.ProcessorDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ProcessorDataAddress +
                            offsetof(struct PIROMProcessorData, NumberOfCores));
    }

    std::optional<bool> HyperThreadingSupport()
    {
       if (!header.ProcessorDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ProcessorDataAddress +
                            offsetof(struct PIROMProcessorData, HyperThreadingSupport)) & 1;
    }

    std::optional<size_t> SystemBusSpeed()
    {
       if (!header.ProcessorDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ProcessorDataAddress +
                            offsetof(struct PIROMProcessorData, SystemBusSpeed));
    }

    std::optional<uint32_t> Cpuid()
    {
       if (!header.ProcessorCoreAddress)
       {
           return std::nullopt;
       }

       return ReadUInt16(header.ProcessorCoreAddress +
                         offsetof(struct PIROMCoreData, Cpuid)) |
              (ReadUInt8(header.ProcessorCoreAddress +
                        offsetof(struct PIROMCoreData, ExtendedModelID)) << 16);
    }

    std::optional<size_t> MaximumP1CoreFrequency()
    {
       if (!header.ProcessorCoreAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ProcessorCoreAddress +
                            offsetof(struct PIROMCoreData, MaximumP1CoreFrequency));
    }

    std::optional<size_t> MaximumP0CoreFrequency()
    {
       if (!header.ProcessorCoreAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ProcessorCoreAddress +
                            offsetof(struct PIROMCoreData, MaximumP0CoreFrequency));
    }

    std::optional<size_t> TcaseMaximum()
    {
       if (!header.ThermalDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ThermalDataAddress +
                            offsetof(struct PIROMTHermalRefData, TcaseMaximum));
    }

    std::optional<size_t> TDP()
    {
       if (!header.ThermalDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ThermalDataAddress +
                            offsetof(struct PIROMTHermalRefData, TDP));
    }

    std::optional<size_t> DTSMaximum()
    {
       if (!header.ThermalDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ThermalDataAddress +
                            offsetof(struct PIROMTHermalRefData, DTSMaximum));
    }

    std::optional<size_t> PowerLimit()
    {
       if (!header.ThermalDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt16BCD(header.ThermalDataAddress +
                            offsetof(struct PIROMTHermalRefData, PowerLimit));
    }

    std::optional<std::string> FamilyNumber()
    {
       if (!header.PartNumberDataAddress)
       {
           return std::nullopt;
       }
       return ReadString(header.PartNumberDataAddress +
                         offsetof(struct PIROMPartNumbers, FamilyNumber),
                         member_size(struct PIROMPartNumbers, FamilyNumber));
    }

    std::optional<std::string> SkuNumber()
    {
       if (!header.PartNumberDataAddress)
       {
           return std::nullopt;
       }
       return ReadString(header.PartNumberDataAddress +
                         offsetof(struct PIROMPartNumbers, SkuNumber),
                         member_size(struct PIROMPartNumbers, SkuNumber));
    }

    std::optional<uint64_t> PPIN()
    {
       if (!header.PPINDataAddress)
       {
           return std::nullopt;
       }
       return ReadUInt64(header.PPINDataAddress +
                         offsetof(struct PIROMPPIN, PPIN));
    }

    PIROMA(std::vector<std::byte>& raw) : PIROM(raw), header()
    {
        if (DataFormatRevision() != 0xA || raw.size() < sizeof(header))
        {
            return;
        }
        slice(0, sizeof(header), &header);
    }
  private:
    struct PIROMHeader header;

};

}; // namespace pirom