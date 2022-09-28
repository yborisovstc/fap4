
#include <functional>

#include "node.h"



Node::Node(const std::string& aName): PDes(),
    mCpOwned3(), mCpOwning3(),
    mCpNode(&mNode), mNode(mUri.mOcp), mOwning(mUri.mOcp), 
    mOwningUri([&]() { mOwningUri.mRes.mData = mOwningUri.Inp1.data() + mOwningUri.Inp2.data(); }),
    tOwnedId([](){})
{
    mBcp.connect(&sName.mScp);
    sName.set(aName);
    // Owned CP
    mOwningUriTr.Inp1.connect(mCpOwned3.mPins.mUri.binded());
    sName.mOcp.connect(mCpOwned3.mPins.mId.binded());
    // Owning CP
    mOwningUriTr.mOcp.connect(mCpOwning3.mPins.mUri.binded());
    // Owning Uri
    mOwningUriTr.Inp2.connect(&sName.mOcp);
    mOwningUriTr.Inp3.connect(&mCpOwned3.oConnected);
}
