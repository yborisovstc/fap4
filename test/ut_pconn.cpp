#include <stdlib.h>
#include <cppunit/extensions/HelperMacros.h>

#include "pconn2.h"

using namespace std;

/** @brief Test of NDEDS native system
*/
class Ut_pconn : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_pconn);
    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_pconn );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_pconn, "Ut_pconn");


void Ut_pconn::setUp()
{
}

void Ut_pconn::tearDown()
{
    //    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}




void Ut_pconn::test_1()
{
    cout << endl << "=== Test of pconn extender ===" << endl;
    //CPPUNIT_ASSERT_MESSAGE("Failed running Des1", data == 5);
}
