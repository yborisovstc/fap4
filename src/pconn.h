
#ifndef __NDEDS_PCONN_H
#define __NDEDS_PCONN_H

#include <set>

#include <assert.h>

/** @brief Primary connection point interface
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
	virtual TPif* provided() = 0;
	virtual const TPif* provided() const = 0;
	virtual bool connect(TPair* aPair) = 0;
	virtual bool disconnect(TPair* aPair) = 0;
	virtual bool disconnect() = 0;
	virtual bool isPair(TPair* aPair) const = 0;
	virtual bool attach(TPair* aPair) = 0;
	virtual bool detach(TPair* aPair) = 0;
	/** @brief Gets pairs count */
	virtual int pcount() const = 0;
};

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
	virtual bool connect(TPair* aPair) override ;
	virtual bool disconnect() override {
	    bool res = true;
	    if (mPair) res = disconnect(mPair);
	    return res;
	}
	virtual bool disconnect(TPair* aPair) override;
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool isPair(TPair* aPair) const override { return mPair && mPair == aPair; }
	/** @brief Gets pairs count */
	virtual int pcount() const override { return mPair ? 1 : 0; }
    public:
	TRif* required() { return mPair ?  mPair->provided() : nullptr; }
    protected:
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

template <class TPif, class TRif>
bool PCpOnp<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isPair(this) && !isPair(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

template <class TPif, class TRif>
bool PCpOnp<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isPair(this) && isPair(aPair));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    return res;
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
	virtual ~PCpOmnp() { disconnect(); }
	virtual TPif* provided() override { return mPx;}
	virtual const TPif* provided() const override { return mPx;}
	virtual bool connect(TPair* aPair) override ;
	virtual bool attach(TPair* aPair) override;
	virtual bool detach(TPair* aPair) override;
	virtual bool disconnect(TPair* aPair) override;
	virtual bool disconnect() override {
	    bool res = true;
	    while (res && begin() != end()) {
		res = disconnect(*begin());
	    }
	    return res;
	}
	virtual bool isPair(TPair* aPair) const override;
	/** @brief Gets pairs count */
	virtual int pcount() const override { return mPairs.size(); }
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
bool PCpOmnp<TPif, TRif>::connect(TPair* aPair)
{
    assert(aPair && !aPair->isPair(this) && !isPair(aPair));
    bool res = aPair->attach(this);
    if (res) {
	res = attach(aPair);
    }
    return res;
}

    template <class TPif, class TRif>
bool PCpOmnp<TPif, TRif>::disconnect(TPair* aPair)
{
    assert(aPair && aPair->isPair(this) && isPair(aPair));
    bool res = aPair->detach(this);
    if (res) {
	res = detach(aPair);
    }
    assert(res);
    return res;
}

template <class TPif, class TRif>
bool PCpOmnp<TPif, TRif>::isPair(TPair* aPair) const
{
    return mPairs.count(aPair) == 1;
}


/** @brief Extender, not proxied
 * Included internal complement connpoint Int
 * Both extd and Int are proxy of one to another to translate ifaces
 * */
template <class TPif, class TRif>
class PExtd : public PCpOmnp<TPif, TRif>
{
    friend class TInt;
    public:
	class TInt : public PCpOmnp<TRif, TPif>
    {
    };

    public:
	PExtd(): PCpOmnp<TPif, TRif>(*mInt), mInt(this) {}
    public:
	TInt mInt;
};

#endif
