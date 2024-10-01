import { setGlobalCallback } from '../useGlobalMessageHandler';
import { Api, RequestWrapper, useApiType } from './types';
import { ApiEnum } from '@/apis';
import { v4 as uuidv4 } from 'uuid';
import useDilute from '@/hooks/useDilute';

const { dilute } = useDilute();

let apiInstance: Api | null = null;

const useApi: useApiType = () => {
  if (apiInstance) {
    return apiInstance;
  }

  const request = async <Request, Response>(
    apiEnum: ApiEnum,
    request: Request,
  ): Promise<Response> => {
    const timestamp = Date.now();
    const uuid = uuidv4();
    const message: RequestWrapper<Request> = { apiEnum, params: request, timestamp, uuid };

    if (typeof apiEnum === 'number') {
      dilute(() => window.webkit.messageHandlers.handleMessage.postMessage(message));
    } else {
      dilute(() => window.webkit.messageHandlers[apiEnum].postMessage(message));
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
  };

  apiInstance = {
    request,
  };

  return apiInstance;
};

export default useApi;
