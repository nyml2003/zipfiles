interface Api {
  request<Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response>
}

import { ApiEnum } from '@/apis'

const useApi = (): Api => {
  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      window.webkit.messageHandlers[apiEnum].postMessage(request)
      return new Promise((resolve: (result: Response) => void, reject: (error: any) => void) => {
        window.addEventListener('message', function handler(event) {
          if (event.data.type === 'resolve') {
            const res: Response = event.data
            resolve(res)
          } else if (event.data.type === 'reject') {
            reject(event.data.error)
          }
          window.removeEventListener('message', handler)
        })
      })
    },
  }

  return api
}

export default useApi
