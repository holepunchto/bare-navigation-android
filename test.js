const test = require('brittle')
const BackHandler = require('.')

test('api surface', (t) => {
  t.is(typeof BackHandler.on, 'function')
  t.is(typeof BackHandler.addListener, 'function')
  t.is(typeof BackHandler.off, 'function')
  t.is(typeof BackHandler.close, 'function')
})
