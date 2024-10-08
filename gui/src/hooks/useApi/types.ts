import { ApiEnum } from "@/apis";

export interface Api {
  request<Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response>;
  call(apiEnum: ApiEnum, request: any): void;
}
export interface ResponseWrapper {
  timestamp: number;
  apiEnum: ApiEnum;
  data?: any;
  // resolve: 返回数据
  // reject: 返回错误信息
  // notify: 返回通知信息
  type: 'resolve' | 'reject' | 'notify';
  message: string;
}

export interface RequestWrapper<Request> {
  apiEnum: ApiEnum;
  params: Request;
  timestamp: number;
  uuid: string;
}

export type useApiType = () => Api;