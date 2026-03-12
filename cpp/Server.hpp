#pragma once

#include <ReactCommon/CallInvoker.h>
#include <jsi/jsi.h>

#include <httplib.h>
#include <thread>

namespace opserver {

namespace jsi = facebook::jsi;
namespace react = facebook::react;

class JSI_EXPORT Server : public jsi::HostObject {
public:
  Server(jsi::Runtime &rt, const std::shared_ptr<react::CallInvoker> &invoker);
  ~Server() override;
  std::vector<jsi::PropNameID> getPropertyNames(jsi::Runtime &rt) override;
  jsi::Value get(jsi::Runtime &rt, const jsi::PropNameID &propNameID) override;
  void stop();

private:
  httplib::Server server;
  std::thread listen_thread;
  std::unordered_map<std::string, jsi::Value> function_map;
};

} // namespace opserver
