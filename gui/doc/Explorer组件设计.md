支持虚拟文件列表以及文件列表的展示

Expolorer组件是一个用于展示文件列表的组件

包含3个主要子组件：

- 文件列表的树状视图，用于展示文件列表的嵌套目录结构
- 文件列表的表格视图，用于展示目录下的文件详细信息
- 文件列表的路径导航，用于展示当前目录的路径

对于每一个Explorer组件，整颗组件树需要共享当前的目录状态，以及高亮目录树中选中的文件

但是可能同时存在多个Explorer组件，每个Explorer组件的状态彼此独立

且Explorer组件及其子组件的状态都比较复杂，但是Redux不建议多个store

使用React Context API来解决这个问题

对于Explorer组件，会创建一个动态的上下文，用于存储当前目录状态和高亮文件

Context内存储2个变量：

- state：当前状态
- actions: 用于修改状态的方法

对于actions，我会假定actions.ts导出的每个方法都符合格式，传入一个state，返回一个新的state

导入所有的导出项，然后遍历每个导出项，将函数名作为key, 函数作为value，存入actions对象

