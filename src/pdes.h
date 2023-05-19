

#ifndef __FAP4_PDES_H
#define __FAP4_PDES_H

#include <functional>
#include <utility>
#include <list>

#include "mpdes.h"
#include "pconn2.h"


/** @brief Primary DEDS base
 * */
class PDesBase : public MDesSyncable
{
    public:
	using TScp = PCpOnp<MDesSyncable, MDesObserver>; //!< DES Syncable CP type
	using TBcp = TScp::TPair;                        //!< DES Observer CP type
    public:
	PDesBase(const std::string& aName = std::string());
	PDesBase(TBcp* aBcp, const std::string& aName = std::string());
	~PDesBase() = default;
	// From MDesSyncable
	std::string MDesSyncable_Uid() const override;
	void MDesSyncable_dump(int aLevel = 0xffff, int aIdt = 0, std::ostream& aOs = std::cout) const override;
    protected:
	void setUpdated();
	void setActivated();
    public:
	TScp mScp;  //!< Syncable CP
    protected:
	bool mUpdNotified;               //<! Sign of that State notified observers on Update
	bool mActNotified;               //<! Sign of that State notified observers on Activation
	std::string mName;
};


/** @brief State base: inputs observer, transition
 * */
class PStateBase : public PDesBase, public MDesInpObserver
{
    public:
	PStateBase(const std::string& aName = std::string()): PDesBase(aName) {}
	PStateBase(TBcp* aBcp, const std::string& aName = std::string()): PDesBase(aBcp, aName) {}
    public:
	// From MDesInpObserver
	virtual void onInpUpdated() override {
	    setActivated();
	}
    protected:
	/** @brief Perform state transition
	 * @return sing of state data changed in result of the transition
	 * */
	virtual void doTrans() {}
	virtual bool isChanged() { return false;}
	// From MDesSyncable
	virtual void update() override;
};


/** @brief Primary DEDS data
 * @param  mData    the data itself
 * @param  mValid   the indication of data is valild
 * */
template <typename T>
struct PDd
{
    T mData;
    bool mValid;
    PDd() : mValid(false) {}
    PDd(const PDd& aSrc): mData(aSrc.mData), mValid(aSrc.mValid) {}
    PDd(const T& aData, bool aValid = true): mData(aData), mValid(aValid) {}
    bool operator ==(const PDd<T>& s) { return mValid && s.mValid && (mData == s.mData) || !mValid && !s.mValid;}
    bool operator !=(const PDd<T>& s) { return mValid && s.mValid && (mData != s.mData) || mValid != s.mValid;}
};


/** @brief Primary DEDS state output CP iface type
 * */
template <typename T> using MPsOutp = PCpOmnp<MDesStateData<PDd<T>>, MDesInpObserver>;

/** @brief Primary DEDS state input CP iface type
 * */
template <typename T> using MPsInp = PCpOnp<MDesInpObserver, MDesStateData<PDd<T>>>;


// TODO Needs to introdice the notification of inp observer of CP connect/disconnect
/** @brief Primary DEDS State input CP
 * */
template <typename TInp>
class PsIcp : public PCpOnp<MDesInpObserver, MDesStateData<PDd<TInp>>> {
    public:
	template <typename T>
	    using TIcp = PCpOnp<MDesInpObserver, MDesStateData<PDd<T>>>;
	using TParent = PCpOnp<MDesInpObserver, MDesStateData<PDd<TInp>>>;
	using TRequired = MDesStateData<PDd<TInp>>;

	PsIcp(MDesInpObserver* aPx) : TIcp<TInp>(aPx) {}
	const PDd<TInp>* sData() const {
	    const PDd<TInp>* res = nullptr;
	    auto lb = TParent::leafsCbegin();
	    if (lb != TParent::leafsCend()) {
		auto lbv = *lb;
		auto* prov = lbv->provided();
		// TODO We keep the approach to traversal all the leafs of the tree even
		// those that doesn't have iface provider. To consider the alternative
		// approach - to traversal thru leafs provideng ifaces only.
		res = prov ? prov->sData() : nullptr;
	    }
	    return res;
	}
	const TInp& data() const { return sData()->mData; }
	bool valid() const { return sData() ? sData()->mValid : false; }
	operator const TInp&() const { return sData()->mData;}
};

