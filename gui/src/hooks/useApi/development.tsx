import { useEffect } from 'react';
import { Api, RequestWrapper, ResponseWrapper, useApiType } from './types';
import { ApiEnum, mockApi } from '@/apis';
//mock
const useApi: useApiType = () => {
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      return new Promise((resolve, reject) => {
        setTimeout(() => {
          resolve(mockApi[apiEnum](request));
        }, 10);
      });
    },
  };
  return api;
};

export default useApi;