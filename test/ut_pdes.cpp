#include <stdlib.h>
#include <cppunit/extensions/HelperMacros.h>

#include "pdes.h"

using namespace std;

/** @brief Test of NDEDS native system
*/
class Ut_pdes : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(Ut_pdes);
//    CPPUNIT_TEST(test_des_tr_1);
    CPPUNIT_TEST(test_des_tr_2);
    CPPUNIT_TEST_SUITE_END();
    public:
    virtual void setUp();
    virtual void tearDown();
    private:
    void test_des_tr_1();
    void test_des_tr_2();
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
	SAdd(TBcp* aBcp, const char* aName = nullptr): PState<int>(aBcp, aName), mIa(this), mIb(this) {}
    protected:
	virtual void doTrans() override {
	    mUdata->mValid = mIa.valid() && mIb.valid();
	    mUdata->mData = mIa.data() + mIb.data();
	    //cout << "SAdd Udata: " << mUdata->mData  << endl;
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

	Des1(const char* aName = nullptr): PDesLauncher(aName), mAdd(&mBcp, "Add"), mConst_1(&mBcp, 1, "Const_1") {
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
    Des1 des("Des1");
    des.Run(5, 2);
    cout << "Des1 uid: " << des.MDesSyncable::Uid() << endl;
    cout << "Des1.mAdd uid: " << des.mAdd.MDesSyncable::Uid() << endl;
    auto data = des.mAdd.mOcp.data();
    CPPUNIT_ASSERT_MESSAGE("Failed running Des1", data == 5);


    Des2 des2;
    des2.Run(5, 2);
    auto data2 = des2.mAdd.mOcp.data();
    CPPUNIT_ASSERT_MESSAGE("Failed running Des2", data2 == 5);
}


struct SPrint : public PState1<int, int>
{
    SPrint(PDesBase::TBcp* aBcp, const char* aName = nullptr): PState1(aBcp, aName) {}
    void doTrans() override {
	mUdata->mValid = Inp1.valid();
	//mUdata->mData = Inp1.data();
	//cout << "Data: " << mUdata->mData << endl;
    }
};

using TPtri = PCpp<MDesInpObserver, MDesStateData<PDd<int>>, false>;

class SCreateChain : public PState<TPtri*>
{
    public:
	SCreateChain(TBcp* aBcp, const char* aName = nullptr): PState<TPtri*>(aBcp, aName), mInp(this) {
	    cout << "Construct SCreateChain" << endl;
	    cout << "Construct SCreateChain 2" << endl;
	}
    protected:
	virtual void doTrans() override {
	    TPtri* newnode = new PsIex<int>();
	    newnode->binded()->connect(mInp.data());
	    mUdata->mValid = true;
	    mUdata->mData = newnode;
	    cout << MDesSyncable::Uid() << ":: Created extender " << mUdata->mData << endl;
	}
    public:
	PsIcp<TPtri*> mInp;

};

class DesT1 : public PDesLauncher
{
    public:
	SPrint sPrint;
	SCreateChain sCc;

	DesT1(const char* aName = nullptr);
};

DesT1::DesT1(const char* aName): PDesLauncher(aName),
    sPrint(&mBcp, "Print"),
    sCc(&mBcp, "CC")
{
    sCc.set(&sPrint.Inp1);
    sCc.mInp.connect(&sCc.mOcp);
}


/** @brief Test of DES transitions
 * */
void Ut_pdes::test_des_tr_2()
{
    cout << endl << "=== Test of primary DES reconfiguring itself ===" << endl;
    DesT1 des("des");
    des.Run(5, 2);
    cout << "des.sPrint uid: " << des.sPrint.MDesSyncable::Uid() << endl;
    auto* ccout = des.sCc.sData();
    cout << "sCc outp dump" << endl;
    ccout->mData->dump();
    //auto data = des.mAdd.mOcp.data();
    //CPPUNIT_ASSERT_MESSAGE("Failed running Des1", data == 5);
}


