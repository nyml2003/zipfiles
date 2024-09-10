<!--
 * @Author: shawnxiao 597035529@qq.com
 * @Date: 2022-11-15 00:15:09
 * @LastEditors: shawnxiao 597035529@qq.com
 * @LastEditTime: 2022-11-18 15:22:30
 * @FilePath: \react\webpack5-ts-react18\note.md
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
-->
## webpack5从零搭建react18 ts项目

### 目录

1. 前言
2. 初始化项目
2. 配置基础版react+ts环境
3. 常用功能配置
4. 配置react模块热替换
5. 优化构建速度
6. 优化构建结果文件
7. 总结

#### 一. 前言

从2020年10月10日，webpack 升级至 5 版本到现在已经快两年，webpack5版本优化了很多原有的功能比如tree-shaking优化，也新增了很多新特性，比如联邦模块等，之前跟着学手敲了vwebpack4，这次结合网上文章对比vwebpack4从零再来一遍。

#### 二. 初始化项目
在开始webpack配置之前，先手动初始化一个基本的react+ts项目，新建项目文件夹webpack5-ts-react18, 在项目下执
```
npm init -y
```
初始化好package.json后,在项目下新增以下所示目录结构和文件
```
├── build
|   ├── webpack.base.js # 公共配置
|   ├── webpack.dev.js # 公共配置
|   ├── webpack.prod.js # 公共配置
|   ├── config.js  # 环境配置文件
|   ├── constants.js # 环境
|   ├── utils.js # 工具方法
|   ├── theme.js # 主题色配置
|   ├── env.js # 环境变量配置文件
|   ├── entry.js # 入口配置文件
|   ├── devServer.js # 访问端口，代理等相关配置
|   ├── optimization.js # optimization
|   ├── plugins.js # plugins
|   └── loaders
|       ├── fileLoaders.js # 文件相关loaders编译器配置
|       ├── jsLoaders.js # js/ts/jsx相关loaders编译器配置
|       ├── styleLoaders.js # style/css/less/sass相关loaders编译器配置
|       └── otherLoaders.js # 其他优化缓存相关loaders编译器配置
├── public
│   └── index.html # html模板
├── src
|   ├── App.tsx 
│   └── index.tsx # react应用入口页面
├── tsconfig.json  # ts配置
└── package.json
```

安装webpack依赖
```
npm i webpack webpack-cli -D
```
安装react依赖 及类型依赖
```
npm i react react-dom -S
npm i @types/react @types/react-dom -D
```
添加public/index.html内容
```
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>webpack5-react-ts</title>
</head>
<body>
  <!-- 容器节点 -->
  <div id="root"></div>
</body>
</html>
```
添加tsconfig.json内容
```
{
  "compilerOptions": {
    "target": "ESNext",
    "lib": ["DOM", "DOM.Iterable", "ESNext"],
    "allowJs": false,
    "skipLibCheck": false,
    "esModuleInterop": false,
    "allowSyntheticDefaultImports": true,
    "strict": true,
    "forceConsistentCasingInFileNames": true,
    "module": "ESNext",
    "moduleResolution": "Node",
    "resolveJsonModule": true,
    "isolatedModules": true,
    "noEmit": true,
    "jsx": "react", // react18这里也可以改成react-jsx
  },
  "include": ["./src"]
}
```
添加src/App.tsx内容
```
import React from 'react'

function App() {
  return <h2>webpack5-react-ts</h2>
}
export default App
```
添加src/index.tsx内容
```
import React from 'react';
import { createRoot } from 'react-dom/client';
import App from './App';

const root = document.getElementById('root');
if(root) {
  createRoot(root).render(<App />)
}
```

#### 三. 配置基础版React+ts环境

3.1. webpack公共配置
修改webpack.base.js

1. 配置入口,出口文件

```
// webpack.base.js
const path = require('path')

module.exports = {
  entry: path.join(__dirname, '../src/index.tsx'), // 入口文件
  // 打包文件出口
  output: {
    filename: 'static/js/[name].js', // 每个输出js的名称
    path: path.join(__dirname, '../dist'), // 打包结果输出路径
    clean: true, // webpack4需要配置clean-webpack-plugin来删除dist文件,webpack5内置了
    publicPath: '/' // 打包后文件的公共前缀路径
  },
}
```
2. 配置loader解析ts和jsx

