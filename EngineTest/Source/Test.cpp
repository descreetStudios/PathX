#define TEST_LOGGER 1

#if TEST_LOGGER
#include "Tests/Core/TestLogger.h"
#else
#error "At least one test should be enabled"
#endif