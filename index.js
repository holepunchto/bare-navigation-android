'use strict'

const EventEmitter = require('bare-events')
const binding = require('./binding')

class BackHandler extends EventEmitter {
  constructor() {
    super()
    this._installed = false
  }

  on(event, listener) {
    if (event === 'back' && !this._installed) {
      this._installed = true
      binding.install(() => this.emit('back'))
    }
    return super.on(event, listener)
  }

  addListener(event, listener) {
    return this.on(event, listener)
  }

  close() {
    binding.finish()
  }
}

module.exports = new BackHandler()