由于webpack默认只能识别js文件,不能识别jsx语法,需要配置loader的预设 [@babel/preset-typescript](https://www.babeljs.cn/docs/)[babel-preset-typescript](https://www.babeljs.cn/docs/babel-preset-react) 来先ts语法转换为 js 语法,再借助预设 @babel/preset-react 来识别jsx语法。

安装babel核心模块和babel预设
```
npm i babel-loader @babel/core @babel/preset-react @babel/preset-typescript -D
```

在webpack.base.js添加module.rules配置

```
// webpack.base.js
module.exports = {
  // ...
  module: {
    rules: [
      {
        test: /.(ts|tsx)$/, // 匹配.ts, tsx文件
        use: {
          loader: 'babel-loader',
          options: {
            // 预设执行顺序由右往左,所以先处理ts,再处理jsx
            presets: [
              '@babel/preset-react',
              '@babel/preset-typescript'
            ]
          }
        }
      }
    ]
  }
}
```

3. 配置extensions

> extensions是webpack的resolve解析配置下的选项，在引入模块时不带文件后缀时，会来该配置数组里面依次添加后缀查找文件，因为ts不支持引入以 .ts, tsx为后缀的文件，所以要在extensions中配置，而第三方库里面很多引入js文件没有带后缀，所以也要配置下js

修改webpack.base.js，注意把高频出现的文件后缀放在前面

```
// webpack.base.js
module.exports = {
  // ...
  resolve: {
    extensions: ['.js', '.tsx', '.ts'],
  }
}
```
这里只配置js, tsx和ts，其他文件引入都要求带后缀，可以提升构建速度。

4. 添加html-webpack-plugin插件

webpack需要把最终构建好的静态资源都引入到一个html文件中,这样才能在浏览器中运行,[html-webpack-plugin](https://www.npmjs.com/package/html-webpack-plugin)就是来做这件事情的,安装依赖：
```
npm i html-webpack-plugin -D
```
因为该插件在开发和构建打包模式都会用到,所以还是放在公共配置webpack.base.js里面

```
// webpack.base.js
const path = require('path')
const HtmlWebpackPlugin = require('html-webpack-plugin')

module.exports = {
  // ...
  plugins: [
    new HtmlWebpackPlugin({
      template: path.resolve(__dirname, '../public/index.html'), // 模板取定义root节点的模板
      inject: true, // 自动注入静态资源
    })
  ]
}
```
到这里一个最基础的react基本公共配置就已经配置好了,需要在此基础上分别配置开发环境和打包环境了。


3.2. webpack开发环境配置

1. 安装 webpack-dev-server
开发环境配置代码在webpack.dev.js中,需要借助 [webpack-dev-server](https://www.npmjs.com/package/webpack-dev-server)在开发环境启动服务器来辅助开发,还需要依赖[webpack-merge](https://www.npmjs.com/package/webpack-merge)来合并基本配置,安装依赖:

```
npm i webpack-dev-server webpack-merge -D
```
修改webpack.dev.js代码, 合并公共配置，并添加开发模式配置
```
// webpack.dev.js
const path = require('path')
const { merge } = require('webpack-merge')
const baseConfig = require('./webpack.base.js')

// 合并公共配置,并添加开发环境配置
module.exports = merge(baseConfig, {
  mode: 'development', // 开发模式,打包更加快速,省了代码优化步骤
  devtool: 'eval-cheap-module-source-map', // 源码调试模式,后面会讲
  devServer: {
    port: 3000, // 服务端口号
    compress: false, // gzip压缩,开发环境不开启,提升热更新速度
    hot: true, // 开启热更新，后面会讲react模块热替换具体配置
    historyApiFallback: true, // 解决history路由404问题
    static: {
      directory: path.join(__dirname, "../public"), //托管静态资源public文件夹
    }
  }
})
```
2. package.json添加dev脚本

在package.json的scripts中添加

```
// package.json
"scripts": {
  "dev": "webpack-dev-server -c build/webpack.dev.js"
},
```

执行npm run dev,就能看到项目已经启动起来了,访问http://localhost:3000/,就可以看到项目界面,具体完善的react模块热替换在下面会讲到。

3.3. webpack打包环境配置

1. 修改webpack.prod.js代码

```
// webpack.prod.js

const { merge } = require('webpack-merge')
const baseConfig = require('./webpack.base.js')
module.exports = merge(baseConfig, {
  mode: 'production', // 生产模式,会开启tree-shaking和压缩代码,以及其他优化
})
```

2. package.json添加build打包命令脚本
在package.json的scripts中添加build打包命令
```
"scripts": {
    "dev": "webpack-dev-server -c build/webpack.dev.js",
    "build": "webpack -c build/webpack.prod.js"
},
```
执行npm run build,最终打包在dist文件中, 打包结果:
```
dist                    
├── static
|   ├── js
|     ├── main.js
├── index.html
```
3. 浏览器查看打包结果
打包后的dist文件可以在本地借助node服务器serve打开,全局安装serve
```
npm i serve -g
```
然后在项目根目录命令行执行serve -s dist,就可以启动打包后的项目了。

到现在一个基础的支持react和ts的webpack5就配置好了,但只有这些功能是远远不够的,还需要继续添加其他配置。

#### 四. 基础功能配置
##### 4.1 配置环境变量
环境变量按作用来分分两种
1. 区分是开发模式还是打包构建模式
2. 区分项目业务环境,开发/测试/预测/正式环境
区分开发模式还是打包构建模式可以用process.env.NODE_ENV,因为很多第三方包里面判断都是采用的这个环境变量。

区分项目接口环境可以自定义一个环境变量process.env.BASE_ENV,设置环境变量可以借助[cross-env](https://www.npmjs.com/package/cross-env)和[webpack.DefinePlugin](https://www.webpackjs.com/plugins/define-plugin/)来设置。
- cross-env：兼容各系统的设置环境变量的包
- webpack.DefinePlugin：webpack内置的插件,可以为业务代码注入环境变量

安装cross-env
```
npm i cross-env -D
```
修改package.json的scripts脚本字段,删除原先的dev和build,改为

dev开头是开发模式,build开头是打包模式,冒号后面对应的dev/test/pre/prod是对应的业务环境的开发/测试/预测/正式环境。

process.env.NODE_ENV环境变量webpack会自动根据设置的mode字段来给业务代码注入对应的development和prodction,这里在命令中再次设置环境变量NODE_ENV是为了在webpack和babel的配置文件中访问到。

在webpack.base.js中打印一下设置的环境变量
```
// webpack.base.js
// ...
console.log('NODE_ENV', process.env.NODE_ENV)
console.log('BASE_ENV', process.env.BASE_ENV)
```
执行npm run build:dev,可以看到打印的信息
```
// NODE_ENV production
// BASE_ENV development
```
当前是打包模式,业务环境是开发环境,这里需要把process.env.BASE_ENV注入到业务代码里面,就可以通过该环境变量设置对应环境的接口地址和其他数据,要借助webpack.DefinePlugin插件。

修改webpack.base.js

```
// webpack.base.js
// ...
const webpack = require('webpack')
module.export = {
  // ...
  plugins: [
    // ...
    new webpack.DefinePlugin({
      'process.env.BASE_ENV': JSON.stringify(process.env.BASE_ENV)
    })
  ]
}
```
配置后会把值注入到业务代码里面去,webpack解析代码匹配到process.env.BASE_ENV,就会设置到对应的值。测试一下，在src/index.tsx打印一下两个环境变量
```
// src/index.tsx
// ...
console.log('NODE_ENV', process.env.NODE_ENV)
console.log('BASE_ENV', process.env.BASE_ENV)
```
执行npm run dev:test,可以在浏览器控制台看到打印的信息
```
// NODE_ENV development
// BASE_ENV test
```
当前是开发模式,业务环境是测试环境。

4.2 处理css和less文件

在src下新增app.css
在src/App.tsx中引入app.css
```
import React from 'react'
import './app.css'

function App() {
  return <h2>webpack5-rea11ct-ts</h2>
}
export default App
```
执行打包命令npm run build:dev,会发现有报错, 因为webpack默认只认识js,是不识别css文件的,需要使用loader来解析css, 安装依赖
```
npm i style-loader css-loader -D
```

- style-loader: 把解析后的css代码从js中抽离,放到头部的style标签中(在运行时做的)
- css-loader: 解析css文件代码
因为解析css的配置开发和打包环境都会用到,所以加在公共配置webpack.base.js中
```
// webpack.base.js
// ...
module.exports = {
  // ...
  module: { 
    rules: [
      // ...
      {
        test: /.css$/, //匹配 css 文件
        use: ['style-loader','css-loader']
      }
    ]
  },
  // ...
}
```
上面提到过,loader执行顺序是从右往左,从下往上的,匹配到css文件后先用css-loader解析css, 最后借助style-loader把css插入到头部style标签中。
配置完成后再npm run build:dev打包,借助serve -s dist启动后在浏览器查看,可以看到样式生效了

4.3 支持less或scss
项目开发中,为了更好的提升开发体验,一般会使用css超集less或者scss,对于这些超集也需要对应的loader来识别解析。以less为例,需要安装依赖:
```
npm i less-loader less -D
```
- less-loader: 解析less文件代码,把less编译为css
- less: less核心

实现支持less也很简单,只需要在rules中添加less文件解析,遇到less文件,使用less-loader解析为css,再进行css解析流程,修改webpack.base.js：

测试一下,新增src/app.less
```
#root {
  h2 {
    font-size: 20px;
  }
}
```
在App.tsx中引入app.less,执行npm run build:dev打包,借助serve -s dist启动项目,可以看到less文件编写的样式编译css后也插入到style标签了了。

4.4 处理css3前缀兼容

虽然css3现在浏览器支持率已经很高了, 但有时候需要兼容一些低版本浏览器,需要给css3加前缀,可以借助插件来自动加前缀, postcss-loader就是来给css3加浏览器前缀的,安装依赖：

```
npm i postcss-loader autoprefixer -D
```

- postcss-loader：处理css时自动加前缀
- autoprefixer：决定添加哪些浏览器前缀到css中

修改webpack.base.js, 在解析css和less的规则中添加配置

```
module.exports = {
  // ...
  module: { 
    rules: [
      // ...
      {
        test: /.(css|less)$/, //匹配 css和less 文件
        use: [
          'style-loader',
          'css-loader',
          // 新增
          {
            loader: 'postcss-loader',
            options: {
              postcssOptions: {
                plugins: ['autoprefixer']
              }
            }
          },
          'less-loader'
        ]
      }
    ]
  },
  // ...
}
```
配置完成后,需要有一份要兼容浏览器的清单,让postcss-loader知道要加哪些浏览器的前缀,在根目录创建 .browserslistrc文件

以兼容到ie9和chrome35版本为例,配置好后,执行npm run build:dev打包,可以看到打包后的css文件已经加上了ie和谷歌内核的前缀

上面可以看到解析css和less有很多重复配置,可以进行提取postcss-loader配置优化一下

postcss.config.js是postcss-loader的配置文件,会自动读取配置,根目录新建postcss.config.js：

```
module.exports = {
  plugins: ['autoprefixer']
}
```

修改webpack.base.js, 取消postcss-loader的options配置

```
// webpack.base.js
// ...
module.exports = {
  // ...
  module: { 
    rules: [
      // ...
      {
        test: /.(css|less)$/, //匹配 css和less 文件
        use: [
          'style-loader',
          'css-loader',
          'postcss-loader',
          'less-loader'
        ]
      },
    ]
  },
  // ...
}
```
提取postcss-loader配置后,再次打包,可以看到依然可以解析css, less文件, css3对应前缀依然存在。

4.5 babel预设处理js兼容

现在js不断新增很多方便好用的标准语法来方便开发,甚至还有非标准语法比如装饰器,都极大的提升了代码可读性和开发效率。但前者标准语法很多低版本浏览器不支持,后者非标准语法所有的浏览器都不支持。需要把最新的标准语法转换为低版本语法,把非标准语法转换为标准语法才能让浏览器识别解析,而babel就是来做这件事的,这里只讲配置,更详细的可以看[Babel 那些事儿](https://juejin.cn/post/6992371845349507108)。


安装依赖
```
npm i babel-loader @babel/core @babel/preset-env core-js -D
```

- babel-loader: 使用 babel 加载最新js代码并将其转换为 ES5（上面已经安装过）
- @babel/corer: babel 编译的核心包
- @babel/preset-env: babel 编译的预设,可以转换目前最新的js标准语法
- core-js: 使用低版本js语法模拟高版本的库,也就是垫片

修改webpack.base.js

```
// webpack.base.js
module.exports = {
  // ...
  module: {
    rules: [
      {
        test: /.(ts|tsx)$/,
        use: {
          loader: 'babel-loader',
          options: {
            // 执行顺序由右往左,所以先处理ts,再处理jsx,最后再试一下babel转换为低版本语法
            presets: [
              [
                "@babel/preset-env",
                {
                  // 设置兼容目标浏览器版本,这里可以不写,babel-loader会自动寻找上面配置好的文件.browserslistrc
                  // "targets": {
                  //  "chrome": 35,
                  //  "ie": 9
                  // },
                   "useBuiltIns": "usage", // 根据配置的浏览器兼容,以及代码中使用到的api进行引入polyfill按需添加
                   "corejs": 3, // 配置使用core-js低版本
                  }
                ],
              '@babel/preset-react',
              '@babel/preset-typescript'
            ]
          }
        }
      }
    ]
  }
}
```
此时再打包就会把语法转换为对应浏览器兼容的语法了。

为了避免webpack配置文件过于庞大,可以把babel-loader的配置抽离出来, 新建babel.config.js文件,使用js作为配置文件,是因为可以访问到process.env.NODE_ENV环境变量来区分是开发还是打包模式。
```
// babel.config.js
module.exports = {
  // 执行顺序由右往左,所以先处理ts,再处理jsx,最后再试一下babel转换为低版本语法
  "presets": [
    [
      "@babel/preset-env",
      {
        // 设置兼容目标浏览器版本,这里可以不写,babel-loader会自动寻找上面配置好的文件.browserslistrc
        // "targets": {
        //  "chrome": 35,
        //  "ie": 9
        // },
        "useBuiltIns": "usage", // 根据配置的浏览器兼容,以及代码中使用到的api进行引入polyfill按需添加
        "corejs": 3 // 配置使用core-js使用的版本
      }
    ],
    "@babel/preset-react",
    "@babel/preset-typescript"
  ]
}
```
移除webpack.base.js中babel-loader的options配置


```
// webpack.base.js
module.exports = {
  // ...
  module: {
    rules: [
      {
        test: /.(ts|tsx)$/,
        use: 'babel-loader'
      },
      // 如果node_moduels中也有要处理的语法，可以把js|jsx文件配置加上
      // {
      //  test: /.(js|jsx)$/,
      //  use: 'babel-loader'
      // }
      // ...
    ]
  }
}
```

4.6 babel处理js非标准语法

现在react主流开发都是函数组件和react-hooks,但有时也会用类组件,可以用装饰器简化代码

新增src/components/Class.tsx组件, 在App.tsx中引入该组件使用

```
import React, { PureComponent } from "react";

// 装饰器为,组件添加age属性
function addAge(Target: Function) {
  Target.prototype.age = 111
}
// 使用装饰圈
@addAge
class Class extends PureComponent {

  age?: number

  render() {
    return (
      <h2>我是类组件---{this.age}</h2>
    )
  }
}

export default Class
```
需要开启一下ts装饰器支持,修改tsconfig.json文件
```
// tsconfig.json
{
  "compilerOptions": {
    // ...
    // 开启装饰器使用
    "experimentalDecorators": true
  }
}
```
上面Class组件代码中使用了装饰器,目前js标准语法是不支持的,现在运行或者打包会报错,不识别装饰器语法,需要借助babel-loader插件,安装依赖

```
npm i @babel/plugin-proposal-decorators -D
```
在babel.config.js中添加插件

```
module.exports = { 
  // ...
  "plugins": [
    ["@babel/plugin-proposal-decorators", { "legacy": true }]
  ]
}
```
现在项目就支持装饰器了。


4.7 复制public文件夹

一般public文件夹都会放一些静态资源,可以直接根据绝对路径引入,比如图片,css,js文件等,不需要webpack进行解析,只需要打包的时候把public下内容复制到构建出口文件夹中,可以借助copy-webpack-plugin插件,安装依赖
```
npm i copy-webpack-plugin -D
```

开发环境已经在devServer中配置了static托管了public文件夹,在开发环境使用绝对路径可以访问到public下的文件,但打包构建时不做处理会访问不到,所以现在需要在打包配置文件webpack.prod.js中新增copy插件配置。

```
// webpack.prod.js
// ..
const path = require('path')
const CopyPlugin = require('copy-webpack-plugin');
module.exports = merge(baseConfig, {
  mode: 'production',
  plugins: [
    // 复制文件插件
    new CopyPlugin({
      patterns: [
        {
          from: path.resolve(__dirname, '../public'), // 复制public下文件
          to: path.resolve(__dirname, '../dist'), // 复制到dist目录中
          filter: source => {
            return !source.includes('index.html') // 忽略index.html
          }
        },
      ],
    }),
  ]
})
```

在上面的配置中,忽略了index.html,因为html-webpack-plugin会以public下的index.html为模板生成一个index.html到dist文件下,所以不需要再复制该文件了。


测试一下,在public中新增一个favicon.ico图标文件,在index.html中引入

```

<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <!-- 绝对路径引入图标文件 -->
  <link data-n-head="ssr" rel="icon" type="image/x-icon" href="/favicon.ico">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>webpack5-react-ts</title>
</head>
<body>
  <!-- 容器节点 -->
  <div id="root"></div>
</body>
</html>
```

再执行npm run build:dev打包,就可以看到public下的favicon.ico图标文件被复制到dist文件中了。

4.8 处理图片文件


对于图片文件,webpack4使用file-loader和url-loader来处理的,但webpack5不使用这两个loader了,而是采用自带的asset-module来处理

修改webpack.base.js,添加图片解析配置


```
module.exports = {
  module: {
    rules: [
      // ...
      {
        test:/.(png|jpg|jpeg|gif|svg)$/, // 匹配图片文件
        type: "asset", // type选择asset
        parser: {
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          }
        },
        generator:{ 
          filename:'static/images/[name][ext]', // 文件输出目录和命名
        },
      },
    ]
  }
}
```
测试一下,准备一张小于10kb的图片和大于10kb的图片,放在src/assets/imgs目录下, 修改App.tsx

```
import React from 'react'
import smallImg from './assets/imgs/5kb.png'
import bigImg from './assets/imgs/22kb.png'
import './app.css'
import './app.less'

function App() {
  return (
    <>
      <img src={smallImg} alt="小于10kb的图片" />
      <img src={bigImg} alt="大于于10kb的图片" />
    </>
  )
}
export default App
```
> 这个时候在引入图片的地方会报：找不到模块“./assets/imgs/22kb.png”或其相应的类型声明，需要添加一个图片的声明文件

新增src/images.d.ts文件，添加内容
```
declare module '*.svg'
declare module '*.png'
declare module '*.jpg'
declare module '*.jpeg'
declare module '*.gif'
declare module '*.bmp'
declare module '*.tiff'
declare module '*.less'
declare module '*.css'
```
添加图片声明文件后,就可以正常引入图片了, 然后执行npm run build:dev打包,借助serve -s dist查看效果,可以看到可以正常解析图片了,并且小于10kb的图片被转成了base64位格式的。

css中的背景图片一样也可以解析,修改app.tsx。

```
import React from 'react'
import smallImg from './assets/imgs/5kb.png'
import bigImg from './assets/imgs/22kb.png'
import './app.css'
import './app.less'

function App() {
  return (
    <>
      <img src={smallImg} alt="小于10kb的图片" />
      <img src={bigImg} alt="大于于10kb的图片" />
      <div className='smallImg'></div> {/* 小图片背景容器 */}
      <div className='bigImg'></div> {/* 大图片背景容器 */}
    </>
  )
}
export default App
```

修改app.less

```
// app.less
#root {
  .smallImg {
    width: 69px;
    height: 75px;
    background: url('./assets/imgs/5kb.png') no-repeat;
  }
  .bigImg {
    width: 232px;
    height: 154px;
    background: url('./assets/imgs/22kb.png') no-repeat;
  }
}
```
可以看到背景图片也一样可以识别,小于10kb转为base64位。

4.9 处理字体和媒体文件
字体文件和媒体文件这两种资源处理方式和处理图片是一样的,只需要把匹配的路径和打包后放置的路径修改一下就可以了。修改webpack.base.js文件：

```
// webpack.base.js
module.exports = {
  module: {
    rules: [
      // ...
      {
        test:/.(woff2?|eot|ttf|otf)$/, // 匹配字体图标文件
        type: "asset", // type选择asset
        parser: {
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          }
        },
        generator:{ 
          filename:'static/fonts/[name][ext]', // 文件输出目录和命名
        },
      },
      {
        test:/.(mp4|webm|ogg|mp3|wav|flac|aac)$/, // 匹配媒体文件
        type: "asset", // type选择asset
        parser: {
          dataUrlCondition: {
            maxSize: 10 * 1024, // 小于10kb转base64位
          }
        },
        generator:{ 
          filename:'static/media/[name][ext]', // 文件输出目录和命名
        },
      },
    ]
  }
}
```

#### 五. 配置react模块热更新

热更新上面已经在devServer中配置hot为true, 在webpack4中,还需要在插件中添加了HotModuleReplacementPlugin,在webpack5中,只要devServer.hot为true了,该插件就已经内置了。


现在开发模式下修改css和less文件，页面样式可以在不刷新浏览器的情况实时生效，因为此时样式都在style标签里面，style-loader做了替换样式的热替换功能。但是修改App.tsx,浏览器会自动刷新后再显示修改后的内容,但我们想要的不是刷新浏览器,而是在不需要刷新浏览器的前提下模块热更新,并且能够保留react组件的状态。


可以借助@pmmmwh/react-refresh-webpack-plugin插件来实现,该插件又依赖于react-refresh, 安装依赖：
```
npm i @pmmmwh/react-refresh-webpack-plugin react-refresh -D
```
配置react热更新插件,修改webpack.dev.js

```
// webpack.dev.js
const ReactRefreshWebpackPlugin = require('@pmmmwh/react-refresh-webpack-plugin');

module.exports = merge(baseConfig, {
  // ...
  plugins: [
    new ReactRefreshWebpackPlugin(), // 添加热更新插件
  ]
})
```
为babel-loader配置react-refesh刷新插件,修改babel.config.js文件

```
const isDEV = process.env.NODE_ENV === 'development' // 是否是开发模式
module.exports = {
  // ...
  "plugins": [
    isDEV && require.resolve('react-refresh/babel'), // 如果是开发模式,就启动react热更新插件
    // ...
  ].filter(Boolean) // 过滤空值
}
```
测试一下,修改App.tsx代码

```
import React, { useState } from 'react'

function App() {
  const [ count, setCounts ] = useState('')
  const onChange = (e: any) => {
    setCounts(e.target.value)
  }
  return (
    <>
      <h2>webpack5+react+ts</h2>
      <p>受控组件</p>
      <input type="text" value={count} onChange={onChange} />
      <br />
      <p>非受控组件</p>
      <input type="text" />
    </>
  )
}
export default App
```
在两个输入框分别输入内容后,修改App.tsx中h2标签的文本,会发现在不刷新浏览器的情况下,页面内容进行了热更新,并且react组件状态也会保留。


> 新增或者删除页面hooks时,热更新时组件状态不会保留。


#### 六. 优化构建速度
6.1 构建耗时分析
当进行优化的时候,肯定要先知道时间都花费在哪些步骤上了,而speed-measure-webpack-plugin插件可以帮我们做到,安装依赖：
```
npm i speed-measure-webpack-plugin -D
```
使用的时候为了不影响到正常的开发/打包模式,我们选择新建一个配置文件,新增webpack构建分析配置文件build/webpack.analy.js
```
const prodConfig = require('./webpack.prod.js') // 引入打包配置
const SpeedMeasurePlugin = require('speed-measure-webpack-plugin'); // 引入webpack打包速度分析插件
const smp = new SpeedMeasurePlugin(); // 实例化分析插件
const { merge } = require('webpack-merge') // 引入合并webpack配置方法

// 使用smp.wrap方法,把生产环境配置传进去,由于后面可能会加分析配置,所以先留出合并空位
module.exports = smp.wrap(merge(prodConfig, {

}))
```
修改package.json添加启动webpack打包分析脚本命令,在scripts新增：
```
{
  // ...
  "scripts": {
    // ...
    "build:analy": "cross-env NODE_ENV=production BASE_ENV=production webpack -c build/webpack.analy.js"
  }
  // ...
}
```
执行npm run build:analy命令

可以在图中看到各plugin和loader的耗时时间,现在因为项目内容比较少,所以耗时都比较少,在真正的项目中可以通过这个来分析打包时间花费在什么地方,然后来针对性的优化。

6.2 开启持久化存储缓存

在webpack5之前做缓存是使用babel-loader缓存解决js的解析结果,cache-loader缓存css等资源的解析结果,还有模块缓存插件hard-source-webpack-plugin,配置好缓存后第二次打包,通过对文件做哈希对比来验证文件前后是否一致,如果一致则采用上一次的缓存,可以极大地节省时间。

webpack5 较于 webpack4,新增了持久化缓存、改进缓存算法等优化,通过配置 webpack 持久化缓存,来缓存生成的 webpack 模块和 chunk,改善下一次打包的构建速度,可提速 90% 左右,配置也简单，修改webpack.base.js
```
// webpack.base.js
// ...
module.exports = {
  // ...
  cache: {
    type: 'filesystem', // 使用文件缓存
  },
}
```
通过开启webpack5持久化存储缓存,再次打包的时间提升了90%。

缓存的存储位置在node_modules/.cache/webpack,里面又区分了development和production缓存

6.3 开启多线程loader

webpack的loader默认在单线程执行,现代电脑一般都有多核cpu,可以借助多核cpu开启多线程loader解析,可以极大地提升loader解析的速度,thread-loader就是用来开启多进程解析loader的,安装依赖

```
npm i thread-loader -D
```
使用时,需将此 loader 放置在其他 loader 之前。放置在此 loader 之后的 loader 会在一个独立的 worker 池中运行。

修改webpack.base.js

```
// webpack.base.js
module.exports = {
  // ...
  module: {
    rules: [
      {
        test: /.(ts|tsx)$/,
        use: ['thread-loader', 'babel-loader']
      }
    ]
  }
}
```

由于thread-loader不支持抽离css插件MiniCssExtractPlugin.loader(下面会讲),所以这里只配置了多进程解析js,开启多线程也是需要启动时间,大约600ms左右,所以适合规模比较大的项目。

6.4 配置alias别名

webpack支持设置别名alias,设置别名可以让后续引用的地方减少路径的复杂度。

修改webpack.base.js

```
module.export = {
  // ...
   resolve: {
    // ...
    alias: {
      '@': path.join(__dirname, '../src')
    }
  }
}
```

修改tsconfig.json,添加baseUrl和paths

```
{
  "compilerOptions": {
    // ...
    "baseUrl": ".",
    "paths": {
      "@/*": [
        "src/*"
      ]
    }
  }
}
```
配置修改完成后,在项目中使用 @/xxx.xx,就会指向项目中src/xxx.xx,在js/ts文件和css文件中都可以用

src/App.tsx app.less 可以修改为
```
import React from 'react'
import smallImg from '@/assets/imgs/5kb.png'
import bigImg from '@/assets/imgs/22kb.png'
import '@/app.css'
import '@/app.less'

function App() {
  return (
    <>
      <img src={smallImg} alt="小于10kb的图片" />
      <img src={bigImg} alt="大于于10kb的图片" />
      <div className='smallImg'></div> {/* 小图片背景容器 */}
      <div className='bigImg'></div> {/* 大图片背景容器 */}
    </>
  )
}
export default App

// app.less
#root {
  .smallImg {
    width: 69px;
    height: 75px;
    background: url('@/assets/imgs/5kb.png') no-repeat;
  }
  .bigImg {
    width: 232px;
    height: 154px;
    background: url('@/assets/imgs/22kb.png') no-repeat;
  }
}
```

6.5 缩小loader作用范围

一般第三库都是已经处理好的,不需要再次使用loader去解析,可以按照实际情况合理配置loader的作用范围,来减少不必要的loader解析,节省时间,通过使用 include和exclude 两个配置项,可以实现这个功能,常见的例如：

* include：只解析该选项配置的模块
* exclude：不解该选项配置的模块,优先级更高

修改webpack.base.js

```
// webpack.base.js
const path = require('path')
module.exports = {
  // ...
  module: {
    rules: [
      {
        include: [path.resolve(__dirname, '../src')], 只对项目src文件的ts,tsx进行loader解析
        test: /.(ts|tsx)$/,
        use: ['thread-loader', 'babel-loader']
      }
    ]
  }
}
```
其他loader也是相同的配置方式,如果除src文件外也还有需要解析的,就把对应的目录地址加上就可以了,比如需要引入antd的css,可以把antd的文件目录路径添加解析css规则到include里面。

6.6 精确使用loader
loader在webpack构建过程中使用的位置是在webpack构建模块依赖关系引入新文件时，会根据文件后缀来倒序遍历rules数组，如果文件后缀和test正则匹配到了，就会使用该rule中配置的loader依次对文件源代码进行处理，最终拿到处理后的sourceCode结果，可以通过避免使用无用的loader解析来提升构建速度，比如使用less-loader解析css文件。

可以拆分上面配置的less和css, 避免让less-loader再去解析css文件

```
// webpack.base.js
// ...
module.exports = {
  module: {
    // ...
    rules: [
      // ...
      {
        test: /.css$/, //匹配所有的 css 文件
        include: [path.resolve(__dirname, '../src')],
        use: [
          'style-loader',
          'css-loader',
          'postcss-loader'
        ]
      },
      {
        test: /.less$/, //匹配所有的 less 文件
        include: [path.resolve(__dirname, '../src')],
        use: [
          'style-loader',
          'css-loader',
          'postcss-loader',
          'less-loader'
        ]
      },
    ]
  }
}
```
ts和tsx也是如此，ts里面是不能写jsx语法的，所以可以尽可能避免使用 @babel/preset-react对 .ts 文件语法做处理。

6.7 缩小模块搜索范围

node里面模块有三种

  node核心模块
  node_modules模块
  自定义文件模块

  使用require和import引入模块时如果有准确的相对或者绝对路径,就会去按路径查询,如果引入的模块没有路径,会优先查询node核心模块,如果没有找到会去当前目录下node_modules中寻找,如果没有找到会查从父级文件夹查找node_modules,一直查到系统node全局模块。
这样会有两个问题,一个是当前项目没有安装某个依赖,但是上一级目录下node_modules或者全局模块有安装,就也会引入成功,但是部署到服务器时可能就会找不到造成报错,另一个问题就是一级一级查询比较消耗时间。可以告诉webpack搜索目录范围,来规避这两个问题。

修改webpack.base.js
```
// webpack.base.js
const path = require('path')
module.exports = {
  // ...
  resolve: {
     // ...
     modules: [path.resolve(__dirname, '../node_modules')], // 查找第三方模块只在本项目的node_modules中查找
  },
}
```

6.8 devtool 配置

开发过程中或者打包后的代码都是webpack处理后的代码,如果进行调试肯定希望看到源代码,而不是编译后的代码, source map就是用来做源码映射的,不同的映射模式会明显影响到构建和重新构建的速度, devtool选项就是webpack提供的选择源码映射方式的配置。
devtool的命名规则为 ^(inline-|hidden-|eval-)?(nosources-)?(cheap-(module-)?)?source-map$

| 关键字 | 描述 |
-|-
inline | 代码内通过 dataUrl 形式引入 SourceMap 
hidden | 生成 SourceMap 文件,但不使用
eval | eval(...) 形式执行代码,通过 dataUrl 形式引入 SourceMap
nosources | 不生成 SourceMap
cheap | 只需要定位到行信息,不需要列信息
module | 展示源代码中的错误位置

开发环境推荐：eval-cheap-module-source-map

* 本地开发首次打包慢点没关系,因为 eval 缓存的原因, 热更新会很快
* 开发中,我们每行代码不会写的太长,只需要定位到行就行,所以加上 cheap
* 我们希望能够找到源代码的错误,而不是打包后的,所以需要加上 module

修改webpack.dev.js
```
// webpack.dev.js
module.exports = {
  // ...
  devtool: 'eval-cheap-module-source-map'
}
```
打包环境推荐：none(就是不配置devtool选项了，不是配置devtool: 'none')

```
// webpack.prod.js
module.exports = {
  // ...
  // devtool: '', // 不用配置devtool此项
}
```
- none话调试只能看到编译后的代码,也不会泄露源代码,打包速度也会比较快。
- 只是不方便线上排查问题, 但一般都可以根据报错信息在本地环境很快找出问题所在。

6.9 其他优化配置

除了上面的配置外，webpack还提供了其他的一些优化方式,本次搭建没有使用到，所以只简单罗列下

- externals: 外包拓展，打包时会忽略配置的依赖，会从上下文中寻找对应变量
- module.noParse: 匹配到设置的模块,将不进行依赖解析，适合jquery,boostrap这类不依赖外部模块的包
- ignorePlugin: 可以使用正则忽略一部分文件，常在使用多语言的包时可以把非中文语言包过滤掉




#### 七. 优化构建结果文件

7.1 webpack包分析工具


webpack-bundle-analyzer是分析webpack打包后文件的插件,使用交互式可缩放树形图可视化 webpack 输出文件的大小。通过该插件可以对打包后的文件进行观察和分析,可以方便我们对不完美的地方针对性的优化,安装依赖：

```
npm install webpack-bundle-analyzer -D
```
修改webpack.analy.js
```
// webpack.analy.js
const prodConfig = require('./webpack.prod.js')
const SpeedMeasurePlugin = require('speed-measure-webpack-plugin');
const smp = new SpeedMeasurePlugin();
const { merge } = require('webpack-merge')
const { BundleAnalyzerPlugin } = require('webpack-bundle-analyzer') // 引入分析打包结果插件
module.exports = smp.wrap(merge(prodConfig, {
  plugins: [
    new BundleAnalyzerPlugin() // 配置分析打包结果插件
  ]
}))
```
配置好后,执行npm run build:analy命令,打包完成后浏览器会自动打开窗口,可以看到打包文件的分析结果页面,可以看到各个文件所占的资源大小。

7.2 抽取css样式文件


在开发环境我们希望css嵌入在style标签里面,方便样式热替换,但打包时我们希望把css单独抽离出来,方便配置缓存策略。而插件mini-css-extract-plugin就是来帮我们做这件事的,安装依赖：

```
npm i mini-css-extract-plugin -D
```

修改webpack.base.js, 根据环境变量设置开发环境使用style-looader,打包模式抽离css

```
// webpack.base.js
// ...
const MiniCssExtractPlugin = require('mini-css-extract-plugin')
const isDev = process.env.NODE_ENV === 'development' // 是否是开发模式
module.exports = {
  // ...
  module: { 
    rules: [
      // ...
      {
        test: /.css$/, //匹配所有的 css 文件
        include: [path.resolve(__dirname, '../src')],
        use: [
          isDev ? 'style-loader' : MiniCssExtractPlugin.loader, // 开发环境使用style-looader,打包模式抽离css
          'css-loader',
          'postcss-loader'
        ]
      },
      {
        test: /.less$/, //匹配所有的 less 文件
        include: [path.resolve(__dirname, '../src')],
        use: [
          isDev ? 'style-loader' : MiniCssExtractPlugin.loader, // 开发环境使用style-looader,打包模式抽离css
          'css-loader',
          'postcss-loader',
          'less-loader'
        ]
      },
    ]
  },
  // ...
}
```

再修改webpack.prod.js, 打包时添加抽离css插件

```
// webpack.prod.js
// ...
const MiniCssExtractPlugin = require('mini-css-extract-plugin')
module.exports = merge(baseConfig, {
  mode: 'production',
  plugins: [
    // ...
    // 抽离css插件
    new MiniCssExtractPlugin({
      filename: 'static/css/[name].css' // 抽离css的输出目录和名称
    }),
  ]
})
```

配置完成后,在开发模式css会嵌入到style标签里面,方便样式热替换,打包时会把css抽离成单独的css文件。

7.3 压缩css文件

上面配置了打包时把css抽离为单独css文件的配置,打开打包后的文件查看,可以看到默认css是没有压缩的,需要手动配置一下压缩css的插件。

可以借助css-minimizer-webpack-plugin来压缩css,安装依赖


```
npm i css-minimizer-webpack-plugin -D
```

修改webpack.prod.js文件， 需要在优化项optimization下的minimizer属性中配置
```
// webpack.prod.js
// ...
const CssMinimizerPlugin = require('css-minimizer-webpack-plugin')
module.exports = {
  // ...
  optimization: {
    minimizer: [
      new CssMinimizerPlugin(), // 压缩css
    ],
  },
}
```
再次执行打包就可以看到css已经被压缩了。

7.4 压缩js文件

设置mode为production时,webpack会使用内置插件terser-webpack-plugin压缩js文件,该插件默认支持多线程压缩,但是上面配置optimization.minimizer压缩css后,js压缩就失效了,需要手动再添加一下,webpack内部安装了该插件,由于pnpm解决了幽灵依赖问题,如果用的pnpm的话,需要手动再安装一下依赖。


```
npm i terser-webpack-plugin -D
```
修改webpack.prod.js文件

```
// ...
const TerserPlugin = require('terser-webpack-plugin')
module.exports = {
  // ...
  optimization: {
    minimizer: [
      // ...
      new TerserPlugin({ // 压缩js
        parallel: true, // 开启多线程压缩
        terserOptions: {
          compress: {
            pure_funcs: ["console.log"] // 删除console.log
          }
        }
      }),
    ],
  },
}
```

配置完成后再打包,css和js就都可以被压缩了。

7.5 合理配置打包文件hash
项目维护的时候,一般只会修改一部分代码,可以合理配置文件缓存,来提升前端加载页面速度和减少服务器压力,而hash就是浏览器缓存策略很重要的一部分。webpack打包的hash分三种：

- hash：跟整个项目的构建相关,只要项目里有文件更改,整个项目构建的hash值都会更改,并且全部文件都共用相同的hash值
- chunkhash：不同的入口文件进行依赖文件解析、构建对应的chunk,生成对应的哈希值,文件本身修改或者依赖文件修改,chunkhash值会变化
- contenthash：每个文件自己单独的 hash 值,文件的改动只会影响自身的 hash 值

hash是在输出文件时配置的,格式是filename: "[name].[chunkhash:8][ext]",[xx] 格式是webpack提供的占位符, :8是生成hash的长度。

| 占位符 | 解释 |
-|-
ext | 文件后缀名
name | 文件名
path | 文件相对路径
folder | 文件所在文件夹
hash | 每次构建生成的唯一 hash 值
chunkhash | 根据 chunk 生成 hash 值
contenthash | 根据文件内容生成hash 值

因为js我们在生产环境里会把一些公共库和程序入口文件区分开,单独打包构建,采用chunkhash的方式生成哈希值,那么只要我们不改动公共库的代码,就可以保证其哈希值不会受影响,可以继续使用浏览器缓存,所以js适合使用chunkhash。

css和图片资源媒体资源一般都是单独存在的,可以采用contenthash,只有文件本身变化后会生成新hash值

修改webpack.base.js,把js输出的文件名称格式加上chunkhash,把css和图片媒体资源输出格式加上contenthash

```
// webpack.base.js
// ...
module.exports = {
  // 打包文件出口
  output: {
    filename: 'static/js/[name].[chunkhash:8].js', // // 加上[chunkhash:8]
    // ...
  },
  module: {
    rules: [
      {
        test:/.(png|jpg|jpeg|gif|svg)$/, // 匹配图片文件
        // ...
        generator:{ 
          filename:'static/images/[name].[contenthash:8][ext]' // 加上[contenthash:8]
        },
      },
      {
        test:/.(woff2?|eot|ttf|otf)$/, // 匹配字体文件
        // ...
        generator:{ 
          filename:'static/fonts/[name].[contenthash:8][ext]', // 加上[contenthash:8]
        },
      },
      {
        test:/.(mp4|webm|ogg|mp3|wav|flac|aac)$/, // 匹配媒体文件
        // ...
        generator:{ 
          filename:'static/media/[name].[contenthash:8][ext]', // 加上[contenthash:8]
        },
      },
    ]
  },
  // ...
}
```

再修改webpack.prod.js,修改抽离css文件名称格式

```
// webpack.prod.js
// ...
const MiniCssExtractPlugin = require('mini-css-extract-plugin')
module.exports = merge(baseConfig, {
  mode: 'production',
  plugins: [
    // 抽离css插件
    new MiniCssExtractPlugin({
      filename: 'static/css/[name].[contenthash:8].css' // 加上[contenthash:8]
    }),
    // ...
  ],
  // ...
})
```
再次打包就可以看到文件后面的hash了

7.6 代码分割第三方包和公共模块

一般第三方包的代码变化频率比较小,可以单独把node_modules中的代码单独打包, 当第三包代码没变化时,对应chunkhash值也不会变化,可以有效利用浏览器缓存，还有公共的模块也可以提取出来,避免重复打包加大代码整体体积, webpack提供了代码分隔功能, 需要我们手动在优化项optimization中手动配置下代码分隔splitChunks规则。

修改webpack.prod.js
```
module.exports = {
  // ...
  optimization: {
    // ...
    splitChunks: { // 分隔代码
      cacheGroups: {
        vendors: { // 提取node_modules代码
          test: /node_modules/, // 只匹配node_modules里面的模块
          name: 'vendors', // 提取文件命名为vendors,js后缀和chunkhash会自动加
          minChunks: 1, // 只要使用一次就提取出来
          chunks: 'initial', // 只提取初始化就能获取到的模块,不管异步的
          minSize: 0, // 提取代码体积大于0就提取出来
          priority: 1, // 提取优先级为1
        },
        commons: { // 提取页面公共代码
          name: 'commons', // 提取文件命名为commons
          minChunks: 2, // 只要使用两次就提取出来
          chunks: 'initial', // 只提取初始化就能获取到的模块,不管异步的
          minSize: 0, // 提取代码体积大于0就提取出来
        }
      }
    }
  }
}
```
配置完成后执行打包,可以看到node_modules里面的模块被抽离到verdors.ec725ef1.js中,业务代码在main.9a6bf38a.js中。

测试一下,此时verdors.js的chunkhash是ec725ef1,main.js文件的chunkhash是9a6bf38a,改动一下App.tsx,再次打包,可以看到下图main.js的chunkhash值变化了,但是vendors.js的chunkhash还是原先的,这样发版后,浏览器就可以继续使用缓存中的verdors.ec725ef1.js,只需要重新请求main.js就可以了。

7.7 tree-shaking清理未引用js
Tree Shaking的意思就是摇树,伴随着摇树这个动作,树上的枯叶都会被摇晃下来,这里的tree-shaking在代码中摇掉的是未使用到的代码,也就是未引用的代码,最早是在rollup库中出现的,webpack在2版本之后也开始支持。模式mode为production时就会默认开启tree-shaking功能以此来标记未引入代码然后移除掉,测试一下。

在src/components目录下新增Demo1,Demo2两个组件

```
// src/components/Demo1.tsx
import React from "react";
function Demo1() {
  return <h3>我是Demo1组件</h3>
}
export default Demo1

// src/components/Demo2.tsx
import React from "react";
function Demo2() {
  return <h3>我是Demo2组件</h3>
}
export default Demo2
```

再在src/components目录下新增index.ts, 把Demo1和Demo2组件引入进来再暴露出去

```
// src/components/index.ts
export { default as Demo1 } from './Demo1'
export { default as Demo2 } from './Demo2'
```
在App.tsx中引入两个组件,但只使用Demo1组件
```
// ...
import { Demo1, Demo2 } from '@/components'

function App() {
  return <Demo1 />
}
export default App
```
执行打包,可以看到在main.js中搜索Demo,只搜索到了Demo1, 代表Demo2组件被tree-shaking移除掉了。


7.8 tree-shaking清理未使用css

js中会有未使用到的代码,css中也会有未被页面使用到的样式,可以通过purgecss-webpack-plugin插件打包的时候移除未使用到的css样式,这个插件是和mini-css-extract-plugin插件配合使用的,在上面已经安装过,还需要glob-all来选择要检测哪些文件里面的类名和id还有标签名称, 安装依赖:

```
npm i purgecss-webpack-plugin glob-all -D
```

修改webpack.prod.js
```
// webpack.prod.js
// ...
const globAll = require('glob-all')
const PurgeCSSPlugin = require('purgecss-webpack-plugin')
const MiniCssExtractPlugin = require('mini-css-extract-plugin')
module.exports = {
  // ...
  plugins: [
    // 抽离css插件
    new MiniCssExtractPlugin({
      filename: 'static/css/[name].[contenthash:8].css'
    }),
    // 清理无用css
    new PurgeCSSPlugin({
      // 检测src下所有tsx文件和public下index.html中使用的类名和id和标签名称
      // 只打包这些文件中用到的样式
      paths: globAll.sync([
        `${path.join(__dirname, '../src')}/**/*.tsx`,
        path.join(__dirname, '../public/index.html')
      ]),
    }),
  ]
}
```

测试一下,用上面配置解析图片文件代码拿过来,修改App.tsx
```
import React from 'react'
import './app.css'
import './app.less'

function App() {
  return (
    <>
      <div className='smallImg'></div>
      <div className='bigImg'></div>
    </>
  )
}
export default App
```
App.tsx中有两个div,类名分别是smallImg和bigImg,当前app.less代码为
```
#root {
  .smallImg {
    width: 69px;
    height: 75px;
    background: url('./assets/imgs/5kb.png') no-repeat;
  }
  .bigImg {
    width: 232px;
    height: 154px;
    background: url('./assets/imgs/22kb.png') no-repeat;
  }
}
```

此时先执行一下打包,查看main.css

因为页面中中有h2标签, smallImg和bigImg类名,所以打包后的css也有,此时修改一下app.less中的 .smallImg为 .smallImg1,此时 .smallImg1就是无用样式了,因为没有页面没有类名为 .smallImg1的节点,再打包后查看 main.css

可以看到main.css已经没有 .smallImg1类名的样式了,做到了删除无用css的功能。

但是purgecss-webpack-plugin插件不是全能的,由于项目业务代码的复杂,插件不能百分百识别哪些样式用到了,哪些没用到,所以请不要寄希望于它能够百分百完美解决你的问题,这个是不现实的。
插件本身也提供了一些白名单safelist属性,符合配置规则选择器都不会被删除掉,比如使用了组件库antd, purgecss-webpack-plugin插件检测src文件下tsx文件中使用的类名和id时,是检测不到在src中使用antd组件的类名的,打包的时候就会把antd的类名都给过滤掉,可以配置一下安全选择列表,避免删除antd组件库的前缀ant。

```
new PurgeCSSPlugin({
  // ...
  safelist: {
    standard: [/^ant-/], // 过滤以ant-开头的类名，哪怕没用到也不删除
  }
})
```

7.9 资源懒加载

像react,vue等单页应用打包默认会打包到一个js文件中,虽然使用代码分割可以把node_modules模块和公共模块分离,但页面初始加载还是会把整个项目的代码下载下来,其实只需要公共资源和当前页面的资源就可以了,其他页面资源可以等使用到的时候再加载,可以有效提升首屏加载速度。
webpack默认支持资源懒加载,只需要引入资源使用import语法来引入资源,webpack打包的时候就会自动打包为单独的资源文件,等使用到的时候动态加载。
以懒加载组件和css为例,新建懒加载组件src/components/LazyDemo.tsx

```
import React from "react";

function LazyDemo() {
  return <h3>我是懒加载组件组件</h3>
}

export default LazyDemo
```
修改App.tsx

```
import React, { lazy, Suspense, useState } from 'react'
const LazyDemo = lazy(() => import('@/components/LazyDemo')) // 使用import语法配合react的Lazy动态引入资源

function App() {
  const [ show, setShow ] = useState(false)
  
  // 点击事件中动态引入css, 设置show为true
  const onClick = () => {
    import('./app.css')
    setShow(true)
  }
  return (
    <>
      <h2 onClick={onClick}>展示</h2>
      {/* show为true时加载LazyDemo组件 */}
      { show && <Suspense fallback={null}><LazyDemo /></Suspense> }
    </>
  )
}
export default App
```

点击展示文字时,才会动态加载app.css和LazyDemo组件的资源。

7.10 资源预加载
上面配置了资源懒加载后,虽然提升了首屏渲染速度,但是加载到资源的时候会有一个去请求资源的延时,如果资源比较大会出现延迟卡顿现象,可以借助link标签的rel属性prefetch与preload,link标签除了加载css之外也可以加载js资源,设置rel属性可以规定link提前加载资源,但是加载资源后不执行,等用到了再执行。

rel的属性值

- preload是告诉浏览器页面必定需要的资源,浏览器一定会加载这些资源。
- prefetch是告诉浏览器页面可能需要的资源,浏览器不一定会加载这些资源,会在空闲时加载。

对于当前页面很有必要的资源使用 preload ,对于可能在将来的页面中使用的资源使用 prefetch


webpack v4.6.0+ 增加了对预获取和预加载的支持,使用方式也比较简单,在import引入动态资源时使用webpack的魔法注释

```
// 单个目标
import(
  /* webpackChunkName: "my-chunk-name" */ // 资源打包后的文件chunkname
  /* webpackPrefetch: true */ // 开启prefetch预获取
  /* webpackPreload: true */ // 开启preload预获取
  './module'
);
```

测试一下,在src/components目录下新建PreloadDemo.tsx, PreFetchDemo.tsx

```
// src/components/PreloadDemo.tsx
import React from "react";
function PreloadDemo() {
  return <h3>我是PreloadDemo组件</h3>
}
export default PreloadDemo

// src/components/PreFetchDemo.tsx
import React from "react";
function PreFetchDemo() {
  return <h3>我是PreFetchDemo组件</h3>
}
export default PreFetchDemo
```

修改App.tsx

```
import React, { lazy, Suspense, useState } from 'react'

// prefetch
const PreFetchDemo = lazy(() => import(
  /* webpackChunkName: "PreFetchDemo" */
  /*webpackPrefetch: true*/
  '@/components/PreFetchDemo'
))
// preload
const PreloadDemo = lazy(() => import(
  /* webpackChunkName: "PreloadDemo" */
  /*webpackPreload: true*/
  '@/components/PreloadDemo'
 ))

function App() {
  const [ show, setShow ] = useState(false)

  const onClick = () => {
    setShow(true)
  }
  return (
    <>
      <h2 onClick={onClick}>展示</h2>
      {/* show为true时加载组件 */}
      { show && (
        <>
          <Suspense fallback={null}><PreloadDemo /></Suspense>
          <Suspense fallback={null}><PreFetchDemo /></Suspense>
        </>
      ) }
    </>
  )
}
export default App
```

然后打包后查看效果,页面初始化时预加载了PreFetchDemo.js组件资源,但是不执行里面的代码,等点击展示按钮后从预加载的资源中直接取出来执行,不用再从服务器请求,节省了很多时间。

> 在测试时发现只有js资源设置prefetch模式才能触发资源预加载,preload模式触发不了,css和图片等资源不管设置prefetch还是preload都不能触发,不知道是哪里没配置好。

7.11 打包时生成gzip文件
前端代码在浏览器运行,需要从服务器把html,css,js资源下载执行,下载的资源体积越小,页面加载速度就会越快。一般会采用gzip压缩,现在大部分浏览器和服务器都支持gzip,可以有效减少静态资源文件大小,压缩率在 70% 左右。

nginx可以配置gzip: on来开启压缩,但是只在nginx层面开启,会在每次请求资源时都对资源进行压缩,压缩文件会需要时间和占用服务器cpu资源，更好的方式是前端在打包的时候直接生成gzip资源,服务器接收到请求,可以直接把对应压缩好的gzip文件返回给浏览器,节省时间和cpu。

webpack可以借助compression-webpack-plugin 插件在打包时生成 gzip 文章,安装依赖
```
npm i compression-webpack-plugin -D
```
添加配置,修改webpack.prod.js

```
const glob = require('glob')
const CompressionPlugin  = require('compression-webpack-plugin')
module.exports = {
  // ...
  plugins: [
     // ...
     new CompressionPlugin({
      test: /.(js|css)$/, // 只生成css,js压缩文件
      filename: '[path][base].gz', // 文件命名
      algorithm: 'gzip', // 压缩格式,默认是gzip
      test: /.(js|css)$/, // 只生成css,js压缩文件
      threshold: 10240, // 只有大小大于该值的资源会被处理。默认值是 10k
      minRatio: 0.8 // 压缩率,默认值是 0.8
    })
  ]
}
```
配置完成后再打包,可以看到打包后js的目录下多了一个 .gz 结尾的文件

因为只有verdors.js的大小超过了10k, 所以只有它生成了gzip压缩文件,借助serve -s dist启动dist,查看verdors.js加载情况

可以看到verdors.js的原始大小是182kb, 使用gzip压缩后的文件只剩下了60.4kb,减少了70% 的大小,可以极大提升页面加载速度。

7.12 友好打包提示

可装可不装的一个美化控制台输出的插件
```
npm i @nuxt/friendly-errors-webpack-plugin webpackbar -D
```
修改 webpack.base.js 配置文件：

```
const FriendlyErrorsWebpackPlugin = require('@nuxt/friendly-errors-webpack-plugin')
const WebpackBar = require('webpackbar');

modules.exports = {
  // ...
  // 进度条
  new WebpackBar({
    name: 'webpack5-ts-react18',
    // react 蓝
    color: '#61dafb',
    basic: false,   // 默认true，启用一个简单的日志报告器
    profile:false,  // 默认false，启用探查器。
  }),
  // 优化控制台输出界面
  new FriendlyErrorsWebpackPlugin({
    compilationSuccessInfo: {
      messages: ['You application is running here http://localhost:3000'],
      notes: ['Some additional notes to be displayed upon successful compilation']
    }
  })
  // ...
};
```

#### 八. 总结

到目前为止已经使用webpack5把react18+ts的开发环境配置完成，并且配置比较完善的保留组件状态的热更新，以及常见的优化构建速度和构建结果的配置.

还有细节需要优化，比如把容易改变的配置单独写个config.js来配置，输出文件路径封装。这篇文章只是配置，如果想学好webpack，还需要学习webpack的构建原理以及loader和plugin的实现机制。

附上上面安装依赖的版本
```
{
  "name": "webpack5-ts-react18",
  "version": "1.0.0",
  "description": "",
  "main": "index.js",
  "scripts": {
    "dev:dev": "cross-env NODE_ENV=development BASE_ENV=development webpack-dev-server -c build/webpack.dev.js",
    "dev:test": "cross-env NODE_ENV=development BASE_ENV=test webpack-dev-server -c build/webpack.dev.js",
    "dev:pre": "cross-env NODE_ENV=development BASE_ENV=pre webpack-dev-server -c build/webpack.dev.js",
    "dev:prod": "cross-env NODE_ENV=development BASE_ENV=production webpack-dev-server -c build/webpack.dev.js",
    "build:dev": "cross-env NODE_ENV=production BASE_ENV=development webpack -c build/webpack.prod.js",
    "build:test": "cross-env NODE_ENV=production BASE_ENV=test webpack -c build/webpack.prod.js",
    "build:pre": "cross-env NODE_ENV=production BASE_ENV=pre webpack -c build/webpack.prod.js",
    "build:prod": "cross-env NODE_ENV=production BASE_ENV=production webpack -c build/webpack.prod.js",
    "build:analy": "cross-env NODE_ENV=production BASE_ENV=production webpack -c build/webpack.analy.js",
    "test": "echo \"Error: no test specified\" && exit 1"
  },
  "keywords": [],
  "author": "",
  "license": "ISC",
  "devDependencies": {
    "@babel/core": "^7.20.2",
    "@babel/plugin-proposal-decorators": "^7.20.2",
    "@babel/preset-env": "^7.20.2",
    "@babel/preset-react": "^7.18.6",
    "@babel/preset-typescript": "^7.18.6",
    "@pmmmwh/react-refresh-webpack-plugin": "^0.5.9",
    "@types/react": "^18.0.25",
    "@types/react-dom": "^18.0.9",
    "autoprefixer": "^10.4.13",
    "babel-loader": "^9.1.0",
    "compression-webpack-plugin": "^10.0.0",
    "copy-webpack-plugin": "^11.0.0",
    "core-js": "^3.26.1",
    "cross-env": "^7.0.3",
    "css-loader": "^6.7.2",
    "css-minimizer-webpack-plugin": "^4.2.2",
    "glob-all": "^3.3.1",
    "html-webpack-plugin": "^5.5.0",
    "less": "^4.1.3",
    "less-loader": "^11.1.0",
    "mini-css-extract-plugin": "^2.6.1",
    "postcss-loader": "^7.0.1",
    "purgecss-webpack-plugin": "^5.0.0",
    "react-refresh": "^0.14.0",
    "speed-measure-webpack-plugin": "^1.5.0",
    "style-loader": "^3.3.1",
    "terser-webpack-plugin": "^5.3.6",
    "thread-loader": "^3.0.4",
    "webpack": "^5.75.0",
    "webpack-bundle-analyzer": "^4.7.0",
    "webpack-cli": "^4.10.0",
    "webpack-dev-server": "^4.11.1",
    "webpack-merge": "^5.8.0"
  },
  "dependencies": {
    "react": "^18.2.0",
    "react-dom": "^18.2.0"
  }
}

```


#### 五  editorconfig统一编辑器配置

由于每个人的vsocde编辑器默认配置可能不一样，比如有的默认缩进是4个空格，有的是2个空格，如果自己编辑器和项目代码缩进不一样，会给开发和项目代码规范带来一定影响，所以需要在项目中为编辑器配置下格式。

5.1 安装vscode插件EditorConfig

打开vsocde插件商店，搜索EditorConfig for VS Code，然后进行安装

5.2 添加.editorconfig配置文件

安装插件后，在根目录新增 .editorconfig配置文件:

```
root = true # 控制配置文件 .editorconfig 是否生效的字段
​
[**] # 匹配全部文件
indent_style = space # 缩进风格，可选space｜tab
indent_size = 2 # 缩进的空格数
charset = utf-8 # 设置字符集
trim_trailing_whitespace = true # 删除一行中的前后空格
insert_final_newline = true # 设为true表示使文件以一个空白行结尾
end_of_line = lf
​
[**.md] # 匹配md文件
trim_trailing_whitespace = false
```
上面的配置可以规范本项目中文件的缩进风格，和缩进空格数等，会覆盖vscode的配置，来达到不同编辑器中代码默认行为一致的作用。


#### 六 prettier自动格式化代码

每个人写代码的风格习惯不一样，比如代码换行，结尾是否带分号，单双引号，缩进等，而且不能只靠口头规范来约束，项目紧急的时候可能会不太注意代码格式，这时候需要有工具来帮我们自动格式化代码，而prettier就是帮我们做这件事的。

6.1 安装vscode插件Prettier
打开vsocde插件商店，搜索Prettier - Code formatter，然后进行安装。

6.2 添加.prettierrc.js配置文件
安装插件后，在根目录新增 .prettierrc.js配置文件，配置内容如下：
```
module.exports = {
  printWidth: 100, // 一行的字符数，如果超过会进行换行
  tabWidth: 2, // 一个tab代表几个空格数，默认就是2
  useTabs: false, // 是否启用tab取代空格符缩进，.editorconfig设置空格缩进，所以设置为false
  semi: false, // 行尾是否使用分号，默认为true
  singleQuote: true, // 字符串是否使用单引号
  trailingComma: 'none', // 对象或数组末尾是否添加逗号 none| es5| all 
  jsxSingleQuote: true, // 在jsx里是否使用单引号，你看着办
  bracketSpacing: true, // 对象大括号直接是否有空格，默认为true，效果：{ foo: bar }
  arrowParens: "avoid", // 箭头函数如果只有一个参数则省略括号
}
```
各个字段配置就是后面注释中的说明

6.3 添加.vscode/settings.json
配置前两步后，虽然已经配置prettier格式化规则，但还需要让vscode来支持保存后触发格式化

在项目根目录新建 .vscode文件夹，内部新建settings.json文件配置文件，内容如下：
```
{
  "search.exclude": {
    "/node_modules": true,
    "dist": true,
    "pnpm-lock.sh": true
  },
  "editor.formatOnSave": true,
  "[javascript]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[javascriptreact]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[typescript]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[typescriptreact]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[json]": {
    "editor.defaultFormatter": "vscode.json-language-features"
  },
  "[html]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[markdown]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[css]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[less]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[scss]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "liveServer.settings.port": 5502
}
```

先配置了忽略哪些文件不进行格式化，又添加了保存代码后触发格式化代码配置，以及各类型文件格式化使用的格式。

这一步配置完成后，修改项目代码，把格式打乱，点击保存时就会看到编辑器自动把代码格式规范化了。

#### 七. eslint+lint-staged检测代码

安装vscode插件ESLint
打开vsocde插件商店，搜索ESLint，然后进行安装。

安装eslint依赖

```
npm i eslint -D
```

配置.eslintrc.js文件

安装eslint后，执行npm init @eslint/config，选择自己需要的配置

这里我们选择了
```
使用eslint检测并问题
项目使用的模块规范是es module
使用的框架是react
使用了typescript
代码选择运行在浏览器端
eslint配置文件使用js格式
是否现在安装相关依赖，选择是
使用pnpm包管理器安装依赖
```
选择完成后会在根目录下生成 .eslint.js文件，默认配置如下

6.4 解决当前项目eslint语法错误


此时eslint基础配置就已经配置好了，此时要解决出现的几个小问题：

看App.tsx页面会发现jsx部分有红色报红，提示 'React' must be in scope when using JSX

这是因为React18版本中使用jsx语法不需要再引入React了，根据eslint-react-plugin中的说明，如果使用了react17版本以上，不需要在使用jsx页面引入React时，在eslint配置文件 .eslint.js的extends字段添加插件plugin:react/jsx-runtime。

此时App.tsx和main.tsx就不会报错了。

看到main.tsx文件带有警告颜色，看警告提示是Forbidden non-null assertion。

这个提示是不允许使用非空操作符!，但实际在项目中经常会用到，所以可以把该项校验给关闭掉。
在eslint配置文件 .eslint.js的rules字段添加插件 '@typescript-eslint/no-non-null-assertion': 'off'。

然后就不会报警告了，如果为了避免代码出现异常，不想关闭该校验，可以提前做判断

.eslintec.js文件也有红色报错，报错是 'module' is not defined.


这个是因为上面选择的浏览器环境，配置文件的module.exports需要node环境，需要在eslint的env环境配置中添加支持node环境
```
  env: {
    browser: true,
    es2021: true,
    node: true
  },
```
到这一步项目目前就没有报错了，在页面里面如果有定义了但未使用的变量，会有eslint的标黄效果提示，如果有语法问题也会警告或者报错提示，等后面出现了其他问题再按照情况进行修复或者调整eslint配置。

6.5 添加eslint语法检测脚本
前面的eslint报错和警告都是我们用眼睛看到的，有时候需要通过脚本执行能检测出来，在package.json的scripts中新增
```
"eslint": "eslint src/**/*.{ts,tsx}"
```

代表检测src目录下以**.ts**,.tsx为后缀的文件，然后在main.tsx里面定义一个未使用的变量a


执行npm run eslint

可以看到黄色代码部分会检测到有变量a没有用到，会报一个警告。
除此之外再解决一个问题就是eslint报的警告React version not specified in eslint-plugin-react settings,需要告诉eslint使用的react版本，在 .eslintrc.js和rules平级添加settings配置，让eslint自己检测react版本,对应issuse

```
 settings: {
    "react": {
      "version": "detect"
    }
  }
```

再执行npm run eslint就不会报这个未设置react版本的警告了。


6.6 使用lint-staged优化eslint检测速度
在上面配置的eslint会检测src文件下所有的 .ts, .tsx文件，虽然功能可以实现，但是当项目文件多的时候，检测的文件会很多，需要的时间也会越来越长，但其实只需要检测提交到暂存区，就是git add添加的文件，不在暂存区的文件不用再次检测，而lint-staged就是来帮我们做这件事情的。
安装依赖（lint-staged最新13版本需要node大于14.13.1版本,此处暂时用12版本，配置是一样的）

```
npm i lint-staged -D
```
修改package.json脚本eslint的配置
```
"eslint": "eslint"
```
在package.json添加lint-staged配置
```
"lint-staged": {
  "src/**/*.{ts,tsx}": [
    "npm run eslint"
  ]
},
```
因为要检测git暂存区代码，所以需要执行git init初始化一下git
```
git init
```
初始化git完成后就可以进行测试了，先提交一下没有语法问题的App.tsx

```
git add src/App.tsx
```

把src/App.tsx提交到暂存区后，执行npx lint-staged

可以看到检测通过了，没有检测到语法问题，再把有语法问题的src/main.tsx文件提交暂存区再检测一下看看

发现虽然main.tsx虽然有eslint语法警告，但依然验证成功了，是因为lint-staged只会检测到语法报错才会有提示只是警告不会，如果需要出现警告也阻止代码提交，需要给eslint检测配置参数 --max-warnings=0

代表允许最多0个警告，就是只要出现警告就会报错，修改完成后再次执行npx lint-staged

就会看到最终执行的命令是

```
eslint --max-warnings=0 "E:/wuyou/react/my-react-app/src/App.tsx" "E:/wuyou/react/my-react-app/src/main.tsx"
```

eslint只检测了git暂存区的App.tsx和main.tsx两个文件，做到了只检测git暂存区中文件的功能，避免每次都全量检测文件。

而添加了 --max-warnings=0 参数后，警告也可以检测出来并报错中断命令行了。


#### 七. 使用tsc检测类型和报错
在项目中使用了ts,但一些类型问题，现在配置的eslint是检测不出来的，需要使用ts提供的tsc工具进行检测，如下示例

在main.tsx定义了函数say,参数name是string类型，当调用传number类型参数时，页面有了明显的ts报错，但此时提交main.tsx文件到暂存区后执行npx lint-staged

发现没有检测到报错，所以需要配置下tsc来检测类型，在package.json添加脚本命令

```
"pre-check": "tsc && npx lint-staged"
```
执行npm run pre-check，发现已经可以检测出类型报错了。

#### 八. 项目git提交时检测语法规范

为了避免把不规范的代码提交到远程仓库，一般会在git提交代码时对代码语法进行检测，只有检测通过时才能被提交，git提供了一系列的githooks ，而我们需要其中的pre-commit钩子，它会在git commit把代码提交到本地仓库之前执行，可以在这个阶段检测代码，如果检测不通过就退出命令行进程停止commit。



.1 代码提交前husky检测语法
而husky就是可以监听githooks的工具，可以借助它来完成这件事情。

8.2 安装husky

```
npm i husky -D
```

8.3 配置husky的pre-commit钩子
生成 .husky配置文件夹（如果项目中没有初始化git,需要先执行git init）
```
npx husky install 
```

会在项目根目录生成 .husky文件夹，生成文件成功后，需要让husky支持监听pre-commit钩子，监听到后执行上面定义的npm run pre-check语法检测。

```
npx husky add .husky/pre-commit 'npm run pre-check'
```

会在 .husky目录下生成pre-commit文件，里面可以看到我们设置的npm run pre-check命令。

然后提交代码进行测试

```
git add .
git commit -m "feat: 测试提交验证"
```


会发现监听pre-commit钩子执行了npm rim pre-check, 使用eslint检测了git暂存区的两个文件，并且发现了main.tsx的警告，退出了命令行，没有执行git commit把暂存区代码提交到本地仓库。

到这里代码提交语法验证就配置完成了，可以有效的保障仓库的代码质量。

#### 九. 代码提交时检测commit备注规范

在提交代码时，良好的提交备注会方便多人开发时其他人理解本次提交修改的大致内容，也方便后面维护迭代，但每个人习惯都不一样，需要用工具来做下限制，在git提供的一系列的githooks 中，commit-msg会在git commit之前执行，并获取到git commit的备注，可以通过这个钩子来验证备注是否合理，而验证是否合理肯定需要先定义一套规范，而commitlint就是用来做这件事情的，它会预先定义一套规范，然后验证git commit的备注是否符合定义的规范。

9.1 安装和配置commitlint
安装commitlint
```
npm i @commitlint/config-conventional @commitlint/cli -D
```

在根目录创建commitlint.config.js文件,添加配置如下
```

module.exports = {
  // 继承的规则
  extends: ['@commitlint/config-conventional'],
  // 定义规则类型
  rules: {
    // type 类型定义，表示 git 提交的 type 必须在以下类型范围内
    'type-enum': [
      2,
      'always',
      [
        'feat', // 新功能 feature
        'fix', // 修复 bug
        'docs', // 文档注释
        'style', // 代码格式(不影响代码运行的变动)
        'refactor', // 重构(既不增加新功能，也不是修复bug)
        'perf', // 性能优化
        'test', // 增加测试
        'chore', // 构建过程或辅助工具的变动
        'revert', // 回退
        'build' // 打包
      ]
    ],
    // subject 大小写不做校验
    'subject-case': [0]
  }
}
```

9.2 配置husky监听commit-msg

上面已经安装了husky,现在需要再配置下husky，让husky支持监听commit-msg钩子，在钩子函数中使用commitlint来验证

```
npx husky add .husky/commit-msg 'npx --no-install commitlint --edit "$1"'
```
会在 .husky目录下生成 commit-msg文件，并且配置 commitlint命令对备注进行验证配置，配置完后就可以进行测试了（要把 main.tsx中的语法报错代码去掉并添加到暂存区）


再次执行git commit -m "修改xx功能"
再次提交后可以看到commit-msg验证没有通过，输入的备注信息为修改xx功能,下面提示项目描述信息和类型不能为空，代表验证起到作用。


使用正确格式的备注再次提交,类型和描述之间需要用冒号加空格隔开

```
git commit -m 'feat: 修改xx功能'
```

就可以看到提交成功了。

#### 十. 添加git commit辅助备注信息

在上面虽然定义了很多提交类型，但都是英文前缀，不容易记忆，可以添加辅助信息在我们提交代码的时候做选择，会方便很多，可以借助**commitizen**来实现这个功能。

10.1 安装commitizen
全局安装commitizen，否则无法执行插件的git cz命令：


```
npm i commitizen -g
```

在项目内安装cz-customizable：

```
npm i cz-customizable -D
```

10.2 配置commitizen自定义提示规则

添加以下配置到 package.json 中：
```
"config": {
    "commitizen": {
        "path": "./node_modules/cz-customizable"
    }
}
```

在根目录创建 .cz-config.js 自定义提示文件：
```
module.exports = {
   // 可选类型，和上面commitlint.config.js配置的规则一一对应
  types: [
    { value: 'feat', name: 'feat: 新功能' },
    { value: 'fix', name: 'fix: 修复' },
    { value: 'docs', name: 'docs: 文档变更' },
    { value: 'style', name: 'style: 代码格式(不影响代码运行的变动)' },
    { value: 'refactor', name: 'refactor: 重构(既不是增加feature，也不是修复bug)' },
    { value: 'perf', name: 'perf: 性能优化' },
    { value: 'test', name: 'test: 增加测试' },
    { value: 'chore', name: 'chore: 构建过程或辅助工具的变动' },
    { value: 'revert', name: 'revert: 回退' },
    { value: 'build', name: 'build: 打包' }
   ],
   // 消息步骤，正常只需要选择
  messages: {
   type: '请选择提交类型:',
     // customScope: '请输入修改范围(可选):',
   subject: '请简要描述提交(必填):',
     // body: '请输入详细描述(可选):',
   footer: '请输入要关闭的issue(可选):',
   confirmCommit: '确认使用以上信息提交？(y/n)'
   },
   // 跳过问题：修改范围，自定义修改范围，详细描述，issue相关
  skipQuestions: ['scope', 'customScope', 'body', 'footer'],
   // subject描述文字长度最长是72
  subjectLimit: 72
 }
```
10.3 测试commitizen辅助提交

使用git add添加文件到暂存区，然后使用git cz替代git commit命令提交代码：



会出现选择提交类型和填写提交描述信息，选择yes后，会触发git提交语法验证，最终提交成功了，提交的备注是feat: 添加commit辅助信息

#### 十一. stylelint规范样式和保存自动修复

随便现在设备还有网络，浏览器性能越来越好，在前端代码性能方面关注的更多的是js层面的，但css层面也能做很多性能优化，css属性的书写顺序，选择器使用，都会对整体应用性能产生影响。所以配置一套完善的css代码书写规范可以有效提升应用的性能，而stylelint就是现在比较流行的csslint库。

6.1 安装vscode插件Stylelint
打开vsocde插件商店，搜索Stylelint，然后进行安装

6.2 安装Stylelint相关依赖
支持检测css文件需要安装stylelint相关依赖

- stylelint: stylelint核心依赖
- stylelint-config-standard: stylelint拓展，支持配置文件拓展一些检测规则
- stylelint-order: 检测css属性书写顺序的规则集合,比如display:flex要写在align-items之前
- stylelint-config-recess-order: stylelint-order 插件的第三方配置

```
npm i stylelint stylelint-config-standard stylelint-order stylelint-config-recess-order -D
```

6.3 配置.stylelintrc.js文件

在根目录新增 .stylelintrc.js文件，添加配置

```
module.exports = {
  extends: [
    'stylelint-config-standard', // 配置stylelint拓展插件
    'stylelint-config-prettier', // 配置stylelint和prettier兼容
    'stylelint-config-recess-order' // 配置stylelint css属性书写顺序插件
  ]
}
```

配置完成后打开src/App.css，可以看到很多红色报错

第一个报错：

vite脚手架创建的react项目，类名App用的大驼峰，默认的stylelint命名规则是小驼峰，这里就会有红色波浪线报错。

这种必须手动改，没有办法自动修复，或者修改styleint默认规则，这里改为不限制类名命名，具体配置可看selector-class-pattern。

```
// .stylelintrc.js
module.exports = {
  // ...
  rules: {
    'selector-class-pattern': null, // 设置类名选择器不遵循 kebab-case
  }
}
```

第二个报错：

这个报错就是css属性顺序的报错，一般display应该放在设置高度之前更合理，这类问题可以通过保存后自动修复。


第三个报错：

和第一个报错类似，是 @keyframes App-logo-spin,动画命名不允许大驼峰，这个可以手动修改为小驼峰命名。

6.4 设置保存自动修复
一般希望在保存文件css文件后自动修复css中的不合理的地方，在安装vscode插件stylelint后，需要修改一下 .vscode/settins.json文件
因为要使用stylelint的规则格式化代码，不使用perttier来格式化css,less,scss文件了，删除掉 .vscode/settins.json中配置的使用perttier格式化css,less,scss的配置。

```
  // 删除该代码
  "[css]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[less]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
  "[scss]": {
    "editor.defaultFormatter": "esbenp.prettier-vscode"
  },
```

在 .vscode/settins.json新增styleint保存文件格式化样式文件配置

```
{
  // ...
  // 开启stylelint自动修复
  "editor.codeActionsOnSave": {
    "source.fixAll.stylelint": true // 开启stylelint自动修复
  },
  // 关闭编辑器内置样式检查（避免与stylelint冲突）
  "css.validate": false,
  "less.validate": false,
  "scss.validate": false,
  // 配置stylelint检查的文件类型范围
  "stylelint.validate": ["css", "less", "postcss", "scss", "sass", "vue"],
  // ...
}
```
在 .vscode/settings.json文件中添加上面styleint保存后自动修复配置代码后，来到src/App.css文件，按ctrl+s保存一下代码，会发现display和min-height的位置调整了

就代表自动修复配置成功了。


6.5 支持scss和less
支持scss

要支持scss需要先安装scss的stylelint配置插件stylelint-config-standard-scss,内部定义了scss的语法规则，让stylelint可以识别scss的语法:

```
npm i stylelint-config-standard-scss -D
```

把插件添加到 .stylelintrc.js的extends中

```
// .stylelintrc.js
module.exports = {
  // ...
  extends: {
    // ...
    'stylelint-config-standard-scss' // 配置stylelint scss插件
  }
}
```

新建src/App.scss文件,代码如下，测试了变量命名，mixin混入和嵌套功能，同时把height高度写在了display之前。

```
$app-height: 200px;
​
@mixin rounded-corners {
  border-radius: 5px;
}
​
#root {
  .App {
    height: $app-height;
    display: block;
​
    @include rounded-corners;
  }
}
```

编辑好App.scss文件后，可以看到里面对应scss本身的语法没有报错，可以识别，同时检测到了height不能放在display前面的问题, 此时按crtl+s，会自动修复格式调整height和display的位置。

scss文件经过测试上面配置的stylelint已经支持scss文件，并且可以进行语法检测和保存自动格式化。

支持less

要支持less，要安装stylelint-less插件来支持

```
npm i stylelint-less -D
```
修改 .stylelintrc.js, 新增以下配置

```
module.exports = {
  // ...
  plugins: ['stylelint-less'],
  rules: {
    // ...
    'at-rule-no-unknown': null,
    'scss/at-rule-no-unknown': null,
    'color-no-invalid-hex': true,
    'less/color-no-invalid-hex': true
  }
}
```

新建src/App.less文件
```
@height: 200px;
​
.rounded-corners {
  border-radius: 5px;
}
​
#root {
  .App {
    display: block;
    height: @height;
  }
}
```

编辑好App.less文件后，可以里面看到有报错，height不能放在display前面, 此时按crtl+s，会自动修复格式。

6.7 通过命令行命令检测和修复。
自动修复只能修复一部分可通过改变属性顺序或者缩进换行来修复的问题，但像类名命名规则不符合规定命名的，自动修复就无能为力，需要手动调整，为了避免把不符合规范的样式代码提交到git远程仓库，需要在提交代码时对本次更改的样式文件进行语法检测。
上面已经配置了husky钩子，git commit时会执行pre-check脚本，只需要修改package.json中的scripts

```
"scripts": {
  // ...
  "stylelint": "stylelint"
},
"lint-staged": {
  // ...
  "src/**/*.{css,less,scss}": [
    "npm run stylelint"
  ]
}
```

测试一下

注释掉 .stylelintrc.js文件中取消类名命名验证的配置

```
module.exports = {
  // ...
  plugins: ['stylelint-less'],
  rules: {
    // 'selector-class-pattern': null, // 注释掉该配置
    // ...
  }
}
```

然后提交src/App.scss文件

```
git add src/App.scss
git commit -m "feat: 添加stylelint检测和修复样式问题"
```

结果如下图，可以看到检测到App.scss文件中有类名的命名没有准守小驼峰加连接符的规则，所以报错中断后续命令了。


处理原有的样式文件

如果原有的样式文件也想做修复处理，一种方式是挨个打开文件去执行crtl+s保存，来达到修复效果，但这样效率很低，可以通过命令一键修复项目内样式文件

```
npx stylelint "src/**/*.{css,less,scss}" --fix
```

具体细节的stylelint配置可以查看stylelint官网
