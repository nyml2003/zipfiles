import { Api, Code, useApiType } from "../types";
import { ApiEnum } from "@/apis";
import { mockApi } from "@/apis/mock";
import { PostCommitRequest } from "@/apis/PostCommit";
import { RestoreRequest } from "@/apis/Restore";
import { finishMessage } from "@/stores/NotificationReducer";
import { useDispatch } from "react-redux";
const useApi: useApiType = () => {
  const dispatch = useDispatch();
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      return new Promise(resolve => {
        // 时不时不稳定
        // if (Math.random() > 0.5) {
        //   reject('请求失败');
        //   return;
        // }
        // 时不时慢
        // if (Math.random() > 0.5) {
        //   setTimeout(() => {
        //     resolve(mockApi[apiEnum](request));
        //   }, 2000);
        //   return;
        // }
        
        setTimeout(() => {
          const response = mockApi[apiEnum](request);
          resolve(response);
        }, 1000);
        setTimeout(() => {
          if (apiEnum === ApiEnum.PostCommit) {
            if (Math.random() > 0.5) {
              dispatch(finishMessage({
                payload: {
                  id: (request as PostCommitRequest).uuid,
                },
                code: Code.POSTCOMMIT_SUCCESS,
                message: "提交成功",
              }));
            } else {
              dispatch(finishMessage({
                payload: {
                  id: (request as PostCommitRequest).uuid,
                },
                code: Code.POSTCOMMIT_FAILED,
                message: "提交失败",
              }));
            }
          }
          if (apiEnum === ApiEnum.Restore) {
            if (Math.random() > 0.5) {
              dispatch(finishMessage({
                payload: {
                  id: (request as RestoreRequest).messageId,
                },
                code: Code.RESTORE_SUCCESS,
                message: "还原成功",
              }));
            } else {
              dispatch(finishMessage({
                payload: {
                  id: (request as RestoreRequest).messageId,
                },
                code: Code.RESTORE_FAILED,
                message: "还原失败",
              }));
            }
          }
        }, 2000);
      });
    },
    call: () => {
      return;
    },
  };
  return api;
};

export default useApi;
