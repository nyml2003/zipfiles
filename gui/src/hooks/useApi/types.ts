import { ApiEnum } from "@/apis";

export interface Api {
  request<Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response>;
}
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

export interface RequestWrapper<Request> {
  apiEnum: ApiEnum;
  params: Request;
  timestamp: number;
  uuid: string;
}

export type useApiType = () => Api;