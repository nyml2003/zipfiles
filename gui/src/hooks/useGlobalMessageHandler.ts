import { ApiEnum } from '@/apis';
import { notification } from 'antd';
import { useEffect } from 'react';
import { RequestWrapper } from './useApi/types';

export interface ResponseWrapper {
  timestamp: number;
  apiEnum: ApiEnum;
  data?: any;
  type: 'resolve' | 'reject' | 'fatal';
  message: string;
}

export interface CallBack<T> {
  request: RequestWrapper<T>;
  resolve: (data: T) => void;
  reject: (message: string) => void;
}

const callbacks: CallBack<any>[] = [];

export const setGlobalCallback = (callback: CallBack<any>) => {
  callbacks.push(callback);
};

const useGlobalMessageHandler = () => {
  useEffect(() => {
    const handler = (event: MessageEvent) => {
      const response = event.data as ResponseWrapper;
      const { type, data, message, apiEnum, timestamp } = response;
      const callbackIndex = callbacks.findIndex(callback => callback.request.timestamp === timestamp && callback.request.apiEnum === apiEnum);
      if (callbackIndex === -1) {
        console.log('未找到对应的请求');
        if (type === 'fatal') {
          notification.error({
            message: message,
          });
        }
        return;
      }
      const [callback] = callbacks.splice(callbackIndex, 1);
      if (type === 'resolve') {
        new Promise<void>(resolve => {
          callback.resolve(data);
          resolve();
        });
      } else if (type === 'reject') {
        new Promise<void>(resolve => {
          callback.reject(message);
          resolve();
        });
      } else if (type === 'fatal') {
        console.log('this should not happen');
      }
    };

    window.addEventListener('message', handler);

    return () => {
      window.removeEventListener('message', handler);
    };
  }, []);
};

export default useGlobalMessageHandler;
