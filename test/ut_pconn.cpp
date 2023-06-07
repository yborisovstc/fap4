#include <stdlib.h>
#include <cppunit/extensions/HelperMacros.h>

#include "pconn2.h"

using namespace std;

/** @brief Test of NDEDS native system
*/
class Ut_pconn : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_pconn);
//    CPPUNIT_TEST(test_1);
    CPPUNIT_TEST(test_pp_1);
//    CPPUNIT_TEST(test_2);
    CPPUNIT_TEST(test_pp_2);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_1();
    void test_pp_1();
    void test_2();
    void test_pp_2();
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

struct Ii1 {
    std::string mId;
    std::string Id() const { return mId; }
};

struct Ii2 {
    std::string mId;
    std::string Id() const { return mId; }
};


void Ut_pconn::test_pp_1()
{
    bool res = false;

    cout << endl << "=== Test #1 of PCpp (id-noid) ===" << endl;

    // nonId
    cout << endl << "=== Unidientified CP ===" << endl;

    PCppr<I1, I2, false, true> p1(nullptr);
    PExdm2<I2, I1> s1;
    PExdm2<I2, I1> s2;
    PCppr<I2, I1, false, true> l3_1(nullptr);
    PExdm2<I2, I1> s2_1;
    PCppr<I2, I1, false, true> l1(nullptr);
    PCppr<I2, I1, false, true> l2(nullptr);
    p1.connect(&s1);
    // p1.connect(&s2); // Assert: more that one pair to OTO
    s1.binded()->connect(&l1);
    s1.binded()->connect(&l2);
    s1.binded()->connect(&s2_1);
    s2_1.binded()->connect(&l3_1);

    for (auto it = p1.leafsBegin(); it != p1.leafsEnd(); it++) {
	cout << "Leaf: " << hex << *it << endl;
    }

    cout << "s1.binded() dump:" << endl;
    s1.binded()->dump();
    s1.binded()->disconnect(&l1);

    // Id
    cout << endl << "=== Idientified CP ===" << endl;
    Ii1 IfId1 = {.mId = "Iface1_1"};
    Ii2 IfId2 = {.mId = "Iface2_1"};
    PCppr<Ii1, Ii2, true, true> p1i(&IfId1);
    PExdm2i<Ii2, Ii1> s1i;
    PExdm2i<Ii2, Ii1> s2i;
    Ii2 IfId2_2 = {.mId = "Iface2_2"};
    PCpOip2<Ii2, Ii1> l1i(&IfId2_2);
    Ii2 IfId2_3 = {.mId = "Iface2_3"};
    PCpOip2<Ii2, Ii1> l2i(&IfId2_3);
    PExdm2i<Ii2, Ii1> s2_1i;
    Ii2 IfId2_4 = {.mId = "Iface2_4"};
    PCpOip2<Ii2, Ii1> l3_1i(&IfId2_4);
    // Connecting
    res =  p1i.connect(&s1i);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting p1i to s1i", res);
    res = s1i.binded()->connect(&l1i);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting l1i to s1i", res);
    res = s1i.binded()->connect(&l2i);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting l2i to s1i", res);
    res = s1i.binded()->connect(&s2_1i);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting s2_1i to s1i", res);
    res = s2_1i.binded()->connect(&l3_1i);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting s2_1i to l3_1i", res);

    int i = 0;
    for (auto it = p1i.leafsBegin(); it != p1i.leafsEnd(); it++, i++) {
	auto elem = *it;
	std::string id;
	elem->getId(id);
	cout << "Leaf: " << id << endl;
	if (i == 0) {
	    CPPUNIT_ASSERT_MESSAGE("Wrong leaf id", id == IfId2_4.Id());
	} else if (i == 1) {
	    CPPUNIT_ASSERT_MESSAGE("Wrong leaf id", id == IfId2_3.Id());
	} else {
	    CPPUNIT_ASSERT_MESSAGE("Wrong leaf id", id == IfId2_2.Id());
	}
    }

    cout << "p1i dump:" << endl;
    p1i.dump();


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


/** @brief Test of extender. id-noid variant
 * */
void Ut_pconn::test_pp_2()
{
    bool res = false;
    cout << endl << "=== Test of pconn extender, id-noid ===" << endl;

    Ii1 IfId1 = {.mId = "Iface1_1"};
    Ii2 IfId2 = {.mId = "Iface2_1"};
    PCppr<Ii1, Ii2, true, true> p1(&IfId1);
    PExdm2i<Ii2, Ii1> e1;
    PExdm2i<Ii1, Ii2> e2;
    Ii2 IfId2_2 = {.mId = "Iface2_2"};
    PCpOip2<Ii2, Ii1> l1(&IfId2_2);
    Ii2 IfId2_3 = {.mId = "Iface2_3"};
    PCpOip2<Ii2, Ii1> l2(&IfId2_3);
    res = p1.connect(&e1);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting p1 to e1", res);
    res = e1.binded()->connect(e2.binded());
    CPPUNIT_ASSERT_MESSAGE("Failed connecting e1 to l1", res);
    res = e1.binded()->connect(&l1);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting e1 to l1", res);
    res = e2.connect(&l2);
    CPPUNIT_ASSERT_MESSAGE("Failed connecting e2 to l2", res);

    // Verifying leafs
    cout << "p1 leafs" << endl;
    for (auto it = p1.leafsBegin(); it != p1.leafsEnd(); it++) {
	auto elem = *it;
	std::string id;
	elem->getId(id);
	cout << "Leaf: " << id << endl;
    }
    cout << "p1 dump:" << endl;
    p1.dump();

    p1.disconnect(&e1);
}
