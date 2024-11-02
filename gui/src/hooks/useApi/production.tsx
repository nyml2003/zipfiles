import { setGlobalCallback } from '../useGlobalMessageHandler';
import { Api, RequestWrapper } from './types';
import { ApiEnum } from '@/apis';
import { v4 as uuidv4 } from 'uuid';
let apiInstance: Api | null = null;

const useApi = () => {
  if (apiInstance) {
    return apiInstance;
  }

  const request = async <Req, Res>(
    apiEnum: ApiEnum,
    request: Req,
  ): Promise<Res> => {
    const timestamp = Date.now();
    const uuid = uuidv4();
    const message: RequestWrapper<Req> = { apiEnum, params: JSON.stringify(request), timestamp, uuid };
    if (!window.webkit?.messageHandlers) {
      console.error('No webkit message handlers');
      return Promise.reject('No webkit message handlers');
    }
    if (!(typeof apiEnum === 'number') && !window.webkit.messageHandlers[apiEnum]) {
      console.error('No such api: ', apiEnum);
      return Promise.reject('No such api: ' + apiEnum);
    }
    console.log('request', message);
    window.webkit.messageHandlers.function.postMessage(message);

    return new Promise((resolve, reject) => {
      setGlobalCallback({
        request: message,
        resolve: (data) => {
          resolve(data);
        },
        reject: (message: string) => {
          reject(message);
        },
        retries: 0,
      });
    });
  };

  const call = (apiEnum: ApiEnum, request: unknown): void => {
    window.webkit.messageHandlers[apiEnum].postMessage({ 'params': request });
  };

  apiInstance = {
    request,
    call,
  };

  return apiInstance;
};

export default useApi;
