#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_circular_t_cpp, "$Id$ $CSID$")

#include <ntsa_circular.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::CircularUint32'.
class CircularUint32Test
{
  public:
    // TODO.
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntsa::CircularUint32Test::verify)
{
    ntsa::CircularUint32 a(10);
    ntsa::CircularUint32 b(a);

    b.decrement(1);
    NTSCFG_TEST_TRUE(b < a);

    b.decrement(10);

    NTSCFG_TEST_TRUE(b < a);
    NTSCFG_TEST_TRUE(b.value() > a.value());
    NTSCFG_TEST_TRUE(b.difference(a) == -11);
    NTSCFG_TEST_TRUE(b - a == -11);
}

// Provide tests for 'ntsa::CircularUint64'.
class CircularUint64Test
{
  public:
    // TODO.
    static void verify();
};

NTSCFG_TEST_FUNCTION(ntsa::CircularUint64Test::verify)
{
    ntsa::CircularUint64 a(10);
    ntsa::CircularUint64 b(a);

    b.decrement(1);
    NTSCFG_TEST_TRUE(b < a);

    b.decrement(10);

    NTSCFG_TEST_TRUE(b < a);
    NTSCFG_TEST_TRUE(b.value() > a.value());
    NTSCFG_TEST_TRUE(b.difference(a) == -11);
    NTSCFG_TEST_TRUE(b - a == -11);
}

}  // close namespace ntsa
}  // close namespace BloombergLP
