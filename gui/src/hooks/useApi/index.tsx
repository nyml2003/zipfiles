import { useApiType } from './types';

let useApi: useApiType;

if (process.env.NODE_ENV === 'production') {
  useApi = require('./production').default;
} else {
  useApi = require('./development').default;
}

export default useApi;
