#include "ESMParser.h"
#include <cstring>
#include <zlib.h>

bool ESMParser::ParseFile(const char* filename)
{
    m_file = fopen(filename, "rb");
    if (!m_file)
    {
        fprintf(stderr, "ESM: Cannot open %s\n", filename);
        return false;
    }

    uint32_t magic = ReadValue<uint32_t>();
    if (magic != RECORD_TES4)
    {
        fprintf(stderr, "ESM: Invalid magic: 0x%08X\n", magic);
        fclose(m_file);
        return false;
    }

    m_header.dataSize     = ReadValue<uint32_t>();
    m_header.version      = ReadValue<uint32_t>();
    m_header.recordCount  = ReadValue<uint32_t>();
    m_header.nextObjectID = ReadValue<uint32_t>();

    SkipData(m_header.dataSize - 12);

    while (!feof(m_file))
    {
        RecordHeader recHeader;
        if (fread(&recHeader, sizeof(RecordHeader), 1, m_file) != 1)
            break;

        if (recHeader.flags & 0x00040000)
        {
            uint32_t decompSize = ReadValue<uint32_t>();
            std::vector<uint8_t> compData = ReadData(recHeader.dataSize - 4);
            std::vector<uint8_t> decompData(decompSize);

            uLongf destLen = decompSize;
            int ret = uncompress(decompData.data(), &destLen, compData.data(), compData.size());
            if (ret != Z_OK)
            {
                fprintf(stderr, "ESM: Decompression failed for record 0x%08X\n", recHeader.type);
                continue;
            }
            printf("ESM: Decompressed record 0x%08X (%zu -> %zu bytes)\n",
                   recHeader.type, compData.size(), destLen);
        }
        else if (recHeader.type == RECORD_GRUP)
        {
            ParseGroup();
        }
        else
        {
            ParseRecord(recHeader);
        }
    }

    fclose(m_file);
    printf("ESM: Parsed %zu cells, %zu references\n", m_cells.size(), m_references.size());
    return true;
}

std::string ESMParser::ReadString()
{
    uint16_t len = ReadValue<uint16_t>();
    if (len == 0) return {};
    std::vector<char> buf(len);
    fread(buf.data(), 1, len, m_file);
    return std::string(buf.data(), len);
}

std::vector<uint8_t> ESMParser::ReadData(size_t size)
{
    std::vector<uint8_t> data(size);
    if (size > 0)
        fread(data.data(), 1, size, m_file);
    return data;
}

void ESMParser::SkipData(size_t size)
{
    fseeko(m_file, (off_t)size, SEEK_CUR);
}

void ESMParser::ParseRecord(const RecordHeader& header)
{
    SkipData(header.dataSize);
}

void ESMParser::ParseGroup()
{
}