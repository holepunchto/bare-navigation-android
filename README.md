# bare-navigation-android

Android back gesture/button handler for the [Bare](https://github.com/holepunchto/bare) runtime.

Intercepts the Android system back gesture/button, prevents the OS default behaviour, and emits a JS `back` event. Once a `back` listener is registered the OS default is fully suppressed. Use `close()` to explicitly finish the Activity.

Requires `minSdk 33` and `android:enableOnBackInvokedCallback="true"` in your manifest.

## Installation

```sh
npm install bare-navigation-android
```

No manual Java setup required — the companion DEX is compiled and included in the prebuilds, and [`bare-link`](https://github.com/holepunchto/bare-link) picks it up automatically when packaging with [`bare-build`](https://github.com/holepunchto/bare-build).

### Android manifest

Add `enableOnBackInvokedCallback` to your `<application>` tag. Without it, edge-swipe gestures are not intercepted (button presses still work):

```xml
<uses-sdk android:minSdkVersion="33" android:targetSdkVersion="36" />
<application
    android:enableOnBackInvokedCallback="true"
    ...>
```

## Usage

```js
const BackHandler = require('bare-navigation-android')

BackHandler.on('back', () => {
  if (myRouter.canGoBack()) {
    myRouter.goBack()
  } else {
    BackHandler.close()
  }
})

// Remove a listener
BackHandler.off('back', listener)

// Explicitly close the app (required once a back listener is active)
BackHandler.close()
```

## API

### `BackHandler.on('back', listener)` / `BackHandler.addListener('back', listener)`

Register a listener for the Android back gesture/button. The first call suppresses the OS default behaviour for the lifetime of the process.

### `BackHandler.off('back', listener)` / `BackHandler.removeListener('back', listener)`

Unregister a listener.

### `BackHandler.close()`

Finish the Android Activity. This is the only way to close the app once a back listener is active, since the OS default is suppressed.

## Building

Uses [bare-make](https://github.com/holepunchto/bare-make) for compiling the native bindings. The prebuilds also include a companion DEX for the `BackHandlerBridge` Java helper.

### Native binding

```sh
bare-make generate --platform android --arch arm64 -D ANDROID_PLATFORM=android-34 -D ANDROID_STL=c++_shared
bare-make build
bare-make install
```

### Companion DEX

After building, compile `BackHandlerBridge.java` to DEX and place it in the prebuild companion directory:

```sh
javac -source 11 -target 11 \
  -cp $ANDROID_HOME/platforms/android-34/android.jar \
  android/BackHandlerBridge.java -d /tmp/bnh-classes

d8 --release \
  --lib $ANDROID_HOME/platforms/android-34/android.jar \
  --output prebuilds/android-arm64/bare-navigation-android \
  /tmp/bnh-classes/bare/navigation/android/BackHandlerBridge.class \
  "/tmp/bnh-classes/bare/navigation/android/BackHandlerBridge\$1.class"
```

## License

Apache-2.0
