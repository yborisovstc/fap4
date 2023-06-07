
#ifndef __NDEDS_PCONN2_H
#define __NDEDS_PCONN2_H

#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <forward_list>
#include <iterator>
#include <type_traits>

#include <assert.h>

/** @brief Primary templated connection points
 * that originate homogeneous tree
 * Leafs traversal using leafs iterator
 * */


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
	~PCpnp() { this->disconnect(); mPx = (TPif*) 0x01;}

	bool connect(TPair* aPair) {
	    bool res = false;
	    if ((!mOto || pcount() == 0 ) &&  aPair && !aPair->isPair(this) && !isPair(aPair)) {
		bool res = aPair->attach(this);
		assert(res); // Debug
		if (res) {
		    res = attach(aPair);
		}
		assert(res); // Debug
	    }
	    return res;
	}

	bool disconnect(TPair* aPair) {
	    bool res = false;
	    if (aPair && aPair->isPair(this) && isPair(aPair)) {
		res = aPair->detach(this);
		//assert(res); // Debug
		if (res) {
		    res = detach(aPair);
		}
		//assert(res); // Debug
	    }
	    return res;
	}

	TPif* provided() { return mPx;}
	const TPif* provided() const { return mPx;}

	virtual bool attach(TPair* aPair) {
	    bool res = false;
	    if ((!mOto || pcount() == 0 ) && aPair && !isPair(aPair)) {
		mPairs.insert(aPair);
		res = true;
	    }
	    return res;
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
	LeafsIterator leafsBegin() {
	    TPairsIterator pit = pairsBegin();
	    LeafsIterator it(pit, pairsEnd());
	    if (pit != pairsEnd()) {
		TSelf* pbnd = (*pit)->binded();
		if (pbnd) {
		    // Node, proceed to leafs
		    auto pli = pbnd->leafsBegin();
		    if (pli != pbnd->leafsEnd()) {
			it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		    }
		} else if (!(*pit)->provided()) {
		    // Leaf but w/o provided, omitting
		    // Following the approach of showning all leafs. To re-consider?
		    //it = LeafsIterator(pairsEnd(), pairsEnd());
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
		    // Node, proceed to leafs
		    auto pli = pbnd->leafsCbegin();
		    if (pli != pbnd->leafsCend()) {
			it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		    }
		} else if (!(*pit)->provided()) {
		    // Leaf but w/o provided, omitting
		    // Following the approach of showning all leafs. To re-consider?
		    //it = LeafsConstIterator(pairsCend(), pairsCend());
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
// TODO The problem: shared ownership of provided iface
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


/** @brief Primary DEDS. Socket, ver.02, one-to-one, no Id
 * Avoided PSockOnp problem with shared ownership
 *
 * */
template <class TPif, class TRif, bool VOto = false>
class PSockOnp2: public PCpnp <TPif, TRif>
{
    public:
	using TParent= PCpnp <TPif, TRif>;
	using TPair= typename TParent::TPair;

	PSockOnp2(): PCpnp<TPif, TRif>(&mPins, VOto) {}
	~PSockOnp2() {
	    // Needs preventive disconnect to disconnect pins before they are destructed
	    this->disconnect();
	}
	bool attach(TPair* aPair) override {
	    bool res =  TParent::mPx->attach(*aPair->provided()) ? TParent::attach(aPair) : false;
	    //assert(res); // Debug
	    return res;
	}
	bool detach(TPair* aPair) override {
	    bool res = TParent::mPx->detach(*aPair->provided()) ? TParent::detach(aPair) : false;
	    //assert(res); // Debug
	    return res;
	}
    public:
	TPif mPins;
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







template <bool Id> struct PcpKey {};
template <> struct PcpKey<true> { typedef std::string TKey;};
template <> struct PcpKey<false> { typedef const void* TKey; };

/** @brief Primary connection point, Id/noId, provided is proxied
 * TODO To audit this design (metaprog) efficiency, consider the legacy design (pconn)
 * (oop, multipurposes connpoint) but improve leafs traversal
 * @tparam TPif  type of provided interface
 * @tparam TRif type of required interface
 * @tparam VId bool, support of Id, otherwise no Id
 * Provided shall expose -id- method for identification
 * */
template <class TPif, class TRif, bool VId>
class PCpp
{
    public:
	using TSelf= PCpp<TPif, TRif, VId>;
	using TPair= PCpp<TRif, TPif, VId>;
	using TIdKey = typename PcpKey<VId>::TKey;
	using TPairs = std::unordered_map<typename PcpKey<VId>::TKey, TPair*>;
	using TPairsIterator = typename TPairs::iterator;
	using TPairsConstIterator = typename TPairs::const_iterator;
    public:
	struct LeafsIterator {
	    LeafsIterator(const TPairsIterator &aRootEnd): mRootEnd(aRootEnd) {}
	    LeafsIterator(const TPairsIterator& aPi, const TPairsIterator &aRootEnd): LeafsIterator(aRootEnd) { mPi.push_front(aPi); }
	    TPair* operator*() {
		return mPi.front()->second;
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
			TSelf* pl = plpit->second->binded();
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
		    TSelf* pbnd = front->second->binded();
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
		return mPi.front()->second;
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
	PCpp() = delete;
	PCpp(TPif* aPx, bool aOto = false): mPx(aPx), mOto(aOto) {}
	~PCpp() { this->disconnectAll(); mPx = (TPif*) 0x01;}

	bool connect(TPair* aPair) {
	    bool res = false;
	    if ((!mOto || pcount() == 0 ) &&  aPair && !aPair->isPair(this) && !isPair(aPair)) {
		res = aPair->attach(this);
		//assert(res); // Debug
		if (res) {
		    res = attach(aPair);
		}
		//assert(res); // Debug
	    }
	    return res;
	}


	/*
	   typename std::enable_if<VId, PcpKey<true>::TKey> id() const {
	   return mPx->Id();
	   }
	   typename std::enable_if<!VId, PcpKey<false>::TKey> id() const {
	   return this;
	}
	*/

	/*
	PcpKey<true>::TKey id(const PcpKey<true>::TKey&) const {
	    return mPx->Id();
	}
	PcpKey<false>::TKey id(const PcpKey<false>::TKey&) const {
	    return this;
	}
	*/
	void getId(PcpKey<true>::TKey& aId) const {
	    aId = mPx ? mPx->Id() : "?";
	}
	void getId(PcpKey<false>::TKey& aId) const {
	    aId = this;;
	}

	bool disconnect(TPair* aPair) {
	    bool res = false;
	    if (aPair && aPair->isPair(this) && isPair(aPair)) {
		res = aPair->detach(this);
		//assert(res); // Debug
		if (res) {
		    res = detach(aPair);
		}
		//assert(res); // Debug
	    }
	    return res;
	}

	TPif* provided() { return mPx;}
	const TPif* provided() const { return mPx;}

	virtual bool attach(TPair* aPair) {
	    bool res = false;
	    if ((!mOto || pcount() == 0 ) && aPair && !isPair(aPair)) {
		auto id = TIdKey();  aPair->getId(id);
		mPairs.insert(typename TPairs::value_type(id, aPair));
		res = true;
	    }
	    return res;
	}
	virtual bool detach(TPair* aPair) {
	    assert(aPair && isPair(aPair));
	    auto id = TIdKey();  aPair->getId(id);
	    mPairs.erase(id);
	    return true;
	}
	virtual bool disconnectAll() {
	    bool res = true;
	    while (res && pairsBegin() != pairsEnd()) {
		res = disconnect(pairsBegin()->second);
	    }
	    return res;
	}
	bool isPair(TPair* aPair) const {
	    auto id = TIdKey();  aPair->getId(id);
	    return mPairs.count(id) == 1;;
	}
	int pcount() const { return mPairs.size(); }
	virtual TPair* binded() { return nullptr;}
	// Local
	TPairsIterator pairsBegin() { return mPairs.begin(); }
	TPairsIterator pairsEnd() { return mPairs.end(); }
	TPairsConstIterator pairsCbegin() const { return mPairs.cbegin(); }
	TPairsConstIterator pairsCend() const { return mPairs.cend(); }
	LeafsIterator leafsEnd() { return LeafsIterator(pairsEnd(), pairsEnd()); }
	LeafsIterator leafsBegin() {
	    TPairsIterator pit = pairsBegin();
	    LeafsIterator it(pit, pairsEnd());
	    if (pit != pairsEnd()) {
		TSelf* pbnd = pit->second->binded();
		if (pbnd) {
		    // Node, proceed to leafs
		    auto pli = pbnd->leafsBegin();
		    if (pli != pbnd->leafsEnd()) {
			it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		    }
		} else if (!pit->second->provided()) {
		    // Leaf but w/o provided, omitting
		    // Following the approach of showning all leafs. To re-consider?
		    //it = LeafsIterator(pairsEnd(), pairsEnd());
		}
	    }
	    return it;
	}
	LeafsConstIterator leafsCbegin() const {
	    TPairsConstIterator pit = pairsCbegin();
	    LeafsConstIterator it(pit, pairsCend());
	    if (pit != pairsCend()) {
		TSelf* pbnd = pit->second->binded();
		if (pbnd) {
		    // Node, proceed to leafs
		    auto pli = pbnd->leafsCbegin();
		    if (pli != pbnd->leafsCend()) {
			it.mPi.splice_after(it.mPi.before_begin(), pli.mPi);
		    }
		} else if (!pit->second->provided()) {
		    // Leaf but w/o provided, omitting
		    // Following the approach of showning all leafs. To re-consider?
		    //it = LeafsConstIterator(pairsCend(), pairsCend());
		}
	    }
	    return it;
	}
	LeafsConstIterator leafsCend() const { return LeafsConstIterator(pairsCend(), pairsCend()); }

	void dumpIndent(int aLevel) {
	    for (int i = 0; i < aLevel; ) {
		std::cout << "";
	    }
	}

	void __attribute__ ((noinline)) dump(int aLevel = 0, const void* aCtx = nullptr, bool aBinded = false) {
	    std::string indent(2 * aLevel, ' ');
	    auto id = TIdKey();  getId(id);
	    std::cout << indent << (aBinded ? ">" : "") <<  id << std::endl;
	    for (auto elem : mPairs) {
		auto* pair = elem.second;
		if (pair != aCtx) {
		    pair->dump(aLevel + 1, this);
		}
		//std::cout << std::hex << elem.second << std::endl;
		//std::cout << id << std::endl;
	    }
	    auto* bnd = binded();
	    if (bnd && bnd != aCtx) {
		//std::cout << indent << "Binded: " <<  std::endl;
		bnd->dump(aLevel + 1, this, true);
	    }
	}

    protected:
	TPairs mPairs;
	TPif* mPx;
	bool mOto;
};


/** @brief Expander, identifying, proxied
 * */
template <class TProv, class TReq, class TCn, bool VId, bool VOto>
class PExp2 : public PCpp<TProv, TReq, VId>
{
    public:
	using TScp = PCpp<TProv, TReq, VId>;  /*!< Self connpoint type */
	using TPair = typename TScp::TPair;
	using TCnode = TCn; /*!< Complement node (pole) type */
    public:
	class Cnode: public TCnode {
	    public:
		Cnode(TReq* aPx, PExp2* aHost): TCnode(aPx, VOto), mHost(aHost) {}
		// From MNcpp
		virtual typename TCnode::TPair* binded() override { return mHost;}
	    private:
		PExp2* mHost;
	};
    public:
	PExp2(): PCpp<TProv, TReq, VId>(nullptr, true), mCnode(nullptr, this) {}
	// From MNcpp
	typename TScp::TPair* binded() override { return &mCnode;}
	bool disconnectAll() override {
	    bool res = TScp::disconnectAll();
	    assert(res); // Debug
	    res = res && mCnode.disconnectAll();
	    assert(res); // Debug
	    return res;
	}
    protected:
	Cnode mCnode;
};

template <class TPif, class TRif, bool VId, bool VOTO = false>
class PCppr : public PCpp<TPif, TRif, VId>
{
    public:
	PCppr(TPif* aPx): PCpp<TPif, TRif, VId>(aPx) { PCpp<TPif, TRif, VId>::mOto = VOTO;}
};


/** @brief Primary DEDS. Socket, ver.02, Id/noId
 * Avoided PSockOnp problem with shared ownership
 * */
template <class TPif, class TRif, bool VId, bool VOto = false>
class PSockP2: public PCpp <TPif, TRif, VId>
{
    public:
	using TParent= PCpp <TPif, TRif, VId>;
	using TPair= typename TParent::TPair;

	PSockP2(): PCpp<TPif, TRif, VId>(&mPins, VOto) {}
	~PSockP2() {
	    // Needs preventive disconnect to disconnect pins before they are destructed
	    this->disconnectAll();
	}
	bool attach(TPair* aPair) override {
	    bool res =  TParent::mPx->attach(*aPair->provided()) ? TParent::attach(aPair) : false;
	    //assert(res); // Debug
	    return res;
	}
	bool detach(TPair* aPair) override {
	    bool res = TParent::mPx->detach(*aPair->provided()) ? TParent::detach(aPair) : false;
	    //assert(res); // Debug
	    return res;
	}
    public:
	TPif mPins;
};




/** @brief Extender one-to-many, no Id
 * */
template <class TProv, class TReq>
using PExdm2 = PExp2<TProv, TReq, PCpp<TReq, TProv, false>, false, false>;

/** @brief Extender one-to-one, no Id
 * */
template <class TProv, class TReq>
using PExd2 = PExp2<TProv, TReq, PCpp<TReq, TProv, false>, false, true>;

/** @brief Extender one-to-many, Id
 * */
template <class TProv, class TReq>
using PExdm2i = PExp2<TProv, TReq, PCpp<TReq, TProv, true>, true, false>;

/** @brief Extender one-to-one, Id
 * */
template <class TProv, class TReq>
using PExd2i = PExp2<TProv, TReq, PCpp<TReq, TProv, true>, true, true>;


/** @brief Aliasing base CP
 * */
template <class TProv, class TReq>
using PCpOnp2 = PCppr<TProv, TReq, false, true>;

template <class TProv, class TReq>
using PCpOip2 = PCppr<TProv, TReq, true, true>;

template <class TProv, class TReq>
using PCpOmnp2 = PCppr<TProv, TReq, false, false>;




/*
template <class TPif, class TRif, bool VId>
bool PCpp<TPif, TRif, true>::isPair(PCpp<TPif, TRif, true>::TPair* aPair) const {
    auto id = aPair->id(); assert(!id.empty()); return mPairs.count(aPair->id()) == 1;
}
*/


#if 0
template <class TPif, class TRif, bool VOto>
class PCpp<TPif, TRif, true, VOto>
{
    using TPair= PCpp<TRif, TPif, true, VOto>;
    bool isPair(TPair* aPair) const {
	auto id = aPair->id(); assert(!id.empty()); return mPairs.count(aPair->id()) == 1;
    }
    bool getPair(bool aId) const;
};


template <class TPif, class TRif, bool VOto>
bool PCpp<TPif, TRif, true, VOto>::getPair(bool aId) const { return true;}
#endif




#define PCONN2_ENABLED

#endif

