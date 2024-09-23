import { useEffect } from 'react';
import { Api, RequestWrapper, ResponseWrapper, useApiType } from './types';
import { ApiEnum } from '@/apis';
let callback = {
  resolve: (data: any) => {},
  reject: (data: any) => {},
};
const useApi: useApiType = () => {
  useEffect(() => {
    const handler = (event: MessageEvent) => {
      const { type, timestamp, data, apiEnum, message } = event.data as ResponseWrapper;
      if (type === 'resolve') {
        callback.resolve(data);
      } else if (type === 'reject') {
        callback.reject(message);
      }
    };

    window.addEventListener('message', handler);

    return () => {
      window.removeEventListener('message', handler);
    };
  }, []);

  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      const timestamp = Date.now();
      const message: RequestWrapper<Request> = { apiEnum, params: request, timestamp };
      window.webkit.messageHandlers[apiEnum].postMessage(message);
      return new Promise((resolve, reject) => {
        callback.resolve = resolve;
        callback.reject = reject;
      });
    },
  };

  return api;
};

export default useApi;