import { setGlobalCallback } from '../useGlobalMessageHandler';
import { Api, RequestWrapper, useApiType } from './types';
import { ApiEnum } from '@/apis';

const useApi: useApiType = () => {
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      const timestamp = Date.now();
      const message: RequestWrapper<Request> = { apiEnum, params: request, timestamp };
      window.webkit.messageHandlers.handleMessage.postMessage(message);

      return new Promise((resolve, reject) => {
        setGlobalCallback({
          request: message,
          resolve: (data: any) => {
            resolve(data);
          },
          reject: (message: any) => {
            reject(message);
          },
        });
      });
    },
  };

  return api;
};

export default useApi;
