#ifndef CPAN_ENDIAN_H
#define CPAN_ENDIAN_H

#include <byteswap.h>
#include <endian.h>
#include <type_traits>
#include <stdint.h>

namespace cpan
{

template <typename T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
mybyteswap(T value)
{
	return (T)bswap_64((uint64_t)value);
}

template <typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value)
{
	uint32_t tmp = static_cast<uint32_t>(value);
	return (T)bswap_32(tmp);
}

template <typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value)
{
	uint16_t tmp = static_cast<uint16_t>(value);
	return (T)bswap_16(tmp);
}


#if BYTE_ORDER == BIG_ENDIAN
template <typename T>
T byteSwapOnBigEndian(T value)
{
	return byteswap(value);
}

template <typename T>
T byteSwapOnLittleEndian(T value)
{
	return value;
}
#else
template <typename T>
T byteSwapOnBigEndian(T value)
{
	return value;
}

template <typename T>
T byteSwapOnLittleEndian(T value)
{
	return byteswap(value);
}
#endif




}





#endif
