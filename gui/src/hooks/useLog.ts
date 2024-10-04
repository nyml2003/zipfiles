import { ApiEnum } from '@/apis';
import { useEffect } from 'react';
import useApi from './useApi';
import { LogRequest } from '@/apis/log';
import { ErrorRequest } from '@/apis/error';
import { WarnRequest } from '@/apis/warn';
import { InfoRequest } from '@/apis/info';

const useLog = () =>
  useEffect(() => {
    const api = useApi();
    if (process.env.BASE_ENV === 'prod') {
      console.log = new Proxy(console.log, {
        apply: (target, thisArg, argArray) => {
          api.call(ApiEnum.Log, { message: argArray.join(' ') } as LogRequest);
          target(...argArray);
        },
      });
      console.error = new Proxy(console.error, {
        apply: (target, thisArg, argArray) => {
          api.call(ApiEnum.Error, { message: argArray.join(' ') } as ErrorRequest);
          target(...argArray);
        },
      });
      console.warn = new Proxy(console.warn, {
        apply: (target, thisArg, argArray) => {
          api.call(ApiEnum.Warn, { message: argArray.join(' ') } as WarnRequest);
          target(...argArray);
        },
      });
      console.info = new Proxy(console.info, {
        apply: (target, thisArg, argArray) => {
          api.call(ApiEnum.Info, { message: argArray.join(' ') } as InfoRequest);
          target(...argArray);
        },
      });
    }
  }, []);

export default useLog;