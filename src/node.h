
#ifndef __FAP4_NODE_H
#define __FAP4_NODE_H

#include <string>

#include "pdes.h"
#include "mowning.h"
#include "mnode.h"
#include "nconn.h"


class Node: public PDes
{
    protected:
	struct Owned : public MOwned {
	    std::string Uid() const override { return std::string();}
	};
	struct Owning : public MOwner {
	    std::string Uid() const override { return std::string();}
	    Owning(PsOcp<GUri>& aUri): MOwner(aUri) {}
	};
	struct Nnode : public MNode {
	    std::string Uid() const override { return std::string();}
	    Nnode(PsOcp<GUri>& aUri): MNode(aUri) {}
	};
    public:
	Node(const std::string& aName);
    public:
	//PState<std::string>::TOcp cpName;
	PState<std::string> sName;
	Owned mOwned;
	Owning mOwning;
	Nnode mNode;
	class TOwningUri : public PTrans3<GUri, GUri, std::string, bool> {
	    void doTrans() override {
		mRes.mValid = Inp2.valid() && Inp3.valid();
		if (mRes.mValid) {
		    if (Inp3.data()) {
			mRes.mValid &= Inp1.valid();
			mRes.mData = Inp1.data() + Inp2.data();
		    } else
			mRes.mData = Inp2.data();
		}
	    }
	};
	MNnOwning<> mOwning2;
	MNnOwning<>::TPair mOwned2;
    public:
	// Interfaces
	//PCpOnp<MOwned, MOwner> mCpOwned;
	//PCpOmnp<MOwner, MOwned> mCpOwning;
	PCpOnp<MNode, MVoid> mCpNode;
	//TOwner<tag_outp, tag_inp> mCpOwning;
	//TOwner<tag_inp, tag_outp> mCpOwned;
	//MNCp<MNnOwning> mCpOwning2;
	//PSockOnp<MNnOwning<>, MNnOwning<>::TPair> mCpOwning2;
	//PSockOnp<MNnOwning<>::TPair, MNnOwning<>> mCpOwned2;
	MNCp2<MNnOwning<>> mCpOwning3;
	MNCp2<MNnOwning<>::TPair> mCpOwned3;
    private:
	PState<GUri> mUri;
	PTransl2<GUri, GUri, std::string> mOwningUri;
    public: // Debug
	// Example of using trans w/o lambda
	TOwningUri mOwningUriTr;
	PTransl1<GUri, std::string> tOwnedId;
};

#endif


