interface Api {
  request<Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response>
}

import { ApiEnum } from '@/apis'
interface MessageEventData<Response> {
  timestamp: number
  apiEnum: ApiEnum
  data: Response | null
    type: 'resolve' | 'reject'
    message: string
}

interface MessageEventError {
    timestamp: number
    apiEnum: ApiEnum
    data: any
    type: 'reject'
}

interface PostMessage<Request> {
  apiEnum: ApiEnum
  request: Request
  timestamp: number
}

const useApi = (): Api => {
  const callbacks: {
    [key: number]: { resolve: (result: any) => void; reject: (error: any) => void }
  } = {}

  window.addEventListener('message', function handler(event: MessageEvent) {
    const { type, timestamp, data, apiEnum } = event.data as MessageEventData<any>
    if (callbacks[timestamp]) {
      if (type === 'resolve') {
        callbacks[timestamp].resolve(data)
      } else if (type === 'reject') {
        callbacks[timestamp].reject(data)
      }
      delete callbacks[timestamp]
    }
  })

  const api: Api = {
    request: async <Request, Response>(apiEnum: ApiEnum, request: Request): Promise<Response> => {
      const timestamp = Date.now()
      const message: PostMessage<Request> = { apiEnum, request, timestamp }
      window.webkit.messageHandlers[apiEnum].postMessage(message)
      return new Promise((resolve, reject) => {
        callbacks[timestamp] = { resolve, reject }
      })
    },
  }

  return api
}

export default useApi
