set(sources
  array_info.cpp
  array_info.h
  array_max_n.h
  reduction.h
  tensor_store.h
  util.h
  util.cpp
)

add_mpqc_library(math_ta sources sources "tiledarray" "mpqc/math/external/tiledarray")
