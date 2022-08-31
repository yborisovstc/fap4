
#ifndef __FAP4_MNODE_H
#define __FAP4_MNODE_H

#include "pdes.h"
#include "guri.h"

/** @brief Native heirarchy node abstraction
 * Manages owner-owned relations, is changeable (non-persistenly mutable)
 * */
struct MNode : public MIface
{
	static const char* Type() { return "MNode";};
	MNode(MPsOutp<GUri>& aUri): mUri(aUri) {}
	///MNode(PsOcp<GUri>& aUri): mUri(aUri) {}
	MPsOutp<GUri>& mUri;   //!< Node URI
	///PsOcp<GUri>& mUri;   //!< Node URI
};


#endif
