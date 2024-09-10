import { IconRefresh } from '@douyinfe/semi-icons';
import { Button } from '@douyinfe/semi-ui';
import React from 'react';

interface EmptyListProps {
  hasRefresh?: boolean;
  refresh?: () => void;
}

const EmptyList = ({ hasRefresh = false, refresh }: EmptyListProps) => {
  return (
    <span className='flex justify-center items-center h-64 text-gray-500 select-none flex-col'>
      <div>暂无数据</div>
      {hasRefresh && (
        <Button
          className=' text-blue-500'
          onClick={() => refresh && refresh()}
          type='tertiary'
          icon={<IconRefresh />}></Button>
      )}
    </span>
  );
};

export default EmptyList;
