import { ArrowLeftOutlined, HomeOutlined } from '@ant-design/icons';
import TreeSelector from './TreeSelector';
import { Breadcrumb, Button, Input, Splitter } from 'antd'; // Add this import statement
import React, { useState } from 'react';
import MetaDataCard from './MetaDataCard';

interface Props {}

const ExplorerPage: React.FC = () => {
  const [selectedItems, setSelectedItems] = useState<string[]>([]);
  const [currentPath, setCurrentPath] = useState<string>('');
  const [currentFile, setCurrentFile] = useState<string>('');

  const handleSelect = (paths: string[]) => {
    //按字典序排序
    setSelectedItems(paths.sort());
  };

  return (
    <div className='flex-1 flex flex-col'>
      <div className='bg-white rounded-xl p-2 m-2 flex items-center'>
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
      <Splitter className='flex-grow overflow-hidden'>
        <Splitter.Panel
          defaultSize='20%'
          min='10%'
          max='70%'
          className='overflow-y-auto overflow-x-hidden bg-gray-100 flex '>
          <TreeSelector
            onSelect={handleSelect}
            currentPath={currentPath}
            setCurrentPath={setCurrentPath}
            setCurrentFile={setCurrentFile}
          />
        </Splitter.Panel>
        <Splitter.Panel>
          {/* 文件信息 */}
          <MetaDataCard currentFile={currentFile} />
        </Splitter.Panel>
      </Splitter>
    </div>
  );
};

export default ExplorerPage;