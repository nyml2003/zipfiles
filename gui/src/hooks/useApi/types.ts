/* eslint-disable no-unused-vars */
import { ApiEnum } from "@/apis";

export interface Api {
  request<Req, Res>(apiEnum: ApiEnum, request: Req): Promise<Res>;
  call(apiEnum: ApiEnum, request: unknown): void;
}

export enum Code {
  ILLEAGAL = 0, // StatusCode为0的情况不合法
  OK = 100, // 正常返回
  SERVER_ERROR = 200, // 默认服务端异常
  CLIENT_ERROR = 300, // 默认客户端异常
  SINGLE_SUCCESS = 400, // 单行文本 success
  SINGLE_WARNING = 401, // 单行文本 warning
  SINGLE_ERROR = 402, // 单行文本 error
  SINGLE_INFO = 403, // 单行文本 info
  DOUBLE_SUCCESS = 410, // 双行文本 success
  DOUBLE_WARNING = 411, // 双行文本 warning
  DOUBLE_ERROR = 412, // 双行文本 error
  DOUBLE_INFO = 413, // 双行文本 info
  BACKUP_SUCCESS = 420, // 备份成功
  BACKUP_FAILED = 421, // 备份失败
  RESTORE_SUCCESS = 430, // 恢复成功
  RESTORE_FAILED = 431, // 恢复失败
  DISABLE_REMOTE_REQUEST = 440, // 禁用远程请求
  ENABLE_REMOTE_REQUEST = 441, // 启用远程请 求
  SOCKET_CONNECT = 442, // Socket正在连接
}

export interface SingleText {
  title: string;
}

export interface DoubleText {
  title: string;
  description: string;
}

export interface BackupAndRestoreEnd {
  messageId: string;
  description: string;
}

export function isOK(code: Code): boolean {
  return code === Code.OK;
}

export function isSingleText(code: Code): boolean {
  return code >= 400 && code < 410;
}

export function isDoubleText(code: Code): boolean {
  return code >= 410 && code < 420;
}

export function isBackup(code: Code): boolean {
  return code >= 420 && code < 430;
}

export function isRestore(code: Code): boolean {
  return code >= 430 && code < 440;
}

export function isSocket(code: Code): boolean {
  return code === 440 || code === 441;
}

export function isClientError(code: Code): boolean {
  return Math.floor(code / 100) === 3;
}

export function isServerError(code: Code): boolean {
  return Math.floor(code / 100) === 2;
}

export function isError(code: Code): boolean {
  return isClientError(code) || isServerError(code);
}

export interface ResponseWrapper {
  uuid: string;
  code: Code;
  payload?: unknown;
}

export interface RequestWrapper {
  api: ApiEnum;
  payload?: unknown;
  uuid: string;
}

export interface Notification<T> {
  code: Code;
  payload: T;
}

export class AcceptableError extends Error {
  constructor(message: string) {
    super(message);
    this.name = "AcceptableError";
  }
}

export type useApiType = () => Api;
