import { useApiType } from './types';
import productionApi from './production';
import developmentApi from './development';

const useApi: useApiType = process.env.NODE_ENV === 'production' ? productionApi : developmentApi;

export default useApi;
