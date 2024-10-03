import { useEffect } from 'react';
import { Api, RequestWrapper, ResponseWrapper, useApiType } from './types';
import { ApiEnum } from '@/apis';
import { mockApi } from '@/apis/mock';
//mock
const useApi: useApiType = () => {
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      return new Promise((resolve, reject) => {
        setTimeout(() => {
          //时不时不稳定
          // if (Math.random() > 0.5) {
          //   reject('请求失败');
          //   return;
          // }
          //时不时慢
          // if (Math.random() > 0.5) {
          //   setTimeout(() => {
          //     resolve(mockApi[apiEnum](request));
          //   }, 2000);
          //   return;
          // }
        //  console.log(mockApi[apiEnum](request));
          resolve(mockApi[apiEnum](request));
        }, 10);
      });
    },
    call: (apiEnum: ApiEnum, request: Request) => {
      return;
    },
  };
  return api;
};

export default useApi;
