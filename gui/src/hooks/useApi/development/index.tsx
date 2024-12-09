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
      return new Promise((resolve, reject) => {
        // 时不时不稳定
        // if (Math.random() > 0.5) {
        //   reject("请求失败");
        //   return;
        // }
        // 时不时慢
        // if (Math.random() > 0.5) {
        //   setTimeout(() => {
        //     resolve(mockApi[apiEnum](request));
        //   }, 2000);
        //   return;
        // }

        try {
          const response = mockApi[apiEnum](request);
          resolve(response);
        } catch (e) {
          reject(e);
        }
        setTimeout(() => {
          if (apiEnum === ApiEnum.PostCommit) {
            if (Math.random() > 0.5) {
              dispatch(
                finishMessage({
                  payload: {
                    messageId: (request as PostCommitRequest).uuid,
                    description: "备份成功"
                  },
                  code: Code.BACKUP_SUCCESS
                })
              );
            } else {
              dispatch(
                finishMessage({
                  payload: {
                    messageId: (request as PostCommitRequest).uuid,
                    description: "备份失败"
                  },
                  code: Code.BACKUP_FAILED
                })
              );
            }
          }
          if (apiEnum === ApiEnum.Restore) {
            if (Math.random() > 0.5) {
              dispatch(
                finishMessage({
                  payload: {
                    messageId: (request as RestoreRequest).messageId,
                    description: "还原成功"
                  },
                  code: Code.RESTORE_SUCCESS
                })
              );
            } else {
              dispatch(
                finishMessage({
                  payload: {
                    messageId: (request as RestoreRequest).messageId,
                    description: "还原失败"
                  },
                  code: Code.RESTORE_FAILED
                })
              );
            }
          }
        }, 2000);
      });
    },
    call: () => {
      return;
    }
  };
  return api;
};

export default useApi;
