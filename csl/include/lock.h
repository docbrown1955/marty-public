// This file is part of MARTY.
// 
// MARTY is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// MARTY is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with MARTY. If not, see <https://www.gnu.org/licenses/>.

/**
 * @file lock.h
 * @brief Contains the static class Lock, allowing to compress expressions 
 * before heavy calculation, and decompress them later.
 * @author Grégoire Uhlrich
 * @version 1.3
 * @date 2020-09-03
 */
#ifndef LOCK_H_INCLUDED
#define LOCK_H_INCLUDED

#include <functional>
#include <string>
#include <map>

namespace csl {

class Expr;

/**
 * @brief Static class allowing to compress expressions given a boolean 
 * predicate.
 *
 * @details When doing heavy calculations, many sub expressions are not needed
 * and just slow down (while taking memory) the calculation. This class can, 
 * given a boolean predicate, abbreviate all these expressions (lock()). After 
 * the calculation, you may re-expand the abbreviations (unlock()).
 */
class Lock {

public:

    /**
     * @brief Typedef for lock id.
     */
    using ID_t = int;
    /**
     * @brief Typedef for the boolean predicate.
     */
    using predicate = std::function<bool(Expr const&)>;

    /**
     * @brief Abbreviates all parts of an expression depending on a condition.
     *
     * @details Any sub expression \b sub in \b expr such that f(sub) == false
     * is abbreviated. This allows to compress all the information that is not 
     * needed when performing a given calculation. Large parts of expressions
     * may be totally useless and must be abbreviated in order to minimize their
     * impact on memory and computation time. To be clear, \b f should return 
     * \b true on objects that are important for the calculation, i.e. that must
     * not be abbreviated.
     *
     * @param init   Expression to lock.
     * @param lockID Lock id, allows to differentiate different locks when 
     * unlocking.
     * @param f      Boolean predicate.
     *
     * @sa unlock()
     */
    static 
    void lock(
            Expr            &init,
            int              lockId,
            predicate const &f
            );

    /**
     * @brief Abbreviates all parts of an expression depending on a condition.
     *
     * @details Any sub expression \b sub in \b expr such that f(sub) == false
     * is abbreviated. This allows to compress all the information that is not 
     * needed when performing a given calculation. Large parts of expressions
     * may be totally useless and must be abbreviated in order to minimize their
     * impact on memory and computation time. To be clear, \b f should return 
     * \b true on objects that are important for the calculation, i.e. that must
     * not be abbreviated.
     *
     * @note This overload does not take any id. It actually calls the first 
     * function with the default id 0.
     *
     * @param init   Expression to lock.
     * @param f      Boolean predicate.
     *
     * @sa unlock()
     */
    static 
    void lock(
            Expr            &init,
            predicate const &f
            );

    /**
     * @brief Expand abbreviations from a previous lock. The lock id should be
     * the same.
     *
     * @param init   Expression to unlock.
     * @param lockID Lock id, allows to differentiate different locks when 
     * unlocking.
     *
     * @sa Lock()
     */
    static 
    void unlock(
            Expr &init,
            int   lockId
            );

    /**
     * @brief Expand abbreviations from a previous lock. The lock id should be
     * the same.
     *
     * @param init Expression to unlock.
     *
     * @note This overload does not take any id. It actually calls the first 
     * function with the default id 0.
     *
     * @sa Lock()
     */
    static 
    void unlock(
            Expr &init
            );

    /**
     * @brief Returns the generic abbreviation name of a lock given its id.
     *
     * @param lockID Id of the lock.
     *
     * @return The name of abbreviations of lock with id \b id.
     *
     * @sa Lock(), Unlock()
     */
    static
    std::string lockNameOf(ID_t id);

private:

    /**
     * @brief Actually locks an expression.
     *
     * @param init   Expression to lock.
     * @param lockID Lock id, allows to differentiate different locks when 
     * unlocking.
     * @param f      Boolean predicate.
     *
     * @sa lock()
     */
    static 
    bool doLock(
            Expr &init,
            ID_t  id,
            predicate const &f
            );

    /**
     * @brief Actually unlocks the expression.
     *
     * @param init   Expression to unlock.
     * @param lockID Lock id, allows to differentiate different locks when 
     * unlocking.
     *
     * @sa unlock()
     */
    static 
    void doUnlock(
            Expr &init,
            ID_t  id
            );

private:

    /**
     * @brief Static map containing lock names as a function of their id.
     */
    static inline 
    std::map<ID_t, std::string> lockName;
};

} // End of namespace mty

#endif