/** @brief Primary DEDS State output CP
 * */
template <typename TData>
class PsOcp : public PCpOmnp<MDesStateData<PDd<TData>>, MDesInpObserver> {
    public:
	using TSData = PDd<TData>;  //!< Type of state data
	template <typename T>
	    using TOcp = PCpOmnp<MDesStateData<PDd<T>>, MDesInpObserver>;

	PsOcp(MDesStateData<TSData>* aPx) : TOcp<TData>(aPx) {}
	const PDd<TData>* sData() const {
	    auto dp = PsOcp::provided();
	    return dp ? dp->sData() : nullptr;
	}
	const TData& data() const { return sData()->mData; }
	bool valid() const { return sData() ? sData()->mValid : false; }
	operator const TData&() const { return sData()->mData;}
};


#ifdef PCONN2_ENABLED

template <class TData>
using PsIex = PExd<MDesInpObserver, MDesStateData<PDd<TData>>>;

template <class TData>
using PsOex = PExd<MDesStateData<PDd<TData>>, MDesInpObserver>;

#else
/** @brief Primary state input extender
 * */
template <class TData>
class PsIex : public PsIcp<TData>, MDesInpObserver, MDesStateData<PDd<TData>>
{
    public:
	using TScp = PsIcp<TData>;  /*!< Self connpoint type */
	using TPair = typename TScp::TPair;
	using TCnode = PsOcp<TData>; /*!< Complement node (pole) type */
    public:
	class Cnode: public TCnode {
	    public:
		Cnode(PsIex* aHost): TCnode(aHost), mHost(aHost) {}
		// From MNcpp
		virtual typename TCnode::TPair* binded() override { return mHost;}
	    private:
		PsIex* mHost;
	};
    public:
	PsIex() : PsIcp<TData>(this), mCnode(this) {}
	// From MDesInpObserver
	void onInpUpdated() override {}
	// From MDesStateData
	const PDd<TData>* sData() const override { return nullptr;}
	// From MNcpp
	virtual typename TScp::TPair* binded() override { return &mCnode;}
	virtual bool disconnect() override {
	    bool res = TScp::disconnect();
	    res = res && mCnode.disconnect();
	    return res;
	}
    protected:
	Cnode mCnode;
};



/** @brief Primary state output extender
 * */
template <class TData>
class PsOex : public PsOcp<TData>, MDesInpObserver, MDesStateData<PDd<TData>>
{
    public:
	using TScp = PsOcp<TData>;  /*!< Self connpoint type */
	using TPair = typename TScp::TPair;
	using TCnode = PsIcp<TData>; /*!< Complement node (pole) type */
    public:
	class Cnode: public TCnode {
	    public:
		Cnode(PsOex* aHost): TCnode(aHost), mHost(aHost) {}
		// From MNcpp
		virtual typename TCnode::TPair* binded() override { return mHost;}
	    private:
		PsOex* mHost;
	};
    public:
	PsOex() : PsOcp<TData>(this), mCnode(this) {}
	// From MDesInpObserver
	void onInpUpdated() override {}
	// From MDesStateData
	const PDd<TData>* sData() const override { return nullptr;}
	// From MNcpp
	virtual typename TScp::TPair* binded() override { return &mCnode;}
	virtual bool disconnect() override {
	    bool res = TScp::disconnect();
	    res = res && mCnode.disconnect();
	    return res;
	}
    protected:
	Cnode mCnode;
};

#endif // PCONN2_ENABLED



// EXPR#Using_tag

struct tag_outp {};
struct tag_inp {};

template <typename T, typename K> class MPsCp;
//template <typename T> class MPsCp<T, tag_outp> : public PsOcp<T> {};
//template <typename T> class MPsCp<T, tag_inp> : public PsIcp<T> {};
template <typename T> struct MPsCp<T, tag_outp> { using Tcp = PsOcp<T>; };
template <typename T> struct MPsCp<T, tag_inp> { using Tcp =  PsIcp<T>; };

