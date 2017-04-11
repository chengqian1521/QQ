#include "utils.h"


unsigned int utils::SDBMHash(const char *str)
{
	unsigned int hash = 0;

	while (*str)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}

	return (hash & 0x7FFFFFFF);
}

int utils::getImageIndexByQString(const QString& string) {
	std::string stdSting = string.toStdString();

	return utils::SDBMHash(stdSting.data())%200 +10;
}