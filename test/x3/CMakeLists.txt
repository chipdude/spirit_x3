set(TESTS actions alternative and_predicate any_parser attr bool char1 char_class debug difference eoi eol eps
          expect int1 kleene lexeme list lit no_skip not_predicate omit optional plus raw
	  real1 real2 real3 real4 real5 rule1 rule2 rule3 rule4 sequence skip symbols1 symbols2 symbols3 tst
	  uint1 uint_radix with)

enable_testing()

foreach(T ${TESTS})
  add_executable(${T} ${T}.cpp)
  target_link_libraries(${T} -lboost_system)
  add_test(${T} ${T})
endforeach()

