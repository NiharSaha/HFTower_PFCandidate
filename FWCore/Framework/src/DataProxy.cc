// -*- C++ -*-
//
// Package:     Framework
// Class  :     DataProxy
//
// Implementation:
//     <Notes on implementation>
//
// Author:      Chris Jones
// Created:     Thu Mar 31 12:49:19 EST 2005
//

#include "FWCore/Framework/interface/DataProxy.h"
#include "FWCore/Framework/interface/ComponentDescription.h"
#include "FWCore/Framework/interface/MakeDataException.h"
#include "FWCore/Framework/interface/EventSetupRecordImpl.h"
#include "FWCore/Utilities/interface/Likely.h"

namespace edm {
  namespace eventsetup {

    static const ComponentDescription* dummyDescription() {
      static const ComponentDescription s_desc;
      return &s_desc;
    }

    DataProxy::DataProxy()
        : description_(dummyDescription()),
          cache_(nullptr),
          cacheIsValid_(false),
          nonTransientAccessRequested_(false) {}

    DataProxy::~DataProxy() {}

    void DataProxy::clearCacheIsValid() {
      nonTransientAccessRequested_.store(false, std::memory_order_release);
      cache_ = nullptr;
      cacheIsValid_.store(false, std::memory_order_release);
    }

    void DataProxy::resetIfTransient() {
      if (!nonTransientAccessRequested_.load(std::memory_order_acquire)) {
        clearCacheIsValid();
        invalidateTransientCache();
      }
    }

    void DataProxy::invalidateTransientCache() { invalidateCache(); }

    namespace {
      void throwMakeException(const EventSetupRecordImpl& iRecord, const DataKey& iKey) {
        throw MakeDataException(iRecord.key(), iKey);
      }

    }  // namespace

    void DataProxy::prefetchAsync(WaitingTaskHolder iTask,
                                  EventSetupRecordImpl const& iRecord,
                                  DataKey const& iKey,
                                  EventSetupImpl const* iEventSetupImpl,
                                  ServiceToken const& iToken,
                                  ESParentContext const& iParent) const {
      const_cast<DataProxy*>(this)->prefetchAsyncImpl(iTask, iRecord, iKey, iEventSetupImpl, iToken, iParent);
    }

    void const* DataProxy::getAfterPrefetch(const EventSetupRecordImpl& iRecord,
                                            const DataKey& iKey,
                                            bool iTransiently) const {
      //We need to set the AccessType for each request so this can't be called in an earlier function in the stack.
      //This also must be before the cache_ check since we want to setCacheIsValid before a possible
      // exception throw. If we don't, 'getImpl' will be called again on a second request for the data.

      if LIKELY (!iTransiently) {
        nonTransientAccessRequested_.store(true, std::memory_order_release);
      }

      if UNLIKELY (!cacheIsValid()) {
        cache_ = getAfterPrefetchImpl();
        cacheIsValid_.store(true, std::memory_order_release);
      }

      if UNLIKELY (nullptr == cache_) {
        throwMakeException(iRecord, iKey);
      }
      return cache_;
    }

  }  // namespace eventsetup
}  // namespace edm
