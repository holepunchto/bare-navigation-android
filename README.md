# bare-navigation-android

Android back gesture/button handler for the [Bare](https://github.com/holepunchto/bare) runtime.

Intercepts the Android system back gesture/button, prevents the OS default behaviour, and emits a JS `back` event. Once a `back` listener is registered the OS default is fully suppressed. Use `close()` to explicitly finish the Activity.

Requires `minSdk 33`.

## Installation

```sh
npm install bare-navigation-android
```

### Android project setup

Copy `android/BackHandlerBridge.java` into your Android app's Java source set, preserving the package path:

```
app/src/main/java/bare/navigation/android/BackHandlerBridge.java
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

Uses [bare-make](https://github.com/holepunchto/bare-make) for compiling the native bindings. Build for Android only:

```sh
bare-make generate -D ANDROID_PLATFORM=android-34 -D ANDROID_STL=c++_shared
bare-make build
bare-make install
```

## License

Apache-2.0
