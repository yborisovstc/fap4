
#include <functional>

#include "node.h"



Node::Node(const std::string& aName): PDes(),
    /*mCpOwned(), mCpOwning(),*/
    /*mCpOwned2(&mOwned2), mCpOwning2(&mOwning2),*/
    mCpOwned3(&mOwned2), mCpOwning3(&mOwning2),
    mCpNode(&mNode), mNode(mUri.mOcp), mOwning(mUri.mOcp), 
    mOwningUri([&]() { mOwningUri.mRes.mData = mOwningUri.Inp1.data() + mOwningUri.Inp2.data(); }),
    tOwnedId([](){})
{
    mBcp.connect(&sName.mScp);
    sName.set(aName);
    //mOwningUri.Inp2.connect(&sName.mOcp);
    /*
    // Owned CP
    mCpOwned.mId = &sName.mOcp;
    mCpOwned.mUri = &mOwningUriTr.Inp1;
    // Owning CP
    mCpOwning.mId = &tOwnedId.Inp1;
    mCpOwning.mUri = &mOwningUriTr.mOcp;
    */
    // Owned CP
    mOwned2.mId = &sName.mOcp;
    mOwned2.mUri = &mOwningUriTr.Inp1;
    // Owning CP
    mOwning2.mId = &tOwnedId.Inp1;
    mOwning2.mUri = &mOwningUriTr.mOcp;
    // Owning Uri
    mOwningUriTr.Inp2.connect(&sName.mOcp);
    mOwningUriTr.Inp3.connect(&mCpOwned3.oConnected);
}
