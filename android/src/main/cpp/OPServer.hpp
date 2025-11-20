#pragma once

#include <ReactCommon/CallInvoker.h>
#include <ReactCommon/CallInvokerHolder.h>
#include <fbjni/fbjni.h>
#include <jsi/jsi.h>

namespace opserver {

using namespace facebook;

class JOPServer final : public jni::HybridClass<JOPServer> {
public:
  static auto constexpr kJavaDescriptor =
      "Lcom/opengineering/opserver/OpServerModule;";
  static constexpr auto TAG = "OpServerModule";

  static void registerNatives();

  static void install(
      jni::alias_ref<jni::JClass>, jlong runtimePointer,
      jni::alias_ref<react::CallInvokerHolder::javaobject> callInvokerHolder);

private:
  explicit JOPServer() = default;
};

} // namespace opserver
