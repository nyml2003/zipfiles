/* eslint-disable no-unused-vars */
import { ApiEnum } from "@/apis";

export interface Api {
  request<Req, Res>(apiEnum: ApiEnum, request: Req): Promise<Res>;
  call(apiEnum: ApiEnum, request: unknown): void;
}

export enum Code {
  ILLEAGAL = 0, // StatusCode为0的情况不合法
  OK = 100, // 正常返回
  ERROR = 200, // 默认服务端异常
  CLIENT_ERROR = 300, // 默认客户端异常
  NOTIFICATION = 400, // 默认通知，应该是文本消息
  POSTCOMMIT_SUCCESS = 401, // 提交Commit成功
  POSTCOMMIT_FAILED = 402, // 提交Commit失败
  RESTORE_SUCCESS = 403, // 恢复成功
  RESTORE_FAILED = 404, // 恢复失败
}

export function isOK(code: Code): boolean {
  return code === Code.OK;
}

export function isNotification(code: Code): boolean {
  return code === Code.NOTIFICATION;
}

export function isClientError(code: Code): boolean {
  return code === Code.CLIENT_ERROR;
}

export function isServerError(code: Code): boolean {
  return code === Code.ERROR;
}

export function isResponseNotification(code: Code): boolean {
  return Math.floor(code / 100) === 4;
}

export interface ResponseWrapper {
  api: ApiEnum;
  uuid: string;
  code: Code;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  payload?: any;
  message?: string;
}

export interface RequestWrapper {
  api: ApiEnum;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  payload: any;
  uuid: string;
}

export interface Notification {
  code: Code;
  message: string;
  // eslint-disable-next-line @typescript-eslint/no-explicit-any
  payload?: any;
}

export type useApiType = () => Api;
