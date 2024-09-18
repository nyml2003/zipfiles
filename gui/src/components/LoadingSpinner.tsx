import { Spin } from '@douyinfe/semi-ui';
import React from 'react';

interface LoadingSpinnerProps {
  text?: string;
}

const LoadingSpinner = ({ text = 'Loading...' }: LoadingSpinnerProps) => (
  <div className='flex justify-center items-center'>
    <div className='flex flex-col items-center justify-center m-16'>
      <Spin size='large'></Spin>
      <div className='text-gray-500 text-sm'>{text}</div>
    </div>
  </div>
);

export default LoadingSpinner;
