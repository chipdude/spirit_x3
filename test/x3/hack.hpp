// work around C++1y with libstdc++ 4.8
#ifdef __linux__
#include <bits/c++config.h>
#undef _GLIBCXX_HAVE_GETS
#endif
