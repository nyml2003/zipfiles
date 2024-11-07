import { setGlobalCallback } from '../useGlobalMessageHandler';
import { Api, RequestWrapper } from './types';
import { ApiEnum } from '@/apis';
import { v4 as uuidv4 } from 'uuid';
let apiInstance: Api | null = null;

const useApi = () => {
  if (apiInstance) {
    return apiInstance;
  }

  const request = async <Req, Res>(api: ApiEnum, request: Req): Promise<Res> => {
    const uuid = uuidv4();
    const message: RequestWrapper = { api, payload: request, uuid };
    if (!window.webkit?.messageHandlers) {
      console.error('No webkit message handlers');
      return Promise.reject('No webkit message handlers');
    }
    if (!(typeof api === 'number') && !window.webkit.messageHandlers[api]) {
      console.error('No such api: ', api);
      return Promise.reject('No such api: ' + api);
    }
    console.log(api, request, uuid);
    window.webkit.messageHandlers.function.postMessage({
      api,
      request: JSON.stringify(message),
      uuid,
    });

    return new Promise((resolve, reject) => {
      setGlobalCallback({
        request: message,
        resolve: data => {
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
    window.webkit.messageHandlers[apiEnum].postMessage(request);
  };

  apiInstance = {
    request,
    call,
  };

  return apiInstance;
};

export default useApi;
