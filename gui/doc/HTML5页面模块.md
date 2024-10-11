# HTML5 页面模块

详细描述本模块的具体实现和其他模块的通信方式。

- HTML5 页面模块
  - 1.项目结构
  - 2.页面布局
  - 3.H5页面与WebView容器通信
    - 3.1.单向通信
    - 3.2.双向通信
  - 4.Mock数据
  - 5.Webpack 打包
  - 6.部署
  - 7.其他
  
## 1. 项目结构

### 1.1 环境

按项目的启动方式分为两种环境：

使用环境变量`NODE_ENV`来区分环境：

- development
    在该环境下, Webpack 会打开开发服务器，监听文件变化，自动编译并刷新页面。
- production
    1. 在该环境下, Webpack 会对代码进行优化，生成最终的生产代码。
    2. 打包生成的文件会放在`dist`目录下，供 WebView 容器加载。

实现方式：

1. 在`package.json`中配置`scripts`字段，设置`NODE_ENV`环境变量。

2. 给出三个 `webpack` 配置文件，分别是`webpack.base.js`、`webpack.dev.js`、`webpack.prod.js`，分别对应不同环境下的配置。
使用webpack-merge插件，将`webpack.base.js`中的配置与`webpack.dev.js`或`webpack.prod.js`中的配置合并。

按项目使用的数据源分为两种环境：

使用环境变量`BASE_ENV`来区分环境：

- dev
    在该环境下, 使用 mock 数据。
- prod
    在该环境下, 使用Webview容器提供的API获取真实数据。

实现方式：

1. 在`package.json`中配置`scripts`字段，设置`BASE_ENV`环境变量。

2. 封装useApi hook，根据`BASE_ENV`环境变量来判断使用哪种数据源。从而实现无感切换mock数据和真实数据, 便于开发和测试。

### 1.2 项目结构

```plaintext
.
├── public
│   ├── index.html
│   └── favicon.ico
└── src
    ├── apis // 接口请求
    │   ├── mock // mock数据
    │   └── Request/Response接口数据结构定义
    ├── assets // 静态资源
    ├── components // 公共组件
    ├── hooks // 自定义hook
    ├── pages // 页面
    ├── router // 路由
    ├── store // 状态管理
    ├── types // 类型定义
    ├── utils // 工具函数
    └── index.tsx // 入口文件
```

## 2. 页面布局

实现一个两栏布局，左侧为导航栏，右侧为内容区域。

其中导航栏的菜单项和路由配合，点击菜单项时，跳转到对应的路由。

内容区域根据路由动态加载对应的页面。

## 3. H5页面与WebView容器通信

### 3.1 单向通信

#### 3.1.1 H5页面发送到WebView容器的单向消息

在本模块的架构中，这种行为被称为Procedure。

Procedure是一种单向消息，用于向WebView容器发送消息，通知容器执行特定的操作。

实现方式：

每一个Procedure对应一个唯一的消息枚举值`apiEnum`，用于标识消息的类型。

消息枚举值的类型是`string`

通过`window.webkit.messageHandlers[apiEnum].postMessage(data)`来发送消息。

> WebView容器侧的机理在WebView容器模块中详细描述。

因为这种行为是单向的，所以不需要等待容器的响应。

类似于一个返回值为void的函数调用，所以称为Procedure。

具体实例有：

使用Proxy代理console.log，将日志信息发送给WebView容器。

#### 3.1.2 H5页面接收WebView容器的单向消息

在本模块的架构中，这种行为被称为Notification。

Notification是一种单向消息，用于接收WebView容器发送的消息，通知页面执行特定的操作。

因为在这种情况下H5页面只能被动接收消息，不可能执行比较复杂的操作，所以使用Ant Design of React的`notification`组件来实现, 用于显示通知消息。

实现方式：

WebView容器通过`window.postMessage`方法向H5页面发送消息。

H5页面通过`window.addEventListener('message', handler)`方法监听消息。

