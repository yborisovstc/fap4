
#include <thread>

#include "pdes.h"

PDesBase::PDesBase(const char* aName) : mScp(this), mUpdNotified(false), mActNotified(false), mName(aName)
{
}

PDesBase::PDesBase(TBcp& aBcp, const char* aName) : mScp(this), mName(aName)
{
    mScp.connect(&aBcp);
    setActivated();
}

std::string PDesBase::MDesSyncable_Uid() const
{
    if (mScp.leafsCbegin() != mScp.leafsCend()) {
	const MDesObserver* obs = (*mScp.leafsCbegin())->provided();
	return obs->Uid() + "." + mName;
    } else {
	return mName;
    }
}

void PDesBase::setActivated()
{
    if (!mActNotified) {
	// Propagate activation to owner
	if (mScp.pairsBegin() != mScp.pairsEnd()) {
	    auto rq = (*mScp.pairsBegin())->provided();
	    rq->onActivated(this);
	    mActNotified = true;
	}
    }
}

void PDesBase::setUpdated()
{
    if (!mUpdNotified) {
	// Propagate update to owner
	if (mScp.pairsBegin() != mScp.pairsEnd()) {
	    auto rq = (*mScp.pairsBegin())->provided();
	    rq->onUpdated(this);
	    mUpdNotified = true;
	}
    }
}



void PStateBase::update() {
    mActNotified = false;
    doTrans();
    if (isChanged()) {
	setUpdated();
    }
}





PDes::PDes(const char* aName): PDesBase(aName), mBcp(this)
{
}

PDes::PDes(TBcp& aBcp, const char* aName): PDesBase(aBcp, aName), mBcp(this)
{
}

void PDes::update()
{
    // TODO ?? mUpdNotified = false;
    mActNotified = false;
    while (!mActive.empty()) {
	auto comp = mActive.front();
	try {
	    comp->update();
	} catch (std::exception e) {
	}
	mActive.pop_front();
    }
}

void PDes::confirm()
{
    mUpdNotified = false;
    while (!mUpdated.empty()) {
	auto comp = mUpdated.front();
	comp->confirm();
	mUpdated.pop_front();
    }
}

void PDes::onActivated(MDesSyncable* aComp)
{
    setActivated();
    if (aComp) {
#ifdef DES_RGS_VERIFY
	for (auto it = mActive.begin(); it !=  mActive.end(); it++) {
	    assert(aComp != *it);
	}
#endif
	mActive.push_back(aComp);
    }
}

void PDes::onUpdated(MDesSyncable* aComp)
{
    setUpdated();
    if (aComp) {
#ifdef DES_RGS_VERIFY
	for (auto it = mUpdated.begin(); it !=  mUpdated.end(); it++) {
	    assert(aComp != *it);
	}
#endif
	mUpdated.push_back(aComp);
    }
}




bool PDesLauncher::Run(int aCount, int aIdleCount)
{
    bool res = true;
    int cnt = 0;
    int idlecnt = 0;
    while (!mStop && (aCount == 0 || cnt < aCount) && (aIdleCount == 0 || idlecnt < aIdleCount)) {
	if (!mActive.empty()) {
	    //Log(TLog(EInfo, this) + ">>> Update [" + to_string(cnt) + "]");
	    update();
	    if (!mUpdated.empty()) {
		//Log(TLog(EInfo, this) + ">>> Confirm [" + to_string(cnt) + "]");
		//outputCounter(cnt);
		confirm();
	    }
	    cnt++;
	} else {
	    OnIdle();
	    idlecnt++;
	}
    }
    return res;
}


void PDesLauncher::Stop()
{
    mStop = true;
}

void PDesLauncher::OnIdle()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //mStop = true;
}


