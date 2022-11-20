
#ifndef __FAP4_NODE_H
#define __FAP4_NODE_H

#include <string>

#include "pdes.h"
#include "mowning.h"
#include "mnode.h"
#include "nconn.h"


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
	 * @param Inp2 - name of Owned to be created
	 * */
	/** TODO
	 * We need this state to have owning the registry as a data instead of 
	 * having the registry as Node context and the state managing it.
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


