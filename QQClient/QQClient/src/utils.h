#pragma once
#include <QString>
namespace utils {

	unsigned int SDBMHash(const char *str);

	int getImageIndexByQString(const QString& string);
}