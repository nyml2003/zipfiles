import { LoadingState } from '@/types';
import LoadingSpinner from './LoadingSpinner';
import NoMoreData from './NoMoreData';
import React from 'react';

interface LoadingWrapperProps {
  loading: LoadingState;
  hasData: () => boolean;
  children: React.ReactNode;
}

const LoadingWrapper = ({ loading, hasData, children }: LoadingWrapperProps) => {
  if (loading === LoadingState.Loading) {
    return <LoadingSpinner />;
  } else if (loading === LoadingState.Done && !hasData()) {
    return <NoMoreData />;
  }
  return <>{children}</>;
};

export default LoadingWrapper;