template <typename T, typename K> class MPsEx;
template <typename T> struct MPsEx<T, tag_outp> { using Tcp = PsOex<T>; };
template <typename T> struct MPsEx<T, tag_inp> { using Tcp =  PsIex<T>; };

// END EXPR#Using_tag




/** @brief Primary layer. Transition.
 * It is usable if system exposes not state but just transition over its boundary
 * */
template <typename T>
class PTrans : public MDesInpObserver, public MDesStateData<PDd<T>>
{
    public:
    using TData = T;        //!< Type of data
    using TSData = PDd<TData>;  //!< Type of state data

    virtual ~PTrans() = default;
    explicit PTrans(): MDesStateData<PDd<T>>(), mOcp(this) {}
    // From MDesStateData
    virtual const TSData* sData() const { const_cast<PTrans*>(this)->doTrans(); return &mRes;}
    // From MDesInpObserver
    virtual void onInpUpdated() override { }
    // Local
    virtual void doTrans() = 0;
    public:
    PsOcp<TData> mOcp;     //!< Output connpoint
    TSData mRes;           //!< Result
};


/** @brief State in DEDS of primary level
 * Combines state itself and transition
 * @param DataType  type of state data
 * */
template <typename T>
class PState : public PStateBase, public MDesStateData<PDd<T>>
{
    public:
//	template <typename TInp>
//	    using TIcp = PCpOnp<MDesInpObserver, MDesStateData<PDd<TInp>>>;

    using TData = T;        //!< Type of data
    using TSData = PDd<TData>;  //!< Type of state data
    //using TOcp = PCpOmnp<MDesStateData<TSData>, MDesInpObserver>; //!< Type of output connpoint

    public:
	virtual ~PState() = default;
	explicit PState(const std::string& aName = std::string());
	explicit PState(TBcp* aBcp, const std::string& aName = std::string());
	PState(const PState& aSrc);
	PState(TBcp* aBcp, const TData& aData, const std::string& aName );

	void set(const TData& aData) {
	    mUdata->mData = aData; mUdata->mValid = true;
	    mCdata->mData = aData; mCdata->mValid = true;
	    setActivated();
	}

	virtual void confirm() override {
	    mUpdNotified = false;
	    // Swap the data and notify observer
	    // TODO Notify on data changed only
	    std::swap(mCdata, mUdata);
	    notifyInpUpdated();
	}

	// From MDesStateData
	virtual const TSData* sData() const { return mCdata;}
	// From PStateBase
	virtual bool isChanged() { return *mUdata != *mCdata; }

    protected:
	void notifyInpUpdated() {
	    for (auto it = mOcp.leafsBegin(); it != mOcp.leafsEnd(); it++) {
		(*it)->provided()->onInpUpdated();
	    }
	}
    public:
	PsOcp<TData> mOcp;     //!< Output connpoint
    protected:
	TSData  mDataP, mDataQ;
	TSData* mUdata;  //!< State's updated data
	TSData* mCdata; //!< State's confirmed data
};

template <typename TData>
PState<TData>::PState(const std::string& aName): PStateBase(aName), mUdata(&mDataP), mCdata(&mDataQ), mOcp(this)
{
}

template <typename TData>
PState<TData>::PState(TBcp* aBcp,const std::string& aName): PStateBase(aBcp, aName), mUdata(&mDataP), mCdata(&mDataQ), mOcp(this)
{
}

template <typename TData>
PState<TData>::PState(const PState& aSrc): PState()
{
    mUdata = aSrc.mUdata;
    mCdata = aSrc.mCdata;
}

template <typename TData>
PState<TData>::PState(TBcp* aBcp, const TData& aData,const std::string& aName): PState(aBcp, aName)
{
    set(aData);
}

/*
template <typename TData>
PState<TData>::PState(PState&& aSrc): mUdata(aSrc.mUdata), mCdata(aSrc.mCdata)
{
}
*/

/** @brief Data in DEDS of primary level
 * Doesn't participate in DEDS update process but just keeps a data
 * @param T  type of state data
 * */
