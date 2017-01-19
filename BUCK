cxx_library(
  name = 'json11',
  header_namespace = 'json11',
  headers = ['json11.hpp'],
  exported_headers = ['json11.hpp'],
  srcs = ['json11.cpp'],
  visibility = ['PUBLIC'],
  compiler_flags = ['-std=c++11']
)

cxx_binary(
  name = 'json11-test',
  deps = [':json11'],
  srcs = ['test.cpp'],
  compiler_flags = ['-std=c++11']
)
