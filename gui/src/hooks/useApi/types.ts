/* eslint-disable no-unused-vars */
import { ApiEnum } from '@/apis';

export interface Api {
  request<Req, Res>(apiEnum: ApiEnum, request: Req): Promise<Res>;
  call(apiEnum: ApiEnum, request: unknown): void;
}

export enum Code {
  ILLEAGAL = 0, // StatusCode为0的情况不合法
  OK = 1, // 正常返回
  ERROR = 2, // 出现异常(默认，可自定义其他StatusCode)
  NOTIFICATION = 3, // 后端主动向前端返回消息
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

export type useApiType = () => Api;