template <typename T>
class PData : public MDesStateData<PDd<T>>
{
    public:
	using TData = T;        //!< Type of data
	using TSData = PDd<TData>;  //!< Type of state data

    public:
	virtual ~PData() = default;
	explicit PData(const std::string& aName = std::string()): mName(aName), mOcp(this) {}
	explicit PData(const TData& aData, const std::string& aName): PData(aName) {
	    set(aData);
	}

	void set(const TData& aData) {
	    mData.mData = aData; mData.mValid = true;
	}
	// From MDesStateData
	virtual const TSData* sData() const { return &mData;}
    public:
	PsOcp<TData> mOcp;     //!< Output connpoint
    protected:
	std::string mName;
	TSData  mData;
};




/** @brief Primary discrete events dynasystem
 * */
class PDes : public PDesBase, public MDesObserver
{
    public:
	using TObsCp = PCpOmnp<MDesObserver, MDesSyncable>; //!< DES Oblserver CP type
    public:
	PDes(const std::string& aName = std::string());
	PDes(TBcp* aBcp, const std::string& aName = std::string());
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
	// From MDesObserver
	std::string MDesObserver_Uid() const override { return MDesSyncable::Uid();}
	void MDesSyncable_dump(int aLevel = 0xffff, int aIdt = 0, std::ostream& aOs = std::cout) const override;
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
	// Local
    protected:
	/** @brief Adds syncable */
	void addSnc(PDesBase& aSnc) {
	    mBcp.connect(&aSnc.mScp);
	}
    protected:
	std::list<MDesSyncable*> mActive;     /*!< Active compoments */
	std::list<MDesSyncable*> mUpdated;     /*!< Updated compoments */
	TObsCp mBcp;  //!< DES Observer CP
};

/** @brief Launcher of DES
 * Runs DES syncable owned
 * */
class PDesLauncher: public PDes, public MPdesLauncher
{
    public:
	PDesLauncher(const std::string& aName = std::string()): PDes(aName), mStop(false) {}

	// From MPDesLauncher
	virtual bool Run(int aCount = 0, int aIdleCount = 0) override;
	virtual void Stop() override;
	// Local
	virtual void OnIdle();
    protected:
	int mCounter = 0;
	bool mStop;
};

using TTrans = void (*)();
using TTransf = std::function<void ()>;

template <typename TData, typename TInp1, typename TInp2, TTrans trans>
class Pst2 : public PState<TData>
{
    public:
	virtual void doTrans() { trans;}
	PsIcp<TInp1> Inp1;
	PsIcp<TInp2> Inp2;
};

template <typename TData, typename TInp1>
class PState1 : public PState<TData>
{
    public:
	PState1(const std::string& aName = std::string()): PState<TData>(aName), Inp1(this) {}
	PState1(PDesBase::TBcp* aBcp, const std::string& aName = std::string()): PState<TData>(aBcp, aName), Inp1(this) {}
	PState1(PDesBase::TBcp* aBcp, const TData& aData): PState<TData>(aBcp, aData), Inp1(this) {}
	PsIcp<TInp1> Inp1;
};



template <typename TData, typename TInp1, typename TInp2>
class PState2 : public PState<TData>
{
    public:
	PState2(): PState<TData>(), Inp1(this), Inp2(this) {}
	PState2(PDesBase::TBcp* aBcp, const std::string& aName = std::string()): PState<TData>(aBcp, aName), Inp1(this), Inp2(this) {}
	PsIcp<TInp1> Inp1;
	PsIcp<TInp2> Inp2;
};

template <typename TData, typename TInp1, typename TInp2, typename TInp3>
class PState3 : public PState<TData>
{
    public:
	PState3(PDesBase::TBcp* aBcp, const std::string& aName = std::string()): PState<TData>(aBcp, aName), Inp1(this), Inp2(this), Inp3(this) {}
	PsIcp<TInp1> Inp1;
	PsIcp<TInp2> Inp2;
	PsIcp<TInp3> Inp3;
};


