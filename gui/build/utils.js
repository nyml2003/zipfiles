(function (root, factory) {
  if (typeof define === 'function' && define.amd) {
    // AMD
    define([], factory);
  } else if (typeof module === 'object' && module.exports) {
    // CommonJS
    module.exports = factory();
  } else {
    // 全局变量
    root.selectEnv = factory();
  }
})(typeof self !== 'undefined' ? self : this, function () {
  function selectEnv(dev, prod) {
    if (process.env.NODE_ENV === 'production') {
      return prod;
    }
    return dev;
  }

  return selectEnv;
});
