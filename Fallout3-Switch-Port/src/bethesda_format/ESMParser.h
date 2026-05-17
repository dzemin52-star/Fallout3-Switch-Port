#pragma once

#include <cstdint>
#include <cstdio>
#include <vector>
#include <string>

struct CellRecord
{
    uint32_t formID = 0;
    uint32_t flags  = 0;
};

struct ObjectReference
{
    uint32_t formID = 0;
    float x = 0.0f, y = 0.0f, z = 0.0f;
};

enum RecordType : uint32_t
{
    RECORD_TES4 = 0x34534554,
    RECORD_GRUP = 0x50555247
};

struct RecordHeader
{
    uint32_t type           = 0;
    uint32_t dataSize       = 0;
    uint32_t flags          = 0;
    uint32_t formID         = 0;
    uint32_t versionControl = 0;
    uint16_t revision       = 0;
    uint16_t unknown        = 0;
};

class ESMParser
{
public:
    bool ParseFile(const char* filename);
    const std::vector<CellRecord>& GetCells() const       { return m_cells; }
    const std::vector<ObjectReference>& GetReferences() const { return m_references; }

private:
    FILE* m_file = nullptr;

    struct ESMHeader
    {
        uint32_t dataSize     = 0;
        uint32_t version      = 0;
        uint32_t recordCount  = 0;
        uint32_t nextObjectID = 0;
    } m_header;

    std::vector<CellRecord>      m_cells;
    std::vector<ObjectReference> m_references;

    template <typename T>
    T ReadValue()
    {
        T val;
        fread(&val, sizeof(T), 1, m_file);
        return val;
    }

    std::string ReadString();
    std::vector<uint8_t> ReadData(size_t size);
    void SkipData(size_t size);
    void ParseRecord(const RecordHeader& header);
    void ParseGroup();
};