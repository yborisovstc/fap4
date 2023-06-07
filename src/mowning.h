
#ifndef __FAP4_MOWNING_H
#define __FAP4_MOWNING_H

#include "miface.h"
#include "pconn2.h"
#include "pdes.h"
#include "nconn.h"
#include "guri.h"

class MOwned;

/** @brief Native hierarchy "owning" interface
 * */
class MOwner : public MIface
{
    public:
	static const char* Type() { return "MOwner";};
	//using TCp =  MPc<MOwned, MOwner>;
	MOwner(MPsOutp<GUri>& aUri): mUri(aUri) {}
	MPsOutp<GUri>& mUri;   //!< Owner URI, is used to form owned URI
};


/** @brief Native hierarchy "owned" interface
 * */
class MOwned : public MIface
{
    public:
	static const char* Type() { return "MOwned";};
	//using TCp =  MPc<MOwned, MOwner>;
};

#if 0
struct MNOwned;

struct MNOwner : public MPcb
{
    MPsOutp<GUri>& mUri;        //!< Owner URI, is used to form owned URI
    MPsInp<std::string>& mId;   //!< Owned ID
    virtual bool connect(MPcb* aPair) override;
};

struct MNOwned : public MPcb
{
    MPsInp<GUri>& mUri;        //!< Owner URI, is used to form owned URI
    MPsOutp<std::string>& mId;   //!< Owned ID
    virtual bool connect(MPcb* aPair) override;
};


bool MNOwner::connect(MPcb* aPair) {
    bool res;
    MNOwned* pair = static_cast<MNOwned*>(aPair);
    res &= mUri.connect(&pair->mUri);
    res &= mId.connect(&pair->mId);
    return res;
}	
#endif



// EXP Using tag

template <typename P = tag_outp, typename Q = tag_inp>
struct TOwner {
    using TPair = TOwner<Q,P>;
    explicit TOwner() {}
    bool connect(TPair* aPair) {
	bool res = true;
	res &= mUri->connect(aPair->mUri);
	res &= mId->connect(aPair->mId);
	return res;
    }
    typename MPsCp<GUri, P>::Tcp* mUri = nullptr;
    typename MPsCp<std::string, Q>::Tcp* mId = nullptr;
};

/** @brief Owning native interface
 * Note: design variant having pins as refs to CPs is wrong - 
 * it causes CP sharing ownership that leads to the problems
 * on the model destructor for instance
 * */
template <typename P = tag_outp, typename Q = tag_inp>
struct MNnOwning {
    using TPair = MNnOwning<Q,P>;
    bool attach(TPair& aPair) {
	bool res = true;
	res &= mUri.attach(&aPair.mUri);
	res &= mId.attach(&aPair.mId);
	return res;
    }
    bool detach(TPair& aPair) {
	bool res = true;
	res &= mUri.detach(&aPair.mUri);
	res &= mId.detach(&aPair.mId);
	return res;
    }
    auto ttr(int aa) -> int {
	return 4;
    }
    std::string Id() const {
	/*
	auto* prov =  mId.provided();
	std::string id;
	auto idd = prov->sData();
	*/
	return std::string();
    }
    /*
    template <typename T> std::enable_if<std::is_same<T, tag_outp>::value, std::string> getId()  {
    }
    template <typename T> std::enable_if<std::is_same<T, tag_inp>::value, std::string> getId()  {
	return std::string();
    }

    std::string Id() {
	return getId<Q>();
    }
    */
    std::string getId(const tag_outp&)  {
	std::string res;
	// Assuming that direct pair provides the data
	auto pairIt =  mId.binded()->pairsBegin();
	if (pairIt != mId.binded()->pairsEnd()) {
	    auto* pair = pairIt->second;
	    auto* prov = pair->provided();
	    auto* idd = prov->sData();
	    res = idd->mData;
	}
	return res;
    }

    std::string getId(const tag_inp&)  {
	return std::string();
    }
    std::string Id() {
	return getId(Q());
    }

    typename MPsEx<std::string, Q>::Tcp mId;
    typename MPsEx<GUri, P>::Tcp mUri;              //!< Owning URI, output
    typename MPsEx<std::string, P>::Tcp mNewNodeId; //!< Embedding MNode, ref ds_nh_mnode_emb. Temporary. To simulate mutation "add"
    // TODO There was the idea to "embed" Des Obsr-Obsbl connection into Owning conn. Disabled atm, to re-consider
    //PExdm<MDesSyncable, MDesObserver> mPdObs;  //<! Primary DES observer, o-o self and o-m embedded cp
};



#endif
