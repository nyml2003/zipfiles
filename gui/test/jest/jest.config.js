// jest.config.js
module.exports = {
  transform: {
    '^.+\\.tsx?$': 'ts-jest', // 告诉 Jest 使用 ts-jest 来转译 TypeScript 文件
  },
  testRegex: '/test/.*\\.(test|spec)?\\.(tsx?|jsx?)$', // 指定测试文件的匹配模式
  moduleFileExtensions: ['ts', 'tsx', 'js', 'jsx', 'json'], // 指定 Jest 可以处理的文件扩展名
};
