#include "exceptions.h"
#include "uoidx.h"
#include <cstring> // for memcpy


namespace uocf
{


UOIdx::UOIdx(const std::string& filePath) : m_filePath(filePath)
{
    m_cachedCount = 0;
}

void UOIdx::openStream()
{
    m_stream.open(m_filePath, std::ifstream::in | std::ifstream::binary);
    if (!m_stream.is_open())
        throw InvalidStreamException("UOIdx", "Couldn't open file.");
}

void UOIdx::closeStream()
{
    if (!m_stream.is_open())
        throw InvalidStreamException("UOIdx", "Trying to close an already closed stream.");
    m_stream.close();
}

void UOIdx::clearCache()
{
    m_cache.reset(nullptr);
}


auto readLookup = [](std::ifstream &fin, UOIdx::Entry* idxEntry) -> bool
{
    // Look up in *idx.mul for the offset of the ID in *.mul
    // - Lookup:    size=4. Is either undefined (0xFFFFFFFF / -1) or the file offset in *.MUL
    // - Size:      size=4. Size of the pointed block.
    // - Extra:     size=4. Extra info, used only by certain mul files.
    //      gumpart.mul:    width = ( Extra >> 16 ) & 0xFFFF;   height = Extra & 0xFFFF;
    char buf[12];
    fin.read(buf, 12);

    if (!fin.good())
        return false;   // Not existing?

    memcpy(&idxEntry->lookup,   buf,     4);
    memcpy(&idxEntry->size,     buf + 4, 4);
    memcpy(&idxEntry->extra,    buf + 8, 4);
    return true;
};

void UOIdx::cacheData()
{
    openStream();

    m_stream.seekg(0, std::fstream::end);
    m_cachedCount = unsigned(m_stream.tellg() / Entry::kSize);
    m_stream.seekg(0, std::fstream::beg);

    m_cache = std::make_unique<Entry[]>(m_cachedCount);
    for (size_t i = 0; i < m_cachedCount; ++i)
        readLookup(m_stream, &m_cache[i]);

    closeStream();
}

bool UOIdx::getLookup(unsigned int id, Entry *idxEntry)
{
    if (hasCache())
    {
        if (id > m_cachedCount)
            return false;
        *idxEntry = m_cache[id];
        return true;
    }

    if (!m_stream.is_open())
        throw InvalidStreamException("UOIdx", "getLookup accessing closed stream.");

    m_stream.seekg(id * Entry::kSize);
    return readLookup(m_stream, idxEntry);
}

bool UOIdx::getLookup(const std::string& filePath, unsigned int id, Entry* idxEntry)   // static
{
    // Open *idx.mul
    std::ifstream fin;
    // it's fundamental to open the file in binary mode, otherwise tellg and seekg won't work properly...
    fin.open(filePath, std::ifstream::in | std::ifstream::binary);
    if (!fin.is_open())
        return false;

    fin.seekg(id * Entry::kSize);
    return readLookup(fin, idxEntry);
}


}
