module.exports = {
  printWidth: 100, // 一行的字符数，如果超过会进行换行
  tabWidth: 2, // 一个tab代表几个空格数，默认就是2
  useTabs: false, // 是否启用tab取代空格符缩进，.editorconfig设置空格缩进，所以设置为false
  semi: true, // 行尾是否使用分号，默认为true
  singleQuote: false, // 字符串是否使用单引号，默认为false，使用双引号
  trailingComma: 'none', // 对象或数组末尾是否添加逗号 none| es5| all
  jsxSingleQuote: true, // 在jsx里是否使用单引号，你看着办
  bracketSpacing: true, // 对象大括号直接是否有空格，默认为true，效果：{ foo: bar }
  arrowParens: 'avoid', // 箭头函数如果只有一个参数则省略括号
  // jsx 标签的反尖括号需要换行
  jsxBracketSameLine: true,
  quoteProps: 'as-needed',
  htmlWhitespaceSensitivity: 'css',
  vueIndentScriptAndStyle: true,
  endOfLine: 'lf',
  rangeStart: 0,
};
