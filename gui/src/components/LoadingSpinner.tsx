import React from 'react';
import { Spin } from 'antd'; // 确保正确导入 Spin 组件

interface LoadingSpinnerProps {
  text?: string;
}

const LoadingSpinner = ({ text = 'Loading...' }: LoadingSpinnerProps) => (
  <div className='flex justify-center items-center w-full'>
    <div className='flex flex-col items-center justify-center m-16'>
      <Spin size='large' />
      <div className='text-gray-500 text-sm'>{text}</div>
    </div>
  </div>
);

export default LoadingSpinner;
