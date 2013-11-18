/*
 * Copyright (C) 2007, 2008, 2013 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "Database.h"

#if ENABLE(SQL_DATABASE)

#include "ChangeVersionWrapper.h"
#include "CrossThreadTask.h"
#include "DatabaseBackendContext.h"
#include "DatabaseCallback.h"
#include "DatabaseContext.h"
#include "DatabaseManager.h"
#include "DatabaseTask.h"
#include "DatabaseThread.h"
#include "DatabaseTracker.h"
#include "Document.h"
#include "Logging.h"
#include "NotImplemented.h"
#include "Page.h"
#include "SQLError.h"
#include "SQLTransactionCallback.h"
#include "SQLTransactionClient.h"
#include "SQLTransactionCoordinator.h"
#include "SQLTransactionErrorCallback.h"
#include "SQLiteStatement.h"
#include "ScriptExecutionContext.h"
#include "SecurityOrigin.h"
#include "VoidCallback.h"
#include <wtf/OwnPtr.h>
#include <wtf/PassOwnPtr.h>
#include <wtf/PassRefPtr.h>
#include <wtf/RefPtr.h>
#include <wtf/StdLibExtras.h>
#include <wtf/text/CString.h>

#if USE(JSC)
#include "JSDOMWindow.h"
#endif

namespace WebCore {

PassRefPtr<Database> Database::create(ScriptExecutionContext*, PassRefPtr<DatabaseBackend> backend)
{
    return static_cast<Database*>(backend.get());
}

Database::Database(PassRefPtr<DatabaseBackendContext> databaseContext,
    const String& name, const String& expectedVersion, const String& displayName, unsigned long estimatedSize)
    : DatabaseBase(databaseContext->scriptExecutionContext())
    , DatabaseBackendAsync(databaseContext, name, expectedVersion, displayName, estimatedSize)
    , m_transactionInProgress(false)
    , m_isTransactionQueueEnabled(true)
    , m_deleted(false)
{
    m_databaseThreadSecurityOrigin = m_contextThreadSecurityOrigin->isolatedCopy();
    setFrontend(this);

    ASSERT(m_databaseContext->databaseThread());
}

class DerefContextTask : public ScriptExecutionContext::Task {
public:
    static PassOwnPtr<DerefContextTask> create(PassRefPtr<ScriptExecutionContext> context)
    {
        return adoptPtr(new DerefContextTask(context));
    }

    virtual void performTask(ScriptExecutionContext* context)
    {
        ASSERT_UNUSED(context, context == m_context);
        m_context.clear();
    }

    virtual bool isCleanupTask() const { return true; }

private:
    DerefContextTask(PassRefPtr<ScriptExecutionContext> context)
        : m_context(context)
    {
    }
    
    RefPtr<ScriptExecutionContext> m_context;
};

Database::~Database()
{
    // The reference to the ScriptExecutionContext needs to be cleared on the JavaScript thread.  If we're on that thread already, we can just let the RefPtr's destruction do the dereffing.
    if (!m_scriptExecutionContext->isContextThread()) {
        // Grab a pointer to the script execution here because we're releasing it when we pass it to
        // DerefContextTask::create.
        ScriptExecutionContext* scriptExecutionContext = m_scriptExecutionContext.get();
        
        scriptExecutionContext->postTask(DerefContextTask::create(m_scriptExecutionContext.release()));
    }
}

Database* Database::from(DatabaseBackendAsync* backend)
{
    return static_cast<Database*>(backend->m_frontend);
}

PassRefPtr<DatabaseBackendAsync> Database::backend()
{
    return this;
}

String Database::version() const
{
    if (m_deleted)
        return String();
    return DatabaseBackend::version();
}

void Database::markAsDeletedAndClose()
{
    if (m_deleted || !databaseContext()->databaseThread())
        return;

    LOG(StorageAPI, "Marking %s (%p) as deleted", stringIdentifier().ascii().data(), this);
    m_deleted = true;

    DatabaseTaskSynchronizer synchronizer;
    if (databaseContext()->databaseThread()->terminationRequested(&synchronizer)) {
        LOG(StorageAPI, "Database handle %p is on a terminated DatabaseThread, cannot be marked for normal closure\n", this);
        return;
    }

    OwnPtr<DatabaseCloseTask> task = DatabaseCloseTask::create(this, &synchronizer);
    databaseContext()->databaseThread()->scheduleImmediateTask(task.release());
    synchronizer.waitForTaskCompletion();
}

void Database::close()
{
    ASSERT(databaseContext()->databaseThread());
    ASSERT(currentThread() == databaseContext()->databaseThread()->getThreadID());

    {
        MutexLocker locker(m_transactionInProgressMutex);
        m_isTransactionQueueEnabled = false;
        m_transactionInProgress = false;
        m_transactionQueue.clear();
    }

    closeDatabase();

    // Must ref() before calling databaseThread()->recordDatabaseClosed().
    RefPtr<Database> protect = this;
    databaseContext()->databaseThread()->recordDatabaseClosed(this);
    databaseContext()->databaseThread()->unscheduleDatabaseTasks(this);
}

void Database::closeImmediately()
{
    ASSERT(m_scriptExecutionContext->isContextThread());
    DatabaseThread* databaseThread = databaseContext()->databaseThread();
    if (databaseThread && !databaseThread->terminationRequested() && opened()) {
        logErrorMessage("forcibly closing database");
        databaseThread->scheduleImmediateTask(DatabaseCloseTask::create(this, 0));
    }
}

unsigned long long Database::maximumSize() const
{
    return DatabaseManager::manager().getMaxSizeForDatabase(this);
}

void Database::changeVersion(const String& oldVersion, const String& newVersion,
                             PassRefPtr<SQLTransactionCallback> callback, PassRefPtr<SQLTransactionErrorCallback> errorCallback,
                             PassRefPtr<VoidCallback> successCallback)
{
    runTransaction(callback, errorCallback, successCallback, ChangeVersionWrapper::create(oldVersion, newVersion), false);
}

void Database::transaction(PassRefPtr<SQLTransactionCallback> callback, PassRefPtr<SQLTransactionErrorCallback> errorCallback, PassRefPtr<VoidCallback> successCallback)
{
    runTransaction(callback, errorCallback, successCallback, 0, false);
}

void Database::readTransaction(PassRefPtr<SQLTransactionCallback> callback, PassRefPtr<SQLTransactionErrorCallback> errorCallback, PassRefPtr<VoidCallback> successCallback)
{
    runTransaction(callback, errorCallback, successCallback, 0, true);
}

static void callTransactionErrorCallback(ScriptExecutionContext*, PassRefPtr<SQLTransactionErrorCallback> callback, PassRefPtr<SQLError> error)
{
    callback->handleEvent(error.get());
}

void Database::runTransaction(PassRefPtr<SQLTransactionCallback> callback, PassRefPtr<SQLTransactionErrorCallback> errorCallback,
                              PassRefPtr<VoidCallback> successCallback, PassRefPtr<SQLTransactionWrapper> wrapper, bool readOnly)
{
    MutexLocker locker(m_transactionInProgressMutex);
    if (!m_isTransactionQueueEnabled) {
        if (errorCallback) {
            RefPtr<SQLError> error = SQLError::create(SQLError::UNKNOWN_ERR, "database has been closed");
            scriptExecutionContext()->postTask(createCallbackTask(&callTransactionErrorCallback, errorCallback, error.release()));
        }
        return;
    }
    RefPtr<SQLTransaction> transaction = SQLTransaction::create(this, callback, errorCallback, successCallback, wrapper, readOnly);
    m_transactionQueue.append(transaction.release());
    if (!m_transactionInProgress)
        scheduleTransaction();
}

void Database::inProgressTransactionCompleted()
{
    MutexLocker locker(m_transactionInProgressMutex);
    m_transactionInProgress = false;
    scheduleTransaction();
}

void Database::scheduleTransaction()
{
    ASSERT(!m_transactionInProgressMutex.tryLock()); // Locked by caller.
    RefPtr<SQLTransaction> transaction;

    if (m_isTransactionQueueEnabled && !m_transactionQueue.isEmpty())
        transaction = m_transactionQueue.takeFirst();

    if (transaction && databaseContext()->databaseThread()) {
        OwnPtr<DatabaseTransactionTask> task = DatabaseTransactionTask::create(transaction);
        LOG(StorageAPI, "Scheduling DatabaseTransactionTask %p for transaction %p\n", task.get(), task->transaction());
        m_transactionInProgress = true;
        databaseContext()->databaseThread()->scheduleTask(task.release());
    } else
        m_transactionInProgress = false;
}

void Database::scheduleTransactionStep(SQLTransactionBackend* transaction, bool immediately)
{
    if (!databaseContext()->databaseThread())
        return;

    OwnPtr<DatabaseTransactionTask> task = DatabaseTransactionTask::create(transaction);
    LOG(StorageAPI, "Scheduling DatabaseTransactionTask %p for the transaction step\n", task.get());
    if (immediately)
        databaseContext()->databaseThread()->scheduleImmediateTask(task.release());
    else
        databaseContext()->databaseThread()->scheduleTask(task.release());
}

class DeliverPendingCallbackTask : public ScriptExecutionContext::Task {
public:
    static PassOwnPtr<DeliverPendingCallbackTask> create(PassRefPtr<SQLTransaction> transaction)
    {
        return adoptPtr(new DeliverPendingCallbackTask(transaction));
    }

    virtual void performTask(ScriptExecutionContext*)
    {
        m_transaction->performPendingCallback();
    }

private:
    DeliverPendingCallbackTask(PassRefPtr<SQLTransaction> transaction)
        : m_transaction(transaction)
    {
    }

    RefPtr<SQLTransaction> m_transaction;
};

void Database::scheduleTransactionCallback(SQLTransaction* transaction)
{
    m_scriptExecutionContext->postTask(DeliverPendingCallbackTask::create(transaction));
}

Vector<String> Database::performGetTableNames()
{
    disableAuthorizer();

    SQLiteStatement statement(sqliteDatabase(), "SELECT name FROM sqlite_master WHERE type='table';");
    if (statement.prepare() != SQLResultOk) {
        LOG_ERROR("Unable to retrieve list of tables for database %s", databaseDebugName().ascii().data());
        enableAuthorizer();
        return Vector<String>();
    }

    Vector<String> tableNames;
    int result;
    while ((result = statement.step()) == SQLResultRow) {
        String name = statement.getColumnText(0);
        if (name != databaseInfoTableName())
            tableNames.append(name);
    }

    enableAuthorizer();

    if (result != SQLResultDone) {
        LOG_ERROR("Error getting tables for database %s", databaseDebugName().ascii().data());
        return Vector<String>();
    }

    return tableNames;
}

SQLTransactionClient* Database::transactionClient() const
{
    return databaseContext()->databaseThread()->transactionClient();
}

SQLTransactionCoordinator* Database::transactionCoordinator() const
{
    return databaseContext()->databaseThread()->transactionCoordinator();
}

Vector<String> Database::tableNames()
{
    // FIXME: Not using isolatedCopy on these strings looks ok since threads take strict turns
    // in dealing with them. However, if the code changes, this may not be true anymore.
    Vector<String> result;
    DatabaseTaskSynchronizer synchronizer;
    if (!databaseContext()->databaseThread() || databaseContext()->databaseThread()->terminationRequested(&synchronizer))
        return result;

    OwnPtr<DatabaseTableNamesTask> task = DatabaseTableNamesTask::create(this, &synchronizer, result);
    databaseContext()->databaseThread()->scheduleImmediateTask(task.release());
    synchronizer.waitForTaskCompletion();

    return result;
}

SecurityOrigin* Database::securityOrigin() const
{
    if (m_scriptExecutionContext->isContextThread())
        return m_contextThreadSecurityOrigin.get();
    if (currentThread() == databaseContext()->databaseThread()->getThreadID())
        return m_databaseThreadSecurityOrigin.get();
    return 0;
}

} // namespace WebCore

#endif // ENABLE(SQL_DATABASE)