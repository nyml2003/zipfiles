import { ApiEnum } from '@/apis';
import { notification } from 'antd';
import { useCallback, useEffect } from 'react';
import { RequestWrapper } from './useApi/types';
import MQ from '@/utils/mq';

export interface ResponseWrapper {
  uuid: string;
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
  retries: number;
}

let callbacks: CallBack<any>[] = [];
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
  const { type, data, message, apiEnum, timestamp , uuid } = response;
  const callbackIndex = callbacks.findIndex(
    callback => callback.request.timestamp === timestamp && callback.request.apiEnum === apiEnum && callback.request.uuid === uuid,
  );
  //console.log('callbacks: ', JSON.stringify(callbacks));
  if (callbackIndex === -1) {
    if (type === 'fatal') {
      notification.error({
        message: message,
      });
    }
    return;
  }
  // console.log('event.type: ', event.type);
  // console.log('event.timestamp: ', response.timestamp);
  // console.log('event.apiEnum: ', response.apiEnum);
  // console.log('callbacks: ', JSON.stringify(callbacks));
  const [callback] = callbacks.splice(callbackIndex, 1);
  mq.push(() => handleCallback(callback)(response));
};
const MAX_REQUEST_TIMEOUT = 500;
const MAX_REQUEST_RETRY = 3;
export const useGlobalMessageHandler = () => {
  useEffect(() => {
    window.addEventListener('message', handler);
    const intervalId = setInterval(() => {
      const now = Date.now();
      callbacks = callbacks.filter(callback => {
        if (callback.request.timestamp + MAX_REQUEST_TIMEOUT < now) {
          const { request } = callback;
          callback.request.timestamp = now;
          if (
            window.webkit &&
            window.webkit.messageHandlers &&
            window.webkit.messageHandlers.handleMessage
          ) {
            window.webkit.messageHandlers.handleMessage.postMessage(request);
          }
          callback.retries += 1;
          if (callback.retries > MAX_REQUEST_RETRY) {
            notification.error({
              message: '请求超时',
            });
            return false; // 移除超时的回调
          }
        }
        return true; // 保留未超时的回调
      });
    }, MAX_REQUEST_TIMEOUT);
    return () => {
      window.removeEventListener('message', handler);
      clearInterval(intervalId);
    };
  }, []);
};