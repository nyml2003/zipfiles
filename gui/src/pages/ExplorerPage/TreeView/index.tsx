import { Splitter } from 'antd';
import React from 'react';
import MetaDataCard from './MetaDataCard';
import TreeMenu from './TreeMenu';
import styles from './index.module.less';

interface Props {
  currentPath: string;
  setCurrentPath: (path: string) => void;
  setCurrentFile: (file: string) => void;
  refresh: boolean;
  setRefresh: (refresh: boolean) => void;
  handleSelect: (paths: string[]) => void;
  currentFile: string;
}

const TreeView = ({
  currentPath,
  setCurrentPath,
  setCurrentFile,
  refresh,
  setRefresh,
  handleSelect,
  currentFile,
}: Props) => {
  return (
    <Splitter className={`flex-grow overflow-hidden ${styles['fade-in-down']}`}>
      <Splitter.Panel
        defaultSize='20%'
        min='10%'
        max='70%'
        className='overflow-y-auto overflow-x-hidden bg-gray-100 flex '>
        <TreeMenu
          onSelect={handleSelect}
          currentPath={currentPath}
          setCurrentPath={setCurrentPath}
          setCurrentFile={setCurrentFile}
          refresh={refresh}
          setRefresh={setRefresh}
        />
      </Splitter.Panel>
      <Splitter.Panel>
        {/* 文件信息 */}
        <MetaDataCard currentFile={currentFile} />
      </Splitter.Panel>
    </Splitter>
  );
};

export default TreeView;
