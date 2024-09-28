import { ApiEnum } from '@/apis';
import { notification } from 'antd';
import { useCallback, useEffect } from 'react';
import { RequestWrapper } from './useApi/types';
import MQ from '@/utils/mq';

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
const handleCallback = (callback: CallBack<any>) => {
  const { request, resolve, reject } = callback;
  return (response: ResponseWrapper) => {
    const { type, data, message, apiEnum, timestamp } = response;
    if (type === 'resolve') {
      resolve(data);
    } else if (type === 'reject') {
      reject(message);
    } else {
      notification.error({
        message: message,
      });
    }
  };
};
const mq = new MQ<() => void>();

export const setGlobalCallback = (callback: CallBack<any>) => {
  callbacks.push(callback);
};

export const handler = (event: MessageEvent) => {
  const response = event.data as ResponseWrapper;
  const { type, data, message, apiEnum, timestamp } = response;
  const callbackIndex = callbacks.findIndex(
    callback => callback.request.timestamp === timestamp && callback.request.apiEnum === apiEnum,
  );
  console.log('callbacks: ', JSON.stringify(callbacks));
  if (callbackIndex === -1) {
    if (type === 'fatal') {
      notification.error({
        message: message,
      });
    }
    return;
  }
  console.log('event.type: ', event.type);
  console.log('event.timestamp: ', response.timestamp);
  console.log('event.apiEnum: ', response.apiEnum);
  console.log('callbacks: ', JSON.stringify(callbacks));
  const [callback] = callbacks.splice(callbackIndex, 1);
  mq.push(() => handleCallback(callback)(response));
};

export const useGlobalMessageHandler = () => {
  useEffect(() => {
    window.addEventListener('message', handler);
    return () => {
      window.removeEventListener('message', handler);
    };
  }, []);
};