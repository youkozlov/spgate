#include "SerialPortFile.hpp"
#include <stdexcept>

namespace sg
{

SerialPortFile::SerialPortFile(Init const& init)
{
    file.open(init.file);
}

SerialPortFile::~SerialPortFile()
{
    file.close();
}

int SerialPortFile::write(void const*, unsigned int size)
{
    file << "Writing this to a file.\n";
    return size;
}

int SerialPortFile::read(void*, unsigned int)
{
    return 0;
}

}