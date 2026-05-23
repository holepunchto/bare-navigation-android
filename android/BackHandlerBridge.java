package bare.navigation.android;

import android.app.Activity;
import android.app.Application;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.window.OnBackInvokedDispatcher;

public class BackHandlerBridge {
  private static volatile Activity current;
  private static final Handler mainHandler = new Handler(Looper.getMainLooper());

  static native void nativeOnBack();

  public static void init(Application app) {
    app.registerActivityLifecycleCallbacks(
        new Application.ActivityLifecycleCallbacks() {
          @Override
          public void onActivityCreated(Activity a, Bundle b) {}

          @Override
          public void onActivityStarted(Activity a) {}

          @Override
          public void onActivityResumed(Activity a) {
            current = a;
          }

          @Override
          public void onActivityPaused(Activity a) {
            if (current == a) current = null;
          }

          @Override
          public void onActivityStopped(Activity a) {}

          @Override
          public void onActivitySaveInstanceState(Activity a, Bundle b) {}

          @Override
          public void onActivityDestroyed(Activity a) {}
        });
  }

  public static void install() {
    mainHandler.post(
        () -> {
          if (current == null) return;
          current
              .getOnBackInvokedDispatcher()
              .registerOnBackInvokedCallback(
                  OnBackInvokedDispatcher.PRIORITY_DEFAULT,
                  BackHandlerBridge::nativeOnBack);
        });
  }

  public static void finish() {
    mainHandler.post(
        () -> {
          if (current != null) current.finish();
        });
  }
}
