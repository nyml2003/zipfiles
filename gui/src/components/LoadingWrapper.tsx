import { LoadingState } from "@/types";
import LoadingSpinner from "./LoadingSpinner";
import NoMoreData from "./NoMoreData";
import React, { ReactNode } from "react";

interface LoadingWrapperProps {
  loading: LoadingState;
  hasData?: () => boolean;
  children: ReactNode;
}

const LoadingWrapper = ({ loading, hasData, children }: LoadingWrapperProps) => {
  if (loading === LoadingState.Loading) {
    return <LoadingSpinner />;
  } else if (loading === LoadingState.Done && hasData && !hasData()) {
    return <NoMoreData />;
  }
  return <>{children}</>;
};

export default LoadingWrapper;
