import { ApiEnum } from "@/apis";

export interface Api {
  request<Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response>;
}
export interface ResponseWrapper {
  timestamp: number;
  apiEnum: ApiEnum;
  data?: any;
  type: 'resolve' | 'reject';
  message: string;
}

export interface RequestWrapper<Request> {
  apiEnum: ApiEnum;
  params: Request;
  timestamp: number;
}

export type useApiType = () => Api;