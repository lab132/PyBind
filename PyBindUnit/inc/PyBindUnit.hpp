#pragma once

#include "catch.hpp"

// Predefine PYB_ASSERT and redirect to the catch assert
#define PYB_ASSERT(...) REQUIRE(__VA_ARGS__)
#include "PyBind.hpp"


using namespace pyb;
