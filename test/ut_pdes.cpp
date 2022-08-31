#include <stdlib.h>
#include <cppunit/extensions/HelperMacros.h>

#include "pdes.h"

using namespace std;

/** @brief Test of NDEDS native system
*/
class Ut_pdes : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_pdes);
    CPPUNIT_TEST(test_des_tr_1);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_des_tr_1();
};

CPPUNIT_TEST_SUITE_REGISTRATION( Ut_pdes );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(Ut_pdes, "Ut_pdes");


void Ut_pdes::setUp()
{
}

void Ut_pdes::tearDown()
{
    //    delete iEnv;
    CPPUNIT_ASSERT_EQUAL_MESSAGE("tearDown", 0, 0);
}

class SAdd : public PState<int>
{
    public:
	SAdd(TBcp& aBcp): PState<int>(aBcp), mIa(this), mIb(this) {}
    protected:
	virtual void doTrans() override {
	    mUdata->mValid = mIa.valid() && mIb.valid();
	    mUdata->mData = mIa.data() + mIb.data();
	}
    public:
	PsIcp<int> mIa;
	PsIcp<int> mIb;

};

struct SAdd2 : public PState2<int, int, int>
{
	void doTrans() override {
	    mUdata->mValid = Inp1.valid() && Inp2.valid();
	    mUdata->mData = Inp1.data() + Inp2.data();
	}
};


class Des1 : public PDesLauncher
{
    public:
	SAdd mAdd;
	PState<int> mConst_1;
	// TODO Not supported with my current g+ version, verify on c++2a
	// Pst2<int, int, int, [*]()->bool {return false;} > mAdd2;

	Des1(): PDesLauncher(), mAdd(mBcp), mConst_1(mBcp, 1) {
	    mAdd.mOcp.connect(&mAdd.mIb);
	    mAdd.mIa.connect(&mConst_1.mOcp);
	    mAdd.set(0);
	}
};

class Des2 : public PDesLauncher
{
    public:
	SAdd2 mAdd;
	PState<int> mConst_1;

	Des2(): PDesLauncher() {
	    addSnc(mAdd);
	    addSnc(mConst_1);
	    mAdd.mOcp.connect(&mAdd.Inp2);
	    mAdd.Inp1.connect(&mConst_1.mOcp);
	    mConst_1.set(1);
	    mAdd.set(0);
	}
};





/** @brief Test of DES transitions
 * */
void Ut_pdes::test_des_tr_1()
{
    cout << endl << "=== Test of simple primary DES ===" << endl;
    Des1 des;
    des.Run(5, 2);
    auto data = des.mAdd.mOcp.data();
    CPPUNIT_ASSERT_MESSAGE("Failed running Des1", data == 5);


    Des2 des2;
    des2.Run(5, 2);
    auto data2 = des2.mAdd.mOcp.data();
    CPPUNIT_ASSERT_MESSAGE("Failed running Des2", data2 == 5);

}
