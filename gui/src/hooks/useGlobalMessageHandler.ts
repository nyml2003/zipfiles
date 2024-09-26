import { ApiEnum } from '@/apis';
import { notification } from 'antd';
import { useEffect } from 'react';
export interface ResponseWrapper {
  timestamp: number;
  apiEnum: ApiEnum;
  data?: any;
  // resolve: 返回数据
  // reject: 返回错误信息
  // fatal: 严重错误
  type: 'resolve' | 'reject' | 'fatal';
  message: string;
}

let callback: { resolve: (data: any) => void; reject: (message: string) => void };

export const setGlobalCallback = (cb: typeof callback) => {
  callback = cb;
};

const useGlobalMessageHandler = () => {
  useEffect(() => {
    const handler = (event: MessageEvent) => {
      const { type, timestamp, data, apiEnum, message } = event.data as ResponseWrapper;
      if (type === 'resolve') {
        callback.resolve(data);
      } else if (type === 'reject') {
        callback.reject(message);
      } else if (type === 'fatal') {
        notification.error({
          message: message,
        });
      }
    };

    window.addEventListener('message', handler);

    return () => {
      window.removeEventListener('message', handler);
    };
  }, []);
};

export default useGlobalMessageHandler;