// TODO Using parameter pack?
template <typename TData, typename... TInps>
class PST : public PState<TData>
{
};


template <typename TData, typename TInp1, typename TInp2>
class PTrans2 : public PTrans<TData>
{
    public:
	PTrans2(): PTrans<TData>(), Inp1(this), Inp2(this) {}
	PsIcp<TInp1> Inp1;
	PsIcp<TInp2> Inp2;
};

template <typename TData, typename TInp1, typename TInp2, typename TInp3>
class PTrans3 : public PTrans<TData>
{
    public:
	PTrans3(): PTrans<TData>(), Inp1(this), Inp2(this), Inp3(this) {}
	PsIcp<TInp1> Inp1;
	PsIcp<TInp2> Inp2;
	PsIcp<TInp3> Inp3;
};


template <typename TData, typename TInp1>
class PTransl1 : public PTrans<TData>
{
    public:
	PTransl1(TTransf aTrans): PTrans<TData>(), mTrans(aTrans), Inp1(this) {}
	virtual void doTrans() { mTrans();}
	TTransf mTrans;
	PsIcp<TInp1> Inp1;
};



template <typename TData, typename TInp1, typename TInp2>
class PTransl2 : public PTrans<TData>
{
    public:
	PTransl2(TTransf aTrans): PTrans<TData>(), mTrans(aTrans), Inp1(this), Inp2(this) {}
	//PTrans2(PDesBase::TBcp& aBcp): PTrans<TData>(aBcp), Inp1(this), Inp2(this) {}
	virtual void doTrans() { mTrans();}
	TTransf mTrans;
	PsIcp<TInp1> Inp1;
	PsIcp<TInp2> Inp2;
};


/** @brief System dynamic controller (SDC) base.
 * The key element of layered system control (LSC)
 * Performs given controlled system target modification
 * Outputs target query
 * */
class PSdcBase : public PDesBase, public MDesInpObserver, public MDesStateData<PDd<bool>>
{
    public:
	PSdcBase(const std::string& aName = std::string()): PDesBase(aName), mOutp(this), mInpEnb(this) {}
	PSdcBase(TBcp* aBcp, const std::string& aName = std::string()): PDesBase(aBcp, aName), mOutp(this), mInpEnb(this) {}
    public:
	// From MDesInpObserver
	virtual void onInpUpdated() override {
	    setActivated();
	}
	// From MDesStateData
	virtual const PDd<bool>* sData() const {
	    bool st = false;
	    getStatus();
	    return &mStatus;
	}
    protected:
	/** @brief Perform state transition
	 * @return sing of success
	 * */
	virtual bool doCtl() { return false;}
	/** @brief Gets target query result
	 * @return sing of success
	 * */
	virtual bool getStatus() const {return false;}
	// From MDesSyncable
	virtual void update() override {
	}
	virtual void confirm() override {}
    public:
	PDd<bool> mStatus;
	PsOcp<bool> mOutp;  //<! Input "Enable controlling"
	PsIcp<bool> mInpEnb;  //<! Input "Enable controlling"
};

/** @brief System dynamic controller (SDC) base.
 * */
template <typename T>
class PSdc : public PSdcBase
{
    public:
	using TControlled = T;        //!< Type of controlled system
    public:
	PSdc(TControlled* aCtd, const std::string& aName = std::string()): PSdcBase(aName), mCtd(aCtd) {}
	PSdc(PDesBase::TBcp* aBcp, TControlled* aCtd, const std::string& aName = std::string()): PSdcBase(aBcp, aName), mCtd(aCtd) {}
    protected:
	TControlled* mCtd;    //<! Controlled system
};

template <typename T, typename TInp1>
class PSdc1 : public PSdc<T>
{
    public:
	PSdc1(T* aCtd, const std::string& aName = std::string()): PSdc<T>(aCtd, aName), Inp1(this) {}
	PSdc1(PDesBase::TBcp* aBcp, T* aCtd, const std::string& aName = std::string()): PSdc<T>(aBcp, aCtd, aName), Inp1(this) {}
	PsIcp<TInp1> Inp1;
};

#endif
