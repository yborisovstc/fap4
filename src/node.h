
#ifndef __FAP4_NODE_H
#define __FAP4_NODE_H

#include <string>

#include "pdes.h"
#include "mowning.h"
#include "mnode.h"
#include "nconn.h"

#if 0
class Node: public PDes
{
    public:
	using TOwningReg = MNCp2<MNnOwning<>>; //!< Owning registry type
    public:
	/** @brief Owning registry representation */
	struct TOwningRegr {
	    bool operator ==(const TOwningRegr& aB) { return true;}
	    bool operator !=(const TOwningRegr& aB) { return false;}
	};
	/** @brief DES observer connpoint representation */
	struct TDobsCpRepr {
	    Node* mNewNode = nullptr;
	    bool operator ==(const TDobsCpRepr& aB) { return true;}
	    bool operator !=(const TDobsCpRepr& aB) { return false;}
	};
    public:
	Node(TBcp& aBcp, const std::string& aName);
	// From MDesSyncable
	std::string MDesSyncable_Uid() const override;
    public:
	//PState<std::string>::TOcp cpName;
	PState<std::string> sName;
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
	/** @brief State of DES observer connpoint
	 * @param Inp1 - ptr to DES observer CP (Sync registry) (see todo below)
	 * @param Inp2 - name of Node (primary DES syncable) to be created
	 * */
	/** TODO
	 * Bad design. This state is to be intended for Observer CP controlling (like SDC)
	 * but access to controlled data is implemented via input TBcp*. Considering redesign.
	 * Also Bcp input doesn't provide notification.
	 * */
	class TDobsCpState: public PState2<TDobsCpRepr, TBcp*, std::string> {
	    public:
		using TBase = PState2<TDobsCpRepr, TBcp*, std::string>;
		TDobsCpState(PDesBase::TBcp& aBcp, const std::string& aName = std::string()): TBase(aBcp, aName) {}
		void doTrans() override {
		    auto* d2 = Inp2.sData();
		    if (Inp1.valid() && Inp2.valid()) {
			if (!Inp2.data().empty()) {
			    Node* newNode = new Node(*Inp1.data(), Inp2.data());
			    if (newNode) {
				mUdata->mData.mNewNode = newNode;
				mUdata->mValid = true;
			    }
			}
		    }
		}
	};

	/** @brief State Owning registry
	 * Manages Owning registry
	 * @param Inp1 - ptr to Owning registry (see todo below)
	 * @param Inp2 - name of Owned to be created
	 * */
	/** TODO
	 * We need this state to have owning registry as a data instead of 
	 * having the registry as Node context and the state managing it.
	 * */
	class TOwningState: public PState3<TOwningRegr, TOwningReg*, std::string, TBcp*> {
	    public:
		using TBase = PState3<TOwningRegr, TOwningReg*, std::string, TBcp*>;
		TOwningState(PDesBase::TBcp& aBcp, const std::string& aName = std::string()): TBase(aBcp, aName) {}
		void doTrans() override {
		    if (Inp1.valid() && Inp2.valid() && Inp3.valid()) {
			if (!Inp2.data().empty()) {
			    Node* newNode = new Node(*Inp3.data(), Inp2.data());
			    if (Inp1.valid() && Inp1.data()) {
				auto* owr = Inp1.data();
				owr->connect(&newNode->mCpOwned3);
			    }
			}
		    }
		}
	};
	/** @brief Creates owned node
	 * */
	class SAddOwned: public PState1<Node*, std::string> {
	};
	// Interfaces proxy, needs to be placed after ifaces to keep right destruction order
	//MNnOwning<> mOwning2;
	//MNnOwning<>::TPair mOwned2;
    public:
	// Interfaces
	MNCp2<MNnOwning<>> mCpOwning3;       //!< MOwning connpoint
	MNCp2<MNnOwning<>::TPair> mCpOwned3; //!< MOwned connpoint
	MNCp2<MNnNode<>> mCpNode;       //!< MNode connpoint
    private:
	PState<GUri> mUri;
	PTransl2<GUri, GUri, std::string> mOwningUri;
	TDobsCpState sDobsCp; //!< DES observer connpoint state
	TOwningState sOwning; //!< Owning, managed mCpOwning3 connpoint
	PData<TOwningReg*> dCpOwning {&mCpOwning3, "dCpOwning"};
	PData<TBcp*> dBcp {&mBcp, "dBcp"};
    public: // Debug
	// Example of using trans w/o lambda
	TOwningUri mOwningUriTr;
	PTransl1<GUri, std::string> tOwnedId;
};
#endif

/** @brief Controlled interface for Node owning
 * */
class MCtdNodeOwning : public MIface
{
    public:
	// From MIface
	virtual std::string Uid() const { return std::string();}
	void dump(int aLevel = 0xffff, int aIdt = 0, std::ostream& aOs = std::cout) const override { }
	// Local
	virtual bool addOwned(MNCp2<MNnOwning<>::TPair>* aCpOwned) = 0;
	virtual bool isOwnedAttached(std::string aOwdName) const = 0;
};


/** @brief Controlled part of Node
 * Controlling approach is similar to what is done in FAP3 - particular iface is exposed via output
 * (in FAP3 it is exposed via DES adapter managed object), the notifications are issued via DES inp observer
 * (in FAP3 - via Node notification mechanism)
 * */
