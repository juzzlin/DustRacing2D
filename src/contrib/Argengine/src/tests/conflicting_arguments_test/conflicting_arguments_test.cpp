// MIT License
//
// Copyright (c) 2023 Jussi Lind <jussi.lind@iki.fi>
//
// https://github.com/juzzlin/Argengine
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "../../argengine.hpp"

// Don't compile asserts away
#ifdef NDEBUG
    #undef NDEBUG
#endif

#include <cassert>
#include <cstdlib>
#include <stdexcept>

using juzzlin::Argengine;

const auto name = "Argengine";

void testConflictingArguments_ConflictingArgumentsGiven_ShouldFail()
{
    const auto a1 = "a1";
    const auto a2 = "a2";
    const auto a3 = "a3";
    Argengine ae({ a1, a2, a3 });
    bool a2Called {};
    ae.addOption({ a2 }, [&] {
        a2Called = true;
    });
    bool a3Called {};
    ae.addOption({ a3 }, [&] {
        a3Called = true;
    });
    ae.addConflictingOptions({ a2, a3 });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(!a2Called);
    assert(!a3Called);
    assert(error == std::string(name) + ": Conflicting options: '" + a2 + "', '" + a3 + "'. These options cannot coexist.");
}

void testConflictingArguments_ConflictingArgumentsSetButNotEnoughGiven_ShouldSucceed()
{
    const auto a1 = "a1";
    const auto a2 = "a2";
    const auto a3 = "a2";
    Argengine ae({ a1, a2 });
    bool a2Called {};
    ae.addOption({ a2 }, [&] {
        a2Called = true;
    });
    ae.addConflictingOptions({ a2, a3 });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(a2Called);
    assert(error.empty());
}

void testConflictingArguments_ConflictingArgumentsGiven_ShouldNotReferToVariants()
{
    const auto a1 = "a1";
    const auto a2 = "a2";
    const auto a3 = "a3";
    Argengine ae({ a1, a2, a3 });
    bool a2Called {};
    ae.addOption({ a2, std::string("--") + a2 }, [&] {
        a2Called = true;
    });
    bool a3Called {};
    ae.addOption({ a3, std::string("--") + a3 }, [&] {
        a3Called = true;
    });
    ae.addConflictingOptions({ a2, a3 });
    std::string error;
    try {
        ae.parse();
    } catch (std::runtime_error & e) {
        error = e.what();
    }
    assert(!a2Called);
    assert(!a3Called);
    assert(error == std::string(name) + ": Conflicting options: '" + a2 + "', '" + a3 + "'. These options cannot coexist.");
}

int main(int, char **)
{
    testConflictingArguments_ConflictingArgumentsGiven_ShouldFail();

    testConflictingArguments_ConflictingArgumentsSetButNotEnoughGiven_ShouldSucceed();

    testConflictingArguments_ConflictingArgumentsGiven_ShouldNotReferToVariants();

    return EXIT_SUCCESS;
}
