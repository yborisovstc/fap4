
#ifndef __NDEDS_PCONN2_H
#define __NDEDS_PCONN2_H

#include <iostream>
#include <vector>
#include <set>
#include <forward_list>
#include <iterator>
#include <type_traits>

#include <assert.h>



/** @brief Primary connection point, no Id, provided is proxied
 * @tparam TPif  type of provided interface
 * @tparam TPair type of pair
 * @tparam VOto  sign of one-to-one connpoint
 * */
template <class TPif, class TRif>
class PCpnp
{
    public:
	using TSelf= PCpnp<TPif, TRif>;
	using TPair= PCpnp<TRif, TPif>;
	//using TSelf= typename MPc<TPif, TRif>::TSelf;
	//using TPair= typename MPc<TPif, TRif>::TPair;
	using TPairs = std::set<TPair*>;
	using TPairsIterator = typename TPairs::iterator;
	using TPairsConstIterator = typename TPairs::const_iterator;
    public:
	struct LeafsIterator {
	    LeafsIterator(const TPairsIterator &aRootEnd): mRootEnd(aRootEnd) {}
	    LeafsIterator(const TPairsIterator& aPi, const TPairsIterator &aRootEnd): LeafsIterator(aRootEnd) { mPi.push_front(aPi); }
	    TPair* operator*() {
		return *(mPi.front());
	    }
	    LeafsIterator operator++(int) {
		auto res = *this;
		bool stop = false;
		bool end = false;
		do {
		    // Find nearest uncompleted node
		    auto& front = mPi.front();
		    auto plit = mPi.begin();
		    plit++;
		    if (plit != mPi.end()) {
			TPairsIterator plpit = *plit;
			TSelf* pl = (*plpit)->binded();
			front++;
			if (front == pl->pairsEnd()) {
			    // Current node is over, go to next one, and set to it's leaf begin
			    mPi.pop_front();
			} else {
			    // Uncompleted node found
			    stop = true;
			}
		    } else {
			// Root node reached
			front++;
			stop = true;
			if (front == mRootEnd) {
			    end = true;
			}
		    }
		} while (!stop);

		if (!end) {
		    auto& front = mPi.front();
		    TSelf* pbnd = (*front)->binded();
		    if (pbnd) {
			auto pli = pbnd->leafsBegin();
			mPi.splice_after(mPi.before_begin(), pli.mPi);
		    }
		}

		return res;
	    }
	    bool operator==(const LeafsIterator& aB) {
		return  mPi == aB.mPi;
	    }
	    bool operator!=(const LeafsIterator& aB) {
		return  mPi != aB.mPi;
	    }
	    //void __attribute__ ((noinline)) dump();
	    void __attribute__ ((noinline)) dump() {
		for (auto it = mPi.begin(); it != mPi.end(); it++) {
		    TPair* ptr = **it;
		    std::cout << std::hex << ptr << std::endl;
		}
	    }
	    std::forward_list<TPairsIterator> mPi;
	    TPairsIterator mRootEnd;
	};

	struct LeafsConstIterator {
	    LeafsConstIterator(const TPairsConstIterator &aRootEnd): mRootEnd(aRootEnd) {}
	    LeafsConstIterator(const TPairsConstIterator& aPi, const TPairsConstIterator& aRootEnd): LeafsConstIterator(aRootEnd) { mPi.push_front(aPi); }
	    const TPair* operator*() {
		return *(mPi.front());
	    }
	    bool operator==(const LeafsConstIterator& aB) {
		return  mPi == aB.mPi;
	    }
	    bool operator!=(const LeafsConstIterator& aB) {
		return  mPi != aB.mPi;
	    }

	    std::forward_list<TPairsConstIterator> mPi;
	    TPairsConstIterator mRootEnd;
	};
    public:
	PCpnp(TPif* aPx, bool aOto = false): mPx(aPx), mOto(aOto) {}
	~PCpnp() { this->disconnect(); }

	bool connect(TPair* aPair) {
	    assert((!mOto || pcount() == 0 ) &&  aPair && !aPair->isPair(this) && !isPair(aPair));
	    bool res = aPair->attach(this);
	    assert(res); // Debug
	    if (res) {
		res = attach(aPair);
	    }
	    assert(res); // Debug
	    return res;
	}

	bool disconnect(TPair* aPair) {
	    assert(aPair && aPair->isPair(this) && isPair(aPair));
	    bool res = aPair->detach(this);
	    assert(res); // Debug
	    if (res) {
		res = detach(aPair);
	    } 
	    assert(res); // Debug
	    return res;
	}

	TPif* provided() { return mPx;}
	const TPif* provided() const { return mPx;}

