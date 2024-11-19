import { Api, Code, useApiType } from "../types";
import { ApiEnum } from "@/apis";
import { mockApi } from "@/apis/mock";
import { PostCommitRequest } from "@/apis/PostCommit";
import { finishMessage } from "@/stores/NotificationReducer";
import { useDispatch } from "react-redux";
const useApi: useApiType = () => {
  const dispatch = useDispatch();
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      return new Promise(resolve => {
        setTimeout(() => {
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
          resolve(mockApi[apiEnum](request));
          if (apiEnum === ApiEnum.PostCommit) {
            setTimeout(() => {
              dispatch(
                finishMessage({
                  payload: { id: (request as PostCommitRequest).uuid },
                  message: "提交成功",
                  code: Code.POSTCOMMIT_SUCCESS,
                }),
              );
            }, 1000);
          }
        }, 10);
      });
    },
    call: () => {
      return;
    },
  };
  return api;
};

export default useApi;
