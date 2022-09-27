
#ifndef __FAP4_NCONN_H
#define __FAP4_NCONN_H

#include "pdes.h"

/** @brief Native layer connections
 * In contrast to primary connections there is no need to have Provided-Required specs
 * because two-ways relations is already defined on primary level.
 * */



#if 0
/** @brief Native connpoint base, PDes based
 * EXPERIMENTAL
 * */
struct MNCp: public PDes
{
    struct SPair : public PState<MNCp*> {
	SPair(TBcp& aBcp) : PState<MNCp*>(aBcp) {}
	using TData = MNCp*;
	void doTrans() override {}
	PsIcp<TData> ciPair{this};
    };
    //MNCp(): sConnected(mBcp) {}
    SPair sPair{mBcp};

};
#endif


/** @brief Native connpoint base, PState based
 * Native connpoint is socket of pins - PState inp/outp
 * State posesses explicit data and implicit data
 * Explicit data is pointer to the pair
 * Implicit data is the pins
 * !! Not completed: error - needs to update pair also
 * */
template <typename T>
struct MNCp: public PState<MNCp<T>*>
{
    using TData = MNCp<T>*;
    using TPins = T;

    MNCp(PDesBase::TBcp& aBcp) : PState<TData>(aBcp) {}
    //virtual bool connectPins() = 0;
    void doTrans() override {
	if (mPins.connect(ciPair.data())) {
	    *PState<TData>::mUdata = { ciPair.data(), true };
	} else {
	    *PState<TData>::mUdata = { nullptr, false };
	}
    }
    PsIcp<TData> ciPair{this};
    TPins mPins;
};

#ifdef PCONN2_ENABLED

template <class TPif, class TRif = typename TPif::TPair>
struct MNCp2: public PSockOnp<TPif, TRif>, public MDesStateData<PDd<bool>>
{
    using TSelf = MNCp2<TPif, TRif>;
    using TParent = PSockOnp<TPif, TRif>;

    MNCp2(TPif* aPx): PSockOnp<TPif, TRif>(aPx) {}
    void notifyConnUpdated() {
	for (auto it = oConnected.leafsBegin(); it != oConnected.leafsEnd(); it++) {
	    (*it)->provided()->onInpUpdated();
	}
    }
    // From MDesStateData
    const PsOcp<bool>::TSData* sData() const override { return &mConnected;}
    // From PSockOnp
    bool attach(typename TParent::TPair* aPair) override {
	bool res = PSockOnp<TPif, TRif>::attach(aPair);
	if (res && !mConnected.mData) {
	    mConnected = {true, true};
	    notifyConnUpdated();
	}
	return res;
    }
    bool detach(typename TParent::TPair* aPair) override {
	bool res = PSockOnp<TPif, TRif>::detach(aPair);
	if (res && mConnected.mData) {
	    mConnected = {false, true};
	    notifyConnUpdated();
	}
	return res;
    }

    PsOcp<bool>::TSData mConnected{ false, true};
    PsOcp<bool> oConnected{this};
};

#else
/** @brief Native connpoint base, PState socket based
 * Output added to expose parent ptr
 * */
template <class TPif, class TRif = typename TPif::TPair>
struct MNCp2: public PSockOnp<TPif, TRif>, public MDesStateData<PDd<bool>>
{
    using TSelf = MNCp2<TPif, TRif>;
    using TParent = PSockOnp<TPif, TRif>;

    MNCp2(TPif* aPx): PSockOnp<TPif, TRif>(aPx) {}
    void notifyConnUpdated() {
	for (auto it = oConnected.leafsBegin(); it != oConnected.leafsEnd(); it++) {
	    (*it)->provided()->onInpUpdated();
	}
    }
    // From MDesStateData
    const PsOcp<bool>::TSData* sData() const override { return &mConnected;}
    // From PSockOnp
    bool attach(typename TParent::TPair* aPair) override {
	bool res = PSockOnp<TPif, TRif>::attach(aPair);
	if (res && !mConnected.mData) {
	    mConnected = {true, true};
	    notifyConnUpdated();
	}
	return res;
    }
    bool detach(typename TParent::TPair* aPair) override {
	bool res = PSockOnp<TPif, TRif>::detach(aPair);
	if (res && mConnected.mData) {
	    mConnected = {false, true};
	    notifyConnUpdated();
	}
	return res;
    }

    PsOcp<bool>::TSData mConnected{ false, true};
    PsOcp<bool> oConnected{this};
};

#endif // PCONN2_ENABLED




#endif


