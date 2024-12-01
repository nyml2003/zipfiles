import { setGlobalCallback } from "../../useGlobalMessageHandler";
import { Api, RequestWrapper } from "../types";
import { ApiEnum } from "@/apis";
import { v4 as uuidv4 } from "uuid";
let apiInstance: Api | null = null;

const useApi = () => {
  if (apiInstance) {
    return apiInstance;
  }

  const isLocalFunction = (api: ApiEnum): boolean => {
    if (typeof api === "string" && window.webkit?.messageHandlers[api]) {
      return true;
    }
    return false;
  };

  const isRemoteFunction = (api: ApiEnum): boolean => {
    if (typeof api === "number" && window.webkit?.messageHandlers) {
      return api >= 100 && api < 200;
    }
    return false;
  };

  const request = async <Req, Res>(api: ApiEnum, request: Req): Promise<Res> => {
    const uuid = uuidv4();
    const message: RequestWrapper = { api, payload: request, uuid };
    if (!window.webkit?.messageHandlers) {
      console.error("No webkit message handlers");
      return Promise.reject("No webkit message handlers");
    }
    const localFunction = isLocalFunction(api);
    const remoteFunction = isRemoteFunction(api);
    if (!localFunction && !remoteFunction) {
      console.error("Invalid api: ", JSON.stringify(request, null, 2));
      return Promise.reject("Invalid api");
    }
    if (localFunction) {
      window.webkit.messageHandlers[api].postMessage({
        api,
        request,
        uuid,
      });
      return new Promise((resolve, reject) => {
        setGlobalCallback({
          request: message,
          resolve: data => {
            resolve(data);
          },
          reject: (message: string) => {
            reject(message);
          },
          retries: 0,
        });
      });
    }
    if (remoteFunction) {
      window.webkit.messageHandlers.function.postMessage({
        api,
        request: JSON.stringify(message),
        uuid,
      });

      return new Promise((resolve, reject) => {
        setGlobalCallback({
          request: message,
          resolve: data => {
            resolve(data);
          },
          reject: (message: string) => {
            reject(message);
          },
          retries: 0,
        });
      });
    }
    return Promise.reject("Unknown error");
  };

  const call = (apiEnum: ApiEnum, request: unknown): void => {
    window.webkit.messageHandlers[apiEnum].postMessage(request);
  };

  apiInstance = {
    request,
    call,
  };

  return apiInstance;
};

export default useApi;
