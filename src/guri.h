#ifndef __FAP4_GURI_H
#define __FAP4_GURI_H

#include <string>
#include <vector>



/** @brief Generic URI
 * Uses owner-owned relation chain for identification
 * */
class GUri
{
    public:
	using TElem = std::string;
	using TElems = std::vector<TElem>;
    public:
	GUri(): mValid(false) {}
	GUri(const GUri& aSrc): mElems(aSrc.mElems), mValid(aSrc.mValid) {}
	GUri(const std::string& aSrc) { parse(aSrc);}
	int size() const { return mElems.size();}
	void clear() { mElems.clear();}
	const std::string& at(int aIdx) const { return mElems.at(aIdx);}
	GUri head(int aIdx) const;
	/** @brief Tail formed by elems from aIdx */
	GUri tail(int aIdx) const;
	/** @brief Tail formed by aNum tail elemement */
	GUri tailn(int aNum) const;
	void tail(const GUri& aHead, GUri& aTail) const;
	bool isHead(const GUri& aHead) const;
	bool isTail(const GUri& aTail) const;
	bool getHead(const GUri& aTail, GUri& aHead) const;
	bool getTail(const GUri& aHead, GUri& aTail) const;
	//const vector<TElem>& elems() const {return mElems;}; // TODO to introduce iters instead
	void append(const GUri& aUri);
	void prepend(const GUri& aUri);
	void appendElem(const TElem& aElem);
	void prependElem(const TElem& aElem);
	bool operator==(const GUri& aSrc) const;
	bool operator!=(const GUri& aSrc) const { return !operator ==(aSrc);}
	bool operator<(const GUri& aSrc) const;
	bool operator<=(const GUri& aSrc) const { return *this < aSrc || *this == aSrc;}
	bool operator>(const GUri& aSrc) const { return aSrc < *this;}
	bool operator>=(const GUri& aSrc) const { return aSrc <= *this;}
	GUri& operator+=(const GUri& aSrc) { append(aSrc); return *this;}
	GUri operator+(const GUri& aSrc) const { GUri res(*this); res.append(aSrc); return res;}
	GUri& operator=(const GUri& aSrc) { this->mElems = aSrc.mElems; this->mValid = aSrc.mValid; return *this;}
	operator std::string() const { return toString();}
	std::string toString() const;
	bool isAbsolute() const;
	bool isValid() const { return mValid;}
	bool isName() const;
    public:
	static const std::string K_Self;
    private:
	void parse(const std::string& aSrc);
    private:
	TElems mElems;
	bool mValid;
};

#endif
