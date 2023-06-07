
#include <functional>

#include "node.h"



#if 0
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

#endif

// Node base

NodeBase::NodeBase(TBcp* aBcp, const std::string& aName): PDes(aBcp, aName), mCpOwning(),
    mCtdOwning(this), mCtdDesObs(this)
{
}

NodeBase::~NodeBase() {}

// Node base

Node::Node(const std::string& aName): Node(nullptr, aName) {}

Node::Node(TBcp* aBcp, const std::string& aName): PDes(aBcp, aName),
    mBase(&mBcp, "Base"), sName(&mBcp, "sName")/*, cAddOwned(mBcp, &mBase, "cAddOwned")*/,
    cAddOwned(&mBcp, "cAddOwned")
{
    sName.set(aName);
    // Owning CP
    mBase.mCpOwning.mPins.mUri.binded()->connect(&trOwningUri.mOcp);
    sName.mOcp.connect(mCpOwned.mPins.mId.binded());
    // Controller AddOutput
    cAddOwned.Inp1.connect(&mBase.mCtdOwning);
    cAddOwned.Inp2.connect(&mBase.mCtdDesObs);
    cAddOwned.Inp3.connect(mCpOwned.mPins.mNewNodeId.binded());
    // trOwningUri
    trOwningUri.Inp1.connect(mCpOwned.mPins.mUri.binded());
    trOwningUri.Inp2.connect(&sName.mOcp);
    trOwningUri.Inp3.connect(&mCpOwned.oConnected);
}

Node::~Node() {}
