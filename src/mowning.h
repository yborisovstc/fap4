
#ifndef __FAP4_MOWNING_H
#define __FAP4_MOWNING_H

#include "miface.h"
#include "pconn.h"
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
	using TCp =  MPc<MOwned, MOwner>;
	MOwner(MPsOutp<GUri>& aUri): mUri(aUri) {}
	MPsOutp<GUri>& mUri;   //!< Owner URI, is used to form owned URI
};


/** @brief Native hierarchy "owned" interface
 * */
class MOwned : public MIface
{
    public:
	static const char* Type() { return "MOwned";};
	using TCp =  MPc<MOwned, MOwner>;
};

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



// EXP Using tag

template <typename P = tag_outp, typename Q = tag_inp> struct TOwner {
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


template <typename P = tag_outp, typename Q = tag_inp>
struct MNnOwning {
    using TPair = MNnOwning<Q,P>;
    bool connect(TPair& aPair) {
	bool res = true;
	res &= mUri->connect(aPair.mUri);
	return res;
    }
    bool disconnect(TPair& aPair) {
	bool res = true;
	res &= mUri->disconnect(aPair.mUri);
	return res;
    }

    typename MPsCp<GUri, P>::Tcp* mUri = nullptr;
    typename MPsCp<std::string, Q>::Tcp* mId = nullptr;
};



#endif