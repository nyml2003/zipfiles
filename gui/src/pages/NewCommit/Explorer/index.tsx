import React from 'react';
import {
  ArrowLeftOutlined,
  FilterFilled,
  FilterOutlined,
  HomeOutlined,
  Loading3QuartersOutlined,
} from '@ant-design/icons';
import { Breadcrumb, Button, Splitter } from 'antd';
import { handleRefresh, updateCurrentPath, updateIsFiltering } from '@/stores/CreateCommitReducer';
import { useDispatch, useSelector } from 'react-redux';
import { RootState } from '@/stores/store';
import { BreadcrumbItemType } from 'antd/es/breadcrumb/Breadcrumb';
import TreeMenu from './TreeMenu';
import TableView from './TableView';
import FilterForm from './FilterForm';

const Explorer: React.FC = () => {
  const isFiltering = useSelector((state: RootState) => state.createCommit.isFiltering);
  const currentPath = useSelector((state: RootState) => state.createCommit.currentPath);
  const dispatch = useDispatch();

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
            }, [] as BreadcrumbItemType[])}
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
        className='
        bg-white
        rounded-xl
        m-2
        p-2
        fade-in-down
        grow-item 
        split-container-row
        '>
        {isFiltering ? (
          <FilterForm />
        ) : (
          <Splitter className='fade-in-down split-container-row grow-item'>
            <Splitter.Panel
              defaultSize='20%'
              min='10%'
              max='70%'
              className='split-container-row grow-item'>
              <TreeMenu />
            </Splitter.Panel>
            <Splitter.Panel className='split-container-row grow-item'>
              <TableView />
            </Splitter.Panel>
          </Splitter>
        )}
      </div>
    </div>
  );
};

export default Explorer;
