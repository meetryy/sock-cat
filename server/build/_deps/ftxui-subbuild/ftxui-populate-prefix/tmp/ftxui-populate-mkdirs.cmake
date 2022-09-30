# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/sockets/server/build/_deps/ftxui-src"
  "D:/sockets/server/build/_deps/ftxui-build"
  "D:/sockets/server/build/_deps/ftxui-subbuild/ftxui-populate-prefix"
  "D:/sockets/server/build/_deps/ftxui-subbuild/ftxui-populate-prefix/tmp"
  "D:/sockets/server/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp"
  "D:/sockets/server/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src"
  "D:/sockets/server/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/sockets/server/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/sockets/server/build/_deps/ftxui-subbuild/ftxui-populate-prefix/src/ftxui-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