	virtual bool attach(TPair* aPair) {
	    assert(aPair && !isPair(aPair));
	    mPairs.insert(aPair);
	    return true;
	}
	virtual bool detach(TPair* aPair) {
	    assert(aPair && isPair(aPair));
	    mPairs.erase(aPair);
	    return true;
	}
	virtual bool disconnect() {
	    bool res = true;
	    while (res && pairsBegin() != pairsEnd()) {
		res = disconnect(*pairsBegin());
	    }
	    return res;
	}
	bool isPair(TPair* aPair) const { return mPairs.count(aPair) == 1;}
	int pcount() const { return mPairs.size(); }
	virtual TPair* binded() { return nullptr;}
	// Local
	TPairsIterator pairsBegin() { return mPairs.begin(); }
	TPairsIterator pairsEnd() { return mPairs.end(); }
	TPairsConstIterator pairsCbegin() const { return mPairs.cbegin(); }
	TPairsConstIterator pairsCend() const { return mPairs.cend(); }
	LeafsIterator leafsEnd() { return LeafsIterator(pairsEnd(), pairsEnd()); }
	/* Ver.01
	LeafsIterator leafsBegin() {
	    LeafsIterator it;
	    TPairsIterator pit = pairsBegin();
	    if (pit != pairsEnd()) {
		TSelf* pbnd = (*pit)->binded();
		if (!pbnd) {
		    it.mPi.push_front(pit);
		} else {
		    auto pli = pbnd->leafsBegin();
		    it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		}
	    }
	    return it;
	}
	*/
	LeafsIterator leafsBegin() {
	    TPairsIterator pit = pairsBegin();
	    LeafsIterator it(pit, pairsEnd());
	    if (pit != pairsEnd()) {
		TSelf* pbnd = (*pit)->binded();
		if (pbnd) {
		    auto pli = pbnd->leafsBegin();
		    it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		}
	    }
	    return it;
	}
	LeafsConstIterator leafsCbegin() const {
	    TPairsConstIterator pit = pairsCbegin();
	    LeafsConstIterator it(pit, pairsCend());
	    if (pit != pairsCend()) {
		TSelf* pbnd = (*pit)->binded();
		if (pbnd) {
		    auto pli = pbnd->leafsBegin();
		    it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		}
	    }
	    return it;
	}
	LeafsConstIterator leafsCend() const { return LeafsConstIterator(pairsCend(), pairsCend()); }

	void __attribute__ ((noinline)) dump() {
	    for (auto elem : mPairs) {
		std::cout << std::hex << elem << std::endl;
	    }
	}

    protected:
	TPairs mPairs;
	TPif* mPx;
	bool mOto;
};

template <class TPif, class TRif, bool VOTO = false>
class PCpnpr : public PCpnp<TPif, TRif>
{
    public:
	PCpnpr(TPif* aPx): PCpnp<TPif, TRif>(aPx) { PCpnp<TPif, TRif>::mOto = VOTO;}
};

/** @brief Expander, proxied
 * */
template <class TProv, class TReq, class TCn, bool VOto = false>
class PExp : public PCpnp<TProv, TReq>
{
    public:
	using TScp = PCpnp<TProv, TReq>;  /*!< Self connpoint type */
	using TPair = typename TScp::TPair;
	using TCnode = TCn; /*!< Complement node (pole) type */
    public:
	class Cnode: public TCnode {
	    public:
		Cnode(TReq* aPx, PExp* aHost): TCnode(aPx, VOto), mHost(aHost) {}
		// From MNcpp
		virtual typename TCnode::TPair* binded() override { return mHost;}
	    private:
		PExp* mHost;
	};
    public:
	//PExp(TProv* aProvPx, TReq* aReqPx): PCpnp<TProv, TReq, true>(aProvPx), mCnode(aReqPx, this) {}
	PExp(): PCpnp<TProv, TReq>(nullptr, true), mCnode(nullptr, this) {}
	// From MNcpp
	typename TScp::TPair* binded() override { return &mCnode;}
	bool disconnect() override {
	    bool res = TScp::disconnect();
	    assert(res); // Debug
	    res = res && mCnode.disconnect();
	    assert(res); // Debug
	    return res;
	}
    protected:
	Cnode mCnode;
};

/** @brief Primary DEDS. Socket, one-to-one, no Id
 * */
template <class TPif, class TRif>
class PSockOnp: public PCpnp <TPif, TRif>
{
    public:
	using TParent= PCpnp <TPif, TRif>;
	using TPair= typename TParent::TPair;

	PSockOnp(TPif* aPx): PCpnp<TPif, TRif>(aPx) {}
	bool attach(TPair* aPair) override {
	    bool res =  TParent::mPx->attach(*aPair->provided()) ? TParent::attach(aPair) : false;
	    assert(res); // Debug
	    return res;
	}
	bool detach(TPair* aPair) override {
	    bool res = TParent::mPx->detach(*aPair->provided()) ? TParent::detach(aPair) : false;
	    assert(res); // Debug
	    return res;
	}
};



/** @brief Extender one-to-many
 * */
template <class TProv, class TReq>
using PExdm = PExp<TProv, TReq, PCpnp<TReq, TProv>>;

/** @brief Extender one-to-one
 * */
template <class TProv, class TReq>
using PExd = PExp<TProv, TReq, PCpnp<TReq, TProv>, true>;

/** @brief Aliasing base CP
 * */
template <class TProv, class TReq>
using PCpOnp = PCpnpr<TProv, TReq, true>;

template <class TProv, class TReq>
using PCpOmnp = PCpnpr<TProv, TReq, false>;


#define PCONN2_ENABLED

#endif

