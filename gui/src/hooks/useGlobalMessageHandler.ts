/* eslint-disable @typescript-eslint/no-explicit-any */
import { useEffect } from 'react';
import { Code, RequestWrapper, ResponseWrapper, Notification, isNotification, isResponseNotification } from './useApi/types';
import { useDispatch } from 'react-redux';
import { addNotification, finishMessage } from '@/stores/NotificationReducer';
import { PlainText } from '@/components/NotificationList/types';
import { notification } from 'antd';

export interface CallBack {
  request: RequestWrapper;
  resolve: (data: any) => void;
  reject: (message: string) => void;
  retries: number;
}

const callbacks: CallBack[] = [];

export const setGlobalCallback = (callback: CallBack) => {
  callbacks.push(callback);
};

export const MAX_REQUEST_TIMEOUT = 500;
const MAX_REQUEST_RETRY = 10;
export const useGlobalMessageHandler = () => {
  console.log('useGlobalMessageHandler is ok');
  const dispatch = useDispatch();
  console.log('dispatch is ok');
  useEffect(() => {
    const handler = (event: MessageEvent) => {
      // console.log('event.data: ', JSON.stringify(event.data));
      const response = event.data as ResponseWrapper;
      const { api, uuid, code, payload, message } = response;
      if (isNotification(code)) {
        dispatch(
          addNotification({
            type: 'plainText',
            text: message,
            state: 'warning',
          } as PlainText),
        );
        return;
      }
      if (isResponseNotification(code)) {
        const notification = event.data as Notification;
        dispatch(finishMessage(notification));
        return;
      }
      const callbackIndex = callbacks.findIndex(
        callback => callback.request.uuid === uuid && callback.request.api === api,
      );
      // console.log('event.type: ', event.type);
      // console.log('event.timestamp: ', response.timestamp);
      // console.log('event.apiEnum: ', response.apiEnum);
      // console.log('callbacks: ', JSON.stringify(callbacks));
      const [callback] = callbacks.splice(callbackIndex, 1);

      if (code === Code.OK) {
        callback.resolve(payload || {});
      } else {
        callback.reject(message || '响应体异常');
      }
    };
    window.addEventListener('message', handler);
    // const intervalId = setInterval(() => {
    //   const now = Date.now();
    //   callbacks = callbacks.filter(callback => {
    //     if (callback.request.timestamp + MAX_REQUEST_TIMEOUT * callback.retries < now) {
    //       const { request } = callback;
    //       callback.request.timestamp = now;
    //       if (
    //         window.webkit &&
    //         window.webkit.messageHandlers &&
    //         window.webkit.messageHandlers.handleMessage
    //       ) {
    //         window.webkit.messageHandlers.handleMessage.postMessage(request);
    //       }
    //       callback.retries += 1;
    //       console.log('retry', callback.retries, 'times for', request);
    //       if (callback.retries > MAX_REQUEST_RETRY) {
    //         notification.error({
    //           message: '请求超时',
    //         });
    //         return false; // 移除超时的回调
    //       }
    //     }
    //     return true; // 保留未超时的回调
    //   });
    // }, MAX_REQUEST_TIMEOUT);
    return () => {
      window.removeEventListener('message', handler);
      // clearInterval(intervalId);
    };
  }, []);
};
