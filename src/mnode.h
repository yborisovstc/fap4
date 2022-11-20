
#ifndef __FAP4_MNODE_H
#define __FAP4_MNODE_H

#include "pdes.h"
#include "guri.h"

/** @brief Native heirarchy node abstraction
 * Manages owner-owned relations, is changeable (non-persistenly mutable)
 * */
template <typename P = tag_outp, typename Q = tag_inp>
struct MNnNode {
    using TPair = MNnNode<Q,P>;
    bool attach(TPair& aPair) {
	bool res = true;
	res &= mNewNodeId.attach(&aPair.mNewNodeId);
	return res;
    }
    bool detach(TPair& aPair) {
	bool res = true;
	res &= mNewNodeId.detach(&aPair.mNewNodeId);
	return res;
    }

    typename MPsEx<std::string, Q>::Tcp mNewNodeId; //!< Temporary. To simulate mutation "add"
};



#endif