具体实例有：

比如请求的首部信息校验失败，WebView容器会发送一个消息通知H5页面，H5页面接收到消息后，使用`notification.error`方法显示错误信息。

### 3.2 双向通信

在本模块的架构中，这种行为被称为Function。

因为H5页面和WebView容器之间的通信在实现上是通过消息传递的方式来实现的，是两次单向通信的组合，但是在底层封装完后，对外表现为传入参数，返回结果的函数调用，所以称为Function。

Function是一种双向消息，用于向WebView容器发送消息，并等待容器的响应。

每个Function对应一个唯一的消息枚举值`apiEnum`，用于标识消息的类型，类型是`number`, 来减少传输开销。

#### 3.2.1 实现方式

在实现上可以视为3.1.1 H5页面发送到WebView容器的单向消息，3.1.2 H5页面接收WebView容器的单向消息的组合。

关键词： `Promise`、`Callback`、`请求消峰`、`首部信息校验`、`请求重发`

让我们来从各个角度来看看这种通信方式：

就**响应的时间**而言，这次请求是异步的，需要等待一段时间才能得到结果。

所以这次请求的结果被定为是一个`Promise`对象。

就**结果**而言，这次请求可能成功，也可能失败。

成功的话，我们需要将请求的结果`Result`返回给调用者。

失败的话，我们需要将失败的原因`Error`返回给调用者。

所以返回值的类型是`Promise<Result | Error>`。

考虑到对象解析的开销，给Response加一个首部。

```typescript
interface ResponseWrapper {
  uuid: string;
  timestamp: number;
  apiEnum: ApiEnum;
  data?: any;
  type: 'resolve' | 'reject' | 'notify';
  message: string;
}
```

其中`type`字段用于标识结果的类型，`message`字段用于存放消息的内容。

因为要共用事件监听器，所以需要对消息进行分类。其中`type`字段为`resolve`时，表示请求成功；`type`字段为`reject`时，表示请求失败；`type`字段为`notify`时，表示通知消息。

就**请求和响应的匹配**而言，这次请求和响应是一一对应的，但是它们的结果到达的顺序是异步的。

所以设计了一个请求首部，用于标识请求的唯一性。

```typescript
interface RequestWrapper<Request> {
  apiEnum: ApiEnum;
  params: Request;
  timestamp: number;
  uuid: string;
}
```

只有当`apiEnum`,`uuid`和`timestamp`都相同时，才能认为是同一个请求。

相应地，响应首部(类型定义已经列出)也需要包含这三个字段。

每次发起请求，将请求首部和请求体一起发送给WebView容器。并在回调函数队列里加入一个对象，包含请求本身，成功回调，失败回调，待收到响应后，根据`apiEnum`,`uuid`和`timestamp`匹配请求和响应，调用相应的回调函数。

```typescript
interface CallBack<T> {
  request: RequestWrapper<T>;
  resolve: (data: T) => void;
  reject: (message: string) => void;
  retries: number;
}
```

#### 3.2.1.1 稳定性

主要是请求消峰和请求重发。

考虑到文件操作是比较快的，所以实现了请求消峰，来减少后端的压力。

请求消峰：在一段时间内，只能发起一次请求，但是每个被注册的请求都一定会被执行。

设置一个

请求重发：在一段时间内，如果没有收到响应，会自动重发请求。

## 4. Mock数据

在开发阶段，为了方便调试和测试，我们需要模拟一些数据。

实现方式：

1. 在`src/apis/mock`目录下，编写每个接口的模拟数据,使用`mockjs`库来生成随机数据。

2. 在`src/hooks/useApi/index.ts`文件中，根据`BASE_ENV`环境变量来判断使用哪种数据源,如果是`dev`环境，则使用`mock`目录下的模拟数据;如果是`prod`环境，则使用Webview容器提供的API获取真实数据。

## 5. Webpack 打包
