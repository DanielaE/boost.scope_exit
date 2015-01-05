
// Copyright (C) 2006-2009, 2012 Alexander Nasonov
// Copyright (C) 2012 Lorenzo Caminiti
// Distributed under the Boost Software License, Version 1.0
// (see accompanying file LICENSE_1_0.txt or a copy at
// http://www.boost.org/LICENSE_1_0.txt)
// Home at http://www.boost.org/libs/scope_exit

#if defined(_MSC_VER)
#pragma warning(disable: 4003) // not enough actual parameters for macro
#endif

#include <boost/scope_exit.hpp>

int main(void) {
    int const i = 0;
    BOOST_SCOPE_EXIT( (&i) ) {
        i = 5;
		(void)i;
    } BOOST_SCOPE_EXIT_END
}

