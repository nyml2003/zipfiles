import { setGlobalCallback } from '../useGlobalMessageHandler';
import { Api, RequestWrapper, useApiType } from './types';
import { ApiEnum } from '@/apis';
// let callback = {
//   resolve: (data: any) => {},
//   reject: (data: any) => {},
// };
const useApi: useApiType = () => {
  // useEffect(() => {
  //   const handler = (event: MessageEvent) => {
  //     const { type, timestamp, data, apiEnum, message } = event.data as ResponseWrapper;
  //     if (type === 'resolve') {
  //       callback.resolve(data);
  //     } else if (type === 'reject') {
  //       callback.reject(message);
  //     } else if (type === 'fatal') {
  //       alert(message);
  //     }
  //   };

  //   window.addEventListener('message', handler);

  //   return () => {
  //     window.removeEventListener('message', handler);
  //   };
  // }, []);

  const api: Api = {
    request: async <Request, Response>(
      apiEnum: ApiEnum,
      request: Request,
      timeout: number = 1000, // 默认超时时间为 5000 毫秒
    ): Promise<Response> => {
      const timestamp = Date.now();
      const message: RequestWrapper<Request> = { apiEnum, params: request, timestamp };
      window.webkit.messageHandlers[apiEnum].postMessage(message);

      return new Promise((resolve, reject) => {
        const timer = setTimeout(() => {
          reject(new Error('请求超时'));
        }, timeout);

        // callback.resolve = (data: any) => {
        //   clearTimeout(timer);
        //   resolve(data);
        // };

        // callback.reject = (message: any) => {
        //   clearTimeout(timer);
        //   reject(message);
        // };

        setGlobalCallback({
          resolve: (data: any) => {
            clearTimeout(timer);
            resolve(data);
          },
          reject: (message: any) => {
            clearTimeout(timer);
            reject(message);
          },
        });
      });
    },
  };

  return api;
};

export default useApi;
