#include "OPServer.hpp"
#include "../cpp/bindings.hpp"

#include <exception>

namespace opserver {

using namespace facebook;

void JOPServer::install(
    jni::alias_ref<jni::JClass>, jlong runtimePointer,
    jni::alias_ref<react::CallInvokerHolder::javaobject> callInvokerHolder) {
  auto runtime = reinterpret_cast<jsi::Runtime *>(runtimePointer);
  if (runtime == nullptr) {
    throw std::invalid_argument("jsi::Runtime was null!");
  }

  if (callInvokerHolder == nullptr) {
    throw std::invalid_argument("CallInvokerHolder was null!");
  }
  auto callInvoker = callInvokerHolder->cthis()->getCallInvoker();
  if (callInvoker == nullptr) {
    throw std::invalid_argument("CallInvoker was null!");
  }

  opserver::install(*runtime, callInvoker);
}

void JOPServer::registerNatives() {
  javaClassStatic()->registerNatives({
      makeNativeMethod("install", JOPServer::install),
  });
}

} // namespace opserver