class NodeBase: public PDes
{
    public:
	/** @brief Controlled Node Owning embedded
	 * Gets PDES access to mCpOwning
	 * **/
	class CtdNodeOwning: public PsOcp<MCtdNodeOwning*>, public MCtdNodeOwning {
	    public:
		CtdNodeOwning(NodeBase* aHost): PsOcp<MCtdNodeOwning*>(&mCtdNodeOwningDt), mHost(aHost), mCtdNodeOwningDt(this, "CtdNodeOwning") {}
		void notifyChanged() {
		    for (auto it = leafsBegin(); it != leafsEnd(); it++) {
			(*it)->provided()->onInpUpdated();
		    }
		}
		// From MCtdNodeOwning
		virtual bool addOwned(MNCp2<MNnOwning<>::TPair>* aCpOwned) override {
		    bool res = false;
		    // Connect to Owning
		    res = mHost->mCpOwning.connect(aCpOwned);
		    if (res) {
			notifyChanged();
		    }
		    return res;
		}
		virtual bool isOwnedAttached(std::string aOwdName) const override { return false;}
	    protected:
		NodeBase* mHost;
		PData<MCtdNodeOwning*> mCtdNodeOwningDt;   //!< Controlled Node owning data
	};
	/** @brief Controlled Des Observer
	 * Gets PDES access to mBcp
	 * **/
	class CtdDesObs: public PsOcp<MCtdDesobs*>, public MCtdDesobs {
	    public:
		CtdDesObs(NodeBase* aHost): PsOcp<MCtdDesobs*>(&mCtdDt), mHost(aHost), mCtdDt(this, "CtdDesObs") {}
		void notifyChanged() {
		    for (auto it = leafsBegin(); it != leafsEnd(); it++) {
			(*it)->provided()->onInpUpdated();
		    }
		}
		// From MCtdDesobs
		virtual bool addSyncable(PCpOnp<MDesSyncable, MDesObserver>* aSnc) override {
		    bool res = false;
		    res = mHost->mBcp.connect(aSnc);
		    return res;
		}
		virtual bool isSyncableIncluded(PCpOnp<MDesSyncable, MDesObserver>* aSnc) const override {
		    return false;
		}
		virtual bool isSyncableIncluded(std::string aSncName) const override {
		    return false;
		}
	    protected:
		NodeBase* mHost;
		PData<MCtdDesobs*> mCtdDt;   //!< Controlled iface data
	};

    public:
	NodeBase(TBcp* aBcp, const std::string& aName);
	virtual ~NodeBase();
	// From MDesSyncable
	//std::string MDesSyncable_Uid() const override;
    public:
	// Interfaces
	MNCp2<MNnOwning<>> mCpOwning;       //!< MOwning connpoint
	CtdNodeOwning mCtdOwning;           //!< Controlled Node Owning
	CtdDesObs mCtdDesObs;               //!< Controlled Des observer
};


/** @brief Native hier Node. Controlling part.
 * */
class Node: public PDes
{

    public:
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
	/*
	class SdcAddSyncable: public PState2<bool, MCtdDesobs*, std::string> {
	    public:
		using TBase = PState2<bool, MCtdDesobs*, std::string>;
		SdcAddSyncable(PDesBase::TBcp& aBcp, const std::string& aName = std::string()): TBase(aBcp, aName) {}
		void doTrans() override {
		    if (Inp1.valid() && Inp2.valid() && Inp1.data()) {
			auto* ctdObs = Inp1.data();
			*mUdata = ctdObs->isSyncableIncluded(Inp2.data());
			if (!Inp2.data().empty()) {
			    Node* newNode = new Node(Inp2.data());
			    ctdObs->addSyncable(&(newNode->mScp));
			}
		    }
		}
	};
	*/

	/** @brief Controller of adding Owned
	 * */
	class SdcAddOwned: public PState3<bool, MCtdNodeOwning*, MCtdDesobs*, std::string> {
	    public:
		using TBase = PState3<bool, MCtdNodeOwning*, MCtdDesobs*, std::string>;
		SdcAddOwned(PDesBase::TBcp* aBcp, const std::string& aName = std::string()): TBase(aBcp, aName) {}
		void doTrans() override {
		    bool res = false;
		    res = Inp3.valid();
		    if (Inp1.valid() && Inp2.valid() && Inp3.valid() && !Inp3.data().empty()) {
			auto* ctdOwd = Inp1.data();
			*mUdata = ctdOwd->isOwnedAttached(Inp3.data());
			if (!mUdata->mData) {
			    // Create new node
			    Node* newNode = new Node(Inp3.data());
			    // Connect new node to Owning
			    res = ctdOwd->addOwned(&(newNode->mCpOwned));
			    if (res) {
				auto* ctdObs = Inp2.data();
				// Connect new node to PDES observer
				res = ctdObs->addSyncable(&newNode->mScp);
			    }
			}
		    }
		}
	};

    public:
	Node(const std::string& aName);
	Node(TBcp* aBcp, const std::string& aName);
	virtual ~Node();
	// From MDesSyncable
	//std::string MDesSyncable_Uid() const override;
    public:
	NodeBase mBase;
	PState<std::string> sName;
	TOwningUri trOwningUri;             //!< Transition "Owning URI"
	// Interfaces
	MNCp2<MNnOwning<>::TPair> mCpOwned; //!< MOwned connpoint
	//MNCp2<MNnOwning<>> mCpOwning;       //!< MOwning connpoint, one-to-many
    protected:
	//PSdc1<NodeBase, std::string> cAddOwned;       //!< Controller "Add Owned"
	SdcAddOwned cAddOwned;       //!< Controller "Add Owned"
};



#endif


