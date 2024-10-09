import React from 'react';
import {
  ArrowLeftOutlined,
  ArrowsAltOutlined,
  DownOutlined,
  FilterFilled,
  FilterOutlined,
  HomeOutlined,
  Loading3QuartersOutlined,
} from '@ant-design/icons';
import { Breadcrumb, Button, Modal } from 'antd';
import styles from './index.module.less';
import FilterForm from './FilterForm';
import TreeView from './TreeView';
import {
  handleRefresh,
  updateCurrentPath,
  updateCurrentView,
  updateIsFiltering,
  updateSelectedFile,
} from '@/stores/file/reducer';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';

const Explorer: React.FC = () => {
  const view = useSelector((state: RootState) => state.file.view);
  const isFiltering = useSelector((state: RootState) => state.file.isFiltering);
  const currentPath = useSelector((state: RootState) => state.file.currentPath);
  const dispatch = useDispatch();

  const handleSelect = (paths: string[]) => {
    dispatch(updateSelectedFile(paths.sort()));
  };

  const renderContent = () => {
    if (isFiltering) {
      return <FilterForm />;
    }
    return <TreeView />;
  };

  return (
    <div className='split-container-col grow-item'>
      <div className='flex rounded-xl items-center justify-between bg-gray-100'>
        <div className='flex p-2 items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() =>
              dispatch(updateCurrentPath(currentPath.split('/').slice(0, -1).join('/')))
            }></Button>
          <Button
            type='text'
            onClick={() => dispatch(handleRefresh())}
            icon={<Loading3QuartersOutlined />}
            disabled={isFiltering}></Button>
          <Breadcrumb
            items={currentPath.split('/').reduce((acc, item, index, arr) => {
              const path = arr.slice(0, index + 1).join('/');
              acc.push({
                title: item === '' ? <HomeOutlined /> : item,
                onClick: () => dispatch(updateCurrentPath(path)),
                className: 'cursor-pointer px-2 py-1 rounded hover:bg-gray-200',
              });
              return acc;
            }, [] as any)}
          />
        </div>
        <div>
          <Button
            type='text'
            icon={isFiltering ? <FilterFilled /> : <FilterOutlined />}
            onClick={() => dispatch(updateIsFiltering(!isFiltering))}>
            筛选
          </Button>
        </div>
      </div>
      <div
        className={`
        bg-white
        rounded-xl
        m-2
        p-2
        ${styles['fade-in-down']}
        grow-item 
        split-container-row
        `}>
        {renderContent()}
      </div>
    </div>
  );
};

export default Explorer;
