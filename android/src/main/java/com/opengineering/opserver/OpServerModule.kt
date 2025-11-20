package com.opengineering.opserver

import androidx.annotation.Keep
import com.facebook.proguard.annotations.DoNotStrip
import com.facebook.react.bridge.ReactApplicationContext
import com.facebook.react.bridge.ReactMethod
import com.facebook.react.common.annotations.FrameworkAPI
import com.facebook.react.turbomodule.core.CallInvokerHolderImpl
import android.util.Log;

@DoNotStrip
@Keep
@OptIn(FrameworkAPI::class)
@Suppress("KotlinJniMissingFunction")
class OpServerModule(val context: ReactApplicationContext) :
  NativeOpServerSpec(context) {

  override fun getName(): String {
    return NAME
  }

  @ReactMethod(isBlockingSynchronousMethod = true)
  override fun install(): Boolean {
    try {
      val jsContext =
        context.javaScriptContextHolder
          ?: return false

      val callInvokerHolder =
        context.jsCallInvokerHolder as? CallInvokerHolderImpl
          ?: return false

      install(jsContext.get(), callInvokerHolder)

      return true
    } catch (e: Throwable) {
      return false
    }
  }

  private external fun install(
    jsRuntimePointer: Long,
    callInvokerHolder: CallInvokerHolderImpl,
  )

  companion object {
    const val NAME = "OpServer"

    @JvmStatic
    var applicationContext: ReactApplicationContext? = null

    init {
      System.loadLibrary("op-server");
      Log.i("OPServer", "Successfully loaded C++ library!");
    }
  }
}
