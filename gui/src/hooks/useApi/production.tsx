import { setGlobalCallback } from '../useGlobalMessageHandler';
import { Api, RequestWrapper, useApiType } from './types';
import { ApiEnum } from '@/apis';
import { v4 as uuidv4 } from 'uuid';

const useApi: useApiType = () => {
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      const timestamp = Date.now();
      const uuid = uuidv4();
      const message: RequestWrapper<Request> = { apiEnum, params: request, timestamp , uuid };
      if (typeof apiEnum === 'number') {
        window.webkit.messageHandlers.handleMessage.postMessage(message);
      } else {
        window.webkit.messageHandlers[apiEnum].postMessage(message);
      }
      if (apiEnum === ApiEnum.Log) {
        return new Promise((resolve, reject) => {
          resolve({} as Response);
        });
      }
      return new Promise((resolve, reject) => {
        setGlobalCallback({
          request: message,
          resolve: (data: any) => {
            resolve(data);
          },
          reject: (message: any) => {
            reject(message);
          },
          retries: 0,
        });
      });
    },
  };

  return api;
};

export default useApi;
