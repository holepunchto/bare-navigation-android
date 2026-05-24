#include <assert.h>
#include <bare.h>
#include <jnitl.h>
#include <js.h>

static js_threadsafe_function_t *back_tsfn = NULL;

static inline java_vm_t
get_jvm () {
  return java_vm_t::get_created().value();
}

static inline java_object_t<"android/app/Application">
get_application (JNIEnv *env) {
  auto cls = java_class_t<"android/app/ActivityThread">(env);
  auto current_app = cls.get_static_method<java_object_t<"android/app/Application">()>("currentApplication");
  return java_object_t<"android/app/Application">(env, current_app());
}

static void
back_cb (js_env_t *env, js_value_t *fn, void *ctx, void *data) {
  if (fn == NULL) return;

  js_value_t *global;
  js_get_global(env, &global);

  js_value_t *result;
  js_call_function(env, global, fn, 0, NULL, &result);
}

static void
on_back_jni (JNIEnv *, jclass) {
  if (back_tsfn == NULL) return;
  js_call_threadsafe_function(back_tsfn, NULL, js_threadsafe_function_nonblocking);
}

static js_value_t *
bare_navigation_android_install (js_env_t *env, js_callback_info_t *info) {
  int err;

  if (back_tsfn != NULL) {
    js_value_t *result;
    err = js_get_undefined(env, &result);
    assert(err == 0);
    return result;
  }

  size_t argc = 1;
  js_value_t *argv[1];
  err = js_get_callback_info(env, info, &argc, argv, NULL, NULL);
  assert(err == 0);

  err = js_create_threadsafe_function(env, argv[0], 0, 1, NULL, NULL, NULL, back_cb, &back_tsfn);
  assert(err == 0);

  err = js_unref_threadsafe_function(env, back_tsfn);
  assert(err == 0);

  auto jvm = get_jvm();
  auto maybe_env = jvm.get_env();
  auto guard = maybe_env.has_value() ? std::move(*maybe_env) : jvm.attach_current_thread();
  JNIEnv *jni = guard;

  auto cls = java_class_t<"bare/navigation/android/BackHandlerBridge">(jni);
  auto install_fn = cls.get_static_method<void()>("install");
  install_fn();

  js_value_t *result;
  err = js_get_undefined(env, &result);
  assert(err == 0);
  return result;
}

static js_value_t *
bare_navigation_android_finish (js_env_t *env, js_callback_info_t *info) {
  int err;

  auto jvm = get_jvm();
  auto maybe_env = jvm.get_env();
  auto guard = maybe_env.has_value() ? std::move(*maybe_env) : jvm.attach_current_thread();
  JNIEnv *jni = guard;

  auto cls = java_class_t<"bare/navigation/android/BackHandlerBridge">(jni);
  auto finish_fn = cls.get_static_method<void()>("finish");
  finish_fn();

  js_value_t *result;
  err = js_get_undefined(env, &result);
  assert(err == 0);
  return result;
}

static js_value_t *
bare_navigation_android_exports (js_env_t *env, js_value_t *exports) {
  int err;

  {
    auto jvm = get_jvm();
    auto maybe_env = jvm.get_env();
    auto guard = maybe_env.has_value() ? std::move(*maybe_env) : jvm.attach_current_thread();
    JNIEnv *jni = guard;

    jclass bridge_class = jni->FindClass("bare/navigation/android/BackHandlerBridge");
    JNINativeMethod native_methods[] = {
      {"nativeOnBack", "()V", (void *) on_back_jni}
    };
    jni->RegisterNatives(bridge_class, native_methods, 1);

    auto bridge_cls = java_class_t<"bare/navigation/android/BackHandlerBridge">(jni, bridge_class);
    jni->DeleteLocalRef(bridge_class);

    auto app = get_application(jni);

    using init_sig = void(java_object_t<"android/app/Application">);
    auto init_fn = bridge_cls.get_static_method<init_sig>("init");
    init_fn(app);
  }

#define V(name, fn) \
  { \
    js_value_t *val; \
    err = js_create_function(env, name, -1, fn, NULL, &val); \
    assert(err == 0); \
    err = js_set_named_property(env, exports, name, val); \
    assert(err == 0); \
  }

  V("install", bare_navigation_android_install)
  V("finish", bare_navigation_android_finish)
#undef V

  return exports;
}

BARE_MODULE(bare_navigation_android, bare_navigation_android_exports)
