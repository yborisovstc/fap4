

#ifndef __NDEDS_STATE_H
#define __NDEDS_STATE_H

#include <utility>
#include <list>

#include "mpdes.h"
#include "pconn.h"

class PDesBase : public MDesSyncable
{
    public:
	using TScp = PCpOnp<MDesSyncable, MDesObserver>; //!< Syncable CP type
	using TBcp = TScp::TPair;                        //!< Observable CP type
    public:
	PDesBase();
	PDesBase(TBcp& aBcp);
	~PDesBase() = default;
    protected:
	void setUpdated();
	void setActivated();
    protected:
	TScp mScp;  //!< Syncable CP
	bool mUpdNotified;               //<! Sign of that State notified observers on Update
	bool mActNotified;               //<! Sign of that State notified observers on Activation
};

/** @brief State base: inputs observer, transition
 * */
class PStateBase : public PDesBase, public MDesInpObserver
{
    public:
	PStateBase(TBcp& aBcp): PDesBase(aBcp) {}
    public:
	// From MDesInpObserver
	virtual void onInpUpdated() override {
	    setActivated();
	}
    protected:
	/** @brief Perform state transition
	 * @return sing of state data changed in result of the transition
	 * */
	virtual bool doTrans() { return false;}
	// From MDesSyncable
	virtual void update() override;
};

/** @brief State in DEDS of primary level
 * Combines state itself and transition
 * @param DataType  type of state data
 * */
template <typename T>
class PState : public PStateBase, public MDesStateData<T>
{
    public:
	template <typename TInp>
	    using TIcp = PCpOnp<MDesInpObserver, MDesStateData<TInp>>;

    using TData = T;
    using TOcp = PCpOmnp<MDesStateData<TData>, MDesInpObserver>; //!< Type of output connpoint

    protected:
        template <typename TInp>
        class Icp : public TIcp<TInp> {
	    public:
		Icp(MDesInpObserver* aPx) : TIcp<T>(aPx) {}
		void getData(TInp& aData) {
		    if (auto rq = TIcp<TInp>::required()) {
			rq->getData(aData);
		    }
		}
	};

    public:
	virtual ~PState() = default;
	explicit PState(TBcp& aBcp);
	PState(const PState& aSrc);
	PState(TBcp& aBcp, const TData& aData);
	
	void set(const TData& aData) {
	    *mUdata = aData;
	    *mCdata = aData;
	    setActivated();
	}

	virtual void confirm() override {
	    mUpdNotified = false;
	    // Swap the data and notify observer
	    std::swap(mCdata, mUdata);
	    notifyInpUpdated();
	}
	
	// From MDesStateData
	virtual void getData(TData& aData) { aData = *mCdata;}
	// From MDesInpObserver
	//virtual void onInpUpdated() override { setActivated();}
    protected:
	void notifyInpUpdated() {
	    for (auto p : mOcp) {
		p->provided()->onInpUpdated();
	    }
	}
    public:
	TOcp mOcp;     //!< Output connpoint
    protected:
	TData  mDataP, mDataQ;
	TData* mUdata;  //!< State's updated data
	TData* mCdata; //!< State's confirmed data
};


template <typename TData>
PState<TData>::PState(TBcp& aBcp): PStateBase(aBcp), mUdata(&mDataP), mCdata(&mDataQ), mOcp(this)
{
}

template <typename TData>
PState<TData>::PState(const PState& aSrc): PState()
{
    mUdata = aSrc.mUdata;
    mCdata = aSrc.mCdata;
}

template <typename TData>
PState<TData>::PState(TBcp& aBcp, const TData& aData): PState(aBcp)
{
    set(aData);
}

/*
template <typename TData>
PState<TData>::PState(PState&& aSrc): mUdata(aSrc.mUdata), mCdata(aSrc.mCdata)
{
}
*/



/** @brief Primary discrete events dynasystem
 * */
class PDes : public PDesBase, public MDesObserver
{
    public:
	using TObsCp = PCpOmnp<MDesObserver, MDesSyncable>; //!< Oblservable CP type
    public:
	PDes();
	PDes(TBcp& aBcp);
	// From MDesSyncable
	virtual void update() override;
	virtual void confirm() override;
	// From MDesObserver
	virtual void onActivated(MDesSyncable* aComp) override;
	virtual void onUpdated(MDesSyncable* aComp) override;
    protected:
	std::list<MDesSyncable*> mActive;     /*!< Active compoments */
	std::list<MDesSyncable*> mUpdated;     /*!< Updated compoments */
	TObsCp mBcp;  //!< Observable CP
};

/** @brief Launcher of DES
 * Runs DES syncable owned
 * */
class PDesLauncher: public PDes, public MPdesLauncher
{
    public:
	PDesLauncher(): PDes(), mStop(false) {}

	// From MPDesLauncher
	virtual bool Run(int aCount = 0, int aIdleCount = 0) override;
	virtual void Stop() override;
	// Local
	virtual void OnIdle();
    protected:
	int mCounter = 0;
	bool mStop;
};


#endif
