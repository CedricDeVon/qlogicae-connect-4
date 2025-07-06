
#include "pch.h"

int main()
{
	QLogicaeCore::CliIO::instance().print_with_new_line(
		std::to_string(QLogicaeConnect4::sum(2, 2))
	);

	return 0;
}
