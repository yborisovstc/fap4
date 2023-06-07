
#ifndef __NDEDS_MPDES_H
#define __NDEDS_MPDES_H

#include "miface.h"
#include "pconn2.h"


class MDesSyncable;

/** @brief Components Observer. Upper layer is observer of components 
 *
 * Components notify upper layer of status changes
* */
class MDesObserver: public MIface
{
    public:
	// From MIface
	virtual std::string Uid() const { return MDesObserver_Uid();}
	virtual std::string MDesObserver_Uid() const = 0;
	/** @brief Notification that component was activated */
	virtual void onActivated(MDesSyncable* aComp) = 0;
	/** @brief Notification that component was changed */
	virtual void onUpdated(MDesSyncable* aComp) = 0;
};

/** @brief Inputs Observer
 *
 * State is notified by its inputs of inputs update
 * */
class MDesInpObserver
{
    public:
	/** @brief Notification that input state was changed */
	virtual void onInpUpdated() = 0;
};

/** @brief State data
 * */
template <typename T>
class MDesStateData
{
    public:
	/** @brief Provides state's data */
//	virtual void getData(T& aData) = 0;
	virtual const T* sData() const = 0;
};


/** @brief Interface of DES syncable
 * */
// TODO do we need set* here, the mehods are for internal use
class MDesSyncable: public MIface
{
    public:
	// From MIface
	virtual std::string Uid() const { return MDesSyncable_Uid();}
	virtual std::string MDesSyncable_Uid() const = 0;
	void dump(int aLevel = 0xffff, int aIdt = 0, std::ostream& aOs = std::cout) const override { MDesSyncable_dump(aLevel, aIdt, aOs);}
	virtual void MDesSyncable_dump(int aLevel = 0xffff, int aIdt = 0, std::ostream& aOs = std::cout) const = 0;
	virtual void update() = 0;
	virtual void confirm() = 0;
//	virtual void setUpdated() = 0;
//	virtual void setActivated() = 0;
};

/** @brief Controlled interface for DES observer
 * */
class MCtdDesobs : public MIface
{
    public:
	// From MIface
	virtual std::string Uid() const { return std::string();}
	void dump(int aLevel = 0xffff, int aIdt = 0, std::ostream& aOs = std::cout) const override { }
	// Local
	virtual bool addSyncable(PCpOnp2<MDesSyncable, MDesObserver>* aSnc) = 0;
	virtual bool isSyncableIncluded(PCpOnp<MDesSyncable, MDesObserver>* aSnc) const = 0;
	virtual bool isSyncableIncluded(std::string aSncName) const = 0;
};

/** @brief Model launcher interface
 * */
class MPdesLauncher
{
    public:
	virtual bool Run(int aCount = 0, int aIdleCount = 0) = 0;
	virtual void Stop() = 0;
};



#endif
