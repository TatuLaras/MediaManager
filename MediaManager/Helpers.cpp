#include "pch.h"
#include "Helpers.h"

std::string Helpers::FormatValue(float value, int precision)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(precision) << value;
	return stream.str();
}
