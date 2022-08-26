
#ifndef __NDEDS_MPDES_H
#define __NDEDS_MPDES_H


class MDesSyncable;

/** @brief Components Observer. Upper layer is observer of components 
 *
 * Components notify upper layer of status changes
* */
class MDesObserver
{
    public:
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
	virtual void getData(T& aData) = 0;
};


/** @brief Interface of DES syncable
 * */
// TODO do we need set* here, the mehods are for internal use
class MDesSyncable
{
    public:
	virtual void update() = 0;
	virtual void confirm() = 0;
//	virtual void setUpdated() = 0;
//	virtual void setActivated() = 0;
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
