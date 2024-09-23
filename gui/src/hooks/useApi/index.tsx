import production from './production';
import development from './development';
import { selectEnvironment } from '@/utils';
import { Api, useApiType } from './types';
const useApi = selectEnvironment<useApiType>(production, development);
export default useApi;
