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
//    CPPUNIT_TEST(test_2);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_1();
    void test_2();
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


struct I1 {
};

struct I2 {
};


void Ut_pconn::test_1()
{
    cout << endl << "=== Test of tree leafs iterator ===" << endl;

    PCpnp<I1, I2> p1(nullptr, true);
    PExdm<I2, I1> s1;
    PExdm<I2, I1> s2;
    PCpnp<I2, I1> l3_1(nullptr, true);
    PExdm<I2, I1> s2_1;
    PCpnp<I2, I1> l1(nullptr, true);
    PCpnp<I2, I1> l2(nullptr, true);
    p1.connect(&s1);
    // p1.connect(&s2); // Assert: more that one pair to OTO
    s1.binded()->connect(&l1);
    s1.binded()->connect(&l2);
    s1.binded()->connect(&s2_1);
    s2_1.binded()->connect(&l3_1);

    for (auto it = p1.leafsBegin(); it != p1.leafsEnd(); it++) {
	cout << "Leaf: " << hex << *it << endl;
    }

    /*
    auto it = p1.leafsBegin();
    auto ll1 = *it;
    it++;
    auto ll2 = *it;
    it++;
    cout << "leafsEnd:" << endl;
    p1.leafsEnd().dump();
    cout << "it:" << endl;
    it.dump();
    bool end = (it == p1.leafsEnd());
    */


    cout << "s1.binded() dump:" << endl;
    s1.binded()->dump();
    s1.binded()->disconnect(&l1);


    //CPPUNIT_ASSERT_MESSAGE("Failed running Des1", data == 5);
}

void Ut_pconn::test_2()
{
    cout << endl << "=== Test of pconn extender ===" << endl;

    PCpnpr<I1, I2> p1(nullptr);
    PExd<I2, I1> e1;
    PExd<I1, I2> e2;
    PCpnp<I2, I1> l1(nullptr, true);
    PCpnp<I2, I1> l2(nullptr, true);
    p1.connect(&e1);
    p1.connect(e2.binded());
    e1.binded()->connect(&l1);
    e2.connect(&l2);

    cout << "p1 leafs" << endl;
    for (auto it = p1.leafsBegin(); it != p1.leafsEnd(); it++) {
	cout << "Leaf: " << hex << *it << endl;
    }

    p1.disconnect(&e1);
}
