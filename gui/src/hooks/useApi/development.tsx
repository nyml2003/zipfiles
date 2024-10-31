import { Api, useApiType } from './types';
import { ApiEnum } from '@/apis';
import { mockApi } from '@/apis/mock';
const useApi: useApiType = () => {
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      return new Promise((resolve) => {
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
