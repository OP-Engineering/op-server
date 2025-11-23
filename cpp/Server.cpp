#pragma once

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "macros.hpp"
#include <future>
#include <iostream>

namespace opserver {

namespace jsi = facebook::jsi;
namespace react = facebook::react;

Server::Server(jsi::Runtime &rt, std::shared_ptr<react::CallInvoker> invoker) {

  function_map["callback"] = HFN2(this, invoker) {
    const std::string method = args[0].asString(rt).utf8(rt);
    const std::string path = args[1].asString(rt).utf8(rt);
    auto callback = std::make_shared<jsi::Value>(rt, args[2]);

    auto handleRequest = [this, &rt, invoker,
                          callback](const httplib::Request &req,
                                    httplib::Response &res) {
      auto responseDone = std::make_shared<std::promise<void>>();
      auto responseFuture = responseDone->get_future();

      // Create shared pointer to Response object that will be modified by JS
      auto responseObj = std::make_shared<Response>();

      invoker->invokeAsync([callback, responseDone, responseObj,
                            req](jsi::Runtime &rt) {
        try {
          // Create Request and Response HostObjects
          auto requestHostObj = std::make_shared<Request>(req);
          auto reqJSI = jsi::Object::createFromHostObject(rt, requestHostObj);
          auto resJSI = jsi::Object::createFromHostObject(rt, responseObj);

          // Call the callback with req and res objects
          auto promise =
              callback->asObject(rt).asFunction(rt).call(rt, reqJSI, resJSI);

          auto promiseObj = promise.asObject(rt);
          auto then_fn = promiseObj.getPropertyAsFunction(rt, "then");
          auto catch_fn = promiseObj.getPropertyAsFunction(rt, "catch");

          auto success_handler = HFN(responseDone) {
            responseDone->set_value();
            return jsi::Value::undefined();
          });

          auto reject_handler = HFN2(responseDone, responseObj) {
            // On error, set error response
            responseObj->set(rt, jsi::PropNameID::forUtf8(rt, "statusCode"),
                             jsi::Value(500));

            std::string errorMsg = "Internal Server Error";
            if (count > 0) {
              if (args[0].isString()) {
                errorMsg = args[0].asString(rt).utf8(rt);
              } else if (args[0].isObject()) {
                auto errObj = args[0].asObject(rt);
                if (errObj.hasProperty(rt, "message")) {
                  auto msgVal = errObj.getProperty(rt, "message");
                  if (msgVal.isString()) {
                    errorMsg = msgVal.asString(rt).utf8(rt);
                  }
                }
              }
            }

            responseObj->set(rt, jsi::PropNameID::forUtf8(rt, "content"),
                             jsi::String::createFromUtf8(rt, errorMsg));
            responseDone->set_value();
            return jsi::Value::undefined();
          });

          then_fn.callWithThis(rt, promiseObj, success_handler);
          catch_fn.callWithThis(rt, promiseObj, reject_handler);

        } catch (const std::exception &e) {
          responseObj->set(rt, jsi::PropNameID::forUtf8(rt, "statusCode"),
                           jsi::Value(500));
          responseObj->set(rt, jsi::PropNameID::forUtf8(rt, "content"),
                           jsi::String::createFromUtf8(
                               rt, std::string("Error: ") + e.what()));
          responseDone->set_value();
        }
      });

      if (responseFuture.wait_for(std::chrono::seconds(5)) ==
          std::future_status::ready) {
        // Apply the Response HostObject changes back to httplib::Response
        responseObj->applyTo(res);
      } else {
        res.status = 408;
        res.set_content("Request Timeout", "text/plain");
      }
    };

    // Register the appropriate HTTP method
    if (method == "GET") {
      server.Get(path, handleRequest);
    } else if (method == "POST") {
      server.Post(path, handleRequest);
    } else if (method == "PUT") {
      server.Put(path, handleRequest);
    } else if (method == "DELETE") {
      server.Delete(path, handleRequest);
    }

    return {};
  });

  function_map["listen"] = HFN(this) {
    int port = 3000;
    if (count > 0 && args[0].isNumber()) {
      port = static_cast<int>(args[0].asNumber());
    }
    std::thread([this, port]() { server.listen("0.0.0.0", port); }).detach();
    return {};
  });

  function_map["stop"] = HFN(this) {
    server.stop();
    return {};
  });
}

std::vector<jsi::PropNameID> Server::getPropertyNames(jsi::Runtime &_rt) {
  std::vector<jsi::PropNameID> keys;
  keys.reserve(function_map.size());
  for (const auto &pair : function_map) {
    keys.emplace_back(jsi::PropNameID::forUtf8(_rt, pair.first));
  }
  return keys;
}

jsi::Value Server::get(jsi::Runtime &rt, const jsi::PropNameID &propNameID) {
  const auto name = propNameID.utf8(rt);
  if (function_map.count(name) != 1) {
    return HFN(name) {
      throw std::runtime_error("[op-server] Function " + name +
                               " not implemented");
    });
  }

  return {rt, function_map[name]};
}

void Server::set(jsi::Runtime &_rt, const jsi::PropNameID &name,
                 const jsi::Value &value) {
  throw std::runtime_error("You cannot write to this object!");
}

void Server::stop() { server.stop(); }

Server::~Server() {

};

} // namespace opserver
