/**
* This Source Code Form is part of UOP Package library by Nolok and subject to the terms of the
* GNU General Public License version 3. More info in the file "uoppackage.h", which is part of this source code package.
*/
#ifndef UOPFILE_H
#define UOPFILE_H

#include "uoperror.h"
#include "uopcompression.h"
#include <fstream>
#include <memory>


namespace uopp
{

class UOPBlock;
class UOPPackage;

static constexpr unsigned int kInvalidIdx = static_cast<unsigned int>(-1);


class UOPFile
{
    friend class UOPBlock;
    friend class UOPPackage;

public:
    UOPFile(UOPBlock* parent, unsigned int index = 0);
    //~UOPFile();

    bool searchByHash(unsigned long long hash) const {
        return (m_fileHash == hash);
    }

    bool read(std::ifstream& fin, UOPError* errorQueue = nullptr);
    bool readPackedData(std::ifstream& fin, UOPError* errorQueue = nullptr);
    void freePackedData();
    bool unpack(std::shared_ptr<char[]>* decompressedData, UOPError* errorQueue = nullptr);   // extract the file

    bool compressAndReplaceData(std::shared_ptr<char[]> sourceDecompressed,   ZLibQuality compression, bool addDataHash, UOPError* errorQueue = nullptr);
    bool createFile(std::ifstream& fin, unsigned long long fileHash,        ZLibQuality compression, bool addDataHash, UOPError* errorQueue = nullptr);    // create file in memory
    bool createFile(std::ifstream& fin, const std::string& packedFileName,  ZLibQuality compression, bool addDataHash, UOPError* errorQueue = nullptr);    // create file in memory

// File structure
private:
    UOPBlock* m_parent;
    unsigned int m_index;
    unsigned long long m_dataBlockAddress;
    unsigned int m_dataBlockLength;
    unsigned int m_compressedSize;
    unsigned int m_decompressedSize;
    unsigned long long m_fileHash;
    unsigned int m_dataBlockHash;
    CompressionFlag m_compression;
    std::string m_fileName;
    std::shared_ptr<char[]> m_data;   // contains the compressed file data

    // Used only when creating a package
    bool m_added;

public:
    UOPBlock* getParent() const                     { return m_parent;              }
    unsigned int getIndex() const                   { return m_index;               }
    unsigned long long getDataBlockAddress() const  { return m_dataBlockAddress;    }
    unsigned int getDataBlockLength() const         { return m_dataBlockLength;     }
    unsigned int getCompressedSize() const          { return m_compressedSize;      }
    unsigned int getDecompressedSize() const        { return m_decompressedSize;    }
    unsigned long long getFileHash() const          { return m_fileHash;            }
    unsigned int getDataBlockHash() const           { return m_dataBlockHash;       }
    CompressionFlag getCompressionType() const      { return m_compression;         }
    const std::string& getFileName() const          { return m_fileName;            } // can be empty!
    bool hasData() const                            { return m_data == nullptr;     }
    const std::shared_ptr<char[]> getData() const   { return m_data;                }
    std::shared_ptr<char[]> getData()               { return m_data;                }
    size_t getDataSize() const                      { return m_compressedSize;      }
    bool isAdded() const                            { return m_added;               }
};


}

#endif // UOPFILE_H
