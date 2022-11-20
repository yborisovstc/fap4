#ifndef __FAP4_MIFACE_H
#define __FAP4_MIFACE_H

#include <string>
#include <iostream>


/** @brief Interface base (primary DEDS)
 * Any interface has to include type static attr const char* mType
 * */
struct MIface
{
    /** @brief Getting id unique in the scope of the env */
    virtual std::string Uid() const = 0;
    /** @brief outputs dump
     * @param aInt  indentation level
     * */
    //inline void dump(int aLevel, int aIdt = 0) const { doDump(aLevel, aIdt, std::cout);}
    virtual void dump(int aLevel = 0xffff, int aIdt = 0, std::ostream& aOs = std::cout) const {}
};

struct MVoid : public MIface
{
    static const char* Type() { return "MVoid";};
};

#endif
