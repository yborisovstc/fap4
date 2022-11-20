
#include <functional>

#include "node.h"



Node::Node(TBcp& aBcp, const std::string& aName): PDes(aBcp, aName),
    mCpOwned3(), mCpOwning3(),
    mOwningUri([&]() { mOwningUri.mRes.mData = mOwningUri.Inp1.data() + mOwningUri.Inp2.data(); }),
    tOwnedId([](){}),
    sName(mBcp, "sName"),
    sDobsCp(mBcp, "SDobsCp"),
    sOwning(mBcp, "SOwning")
{
    sName.set(aName);
    // Owned CP
    mOwningUriTr.Inp1.connect(mCpOwned3.mPins.mUri.binded());
    sName.mOcp.connect(mCpOwned3.mPins.mId.binded());
    // Owning CP
    mOwningUriTr.mOcp.connect(mCpOwning3.mPins.mUri.binded());
    // Owning Uri
    mOwningUriTr.Inp2.connect(&sName.mOcp);
    mOwningUriTr.Inp3.connect(&mCpOwned3.oConnected);
    // DES observer connpoint state
    sDobsCp.Inp1.connect(&dBcp.mOcp);
    sDobsCp.Inp2.connect(mCpNode.mPins.mNewNodeId.binded());
    // Owning managing state
    //sOwning.Inp1.connect(&dCpOwning.mOcp);
    //sOwning.Inp2.connect(mCpNode.mPins.mNewNodeId.binded());
    //sOwning.Inp3.connect(&dBcp.mOcp);
}

std::string Node::MDesSyncable_Uid() const
{
    std::string res;
    if (mScp.leafsCbegin() != mScp.leafsCend()) {
	const MDesObserver* obs = (*mScp.leafsCbegin())->provided();
	res = obs->Uid() + ".";
    }
    auto* name = sName.mOcp.sData();
    res += (name && name->mValid && !name->mData.empty()) ? name->mData : "?";
    return res;
}


