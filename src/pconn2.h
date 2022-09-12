
#ifndef __NDEDS_PCONN2_H
#define __NDEDS_PCONN2_H

#include <set>

#include <assert.h>



/** @brief Primary connection point interface specifying complementary pair of OOP ifaces
 * @tparam  TPif  provided interface
 * @tparam  TRif  required interface
 * */
template <class TPif, class TRif>
class MPc
{
    public:
	using TSelf = MPc<TPif, TRif>;
	using TPair = MPc<TRif, TPif>;
	virtual ~MPc() = default;
	bool connect(TPair* aPair);
	bool disconnect(TPair* aPair);
	// Local
	virtual TPif* provided() = 0;
	virtual const TPif* provided() const = 0;
	virtual bool disconnect() = 0;
	virtual bool isPair(TPair* aPair) const = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool detach(TPair* aPair) = 0;
	/** @brief Gets pairs count */
	virtual int pcount() const = 0;
	/** @brief Gets binded connpoint. Used in tree node or extender. */
	virtual TPair* binded() = 0;
	const TPair* binded() const { const_cast<const TPair*>(const_cast<TSelf*>(this)->binded());}
	/** @brief Pairs traversal */
	// TODO This traversal scheme is very ineffective. To redesign. The reason of the current scheme is that
	// it was assumed that the tree is not homogeneous. To consider homogeneous tree.
	virtual TPair* firstPair() = 0;
	virtual TPair* nextPair(TPair* aPair) = 0;
	/** @brief Tree leafs traversal */
	TPair* firstLeaf() {
	    TPair* res = nullptr;
	    auto pair = firstPair();
	    while (pair) {
		if (res = pair->binded() ? pair->binded()->firstLeaf() : pair) break;
		pair = nextPair(pair);
	    }
	    return res;
	}
	TPair* nextLeaf(TPair* aLeaf) {
	    TPair* res = nullptr;
	    auto np = nextPair(aLeaf);
	    while (np) {
		res = np->binded() ? np->binded()->firstLeaf() : np;
		if (res) break;
		np = nextPair(np);
	    }
	    if (! res && binded()) {
		res = binded()->nextLeaf();
	    }
	    return res;
	}
};

    template <class TPif, class TRif>
bool MPc<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isPair(this) && !isPair(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool MPc<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isPair(this) && isPair(aPair));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    return res;
}




/** @brief Native connection point, one-to-one, no Id, proxy
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam THost type of host
 * @tparam TPx type of provided iface proxy
 * */
template <class TPif, class TRif>
class PCpOnp : public MPc<TPif, TRif>
{
    public:
	using TSelf= typename MPc<TPif, TRif>::TSelf;
	using TPair= typename MPc<TPif, TRif>::TPair;
    public:
	PCpOnp(TPif* aPx): mPair(nullptr), mPx(aPx) {}
	virtual ~PCpOnp() { disconnect(); }
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool disconnect() override {
	    bool res = true;
	    if (mPair) res = MPc<TPif, TRif>::disconnect(mPair);
	    return res;
	}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool isPair(TPair* aPair) const override { return mPair && mPair == aPair; }
	virtual int pcount() const override { return mPair ? 1 : 0; }
	TPair* binded() override { return nullptr;}
	TPair* firstPair() override { return  mPair;}
	TPair* nextPair(TPair* aPair) override { return nullptr;}
    public:
	TRif* required() { return mPair ?  mPair->provided() : nullptr; }
	const TRif* required()  const { return mPair ?  mPair->provided() : nullptr; }
    //!!protected:
    public:
	TPair* mPair;
	TPif* mPx;
};

template <class TPif, class TRif>
bool PCpOnp<TPif, TRif>::attach(TPair* aPair)
{
    assert(aPair && !mPair && !isPair(aPair));
    mPair = aPair;
    return true;
}

template <class TPif, class TRif>
bool PCpOnp<TPif, TRif>::detach(TPair* aPair)
{
    assert(aPair && isPair(aPair));
    mPair = nullptr;
    return true;
}




/** @brief Primary connection point, one-to-many, no Id, provided is proxied
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam TPx type of provided iface proxy
 * */
template <class TPif, class TRif>
class PCpOmnp: public MPc<TPif, TRif>
{
    public:
	using TThis= typename PCpOmnp<TPif, TRif>::TSelf;
	using TSelf= typename MPc<TPif, TRif>::TSelf;
	using TPair= typename MPc<TPif, TRif>::TPair;
	using TPairs = std::set<TPair*>;
	using iterator = typename TPairs::iterator;
    public:
	PCpOmnp(TPif* aPx): mPx(aPx) {}
	virtual ~PCpOmnp() { PCpOmnp<TPif, TRif>::disconnect(); }
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool disconnect() override {
	    bool res = true;
	    while (res && begin() != end()) {
		res = MPc<TPif, TRif>::disconnect(*begin());
	    }
	    return res;
	}
	virtual bool isPair(TPair* aPair) const override;
	virtual int pcount() const override { return mPairs.size(); }
	TPair* binded() override { return nullptr;}
	TPair* firstPair() override { return mPairs.size() > 0 ? *mPairs.begin() : nullptr;}
	TPair* nextPair(TPair* aPair) override {
	    auto it = mPairs.find(aPair); it++;
	    auto res = (it != mPairs.end()) ? *it : nullptr; 
	    return res;
	}
	// Local
	iterator begin() { return mPairs.begin(); }
	iterator end() { return mPairs.end(); }
    protected:
	TPairs mPairs;
	TPif* mPx;
};


    template <class TPif, class TRif>
bool PCpOmnp<TPif, TRif>::attach(TPair* aPair)
{
    assert(aPair && !isPair(aPair));
    mPairs.insert(aPair);
    return true;
}

    template <class TPif, class TRif>
bool PCpOmnp<TPif, TRif>::detach(TPair* aPair)
{
    assert(aPair && isPair(aPair));
    mPairs.erase(aPair);
    return true;
}

template <class TPif, class TRif>
bool PCpOmnp<TPif, TRif>::isPair(TPair* aPair) const
{
    return mPairs.count(aPair) == 1;
}



/** @brief Extender, proxied
 * */
template <class TProv, class TReq, class TCn>
class PExtd : public PCpOnp<TProv, TReq>
{
    public:
	using TScp = PCpOnp<TProv, TReq>;  /*!< Self connpoint type */
	using TPair = typename TScp::TPair;
	using TNodeIf = MPc<TProv, TReq>; /*!< Node (pole) iface type */
	using TCnodeIf = MPc<TReq, TProv>; /*!< Complement node (pole) type */
	using TCnode = TCn; /*!< Complement node (pole) type */
    public:
	class Cnode: public TCnode {
	    public:
		Cnode(TReq* aPx, PExtd* aHost): TCnode(aPx), mHost(aHost) {}
		// From MNcpp
		virtual typename TCnode::TPair* binded() override { return mHost;}
	    private:
		PExtd* mHost;
	};
    public:
	PExtd(TProv* aProvPx, TReq* aReqPx): PCpOnp<TProv, TReq>(aProvPx), mCnode(aReqPx, this) {}
	// From MNcpp
	typename TScp::TPair* binded() override { return &mCnode;}
	bool disconnect() override {
	    bool res = TScp::disconnect();
	    res = res && mCnode.disconnect();
	    return res;
	}
    protected:
	Cnode mCnode;
};





#endif

