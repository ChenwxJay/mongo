
/**
 *    Copyright (C) 2018-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#pragma once

#include <boost/optional.hpp>

#include "mongo/base/disallow_copying.h"
#include "mongo/db/concurrency/d_concurrency.h"
#include "mongo/util/time_support.h"

namespace mongo {
namespace repl {

/**
 * This object handles acquiring the RSTL for replication state transitions, as well as any actions
 * that need to happen in between enqueuing the RSTL request and waiting for it to be granted.
 */
class ReplicationStateTransitionLockGuard {
    MONGO_DISALLOW_COPYING(ReplicationStateTransitionLockGuard);

public:
    struct Args {
        // How long to wait for the RSTL in mode X.
        Date_t lockDeadline = Date_t::max();

        // If true, will kill all user operations in between enqueuing the RSTL request and waiting
        // for it to be granted.
        bool killUserOperations = false;
    };

    /**
     * Acquires the RSTL in mode X.
     */
    ReplicationStateTransitionLockGuard(OperationContext* opCtx);

    /**
     * Acquires the RSTL in mode X and performs any other required actions according to the Args
     * provided.
     */
    ReplicationStateTransitionLockGuard(OperationContext* opCtx, const Args& args);

    ~ReplicationStateTransitionLockGuard();

    /**
     * Temporarily releases the RSTL in mode X.  Must be followed by a call to reacquireRSTL().
     */
    void releaseRSTL();

    /**
     * Re-acquires the RSTL in mode X after it was released via a call to releaseRSTL.  Ignores
     * the configured 'lockDeadline' and instead waits forever for the lock to be acquired.
     */
    void reacquireRSTL();

private:
    OperationContext* const _opCtx;
    Args _args;
};

}  // namespace repl
}  // namespace mongo
