// import { ApiEnum } from "@/apis";
// import {
//   MockManyNotificationsRequest,
//   MockManyNotificationsResponse,
// } from "@/apis/MockManyNotifications";
// import useApi from "@useApi";
// import { decrement, increment } from "@/stores/counter/action";
// import { RootState } from "@/stores/store";
// import { Button, message } from "antd";
// import React from "react";
// import { useSelector, useDispatch } from "react-redux";
// import { Workflow } from "@/components/Step";

// const Counter: React.FC = () => {
//   const count = useSelector((state: RootState) => state.counter.count);
//   const dispatch = useDispatch();

//   return (
//     <div>
//       <h1>计数器: {count}</h1>
//       <Button onClick={() => dispatch(increment())}>增加</Button>
//       <Button onClick={() => dispatch(decrement())}>减少</Button>
//     </div>
//   );
// };
// const steps = [
//   {
//     title: "Step 1 - 计算 1 + 2",
//     pending: {
//       content: "pending",
//       onEnter: () => {
//         console.log("等待任务开始");
//       },
//       onExit: () => {
//         console.log("任务开始");
//       },
//     },
//     process: {
//       content: "计算中",
//       onEnter: () => {
//         console.log("计算中");
//       },
//       onExit: () => {
//         console.log("计算完成");
//       },
//     },
//     completed: {
//       content: "计算成功",
//       onEnter: () => {
//         console.log("计算成功");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//     fail: {
//       content: "计算失败",
//       onEnter: () => {
//         console.log("计算失败");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//     cancel: {
//       content: "取消任务",
//       onEnter: () => {
//         console.log("取消任务");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//   },
//   {
//     title: "Step 1 - 计算 1 + 2",
//     pending: {
//       content: "pending",
//       onEnter: () => {
//         console.log("等待任务开始");
//       },
//       onExit: () => {
//         console.log("任务开始");
//       },
//     },
//     process: {
//       content: "计算中",
//       onEnter: () => {
//         console.log("计算中");
//       },
//       onExit: () => {
//         console.log("计算完成");
//       },
//     },
//     completed: {
//       content: "计算成功",
//       onEnter: () => {
//         console.log("计算成功");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//     fail: {
//       content: "计算失败",
//       onEnter: () => {
//         console.log("计算失败");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//     cancel: {
//       content: "取消任务",
//       onEnter: () => {
//         console.log("取消任务");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//   },
//   {
//     title: "Step 1 - 计算 1 + 2",
//     pending: {
//       content: "pending",
//       onEnter: () => {
//         console.log("等待任务开始");
//       },
//       onExit: () => {
//         console.log("任务开始");
//       },
//     },
//     process: {
//       content: "计算中",
//       onEnter: () => {
//         console.log("计算中");
//       },
//       onExit: () => {
//         console.log("计算完成");
//       },
//     },
//     completed: {
//       content: "计算成功",
//       onEnter: () => {
//         console.log("计算成功");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//     fail: {
//       content: "计算失败",
//       onEnter: () => {
//         console.log("计算失败");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//     cancel: {
//       content: "取消任务",
//       onEnter: () => {
//         console.log("取消任务");
//       },
//       onExit: () => {
//         console.log("任务结束");
//       },
//     },
//   },
// ];
// const TestPage = () => {
//   const api = useApi();
//   const tryMockManyNotifications = () => {
//     api
//       .request<MockManyNotificationsRequest, MockManyNotificationsResponse>(
//         ApiEnum.MOCK_MANY_NOTIFICATIONS,
//         {},
//       )
//       .then(res => {
//         message
//           .success({
//             content: JSON.stringify(res),
//             duration: 2,
//           })
//           .then();
//       });
//   };
//   return (
//     <div>
//       <h1>Test Page</h1>
//       <div
//         className='text-blue-500 cursor-pointer bg-white p-2 rounded'
//         onClick={() => {
//           console.log(process.env);
//         }}>
//         点击查看环境变量
//       </div>
//       <Counter />
//       <div>
//         <Button onClick={tryMockManyNotifications} type='primary' className='mt-4'>
//           测试按钮
//         </Button>
//       </div>
//       <div className='w-64 h-32 bg-white'>
//         <Workflow steps={steps} />
//       </div>
//     </div>
//   );
// };

// export default TestPage;
import React, { ReactNode, useMemo, useState } from "react";
import { Status, Step, Workflow } from "@/components/Step";

const TestPage = () => {
  const [a, setA] = useState<number | null>(null);
  const [b, setB] = useState<number | null>(null);
  const [result, setResult] = useState<number | null>(null);

  const getNumber = async () => {
    return new Promise<void>(resolve => {
      setTimeout(() => {
        const a = Math.floor(Math.random() * 1000);
        const b = Math.floor(Math.random() * 1000);
        setA(a);
        setB(b);
        resolve();
      }, 1000);
    });
  };

  const getSum = async () => {
    return new Promise<void>(resolve => {
      setTimeout(() => {
        setResult(a! + b!);
        resolve();
      }, 1000);
    });
  };

  const steps = [
    {
      title: "输入数值",
      pending: {
        content: "请输入两个数值",
      },
      process: {
        content: "正在获取输入...",
        onEnter: async (status?: Status) => {
          if (!status) {
            return;
          }
          await getNumber();
        },
      },
      completed: {
        content: "输入获取完成",
        onEnter: async (status?: Status) => {
          if (!status) {
            return;
          }
          status.content = status.content + " 将要计算: " + a + " + " + b;
        },
      },
      fail: {
        content: "输入错误",
      },
      cancel: {
        content: "取消操作",
      },
    },
    {
      title: "计算和",
      pending: {
        content: "等待计算",
      },
      process: {
        content: "正在计算...",
        onEnter: async (status?: Status) => {
          if (!status) {
            return;
          }
          await getSum();
        },
      },
      completed: {
        content: "计算完成",
        onEnter: async (status?: Status) => {
          if (!status) {
            return;
          }
          status.content = status.content + " 结果为: " + result;
        },
      },
      fail: {
        content: "计算错误",
      },
      cancel: {
        content: "取消计算",
      },
    },
    {
      title: "显示结果",
      pending: {
        content: "等待显示结果",
      },
      process: {
        content: "正在显示结果...",
      },
      completed: {
        content: "结果显示完成",
      },
      fail: {
        content: "显示错误",
      },
      cancel: {
        content: "取消显示",
      },
    },
  ];

  return (
    <div>
      <Workflow steps={steps} />
      {a}
      {b}
    </div>
  );
};

export default TestPage;
