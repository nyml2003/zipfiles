import { useEffect, useState } from 'react';
import { useApi } from '../useApi';
import { ApiEnum, PagedRequest, PagedResponse } from '@/apis';
import { IngoreError } from '@/types';
import { message } from 'antd';
import { LoadingState } from './types';
const { api } = useApi();
const requestedPages = new Set<number>();

interface UseFetchProps<
  Request extends PagedRequest,
  ResponseCotent,
  Response extends PagedResponse<ResponseCotent>,
  RawRequest = Request,
> {
  apiEnum: ApiEnum;
  request: Request;
  transform?: (request: RawRequest) => Request;
}

const useFetch = <
  Request extends PagedRequest,
  ResponseCotent,
  Response extends PagedResponse<ResponseCotent>,
  RawRequest = Request,
>({
  apiEnum,
  request,
  transform,
}: UseFetchProps<Request, ResponseCotent, Response>) => {
  const [data, setData] = useState<{ count: number; results: ResponseCotent[] }>({
    count: 0,
    results: [],
  });
  const [loading, setLoading] = useState(LoadingState.Done);
  const [loadingMore, setLoadingMore] = useState(true);
  const [page, setPage] = useState(1);
  const refresh = async () => {
    requestedPages.clear();
    setData({ count: 0, results: [] });
    setLoadingMore(true);
    await fetchData(1);
  };

  useEffect(() => {
    refresh();
  }, []);

  const handleError = (error: any) => {
    if (error instanceof Error) {
      if (!(error instanceof IngoreError)) {
        message.error(error.message);
      }
    } else {
      message.error('未知错误');
    }
    setLoadingMore(false);
  };

  const fetchData = async (newPage: number) => {
    if (!loadingMore || requestedPages.has(newPage)) return;
    setLoading(LoadingState.Loading);
    requestedPages.add(newPage);
    const pageSize = request.pageSize || 10;
    const _transform = transform || ((i: any) => i);
    try {
      const res = await api.request<Request, Response>(
        apiEnum,
        _transform({ ...request, page: newPage }),
      );
      setData({
        ...data,
        results: [
          ...data.results,
          ...res.results.map((i, index) => {
            return {
              ...i,
              id: newPage * 100 + i.id,
              title: newPage + '-' + index,
            };
          }),
        ],
      });
      setPage(newPage);
      setLoadingMore(res.results.length >= pageSize);
    } catch (error: any) {
      handleError(error);
    } finally {
      setLoading(LoadingState.Done);
    }
  };
  const loadMore = async () => {
    try {
      await fetchData(page + 1);
    } catch (error: any) {
      handleError(error);
    }
  };

  return { data, loading, loadingMore, refresh, loadMore, setLoading };
};

export default useFetch;
