// MIT License
//
// Copyright (c) 2020 Jussi Lind <jussi.lind@iki.fi>
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

#include "argengine.hpp"

#include <cstdlib>
#include <iostream>

using juzzlin::Argengine;

//
// Ex1: Demonstrate basic usage.
//

int main(int argc, char ** argv)
{
    // Instantiate Argengine and give it the raw argument data.
    Argengine ae(argc, argv);

    // Add option "-a" that, when set, prints all arguments given to the binary.
    ae.addOption(
      { "-a", "--arguments" }, [&] {
          for (int i = 0; i < argc; i++) {
              std::cout << argv[i] << std::endl;
          }
      },
      false, "Print arguments."); // Set the option non-required and set documentation string.

    // Add option "-p" that prints the length of the given string e.g. "-p FOO".
    ae.addOption(
      { "-p" }, [](std::string value) {
          std::cout << value.size() << std::endl;
      },
      true, "Print length of given text. This option is required.", "TEXT"); // Set the option required and set documentation string and variable name shown in the documentation.

    // Parse the arguments and store possible error to variable error.
    Argengine::Error error;
    ae.parse(error);

    // Check error and print the possible error message.
    if (error.code != Argengine::Error::Code::Ok) {
        std::cerr << error.message << std::endl
                  << std::endl;
        ae.printHelp(); // Print the auto-generated help.
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
