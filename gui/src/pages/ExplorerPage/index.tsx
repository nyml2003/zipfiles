import React, { useState } from 'react';
import {
  ArrowLeftOutlined,
  ArrowsAltOutlined,
  DownOutlined,
  FilterFilled,
  FilterOutlined,
  HomeOutlined,
  Loading3QuartersOutlined,
} from '@ant-design/icons';
import { Breadcrumb, Button, Input, Splitter } from 'antd';
import TreeSelector from './TreeView/TreeMenu';
import MetaDataCard from './TreeView/MetaDataCard';
import styles from './index.module.less';
import FilterForm from './FilterForm';
import TreeView from './TreeView';
import TableView from './TableView';

const ExplorerPage: React.FC = () => {
  const [selectedItems, setSelectedItems] = useState<string[]>([]);
  const [currentPath, setCurrentPath] = useState<string>('');
  const [currentFile, setCurrentFile] = useState<string>('');
  const [refresh, setRefresh] = useState<boolean>(false);
  const [filter, setFilter] = useState<boolean>(false);
  const [view, setView] = useState<'table' | 'tree'>('table');

  const handleSelect = (paths: string[]) => {
    //按字典序排序
    setSelectedItems(paths.sort());
  };

  const renderContent = () => {
    if (filter) {
      return <FilterForm />;
    }
    if (view === 'table') {
      return <TableView currentPath={currentPath} />;
    }
    if (view === 'tree') {
      return (
        <TreeView
          currentPath={currentPath}
          setCurrentPath={setCurrentPath}
          setCurrentFile={setCurrentFile}
          refresh={refresh}
          setRefresh={setRefresh}
          handleSelect={handleSelect}
          currentFile={currentFile}
        />
      );
    }
  };

  return (
    <div className='flex-1 flex flex-col'>
      <div className='bg-white rounded-xl p-2 m-2 flex items-center justify-between'>
        <div className='p-2 flex flex-row items-center'>
          <Button
            type='text'
            icon={<ArrowLeftOutlined />}
            onClick={() =>
              setCurrentPath(prevPath => {
                const pathArr = prevPath.split('/');
                pathArr.pop();
                return pathArr.join('/');
              })
            }></Button>
          <Button
            type='text'
            onClick={() => setRefresh(true)}
            icon={<Loading3QuartersOutlined />}
          />
          <Breadcrumb
            items={currentPath.split('/').reduce((acc, item, index, arr) => {
              const path = arr.slice(0, index + 1).join('/');
              acc.push({
                title: item === '' ? <HomeOutlined /> : item,
                onClick: () => setCurrentPath(path),
                className: 'cursor-pointer px-2 py-1 rounded hover:bg-gray-200',
              });
              return acc;
            }, [] as any)}
          />
        </div>
        <div>
          <Button
            type='text'
            icon={view === 'table' ? <ArrowsAltOutlined /> : <DownOutlined />}
            onClick={() => setView(view === 'table' ? 'tree' : 'table')}
            disabled={filter}>
            视图
          </Button>
          <Button
            type='text'
            icon={filter ? <FilterFilled /> : <FilterOutlined />}
            onClick={() => setFilter(!filter)}>
            筛选
          </Button>
        </div>
      </div>
      <div className={`bg-white rounded-xl m-2 p-2 ${styles['fade-in-down']} overflow-auto flex-1`}>
        {renderContent()}
      </div>
    </div>
  );
};

export default ExplorerPage;
