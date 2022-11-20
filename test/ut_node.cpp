#include <stdlib.h>
#include <cppunit/extensions/HelperMacros.h>

#include "pdes.h"
#include "node.h"
#include "pconn2.h"

using namespace std;

/** @brief Test of node
*/
class Ut_node : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_node);
//    CPPUNIT_TEST(test_owning_1);
    CPPUNIT_TEST(test_add_owned);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_owning_1();
    void test_add_owned();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_node );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_node, "Ut_node");


void Ut_node::setUp()
{
}

void Ut_node::tearDown()
{
    //    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

/** @brief Creates test DES
 * Creates primitive native hier mNode1-mNode2
 * */
class DesO1 : public PDesLauncher
{
    public:
	Node mNode1;
	Node mNode2;

	DesO1(const char* aName = nullptr): PDesLauncher(aName), mNode1(mBcp, "Node1"), mNode2(mBcp, "Node2") {
	    mBcp.connect(&mNode1.mScp);
	    mBcp.connect(&mNode2.mScp);
	    mNode1.mCpOwning3.connect(&mNode2.mCpOwned3);
	}
};




/** @brief Test of owner-owned relation
 * */
void Ut_node::test_owning_1()
{
    cout << endl << "=== Test of owner-owned relation ===" << endl;
    DesO1 des("Des01");
    des.Run(5, 2);
    GUri n2uri = des.mNode2.mOwningUriTr.mOcp.data();
    std::string n2uris = n2uri;
    CPPUNIT_ASSERT_MESSAGE("Failed running DesO1", n2uris == "Node1.Node2");
    //des.mNode1.mCpOwning3.disconnect();
    //des.mNode2.mCpOwned3.disconnect();
    cout << endl << "=== END. Test of owner-owned relation ===" << endl;
}


/** @brief Creates test DES
 * */
class DesO2 : public PDesLauncher
{
    public:
	Node mNode1;

	DesO2(const char* aName = nullptr): PDesLauncher(aName), mNode1(mBcp,"Node1") {
	}
};


/** @brief Test of owner-owned relation
 * Adding owned
 * */
void Ut_node::test_add_owned()
{
    cout << endl << "=== Test of adding owned ===" << endl;
    DesO2 des("Des02");
    PData<std::string> newNodeId(std::string("Node2"), std::string("name"));
    MNCp2<MNnNode<tag_inp, tag_outp>> cpNode;
    newNodeId.mOcp.connect(cpNode.mPins.mNewNodeId.binded());
    des.mNode1.mCpNode.connect(&cpNode);
    des.Run(5, 2);
    des.MDesSyncable_dump(0, 0);
    //CPPUNIT_ASSERT_MESSAGE("Failed running DesO1", n2uris == "Node1.Node2");
    cout << endl << "=== END. Test of adding owned ===" << endl;
}
