// Not actually ran with a fuzzer yet. Im having troubles building. Maybe another time

#include <iostream>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "AUI/Xml/AXml.h"
#include "AUI/IO/IInputStream.h"

class RawInputStream : public IInputStream {
    const char* mData;
    size_t mSize;
    size_t mPos = 0;

public:
    RawInputStream(const uint8_t* data, size_t size)
        : mData(reinterpret_cast<const char*>(data)), mSize(size) {}

    size_t read(char* dst, size_t size) override {
        size_t remaining = mSize - mPos;
        size_t toRead = remaining < size ? remaining : size;
        std::memcpy(dst, mData + mPos, toRead);
        mPos += toRead;
        return toRead;
    }

    void reset() {
        mPos = 0;
    }

    const char* raw() const {
        return mData;
    }

    size_t size() const {
        return mSize;
    }
};


class DummyVisitor : public IXmlDocumentVisitor {
public:
    _<IXmlHeaderVisitor> visitHeader() override {
        return nullptr;
    }

    void visitAttribute(const AString& name, AString value) override {}

    _<IXmlEntityVisitor> visitEntity(AString entityName) override {
        return nullptr;
    }

    void visitTextEntity(const AString& entity) override {}
};

int main(){
    std::vector<char> inputData;
    char buffer[4096];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
        inputData.insert(inputData.end(), buffer, buffer + bytesRead);
    }

    if (inputData.empty()) return 0;

    auto stream = _new<RawInputStream>(reinterpret_cast<const uint8_t*>(inputData.data()), inputData.size());

// Dump contents via stream
std::cerr << "==== Begin XML Input ====" << std::endl;
std::cerr.write(stream->raw(), stream->size());
std::cerr << "\n==== End XML Input ====" << std::endl;

stream->reset(); // rewind so parser can read it again
    auto visitor = _new<DummyVisitor>();

    AXml::read(stream, visitor);

    return 0;
}