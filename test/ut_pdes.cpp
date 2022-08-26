#include <stdlib.h>
#include <cppunit/extensions/HelperMacros.h>

#include "pdes.h"

using namespace std;

/** @brief Test of NDEDS native system
*/
class Ut_syst : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_syst);
    CPPUNIT_TEST(test_des_tr_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_des_tr_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_syst );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_syst, "Ut_syst");


void Ut_syst::setUp()
{
}

void Ut_syst::tearDown()
{
    //    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

class SAdd : public PState<int>
{
    public:
	SAdd(TBcp& aBcp): PState<int>(aBcp), mIa(this), mIb(this) {}
    protected:
	virtual bool doTrans() override {
	    int a = 0, b = 0;
	    mIa.getData(a);
	    mIb.getData(b);
	    *mUdata = a + b;
	    return *mUdata != *mCdata;
	}
    public:
	Icp<int> mIa;
	Icp<int> mIb;
    
};

class Des1 : public PDesLauncher
{
    public:
	Des1();
	SAdd mAdd;
	PState<int> mConst_1;

};

Des1::Des1(): PDesLauncher(), mAdd(mBcp), mConst_1(mBcp, 1)
{
    mAdd.mOcp.connect(&mAdd.mIb);
    mAdd.mIa.connect(&mConst_1.mOcp);
    mAdd.set(0);
}



/** @brief Test of DES transitions
 * */
void Ut_syst::test_des_tr_1()
{
    Des1 des;
    cout << endl << "=== Test of DES transisions ===" << endl;
    des.Run(5, 2);
}
