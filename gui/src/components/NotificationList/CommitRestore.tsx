// import React, { useEffect, useState } from "react";
// import { CommitRestore as CommitRestoreProps, CommitRestoreProgress } from "./types";
// import { Button, Form, Input, Space, Steps } from "antd";
// import { FileType } from "@/types";
// import { ApiEnum } from "@/apis";
// import useApi from "@useApi";
// import { useDispatch } from "react-redux";
// import { updateProgress } from "@/stores/NotificationReducer";
// import { StepsProps } from "antd/lib";
// import { CheckCircleOutlined, LoadingOutlined, StopOutlined } from "@ant-design/icons";
// import { Code } from "@/hooks/useApi/types";
// import { RestoreRequest } from "@/apis/Restore";

// interface File {
//   name: string;
//   type: FileType;
// }

// const progresses: CommitRestoreProgress[] = [
//   CommitRestoreProgress.ConfirmRequest,
//   CommitRestoreProgress.Finish,
// ];

// const CommitRestore = ({ progress, isEncrypt, id, result }: CommitRestoreProps) => {
//   const dispatch = useDispatch();
//   const [backupFiles, setBackupFiles] = useState<string[]>([]);

//   const api = useApi();

//   const [steps, setSteps] = useState<StepsProps["items"]>([
//     {
//       title: "确认请求",
//     },
//     {
//       title: "发送请求",
//       description: "还原备份中...",
//     },
//     {
//       title: "完成",
//     },
//   ]);

//   const restore = async () => {
//     setSteps(prev => {
//       if (!prev) return prev;
//       prev[1].description = "正在发送还原备份请求...";
//       prev[1].status = "process";
//       prev[1].icon = <LoadingOutlined />;
//       return [...prev];
//     });

//     setSteps(prev => {
//       if (!prev) return prev;
//       prev[1].description = "请求已收到, 正在还原备份...";
//       prev[1].status = "finish";
//       prev[1].icon = <CheckCircleOutlined />;
//       prev[2].description = "还原中";
//       prev[2].status = "process";
//       prev[2].icon = <LoadingOutlined />;
//       return [...prev];
//     });
//   };

//   const nextProgress = async () => {
//     if (progress === CommitRestoreProgress.ConfirmRequest) {
//       await restore();
//     }
//     dispatch(updateProgress({ id, progress: progresses[progress + 1] }));
//   };

//   const cancel = () => {
//     dispatch(updateProgress({ id, progress: CommitRestoreProgress.Cancel }));
//   };

//   useEffect(() => {
//     if (!result) return;
//     if (result.code === Code.RESTORE_SUCCESS) {
//       setSteps(prev => {
//         if (!prev) return prev;
//         prev[2].description = "还原成功";
//         prev[2].status = "finish";
//         prev[2].icon = <CheckCircleOutlined />;
//         return [...prev];
//       });
//     }
//     if (result.code === Code.RESTORE_FAILED) {
//       setSteps(prev => {
//         if (!prev) return prev;
//         prev[2].description = "还原失败";
//         prev[2].status = "error";
//         prev[2].icon = <StopOutlined />;
//         return [...prev];
//       });
//     }
//   }, [result]);

//   return progress !== CommitRestoreProgress.Cancel ? (
//     <div>
//       <Steps current={progress} size='small' direction='vertical' items={steps} />
//       {progress !== CommitRestoreProgress.Finish && (
//         <div className='absolute bottom-2 right-2'>
//           <Space>
//             <Button type='text' size='small' onClick={cancel}>
//               取消
//             </Button>
//             <Button type='primary' size='small' onClick={nextProgress}>
//               继续
//             </Button>
//           </Space>
//         </div>
//       )}
//     </div>
//   ) : (
//     <Steps
//       current={0}
//       size='small'
//       direction='vertical'
//       items={[
//         {
//           title: "取消",
//           description: "取消提交",
//           status: "error",
//         },
//       ]}
//     />
//   );
// };

// export default CommitRestore;
