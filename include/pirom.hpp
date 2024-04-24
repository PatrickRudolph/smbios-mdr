/*
// Copyright (c) 2024 9elements GmbH
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

#include <boost/algorithm/string.hpp>
#include <boost/endian/conversion.hpp>

#define member_size(type, member) (sizeof( ((type *)0)->member ))

namespace pirom
{
class PIROM
{
  public:
    static PIROM PIROMFabric(std::vector<std::byte>& pirom)
    {
        PIROM pirom(pirom);
        switch(pirom.DataFormatRevision()) {
            case 9:
                return new PIROM9(pirom);
            default:
        }
        return nullptr;
    }

    PIROM(std::vector<std::byte>& pirom) : rom(pirom)
    {
    }

    uint8_t DataFormatRevision() {
        return std::to_integer<uint8_t>(rom.at(0));
    }

    virtual std::optional<uint16_t> Size() { return std::nullopt; }
    virtual std::optional<std::string> Sspec() { return std::nullopt; }
    virtual std::optional<bool> ProductionGrade() { return std::nullopt; }
    virtual std::optional<size_t> NumberOfCores() { return std::nullopt; }
    virtual std::optional<bool> HyperThreadingSupport()  { return std::nullopt; }
    virtual std::optional<size_t> SystemBusSpeed() { return std::nullopt; }
    virtual std::optional<uint32_t> Cpuid() { return std::nullopt; }
    virtual std::optional<size_t> MaximumP1CoreFrequency() { return std::nullopt; }
    virtual std::optional<size_t> MaximumP0CoreFrequency() { return std::nullopt; }
    virtual std::optional<size_t> TcaseMaximum() { return std::nullopt; }
    virtual std::optional<size_t> TDP() { return std::nullopt; }
    virtual std::optional<size_t> DTSMaximum() { return std::nullopt; }
    virtual std::optional<size_t> PowerLimit() { return std::nullopt; }
    virtual std::optional<std::string> FamilyNumber() { return std::nullopt; }
    virtual std::optional<std::string> SkuNumber() { return std::nullopt; }
    virtual std::optional<uint64_t> PPIN() { return std::nullopt; }

  protected:
    void slice(size_t start, size_t len, void *dest)
    {
        auto chunk = std::vector<std::byte>(rom.begin() + start, rom.begin() + start + len);
        memcpy(dest, chunk.data(), len);
    }

    std::string ReadString(size_t start, size_t cnt)
    {
        std::string ret;
        for (size_t i = start; i < start + cnt; i++)
        {
            if (!std::isprint(std::to_integer<uint8_t>(rom.at(i))))
            {
                continue;
            }
            ret.push_back(std::to_integer<uint8_t>(rom.at(i)));
        }
        boost::algorithm::trim(ret);
        return ret;
    }

    uint8_t ReadUInt8(size_t start)
    {
        return std::to_integer<uint8_t>(rom.at(start));
    }

    uint16_t ReadUInt16(size_t start)
    {
        union {
            uint16_t val;
            uint8_t raw[2];
        } tmp;

        for (size_t i = 0; i < sizeof(tmp.val); i++)
            tmp.raw[i] = ReadUInt8(start + i);

        return boost::endian::little_to_native(tmp.val);
    }

    uint8_t ReadUInt8BCD(size_t start)
    {
        uint8_t tmp = ReadUInt8(start);
        return (tmp >> 4) * 10 + (tmp & 0xf);
    }

    uint16_t ReadUInt16BCD(size_t start)
    {
        uint16_t tmp = ReadUInt16(start);
        tmp = ((tmp >> 16) & 0xf) * 1000 +
            ((tmp >> 8) & 0xf) * 100 +
            ((tmp >> 4) & 0xf) * 10 +
            (tmp & 0xf);

        return tmp;
    }

    uint64_t ReadUInt64(size_t start)
    {
        union {
            uint16_t val;
            uint8_t raw[8];
        } tmp;

        for (size_t i = 0; i < sizeof(tmp.val); i++)
            tmp.raw[i] = ReadUInt8(start + i);

        return boost::endian::little_to_native(tmp.val);
    }
    private:
      std::vector<std::byte> rom;
};
}; // namespace pirom