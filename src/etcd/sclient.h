// Author: Zhongwen Lan(runbus@qq.com)
// Created: 2018/07/05
#ifndef _ETCD_SCLIENT_H_
#define _ETCD_SCLIENT_H_

#include <atomic>
#include <string>

#include <grpc++/grpc++.h>
#include "proto/rpc.grpc.pb.h"

#include "etcd/concurrentmap.h"
#include "etcd/task.h"
#include <gtest/gtest_prod.h>

using etcdserverpb::KV;
using etcdserverpb::Lease;
using etcdserverpb::Watch;

using grpc::Channel;
using grpc::ClientContext;

namespace etcd
{
class SClient
{
public:
  /**
     * Constructor create etcd safe client with format like "192.168.1.2:2379"
     * @param etcd address include ip address and port
     * TODO: etcd cluster support
     */
  SClient(const std::string &etcd_addr);
  virtual ~SClient();

  /**
     * Set value of a key with specified lease id
     * @param key is the key to be created or modified
     * @param value is the new value to be set
     * @param lease_id is the lease attached to the key
     * @return true if grpc success otherwise false
     */
  bool Set(const std::string &key, const std::string &value, int64_t lease_id);

  /**
     * Get the value of the specified key
     * @param the request key
     * @return return value string if success otherwise empty string
     */
  std::string Get(const std::string &key);

  /**
     * Delete the record of a key
     * @param key is intended to delete
     * @return true if grpc success otherwise false
     */
  bool Delete(const std::string &key);

  /**
     * LeaseGrant request a lease id with ttl interval
     * @param ttl is the living time of the lease id
     * @return lease id if sucess otherwise 0
     */
  int64_t LeaseGrant(int64_t ttl);

  /**
     * SRegister means safe-register which could allow to register multiple keys within a client
     * @param key request to register
     * @param value to be set
     * @param ttl how long key to live
     * @return true if success otherwise false
     */
  bool SRegister(const std::string &key, const std::string &value, int64_t ttl);
  /**
     * SetDeadline set timeout value in seconds for context
     * @param context to be set
     * @param timeout_seconds timeout value in seconds
     */
  void SetDeadline(ClientContext &context, uint32_t timeout_seconds);

protected:
  /**
     * KeepAlive keep alive for the key and start with lease_id
     * @param the key for keepalive
     * @param lease_id to keep alive
     * @return return the task pointer for this key
     */
  Task *KeepAlive(const std::string &key, int64_t lease_id);

  /**
     * WatchGuard keep watch a key and will re-register key with specified value while detected DELETE happen on the key
     * @param the key request to watch and re-register
     * @param the value to be re-registered with the key
     * @param re-register with the ttl
     * @return return the task pointer for this key
     * Note: As using sync method, one watcher one thread because the assumption is watcher operations might be low frequency
     */
  Task *WatchGuard(const std::string &key, const std::string &value, int64_t ttl);

private:
  // TODO: multiple endpoints share channel and it could automatically switch
  std::shared_ptr<Channel> channel_;
  std::unique_ptr<KV::Stub> kv_stub_;
  //std::unique_ptr<Lease::Stub> lease_stub_;
  // For multiple keys registration
  ConCurrentMap map_;
  FRIEND_TEST(SClientTest, SRegister1);
  FRIEND_TEST(SClientTest, SRegister2);
};
} // namespace etcd

#endif // _ETCD_SCLIENT_H_
