/* eslint-disable @typescript-eslint/no-explicit-any */
import { useEffect } from "react";
import {
  Code,
  RequestWrapper,
  ResponseWrapper,
  Notification,
  isSingleText,
  SingleText,
  isDoubleText,
  DoubleText,
  isBackup,
  BackupAndRestoreEnd,
  isRestore,
  isError,
} from "./useApi/types";
import { useDispatch } from "react-redux";
import { addNotification, finishMessage } from "@/stores/NotificationReducer";
import { disableSocket, enableSocket } from "@/stores/SocketReducer";

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
// const MAX_REQUEST_RETRY = 10;
export const useGlobalMessageHandler = () => {
  const dispatch = useDispatch();
  useEffect(() => {
    const handler = (event: MessageEvent) => {
      const { code } = event.data as ResponseWrapper;
      if (isSingleText(code)) {
        const { payload } = event.data as Notification<SingleText>;
        const singleText = {
          text: payload.title,
          state: ["success", "info", "warning", "error"][code - 400] as
            | "success"
            | "info"
            | "warning"
            | "error",
          type: "plainText",
        };
        dispatch(addNotification(singleText));
        return;
      }
      if (isDoubleText(code)) {
        const { payload } = event.data as Notification<DoubleText>;
        const doubleText = {
          text: payload.title,
          description: payload.description,
          state: ["success", "warning", "error", "info"][code - 410] as
            | "success"
            | "info"
            | "warning"
            | "error",
          type: "plainText",
        };
        dispatch(addNotification(doubleText));
        return;
      }
      if (isBackup(code) || isRestore(code)) {
        console.log("callbacks: ", callbacks);
        console.log("event.data: ", event.data);
        dispatch(finishMessage(event.data as Notification<BackupAndRestoreEnd>));
        return;
      }
      if (code === Code.ENABLE_REMOTE_REQUEST) {
        dispatch(enableSocket());
        return;
      }
      if (code === Code.DISABLE_REMOTE_REQUEST) {
        dispatch(disableSocket());
        return;
      }
      const { payload, uuid } = event.data as ResponseWrapper;
      const callbackIndex = callbacks.findIndex(callback => callback.request.uuid === uuid);
      // console.log('event.type: ', event.type);
      // console.log('event.timestamp: ', response.timestamp);
      // console.log('event.apiEnum: ', response.apiEnum);
      // console.log('callbacks: ', JSON.stringify(callbacks));
      const [callback] = callbacks.splice(callbackIndex, 1);

      if (code === Code.OK) {
        callback.resolve(payload || {});
      } else if (isError(code)) {
        callback.reject("");
        const doubleText = {
          text: (payload as DoubleText).title,
          description: (payload as DoubleText).description,
          state: "error",
          type: "plainText",
        };
        dispatch(addNotification(doubleText));
      } else {
        callback.reject("未知错误");
      }
    };
    window.addEventListener("message", handler);
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
      window.removeEventListener("message", handler);
      // clearInterval(intervalId);
    };
  }, []);
};
