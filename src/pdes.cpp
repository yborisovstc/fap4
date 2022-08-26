
#include <thread>

#include "pdes.h"

PDesBase::PDesBase() : mScp(this), mUpdNotified(false), mActNotified(false)
{
}

PDesBase::PDesBase(TBcp& aBcp) : mScp(this)
{
    mScp.connect(&aBcp);
    setActivated();
}

void PDesBase::setActivated()
{
    if (!mActNotified) {
	// Propagate activation to owner
	if (auto rq = mScp.required()) {
	    rq->onActivated(this);
	    mActNotified = true;
	}
    }
}

void PDesBase::setUpdated()
{
    if (!mUpdNotified) {
	// Propagate update to owner
	if (auto rq = mScp.required()) {
	    rq->onUpdated(this);
	    mUpdNotified = true;
	}
    }
}



void PStateBase::update() {
    mActNotified = false;
    if (doTrans()) {
	setUpdated();
    }
}





PDes::PDes(): PDesBase(), mBcp(this)
{
}

PDes::PDes(TBcp& aBcp): PDesBase(aBcp), mBcp(this)
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


